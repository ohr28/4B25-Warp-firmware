#include <stdlib.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"
#include "devINA219.h"


extern volatile WarpI2CDeviceState	deviceINA219State;
extern volatile uint32_t		gWarpI2cBaudRateKbps;
extern volatile uint32_t		gWarpI2cTimeoutMilliseconds;

// Initialise calibration values
uint16_t INA219_calibrationValue = 0;
uint16_t INA219_currentMultiplier_uA = 0;
uint16_t INA219_powerMultiplier_uW = 0;





uint32_t readRegisterINA219(uint8_t registerPointer, int numBytes) {
	
	
	uint8_t		cmdBuf[1] = {0xFF};
	i2c_status_t	status0, status1;

	i2c_device_t slave =
	{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = kWarpDefaultI2cBaudRateKbps
	};

	cmdBuf[0] = registerPointer;
	warpEnableI2Cpins();

	// Write the register pointer
	status0 = I2C_DRV_MasterSendDataBlocking(
		0 /* I2C instance */,
		&slave,
		cmdBuf,
		1,
		NULL,
		0,
		gWarpI2cTimeoutMilliseconds);

	if (status0 != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	// OSA_TimeDelay(10);

	// Read the contents of the selected register
	status1 = I2C_DRV_MasterReceiveDataBlocking(
		0 /* I2C peripheral instance */,
		&slave,
		NULL,
		0,
		(uint8_t *)deviceINA219State.i2cBuffer,
		numBytes,
		kWarpDefaultI2cTimeoutMilliseconds);

	if (status1 != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}





uint32_t writeRegisterINA219(uint8_t registerPointer, uint16_t payload) {

	uint8_t		payloadByte[2], commandByte[1];
	WarpStatus	status;

	i2c_device_t slave =
	{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = kWarpDefaultI2cBaudRateKbps
	};

	commandByte[0] = registerPointer;
	payloadByte[0] = (payload >> 8) & 0xFF;
	payloadByte[1] = payload & 0xFF;
	warpEnableI2Cpins();

	status = I2C_DRV_MasterSendDataBlocking(
		0 /* I2C instance */,
		&slave,
		commandByte,
		1,
		payloadByte,
		2,
		gWarpI2cTimeoutMilliseconds);
	
	if (status != kStatus_I2C_Success)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}





uint32_t initINA219(const uint8_t i2cAddress) {
	deviceINA219State.i2cAddress	= i2cAddress;
	deviceINA219State.operatingVoltageMillivolts	= 1800;
	warpPrint("%d\n", deviceINA219State.i2cAddress);

	warpEnableI2Cpins();

	// Adafruit documentation shows typical current draw for SSD1331 OLED board as 25mA.

	// Therefore
	// VBUS_MAX = 16V
	// VSHUNT_MAX = 0.04
	// RSHUNT = 0.1

	// Determine maximum possible current
	// MaxPossible_I = VSHUNT_MAX / RSHUNT = 0.40A

	// Determine possible range of LSBs
	// MaxExpected_I = 0.25A

	// MinimumLSB = MaxExpected_I/32767
	// MinimumLSB = 0.0000076 (7.6uA per bit)
	// MaximumLSB = MaxExpected_I/4096
	// MaximumLSB = 0,0000610 (61uA per bit)

	// Choose an appropraite LSB
	// CurrentLSB = 0.00001 (10uA per bit)
	
	INA219_currentMultiplier_uA = 10;

	// Compute the calibration register
	// Cal = trunc (0.04096 / (CurrentLSB * RSHUNT)) = 40960 = 0xA000

	INA219_calibrationValue = 40960;

	// Calculate the power LSB
	// PowerLSB = 20 * CurrentLSB = 0.0002 (0.2mW per bit)
	
	INA219_powerMultiplier_uW = 200;

	// Compute maximum current and shunt voltage values before overflow
	// Max_Current = CurrentLSB * 32767 = 0.32767A before overflow
	// Max_Current_Before_Overflow = Max_Current = 0.32767A

  	// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT = 0.032767V
	// Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage = 0.032767V

	// Compute the maximum power
	// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX = 5.24272W

	uint32_t status = 0;
	status += writeRegisterINA219(INA219_REG_CALIBRATION, INA219_calibrationValue);
	warpPrint("cal write: %d\n", status);
	
	  // Set configuration register to take into account these settings
 	 uint16_t config =  INA219_CONFIG_BVOLTAGERANGE_16V |
						INA219_CONFIG_GAIN_1_40MV | 
						INA219_CONFIG_BADCRES_12BIT |
						INA219_CONFIG_SADCRES_12BIT_1S_532US |
						INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

	status += writeRegisterINA219(INA219_REG_CONFIG, config);
	warpPrint("reg write: %d\n", status);

	OSA_TimeDelay(100);
	status += readRegisterINA219(INA219_REG_CALIBRATION, 2);
	warpPrint("read calib: %d\n", status);
	status += readRegisterINA219(INA219_REG_CONFIG, 2);
	warpPrint("read config: %d\n", status);

	return status;
}




uint16_t getBusVoltageRawINA219(void) {
	uint16_t read = readRegisterINA219(INA219_REG_BUSVOLTAGE, 2);

	// Swap byte order
	uint16_t value = deviceINA219State.i2cBuffer[1] | (deviceINA219State.i2cBuffer[0] << 8);
  
  	// Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  	return (int16_t)((value >> 3) * 4);
}

uint16_t getShuntVoltageRawINA219(void) {
	uint16_t read = readRegisterINA219(INA219_REG_SHUNTVOLTAGE, 2);
	
	// Swap byte order
	uint16_t value = deviceINA219State.i2cBuffer[1] | (deviceINA219State.i2cBuffer[0] << 8);

	return value;
}

uint16_t getCurrentRawINA219(void) {
	uint16_t read = readRegisterINA219(INA219_REG_CURRENT, 2);

	// Swap byte order
	uint16_t value = deviceINA219State.i2cBuffer[1] | (deviceINA219State.i2cBuffer[0] << 8);

	return value;
}

uint16_t getPowerRawINA219(void) {
	uint16_t read = readRegisterINA219(INA219_REG_POWER, 2);
	
	// Swap byte order
	uint16_t value = deviceINA219State.i2cBuffer[1] | (deviceINA219State.i2cBuffer[0] << 8);

	return value;
}




uint32_t getBusVoltage_mV_INA219(void) {
	uint16_t value = getBusVoltageRawINA219();
	return value;
}

uint32_t getShuntVoltage_mV_INA219(void) {
	uint16_t value = getShuntVoltageRawINA219();
	return (value * 10) / 1000;
}

uint32_t getCurrent_uA_INA219(void) {
	uint16_t value = getCurrentRawINA219();
	value *= INA219_currentMultiplier_uA;
	return value;
}

uint32_t getPower_uW_INA219(void) {
	uint16_t value = getPowerRawINA219();
	value += INA219_powerMultiplier_uW;
	return value;
}




uint32_t getCurrent_mA_INA219(void) {
	return getCurrent_uA_INA219() / 1000;
}

uint32_t getPower_mW_INA219(void) {
	return getPower_uW_INA219() / 1000;
}

void printCurrent_mA_INA219(void) {
	warpPrint("%d mA\n", getCurrent_mA_INA219());
}

void printPower_mW_INA219(void) {
	warpPrint("%d mW\n", getPower_mW_INA219());
}




// Obtain X measurements of the current reading
void getXCurrents(int x, int delay) {

	for (int i=0; i<x; i++) {
		warpPrint("%d: Current is %d uA; Power is %d mW; Bus voltage is %d mV; Shunt voltage is %d mV.\n", 
		i,
		getCurrent_uA_INA219(),
		getPower_uW_INA219(),
		getBusVoltage_mV_INA219(),
		getShuntVoltage_mV_INA219()
		);
		OSA_TimeDelay(delay);
	}
}

// Obtain currents in CSV format with some given delay
void getXCurrentsCSV(int x, int delay) {
	warpPrint("Measurement #, Current (uA), Power (uW), Bus voltage (mV), Shunt voltage (mV) \n");
	for (int i=0; i<x; i++) {
		warpPrint("%d, %d, %d, %d, %d\n", 
		i,
		getCurrent_uA_INA219(),
		getPower_uW_INA219(),
		getBusVoltage_mV_INA219(),
		getShuntVoltage_mV_INA219()
		);
		OSA_TimeDelay(delay);
	}
}

// Compute averages, also possible in hardware in INA219
void avgXReadings(int x, int m) {
	uint32_t currentBuf[x];
	uint32_t currentSum = 0;
	uint32_t powerBuf[x];
	uint32_t powerSum = 0;

	for (int i=0; i<x; i++) {
		currentBuf[x] = getCurrent_uA_INA219();
		currentSum += currentBuf[x];

		powerBuf[x] = getPower_uW_INA219();
		powerSum += powerBuf[x];
	}

	uint32_t currentAvg = currentSum / x;
	uint32_t powerAvg = powerSum / x;


	warpPrint("%d, %d, %d\n", 
	m,
	currentAvg,
	powerAvg
	);
}

void callXAvgs(void) {
	for (int i=0; i<1000; i++) {
		avgXReadings(10, i);
		OSA_TimeDelay(10);
	}
}

