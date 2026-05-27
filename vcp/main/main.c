/* The example of SI4432
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"

#include "si4432.h"

static const char *TAG = "MAIN";

MessageBufferHandle_t xMessageBufferTx;
MessageBufferHandle_t xMessageBufferRx;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;
// The size, in bytes, required to hold each item in the message,
size_t xItemSize = 64; // Maximum Payload size of SI4432 is 64

#if CONFIG_SENDER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[xItemSize];
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferRx, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);

		bool sent = sendPacket(received, buf);
		if (!sent) {
			ESP_LOGE(pcTaskGetName(NULL), "sendPacket Fail");
		}
	} // end while

	// never reach here
	vTaskDelete( NULL );
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

			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferTx );
			ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferTx, buf, rxLen, 100);
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
#endif // CONFIG_RECEIVER

void cdc_acm_vcp_task(void *pvParameters);

void app_main()
{
	// Create MessageBuffer
	xMessageBufferTx = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferTx );
	xMessageBufferRx = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferRx );

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
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "RX", 1024*4, NULL, 5, NULL);
#endif
    // Start CDC_ACM_VCP
    xTaskCreate(&cdc_acm_vcp_task, "CDC_ACM_VCP", 1024*4, NULL, 5, NULL);
}
