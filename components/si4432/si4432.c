#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "si4432.h"

#define TAG "SI4432"

// SPI Stuff
#if CONFIG_SPI2_HOST
#define HOST_ID SPI2_HOST
#elif CONFIG_SPI3_HOST
#define HOST_ID SPI3_HOST
#endif

static spi_device_handle_t _handle;

#define MAX_TRANSMIT_TIMEOUT 200
// #define DEBUG

//values here are kept in khz x 10 format (for not to deal with decimals) - look at AN440 page 26 for whole table
const uint16_t IFFilterTable[][2] = { { 322, 0x26 }, { 3355, 0x88 }, { 3618, 0x89 }, { 4202, 0x8A }, { 4684, 0x8B }, {
		5188, 0x8C }, { 5770, 0x8D }, { 6207, 0x8E } };

static uint64_t _freqCarrier;
static uint8_t _freqChannel;
static uint16_t _kbps;
static uint16_t _packageSign;

#define delayMicroseconds(microsec) esp_rom_delay_us(microsec)
#define delay(millsec) esp_rom_delay_us(millsec*1000)
#define millis() xTaskGetTickCount()*portTICK_PERIOD_MS

void spi_init()
{
	gpio_reset_pin(CONFIG_SEL_GPIO);
	gpio_set_direction(CONFIG_SEL_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(CONFIG_SEL_GPIO, 1);

	spi_bus_config_t buscfg = {
		.sclk_io_num = CONFIG_SCK_GPIO, // set SPI CLK pin
		.mosi_io_num = CONFIG_MOSI_GPIO, // set SPI MOSI pin
		.miso_io_num = CONFIG_MISO_GPIO, // set SPI MISO pin
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	esp_err_t ret;
	ret = spi_bus_initialize( HOST_ID, &buscfg, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	// Hardware CS control don't work.
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = 5000000, // SPI clock is 5 MHz!
		.queue_size = 7,
		.mode = 0, // SPI mode 0
		.spics_io_num = -1, // we will use manual CS control
		.flags = SPI_DEVICE_NO_DUMMY
	};

	ret = spi_bus_add_device( HOST_ID, &devcfg, &_handle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
}

bool spi_write_byte(uint8_t* Dataout, size_t DataLength)
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( _handle, &SPITransaction );
	}

	return true;
}

bool spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength)
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( _handle, &SPITransaction );
	}

	return true;
}

uint8_t spi_transfer(uint8_t address)
{
	uint8_t datain[1];
	uint8_t dataout[1];
	dataout[0] = address;
	//spi_write_byte(dev, dataout, 1 );
	//spi_read_byte(datain, dataout, 1 );

	spi_transaction_t SPITransaction;
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 8;
	SPITransaction.tx_buffer = dataout;
	SPITransaction.rx_buffer = datain;
	spi_device_transmit( _handle, &SPITransaction );

	return datain[0];
}

void setFrequency(unsigned long baseFrequencyMhz) {

	if ((baseFrequencyMhz < 240) || (baseFrequencyMhz > 930))
		return; // invalid frequency

	_freqCarrier = baseFrequencyMhz;
	byte highBand = 0;
	if (baseFrequencyMhz >= 480) {
		highBand = 1;
	}

	//double fPart = (baseFrequencyMhz / (10 * (highBand + 1))) - 24;
	double fPart = ((double)baseFrequencyMhz / (10 * (highBand + 1))) - 24;

	uint8_t freqband = (uint8_t) fPart; // truncate the int

	uint16_t freqcarrier = (fPart - freqband) * 64000;

	// sideband is always on (0x40) :
	byte vals[3] = { 0x40 | (highBand << 5) | (freqband & 0x3F), freqcarrier >> 8, freqcarrier & 0xFF };

	BurstWrite(REG_FREQBAND, vals, 3);

}

void setCommsSignature(uint16_t signature) {
	_packageSign = signature;

	ChangeRegister(REG_TRANSMIT_HEADER3, _packageSign >> 8); // header (signature) byte 3 val
	ChangeRegister(REG_TRANSMIT_HEADER2, (_packageSign & 0xFF)); // header (signature) byte 2 val

	ChangeRegister(REG_CHECK_HEADER3, _packageSign >> 8); // header (signature) byte 3 val for receive checks
	ChangeRegister(REG_CHECK_HEADER2, (_packageSign & 0xFF)); // header (signature) byte 2 val for receive checks
}

