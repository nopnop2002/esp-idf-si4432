/* The example of SI4432
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_cdc_acm.h"
#include "esp_log.h"

#include "si4432.h"

static const char *TAG = "MAIN";

MessageBufferHandle_t xMessageBufferTrans;
MessageBufferHandle_t xMessageBufferRecv;
QueueHandle_t xQueueTinyusb;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;
// The size, in bytes, required to hold each item in the message,
size_t xItemSize = 64; // Maximum Payload size of SI4432 is 64

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
	/* initialization */
	size_t rx_size = 0;
	ESP_LOGD(TAG, "CONFIG_TINYUSB_CDC_RX_BUFSIZE=%d", CONFIG_TINYUSB_CDC_RX_BUFSIZE);

	/* read */
	uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE];
	esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
	if (ret == ESP_OK) {
		ESP_LOGD(TAG, "Data from channel=%d rx_size=%d", itf, rx_size);
		ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
#if CONFIG_SENDER
		for(int i=0;i<rx_size;i++) {
			xQueueSendFromISR(xQueueTinyusb, &buf[i], NULL);
		}
#endif
	} else {
		ESP_LOGE(TAG, "tinyusb_cdcacm_read error");
	}
}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
	int dtr = event->line_state_changed_data.dtr;
	int rts = event->line_state_changed_data.rts;
	ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferRecv, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);

		bool sent = sendPacket(received, buf);
		if (!sent) {
			ESP_LOGE(pcTaskGetName(NULL), "sendPacket Fail");
		}
	} // end while

	// never reach here
	vTaskDelete( NULL );
}

void usb_rx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	char buffer[xItemSize];
	int index = 0;
	while(1) {
		char ch;
		if(xQueueReceive(xQueueTinyusb, &ch, portMAX_DELAY)) {
			ESP_LOGD(pcTaskGetName(NULL), "ch=0x%x",ch);
			if (ch == 0x0d || ch == 0x0a) {
				if (index > 0) {
					ESP_LOGI(pcTaskGetName(NULL), "[%.*s]", index, buffer);
					size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
					ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
					size_t sended = xMessageBufferSend(xMessageBufferRecv, buffer, index, 100);
					if (sended != index) {
						ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail index=%d sended=%d", index, sended);
						break;
					}
					index = 0;
				}
			} else {
				if (index == xItemSize) continue;
				buffer[index++] = ch;
			}
		}
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_SENDER

#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	uint8_t rxLen;
	startListening();

	while(1) {
		bool recv = isPacketReceived();
		ESP_LOGD(pcTaskGetName(NULL), "recv=%d", recv);
		if (recv) {
			getPacketReceived(&rxLen, buf);
			ESP_LOGI(pcTaskGetName(NULL),"rxLen:%d", rxLen);
			ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), buf, rxLen, ESP_LOG_INFO);

			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferTrans );
			ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferTrans, buf, rxLen, 100);
			if (sended != rxLen) {
				ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail rxLen=%d sended=%d", rxLen, sended);
				break;
			}

			startListening(); // restart the listening.
		} // end recv
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while
	vTaskDelete( NULL );
}

void usb_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	uint8_t crlf[2] = { 0x0d, 0x0a };
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferTrans, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", received, received, buf);
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, buf, received);
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, crlf, 2);
		tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_RECEIVER

void app_main()
{
	ESP_LOGI(TAG, "USB initialization");
	tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
#ifdef CONFIG_IDF_TARGET_ESP32P4
	tusb_cfg.port = TINYUSB_PORT_FULL_SPEED_0;
#endif
	ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

	tinyusb_config_cdcacm_t acm_cfg = {
		.cdc_port = TINYUSB_CDC_ACM_0,
		.callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
		.callback_rx_wanted_char = NULL,
		.callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
		.callback_line_coding_changed = NULL
	};
	ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));

#if (CONFIG_TINYUSB_CDC_COUNT > 1)
	acm_cfg.cdc_port = TINYUSB_CDC_ACM_1;
	ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));
	ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
		TINYUSB_CDC_ACM_1,
		CDC_EVENT_LINE_STATE_CHANGED,
		&tinyusb_cdc_line_state_changed_callback));
#endif

	// Create MessageBuffer
	xMessageBufferTrans = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferTrans );
	xMessageBufferRecv = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferRecv );

	// Create Queue
	xQueueTinyusb = xQueueCreate(100, sizeof(char));
	configASSERT( xQueueTinyusb );

	// Initialize radio
	bool radio = init();
	if (radio == false) {
		ESP_LOGE(TAG, "SI4432 not installed");
		while(1) { vTaskDelay(1); }
	}

	unsigned long freq;
#if CONFIG_SI4432_FREQ_433
	freq = 433;
#elif CONFIG_SI4432_FREQ_470
	freq = 470;
#elif CONFIG_SI4432_FREQ_868
	freq = 868;
#elif CONFIG_SI4432_FREQ_915
	freq = 915;
#endif
	ESP_LOGW(TAG, "Set frequency to %luMHz", freq);
	setFrequency(freq);
	ESP_LOGW(TAG, "Set channel to %d", CONFIG_SI4432_CHANNEL);
	setChannel(CONFIG_SI4432_CHANNEL);
	setBaudRate(70);
	//readAll();

#if CONFIG_SENDER
	xTaskCreate(&tx_task, "TX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&usb_rx, "USB_RX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&usb_tx, "USB_TX", 1024*4, NULL, 5, NULL);
#endif
}
