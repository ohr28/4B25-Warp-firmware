uint32_t readRegisterINA219(uint8_t registerPointer, int numBytes);
uint32_t writeRegisterINA219(uint8_t registerPointer, uint16_t payload);
uint32_t initINA219(const uint8_t i2cAddress);
uint16_t getBusVoltageRawINA219(void);
uint16_t getShuntVoltageRawINA219(void);
uint16_t getCurrentRawINA219(void);
uint16_t getPowerRawINA219(void) ;
uint32_t getBusVoltage_mV_INA219(void);
uint32_t getShuntVoltage_mV_INA219(void);
uint32_t getCurrent_uA_INA219(void);
uint32_t getPower_uW_INA219(void);
uint32_t getCurrent_mA_INA219(void);
uint32_t getPower_mW_INA219(void);
void printCurrent_mA_INA219(void);
void printPower_mW_INA219(void);
void getXCurrents(int x, int delay);
void getXCurrentsCSV(int x, int delay);
void avgXReadings(int x, int m);
void callXAvgs(void);

/** config register address **/
#define INA219_REG_CONFIG (0x00)

/** shunt voltage register **/
#define INA219_REG_SHUNTVOLTAGE (0x01)

/** bus voltage register **/
#define INA219_REG_BUSVOLTAGE (0x02)

/** power register **/
#define INA219_REG_POWER (0x03)

/** current register **/
#define INA219_REG_CURRENT (0x04)

/** calibration register **/
#define INA219_REG_CALIBRATION (0x05)



#define INA219_CONFIG_BVOLTAGERANGE_16V (0x0000)
#define INA219_CONFIG_GAIN_1_40MV (0x0000)
#define INA219_CONFIG_BADCRES_12BIT (0x0180)
#define INA219_CONFIG_SADCRES_12BIT_1S_532US (0x0018)
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x07)