bool init() {
	ESP_LOGI(TAG, "CONFIG_SEL_GPIO=%d", CONFIG_SEL_GPIO); // SPI CS
	ESP_LOGI(TAG, "CONFIG_SDN_GPIO=%d", CONFIG_SDN_GPIO); // Shutdown Input
	ESP_LOGI(TAG, "CONFIG_IRQ_GPIO=%d", CONFIG_IRQ_GPIO); // IRQ Output

	_freqCarrier = 433000000;
	_freqChannel = 0;
	_kbps = 100;
	_packageSign = 0xDEAD;

	gpio_reset_pin(CONFIG_IRQ_GPIO);
	gpio_set_direction(CONFIG_IRQ_GPIO, GPIO_MODE_INPUT);

	gpio_reset_pin(CONFIG_SDN_GPIO);
	gpio_set_direction(CONFIG_SDN_GPIO, GPIO_MODE_OUTPUT);
	turnOff();

	spi_init();
	ESP_LOGI(TAG, "SPI is initialized now.");

	hardReset();

	// Check Sync Word
	byte syncWord3 = ReadRegister(REG_SYNC_WORD3);
	byte syncWord2 = ReadRegister(REG_SYNC_WORD2);
	byte syncWord1 = ReadRegister(REG_SYNC_WORD1);
	byte syncWord0 = ReadRegister(REG_SYNC_WORD0);
	ESP_LOGI(TAG, "syncWord3=%x, syncWord2=%x, syncWord1=%x, syncWord0=%x", syncWord3, syncWord2, syncWord1, syncWord0);
	
	if (syncWord3 != 0x2D || syncWord2 != 0xD4) return false;
	return true;
}

void boot() {
	/*
	byte currentFix[] = { 0x80, 0x40, 0x7F };
	BurstWrite(REG_CHARGEPUMP_OVERRIDE, currentFix, 3); // refer to AN440 for reasons
	*/

	ChangeRegister(REG_AFC_TIMING_CONTROL, 0x02); // refer to AN440 for reasons
	ChangeRegister(REG_AFC_LIMITER, 0xFF); // write max value - excel file did that.
	ChangeRegister(REG_AGC_OVERRIDE, 0x60); // max gain control
	ChangeRegister(REG_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x3C); // turn off AFC

	ChangeRegister(REG_DATAACCESS_CONTROL, 0xAD); // enable rx packet handling, enable tx packet handling, enable CRC, use CRC-IBM
	ChangeRegister(REG_HEADER_CONTROL1, 0x0C); // no broadcast address control, enable check headers for bytes 3 & 2
	ChangeRegister(REG_HEADER_CONTROL2, 0x22); // enable headers byte 3 & 2, no fixed package length, sync word 3 & 2
	ChangeRegister(REG_PREAMBLE_LENGTH, 0x08); // 8 * 4 bits = 32 bits (4 bytes) preamble length
	ChangeRegister(REG_PREAMBLE_DETECTION, 0x3A); // validate 7 * 4 bits of preamble	in a package
	setCommsSignature(_packageSign); // default signature

	ChangeRegister(REG_SYNC_WORD3, 0x2D); // Set Synchronization Word 3
	ChangeRegister(REG_SYNC_WORD2, 0xD4); // Set Synchronization Word 2
	ChangeRegister(REG_TX_POWER, 0x03); // Set TX Power
	ChangeRegister(REG_CHANNEL_STEPSIZE, 0x64); // Set Frequency Hopping Step Size

	setFrequency(_freqCarrier); // default freq
	setBaudRate(_kbps); // default baud rate is 100kpbs
	setChannel(_freqChannel); // default channel is 0
	setCommsSignature(_packageSign); // default signature

	switchMode(Ready);

}


