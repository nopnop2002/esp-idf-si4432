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
#include "esp_log.h"

#include "si4432.h"

static const char *TAG = "MAIN";

#if CONFIG_TRANSMITTER
void tx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txBuf[64];

	while(1) {
		sprintf((char *)txBuf,"Hello World %"PRIu32, xTaskGetTickCount());
		uint8_t txLen = strlen((char *)txBuf);

		bool sent = sendPacket(txLen, txBuf);
		if (sent) {
		
#if CONFIG_RESPONCE
			uint8_t rxLen;
			uint8_t rxBuf[64];
			TickType_t startTicks = xTaskGetTickCount();
			printf("startListening\n");
			startListening(); // restart the listening.
			while(1) {
				bool recv = isPacketReceived();
				if (recv) {
					getPacketReceived(&rxLen, rxBuf);
					TickType_t receiveTicks = xTaskGetTickCount();
					TickType_t elaspedTicks = receiveTicks - startTicks;
					printf("rxLen:%d elasped:%"PRIu32"d\n", rxLen, elaspedTicks);

					for (int i = 0; i < rxLen; ++i) {
						printf("%x ", rxBuf[i]);
					}
					printf("\n");
		
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
					break;
				} // end if

				if ( (xTaskGetTickCount() - startTicks) > 100) {
					printf("No responce within 100 ticks\n");
					break;
				}
			
		} // end while
#endif

		} else {
			printf("sendPacket Fail\n");
			vTaskDelay(1000);
		}
		vTaskDelay(100);
	} // end while

}
#endif // CONFIG_TRANSMITTER



#if CONFIG_RECEIVER
void rx_task(void *pvParameter)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	startListening();
	uint8_t rxBuf[64];
	uint8_t rxLen;

	while(1) {
		bool recv = isPacketReceived();
		if (recv) {
			getPacketReceived(&rxLen, rxBuf);
			printf("rxLen:%d\n", rxLen);

			for (int i = 0; i < rxLen; ++i) {
				printf("%x ", rxBuf[i]);
			}
			printf("\n");

			for (int i = 0; i < rxLen; ++i) {
				char c = rxBuf[i];
				if (c > 0x20 && c < 0xFF) {
					printf("%c", c);
				} else {
					printf(" ");
				}
			}
			printf("\n");

#if CONFIG_RESPONCE
			uint8_t txLen;
			uint8_t txBuf[64];
			for (int i = 0; i < rxLen; ++i) {
				char c = rxBuf[i];
				if (islower(c)) {
					txBuf[i] = toupper(c);
				} else {
					txBuf[i] = tolower(c);
				}
			}
			txLen = rxLen;

			vTaskDelay(2);
			printf("Sending response... \n");
			sendPacket(txLen, txBuf);
#endif
			startListening(); // restart the listening.
		} else {
			//printf("No packet this cycle\n");
		} // endif

		vTaskDelay(1);
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_RECEIVER

void app_main()
{
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

#if CONFIG_TRANSMITTER
	xTaskCreate(&tx_task, "tx_task", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&rx_task, "rx_task", 1024*4, NULL, 1, NULL);
#endif
}
