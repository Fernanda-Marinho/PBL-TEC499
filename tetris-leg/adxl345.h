#ifndef ACCELEROMETER_ADXL345_H_
#define ACCELEROMETER_ADXL345_H_

#include <stdint.h>
#include <stdbool.h> 

/* Expressed as output data rate and correspond to how 
frequently the accelerometer can provide updated readings*/
#define XL345_RATE_3200       0x0F
#define XL345_RATE_1600       0x0E
#define XL345_RATE_800        0x0D
#define XL345_RATE_400        0x0C
#define XL345_RATE_200        0x0B
#define XL345_RATE_100        0x0A
#define XL345_RATE_50         0x09
#define XL345_RATE_25         0x08
#define XL345_RATE_12_5       0x07
#define XL345_RATE_6_25       0x06
#define XL345_RATE_3_125      0x05
#define XL345_RATE_1_563      0x04
#define XL345_RATE__782       0x03
#define XL345_RATE__39        0x02
#define XL345_RATE__195       0x01
#define XL345_RATE__098       0x00

/* Defines the measurement range of the accelerometer, data 
justification, resolution, interrupt mode, SPI mode, and 
self-test configuration. */
#define XL345_RANGE_2G             0x00
#define XL345_RANGE_4G             0x01
#define XL345_RANGE_8G             0x02
#define XL345_RANGE_16G            0x03
#define XL345_DATA_JUST_RIGHT      0x00
#define XL345_DATA_JUST_LEFT       0x04
#define XL345_10BIT                0x00
#define XL345_FULL_RESOLUTION      0x08
#define XL345_INT_LOW              0x20
#define XL345_INT_HIGH             0x00
#define XL345_SPI3WIRE             0x40
#define XL345_SPI4WIRE             0x00
#define XL345_SELFTEST             0x80

/* Defines specific events or conditions used in interrupt 
handling routines or polling, including an indication that 
new data is ready to be read from the accelerometer. */
#define XL345_OVERRUN              0x01
#define XL345_WATERMARK            0x02
#define XL345_FREEFALL             0x04
#define XL345_INACTIVITY           0x08
#define XL345_ACTIVITY             0x10
#define XL345_DOUBLETAP            0x20
#define XL345_SINGLETAP            0x40
#define XL345_DATAREADY            0x80

/* Defines the power management, wake-up frequency, and activity/
inactivity detection modes of the accelerometer. Helps balance the 
trade-off between power consumption and responsiveness.   */
#define XL345_WAKEUP_8HZ           0x00
#define XL345_WAKEUP_4HZ           0x01
#define XL345_WAKEUP_2HZ           0x02
#define XL345_WAKEUP_1HZ           0x03
#define XL345_SLEEP                0x04
#define XL345_MEASURE              0x08
#define XL345_STANDBY              0x00
#define XL345_AUTO_SLEEP           0x10
#define XL345_ACT_INACT_SERIAL     0x20
#define XL345_ACT_INACT_CONCURRENT 0x00

/* ADXL345 Register List  */
#define ADXL345_REG_DEVID       	0x00
#define ADXL345_REG_POWER_CTL   	0x2D
#define ADXL345_REG_DATA_FORMAT 	0x31
#define ADXL345_REG_BW_RATE     	0x2C
#define ADXL345_REG_INT_ENABLE  	0x2E  // default value: 0x00
#define ADXL345_REG_INT_MAP     	0x2F  // default value: 0x00
#define ADXL345_REG_INT_SOURCE  	0x30  // default value: 0x02
#define ADXL345_REG_DATAX0      	0x32  // read only
#define ADXL345_REG_DATAX1      	0x33  // read only
#define ADXL345_REG_DATAY0      	0x34  // read only
#define ADXL345_REG_DATAY1      	0x35  // read only
#define ADXL345_REG_DATAZ0      	0x36  // read only
#define ADXL345_REG_DATAZ1      	0x37  // read only
#define ADXL345_REG_OFSX        	0x1E
#define ADXL345_REG_OFSY        	0x1F
#define ADXL345_REG_OFSZ        	0x20
#define ADXL345_REG_THRESH_ACT		0x24
#define ADXL345_REG_THRESH_INACT	0x25
#define ADXL345_REG_TIME_INACT		0x26
#define ADXL345_REG_ACT_INACT_CTL	0x27