bool sendPacket(uint8_t length, const byte* data) {
	if (length > 64) return false;

	clearTxFIFO();
	ChangeRegister(REG_PKG_LEN, length);
	BurstWrite(REG_FIFO, data, length);

	ChangeRegister(REG_INT_ENABLE1, 0x04); // set interrupts on for package sent
	ChangeRegister(REG_INT_ENABLE2, 0x00); // set interrupts off for anything else
	// read interrupt registers to clean them
	ReadRegister(REG_INT_STATUS1);
	ReadRegister(REG_INT_STATUS2);

	switchMode(TXMode | Ready);

	uint64_t enterMillis = millis();
 
	while (millis() - enterMillis < MAX_TRANSMIT_TIMEOUT) {

		//if ((CONFIG_IRQ_GPIO != 0) && (gpio_get_level(CONFIG_IRQ_GPIO) != 0)) {
		if (gpio_get_level(CONFIG_IRQ_GPIO) != 0) {
			vTaskDelay(1);
			continue;
		}

		byte intStatus1 = ReadRegister(REG_INT_STATUS1);
		//ReadRegister(REG_INT_STATUS2);
		ESP_LOGD(__FUNCTION__, "REG_INT_STATUS1=0x%x", intStatus1);
		
		if (intStatus1 & 0x04) { // Set once a packet is successfully sent (no TX abort).
		//if ( (intStatus1 & 0x04) == 0x04 || (intStatus1 & 0x20) == 0x20) { // TX FIFO Almost Empty.
			//switchMode(Ready | TuneMode);
			return true;
		}
		vTaskDelay(1);
	} // end while

	//timeout occurred.
	ESP_LOGW(__FUNCTION__, "Timeout in Transit -- ");
	hardReset(); // nop

	return false;
}


void getPacketReceived(uint8_t* length, byte* readData) {

	*length = ReadRegister(REG_RECEIVED_LENGTH);

	BurstRead(REG_FIFO, readData, *length);

	clearRxFIFO(); // which will also clear the interrupts
}

void setChannel(byte channel) {

	ChangeRegister(REG_FREQCHANNEL, channel);

}

void switchMode(byte mode) {
	ESP_LOGD(__FUNCTION__, "switchMode mode=0x%x", mode);
	ChangeRegister(REG_STATE, mode);
}

void ChangeRegister(Registers reg, byte value) {
	BurstWrite(reg, &value, 1);

	// reg = 0x07 and value = 0x80 is Software Register Reset Bit.
	// This bit will be automatically cleared.
	if (reg == 0x07 && value == 0x80) return;
	byte _value;
	BurstRead(reg, &_value, 1);
	if (value != _value) {
		ESP_LOGE(__FUNCTION__, "ChangeRegister Fail");
		ESP_LOGE(__FUNCTION__, "reg=0x%x value=0x%x", reg, value);
	}
}

