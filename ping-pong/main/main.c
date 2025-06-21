/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "si4432.h"

static const char *TAG = "MAIN";

#if CONFIG_PRIMARY
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t txLen;
	uint8_t rxLen;

	while(1) {
		txLen = sprintf((char *)txBuf,"Hello World %"PRIu32, xTaskGetTickCount());
		bool sent = sendPacket(txLen, txBuf);
		if (sent) {
			bool waiting = true;
			ESP_LOGI(pcTaskGetName(NULL), "startListening");
			TickType_t startTicks = xTaskGetTickCount();
			int64_t startTime = esp_timer_get_time();
			startListening(); // restart the listening.

			while(waiting) {
				bool recv = isPacketReceived();
				if (recv) {
					waiting = false;
					getPacketReceived(&rxLen, rxBuf);
					int64_t elaspedTime = esp_timer_get_time() - startTime;
					ESP_LOGI(pcTaskGetName(NULL), "rxLen:%d elasped:%"PRId64" uSec", rxLen, elaspedTime);
					ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), rxBuf, rxLen, ESP_LOG_INFO);

					for (int i = 0; i < txLen; ++i) {
						char c = txBuf[i];
						if (c > 0x20 && c < 0xFF) {
							printf("%c", c);
						} else {
							printf(" ");
						}
					}
					printf("-->");
					for (int i = 0; i < rxLen; ++i) {
						char c = rxBuf[i];
						if (c > 0x20 && c < 0xFF) {
							printf("%c", c);
						} else {
							printf(" ");
						}
					}
					printf("\n");
				} else {
					if ( (xTaskGetTickCount() - startTicks) > 100) {
						ESP_LOGW(pcTaskGetName(NULL), "No responce within 100 ticks");
						waiting = false;
					}
				}
			} // end while

		} else {
			ESP_LOGE(pcTaskGetName(NULL), "sendPacket Fail");
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_PRIMARY

#if CONFIG_SECONDARY
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];
	uint8_t rxBuf[64];
	uint8_t txLen;
	uint8_t rxLen;
	startListening();

	while(1) {
		bool recv = isPacketReceived();
		ESP_LOGD(pcTaskGetName(NULL), "recv=%d", recv);
		if (recv) {
			getPacketReceived(&rxLen, rxBuf);
			ESP_LOGI(pcTaskGetName(NULL),"rxLen:%d", rxLen);
			ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), rxBuf, rxLen, ESP_LOG_INFO);

			for (int i = 0; i < rxLen; ++i) {
				char c = rxBuf[i];
				if (islower(c)) {
					txBuf[i] = toupper(c);
				} else {
					txBuf[i] = tolower(c);
				}
			}
			txLen = rxLen;

			ESP_LOGI(pcTaskGetName(NULL), "Sending response...");
			bool sent = sendPacket(txLen, txBuf);
			if (!sent) {
				ESP_LOGE(pcTaskGetName(NULL), "sendPacket Fail");
			}
			startListening(); // restart the listening.
		} // end recv
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_SECONDARY

void app_main()
{
	// Initialize radio
	bool ret = init();
	if (ret == false) {
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

#if CONFIG_PRIMARY
	xTaskCreate(&tx_task, "PRIMARY", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_SECONDARY
	xTaskCreate(&rx_task, "SECONDARY", 1024*4, NULL, 1, NULL);
#endif
}