/* I2C0 Controller Registers */
#define I2C0_CON                ((volatile unsigned int *) 0xFFC04000)
#define I2C0_TAR                ((volatile unsigned int *) 0xFFC04004)
#define I2C0_DATA_CMD           ((volatile unsigned int *) 0xFFC04010)
#define I2C0_SS_SCL_HCNT        ((volatile unsigned int *) 0xFFC04014)
#define I2C0_SS_SCL_LCNT        ((volatile unsigned int *) 0xFFC04028)
#define I2C0_FS_SCL_HCNT        ((volatile unsigned int *) 0xFFC0401C)
#define I2C0_FS_SCL_LCNT        ((volatile unsigned int *) 0xFFC04020)
#define I2C0_INTR_MASK          ((volatile unsigned int *) 0xFFC04030)
#define I2C0_CLR_INTR           ((volatile unsigned int *) 0xFFC04040)
#define I2C0_TX_ABRT            ((volatile unsigned int *) 0xFFC04054)
#define I2C0_ENABLE             ((volatile unsigned int *) 0xFFC0406C)
#define I2C0_STATUS             ((volatile unsigned int *) 0xFFC04070)
#define I2C0_TXFLR              ((volatile unsigned int *) 0xFFC04074)
#define I2C0_RXFLR              ((volatile unsigned int *) 0xFFC04078)
#define I2C0_TX_ABRT_SOURCE     ((volatile unsigned int *) 0xFFC04080)
#define I2C0_ENABLE_STATUS      ((volatile unsigned int *) 0xFFC0409C)

/* SYSMGR Pin Muxing Registers */
#define SYSMGR_GENERALIO7       ((volatile unsigned int *) 0xFFD0849C)   //trace_d6 in address_map_arm.h
#define SYSMGR_GENERALIO8       ((volatile unsigned int *) 0xFFD084A0)   //trace_d7 in address_map_arm.h
#define SYSMGR_GPLMUX55         ((volatile unsigned int *) 0xFFD086B0)
#define SYSMGR_GPLMUX56         ((volatile unsigned int *) 0xFFD086B4)
#define SYSMGR_I2C0USEFPGA      ((volatile unsigned int *) 0xFFD08704)   //0: HPS Pins and 1: FPGA Interface
     
/* Reset Manager Registers  */
#define RSTMGR_BRGMODRST        ((volatile unsigned int *) 0xFFD0501C)
     
/* Security settings registers for peripherals  */
#define L3REGS_L4SP             ((volatile unsigned int *) 0xFF80000C)
#define L3REGS_L4OSC1           ((volatile unsigned int *) 0xFF800014)
     
/* Rounded division macro  */
#define ROUNDED_DIVISION(n, d) (((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d))

/* ADXL345 Functions */
void ADXL345_Init();
void ADXL345_Calibrate();
bool ADXL345_IsDataReady();
bool ADXL345_WasActivityUpdated();
void ADXL345_XYZ_Read(int16_t szData16[3]);
void ADXL345_IdRead(uint8_t *pId);
void ADXL345_REG_READ(uint8_t address, uint8_t *value);
void ADXL345_REG_WRITE(uint8_t address, uint8_t value);
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len);
uint8_t ADXL345_ConfigureToGame(void); 

/* I2C0 Functions  */
void I2C0_Init();
void I2C0_Enable_FPGA_Access();

/* Pinmux Functions */
void Pinmux_Config();

/* Logical Functions */  
int movement(int); 


#endif 