void setBaudRate(uint16_t kbps) {

	// chip normally supports very low bps values, but they are cumbersome to implement - so I just didn't implement lower bps values
	if ((kbps > 256) || (kbps < 1))
		return;
	_kbps = kbps;

	// set Modulation Mode Control 1/2/3
	byte freqDev = kbps <= 10 ? 15 : 150;		// 15khz / 150 khz
	//byte modulationMode1 = _kbps < 30 ? 0x2c : 0x0c; // use FIFO Mode, GFSK, low baud mode on / off
	byte modulationMode1 = _kbps < 30 ? 0x26 : 0x06; // Manchester Data Inversion is Enabled, Manchester Coding is Enabled
	byte modulationMode2 = 0x23; // Modulation Source = FIFO Mode, Modulation Type=GFSK
	byte modulationMode3 = round((freqDev * 1000.0) / 625.0);
	ESP_LOGD(__FUNCTION__, "modulationMode1=0x%x",  modulationMode1);
	ESP_LOGD(__FUNCTION__, "modulationMode2=0x%x",  modulationMode2);
	ESP_LOGD(__FUNCTION__, "modulationMode3=0x%x",  modulationMode3);

	//byte modulationVals[] = { modulationmode1, 0x23, round((freqDev * 1000.0) / 625.0) }; // msb of the kpbs to 3rd bit of register
	byte modulationVals[] = { modulationMode1, modulationMode2, modulationMode3 }; // msb of the kpbs to 3rd bit of register
	BurstWrite(REG_MODULATION_MODE1, modulationVals, 3); // 0x70

	// set TX Data Rate 1
	uint16_t bpsRegVal = round((kbps * (kbps < 30 ? 2097152 : 65536.0)) / 1000.0);
	byte datarateVals[] = { bpsRegVal >> 8, bpsRegVal & 0xFF };

	BurstWrite(REG_TX_DATARATE1, datarateVals, 2); // 0x6E

	//now set the timings
	uint16_t minBandwidth = (2 * (uint32_t) freqDev) + kbps;
	ESP_LOGD(__FUNCTION__, "min Bandwidth value: 0x%x", minBandwidth);
	byte IFValue = 0xff;
	//since the table is ordered (from low to high), just find the 'minimum bandwidth which is greater than required'
	for (byte i = 0; i < 8; ++i) {
		if (IFFilterTable[i][0] >= (minBandwidth * 10)) {
			IFValue = IFFilterTable[i][1];
			break;
		}
	}
	ESP_LOGD(__FUNCTION__, "Selected IF value: 0x%x", IFValue);

	ChangeRegister(REG_IF_FILTER_BW, IFValue);

	byte dwn3_bypass = (IFValue & 0x80) ? 1 : 0; // if msb is set
	byte ndec_exp = (IFValue >> 4) & 0x07; // only 3 bits

	uint16_t rxOversampling = round((500.0 * (1 + 2 * dwn3_bypass)) / ((pow(2, ndec_exp - 3)) * (double ) kbps));

	uint32_t ncOffset = ceil(((double) kbps * (pow(2, ndec_exp + 20))) / (500.0 * (1 + 2 * dwn3_bypass)));

	uint16_t crGain = 2 + ((65535 * (int64_t) kbps) / ((int64_t) rxOversampling * freqDev));
	byte crMultiplier = 0x00;
	if (crGain > 0x7FF) {
		crGain = 0x7FF;
	}
	ESP_LOGD(__FUNCTION__, "dwn3_bypass value: 0x%x", dwn3_bypass);
	ESP_LOGD(__FUNCTION__, "ndec_exp value: 0x%x", ndec_exp);
	ESP_LOGD(__FUNCTION__, "rxOversampling value: 0x%x", rxOversampling);
	ESP_LOGD(__FUNCTION__, "ncOffset value: 0x%"PRIu32, ncOffset);
	ESP_LOGD(__FUNCTION__, "crGain value: 0x%x", crGain);
	ESP_LOGD(__FUNCTION__, "crMultiplier value: 0x%x", crMultiplier);

	byte timingVals[] = { rxOversampling & 0x00FF, ((rxOversampling & 0x0700) >> 3) | ((ncOffset >> 16) & 0x0F),
			(ncOffset >> 8) & 0xFF, ncOffset & 0xFF, ((crGain & 0x0700) >> 8) | crMultiplier, crGain & 0xFF };

	BurstWrite(REG_CLOCK_RECOVERY_OVERSAMPLING, timingVals, 6);

}

byte ReadRegister(Registers reg) {
	byte val[1];
	BurstRead(reg, val, 1);
	return val[0];
}

void BurstWrite(Registers startReg, const byte value[], uint8_t length) {

	byte regVal = (byte) startReg | 0x80; // set MSB

	gpio_set_level(CONFIG_SEL_GPIO, 0);
	spi_transfer(regVal);

	for (byte i = 0; i < length; ++i) {
		ESP_LOGD(__FUNCTION__, "Writing: %x | %x", (regVal != 0xFF ? (regVal + i) & 0x7F : 0x7F), value[i]);
		spi_transfer(value[i]);
	}

	gpio_set_level(CONFIG_SEL_GPIO, 1);
}

void BurstRead(Registers startReg, byte value[], uint8_t length) {

	byte regVal = (byte) startReg & 0x7F; // clear MSB

	gpio_set_level(CONFIG_SEL_GPIO, 0);
	spi_transfer(regVal);

	for (byte i = 0; i < length; ++i) {
		value[i] = spi_transfer(0xFF);
		ESP_LOGD(__FUNCTION__, "Reading: %x | %x", (regVal != 0x7F ? (regVal + i) & 0x7F : 0x7F), value[i]);
	}

	gpio_set_level(CONFIG_SEL_GPIO, 1);
}

void readAll() {

	byte allValues[0x7F];

	BurstRead(REG_DEV_TYPE, allValues, 0x7F);

	for (byte i = 0; i < 0x7f; ++i) {
		ESP_LOGI(TAG, "REG(%x) : %x", REG_DEV_TYPE + i, allValues[i]);
	}

}

void clearTxFIFO() {
	ChangeRegister(REG_OPERATION_CONTROL, 0x01);
	ChangeRegister(REG_OPERATION_CONTROL, 0x00);
}

void clearRxFIFO() {
	ChangeRegister(REG_OPERATION_CONTROL, 0x02);
	ChangeRegister(REG_OPERATION_CONTROL, 0x00);
}

void clearFIFO() {
	ChangeRegister(REG_OPERATION_CONTROL, 0x03);
	ChangeRegister(REG_OPERATION_CONTROL, 0x00);
}

void softReset() {
	ChangeRegister(REG_STATE, 0x01);
	ChangeRegister(REG_STATE, 0x80);

	byte reg = ReadRegister(REG_INT_STATUS2);
	while ((reg & 0x02) != 0x02) {
		vTaskDelay(1);
		reg = ReadRegister(REG_INT_STATUS2);
	}

	boot();
}

void hardReset() {
	// toggle Shutdown Pin
	turnOff();
	turnOn();

	byte reg = ReadRegister(REG_INT_STATUS2);
	while ((reg & 0x02) != 0x02) {
		vTaskDelay(1);
		reg = ReadRegister(REG_INT_STATUS2);
	}

	boot();
}

void startListening() {

	clearRxFIFO(); // clear first, so it doesn't overflow if packet is big

	ChangeRegister(REG_INT_ENABLE1, 0x03); // set interrupts on for package received and CRC error

#ifdef DEBUG
	ChangeRegister(REG_INT_ENABLE2, 0xC0);
#else
	ChangeRegister(REG_INT_ENABLE2, 0x00); // set other interrupts off
#endif
	//read interrupt registers to clean them
	ReadRegister(REG_INT_STATUS1);
	ReadRegister(REG_INT_STATUS2);

	switchMode(RXMode | Ready);
}

bool isPacketReceived() {

	//if ((CONFIG_IRQ_GPIO != 0) && (gpio_get_level(CONFIG_IRQ_GPIO) != 0)) {
	if (gpio_get_level(CONFIG_IRQ_GPIO) != 0) {
		return false; // if no interrupt occurred, no packet received is assumed (since startListening will be called prior, this assumption is enough)
	}

	// check for package received status interrupt register
	byte intStatus1 = ReadRegister(REG_INT_STATUS1);
	ESP_LOGD(__FUNCTION__, "isPacketReceived REG_INT_STATUS1=0x%x", intStatus1);

#ifdef DEBUG
	byte intStatus2 = ReadRegister(REG_INT_STATUS2);

	if (intStatus2 & 0x40) { //interrupt occurred, check it && read the Interrupt Status1 register for 'preamble '

		ESP_LOGI(TAG, "HEY!! HEY!! Valid Preamble detected -- 0x%x", intStatus2);

	}

	if (intStatus2 & 0x80) { //interrupt occurred, check it && read the Interrupt Status1 register for 'preamble '

		ESP_LOGI(TAG, "HEY!! HEY!! SYNC WORD detected -- 0x%x", intStatus2);

	}
#endif

	if (intStatus1 & 0x02) { //interrupt occurred, check it && read the Interrupt Status1 register for 'valid packet'
		switchMode(Ready | TuneMode); // if packet came, get out of Rx mode till the packet is read out. Keep PLL on for fast reaction
		ESP_LOGD(__FUNCTION__, "Packet detected -- 0x%x", intStatus1);
		return true;
	} else if (intStatus1 & 0x01) { // packet crc error
		switchMode(Ready); // get out of Rx mode till buffers are cleared
		ESP_LOGW(__FUNCTION__, "CRC Error in Packet detected!-- 0x%x", intStatus1);
		clearRxFIFO();
		switchMode(RXMode | Ready); // get back to work
		return false;
	}

	//no relevant interrupt? no packet!

	return false;
}

void turnOn() {
	gpio_set_level(CONFIG_SDN_GPIO, 0);
	vTaskDelay(1);
}

void turnOff() {
	gpio_set_level(CONFIG_SDN_GPIO, 1);
	vTaskDelay(1);
}
