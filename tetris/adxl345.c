#include "adxl345.h"
#include "address_map_arm.h"
#include "physical.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h> 
#include <fcntl.h>

/* Manage registers, memory mapping and access to I2C0   */
static unsigned int * i2c0_base_ptr, * sysmgr_base_ptr;
static void * i2c0base_virtual, * sysmgrbase_virtual;
static int fd_i2c0base = -1, fd_sysmgr = -1;

/* Tries to open /dev/mem to give access to physical addresses  */
int open_physical (int fd){
	if (fd == -1)
		if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1){
			printf ("ERROR: could not open \"/dev/mem\"...\n");
			return (-1);
		}
	return fd;
}

/* Closes /dev/mem to stop access to physical addresses  */
void close_physical (int fd){
	close (fd);
}

/* Maps a physical address range to a virtual address space.
Receives: a file descriptor (fd), the base physical address (base), 
and the size of the memory region (span).
Returns: a pointer to the mapped virtual address, or NULL if the 
mapping fails.                                                    */
void* map_physical(int fd, unsigned int base, unsigned int span){
	void *virtual_base;

	virtual_base = mmap (NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
	if (virtual_base == MAP_FAILED){
		printf ("ERROR: mmap() failed...\n");
		close (fd);
		return (NULL);
	}

	return virtual_base;
}

/* Unmaps a previously mapped virtual address space.
Receives: a pointer to the virtual base address (virtual_base) 
and the size of the memory region (span).
Returns: 0 on success, or -1 in case of failure.             */
int unmap_physical(void * virtual_base, unsigned int span){
	if (munmap (virtual_base, span) != 0){
		printf ("ERRO: munmap() failed...\n");
		return (-1);
	}
	return 0;
}

/* Configures the pin multiplexing (in sysmgr) to connect ADXL345 wires to I2C0.  */
void Pinmux_Config(){
    *(sysmgr_base_ptr + SYSMGR_I2C0USEFPGA) = 0;
    *(sysmgr_base_ptr + SYSMGR_GENERALIO7) = 1;
    *(sysmgr_base_ptr + SYSMGR_GENERALIO8) = 1;
}

/* Initializes the I2C0 controller for communication with the ADXL345 accelerometer.  */
void I2C0_Init(){
    *(i2c0_base_ptr + I2C0_ENABLE) = 2;
    while(((*(i2c0_base_ptr + I2C0_ENABLE_STATUS))&0x1) == 1){} // wait until I2C0 be disabled 
    
    *(i2c0_base_ptr + I2C0_CON) = 0x65;
    *(i2c0_base_ptr + I2C0_TAR) = 0x53;
    
    *(i2c0_base_ptr + I2C0_FS_SCL_HCNT) = 60 + 30; // 0.6us + 0.3us
    *(i2c0_base_ptr + I2C0_FS_SCL_LCNT) = 130 + 30; // 1.3us + 0.3us
    
    *(i2c0_base_ptr + I2C0_ENABLE) = 1; // enable the controller
    while(((*(i2c0_base_ptr + I2C0_ENABLE_STATUS))&0x1) == 0){} //// wait until I2C0 be enabled
    
}

/* Enables FPGA-side components to access the I2C0 controller via.   */
void I2C0_Enable_FPGA_Access(){
    *(RSTMGR_BRGMODRST) = 0;
    *(L3REGS_L4SP) = *(L3REGS_L4SP) | 0x4;
    *(L3REGS_L4OSC1) = *(L3REGS_L4OSC1) | 0x10;
}

/* Writes a value to an internal register of the adxl345 at the specified address.
Receives the register address and the value to be written. */
void ADXL345_REG_WRITE(uint8_t address, uint8_t value){
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400; // send the register address 
    *(i2c0_base_ptr + I2C0_DATA_CMD) = value; // send the value 
}

/* Reads a value from an internal register of the adxl345 at the specified address.
Receives the register address and stores the read value in the provided pointer. */
void ADXL345_REG_READ(uint8_t address, uint8_t *value){
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400; // send the register 
    *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;  // send the read signal
     
    while (*(i2c0_base_ptr + I2C0_RXFLR) == 0){} 
    *value = *(i2c0_base_ptr + I2C0_DATA_CMD);  // read the response  
}

/* Reads multiple consecutive internal registers from the adxl345.
Receives the starting register address, an array to store the read values, 
and the number of registers to read (len). */
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len){

    int i;
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;
    for (i=0;i<len;i++)
        *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;

    int nth_byte=0;
    while (len){
        if ((*(i2c0_base_ptr + I2C0_RXFLR)) > 0){
            values[nth_byte] = *(i2c0_base_ptr + I2C0_DATA_CMD);  // store each read byte in the array
            nth_byte++;
            len--;
        }
    }
}

/* Initializes the ADXL345 accelerometer chip.   */
void ADXL345_Init(){

    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, XL345_RANGE_16G | XL345_FULL_RESOLUTION); // range to +- 16g
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, XL345_RATE_200);  // rate of 200hz

    ADXL345_REG_WRITE(ADXL345_REG_THRESH_ACT, 0x04);	
    ADXL345_REG_WRITE(ADXL345_REG_THRESH_INACT, 0x02);	
    ADXL345_REG_WRITE(ADXL345_REG_TIME_INACT, 0x02);	
    ADXL345_REG_WRITE(ADXL345_REG_ACT_INACT_CTL, 0xFF);	
    ADXL345_REG_WRITE(ADXL345_REG_INT_ENABLE, XL345_ACTIVITY | XL345_INACTIVITY );	
    
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);  // standby mode
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);  // start measurement
}

/* Calibrates the ADXL345 accelerometer.  */
void ADXL345_Calibrate(){
    int i = 0;
    int average_x = 0;
    int average_y = 0;
    int average_z = 0;

    int16_t XYZ[3];
    
    int8_t offset_x;
    int8_t offset_y;
    int8_t offset_z;
    
    uint8_t saved_bw;
    uint8_t saved_dataformat;
    
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);  // standby mode
    
    // get current offsets
    ADXL345_REG_READ(ADXL345_REG_OFSX, (uint8_t *)&offset_x);
    ADXL345_REG_READ(ADXL345_REG_OFSY, (uint8_t *)&offset_y);
    ADXL345_REG_READ(ADXL345_REG_OFSZ, (uint8_t *)&offset_z);
    
    // get current offsets
    ADXL345_REG_READ(ADXL345_REG_BW_RATE, &saved_bw);
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, XL345_RATE_100);
    
    // save current rate 
    ADXL345_REG_READ(ADXL345_REG_DATA_FORMAT, &saved_dataformat);
    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, XL345_RANGE_16G | XL345_FULL_RESOLUTION);
 
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);  // start measurement 
    
    while (i < 32){
        if (ADXL345_IsDataReady()){  
            ADXL345_XYZ_Read(XYZ);  // collect averages for x, y, z
            average_x += XYZ[0];
            average_y += XYZ[1];
            average_z += XYZ[2];
            i++;
        }
    }

    average_x = ROUNDED_DIVISION(average_x, 32);
    average_y = ROUNDED_DIVISION(average_y, 32);
    average_z = ROUNDED_DIVISION(average_z, 32);
    
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);  // standby mode
    
    // calculate offsets and set registers (LSB 15.6 mg)
    offset_x += ROUNDED_DIVISION(0-average_x, 4);
    offset_y += ROUNDED_DIVISION(0-average_y, 4);
    offset_z += ROUNDED_DIVISION(256-average_z, 4);
    ADXL345_REG_WRITE(ADXL345_REG_OFSX, offset_x);
    ADXL345_REG_WRITE(ADXL345_REG_OFSY, offset_y);
    ADXL345_REG_WRITE(ADXL345_REG_OFSZ, offset_z);
    
    // restore original bandwidth rate and data format 
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, saved_bw);
    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, saved_dataformat);
    
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);  // start measurement
}

/* Checks the XL345_ACTIVITY bit and returns true if there has been 
activity since the last reading; otherwise, returns false.*/ 
bool ADXL345_WasActivityUpdated(){
	bool bReady = false;
    uint8_t data8;
    
    ADXL345_REG_READ(ADXL345_REG_INT_SOURCE,&data8);
    if (data8 & XL345_ACTIVITY)
        bReady = true;
    
    return bReady;
}

/* Checks the XL345_DATAREADY bit and returns true if there is new data 
available; otherwise, returns false.    */
bool ADXL345_IsDataReady(){
    bool bReady = false;
    uint8_t data8;
    
    ADXL345_REG_READ(ADXL345_REG_INT_SOURCE,&data8);
    if (data8 & XL345_DATAREADY)
        bReady = true;
    
    return bReady;
}

/* Reads acceleration data from three axes.
Receives an array to store the acceleration data.     */
void ADXL345_XYZ_Read(int16_t szData16[3]){
    uint8_t szData8[6];
    ADXL345_REG_MULTI_READ(0x32, (uint8_t *)&szData8, sizeof(szData8));

    szData16[0] = (szData8[1] << 8) | szData8[0]; 
    szData16[1] = (szData8[3] << 8) | szData8[2];
    szData16[2] = (szData8[5] << 8) | szData8[4];
}

/* Reads the device ID register and stores the result in the provided pointer.
Receives a pointer where the device ID will be stored. */
void ADXL345_IdRead(uint8_t *pId){
    ADXL345_REG_READ(ADXL345_REG_DEVID, pId);
}


/* Configures the adxl345, initializes the I2C and returns the device ID. */
uint8_t ADXL345_ConfigureToGame(void){

    uint8_t devid;

    // memory mapping for system manager
	if ((fd_sysmgr = open_physical(fd_sysmgr)) == -1) {
        return(-1);
    }
    if ((sysmgrbase_virtual = map_physical(fd_sysmgr, SYSMGR_BASE, SYSMGR_SPAN)) == NULL) {
        return(-1);
    }
    sysmgr_base_ptr = (unsigned int *) sysmgrbase_virtual;

    // memory mapping for I2C0
    if ((fd_i2c0base = open_physical(fd_i2c0base)) == -1) {
        return(-1);
    }
    if ((i2c0base_virtual = map_physical(fd_i2c0base, I2C0_BASE, I2C0_SPAN)) == NULL) {
        return(-1);
    }
    i2c0_base_ptr = (unsigned int *) i2c0base_virtual;
	
    // I2C0 initialization
    printf("Starting I2C0_Init()\n");
    I2C0_Init();
	
    printf("Getting ID\n");
    ADXL345_IdRead(&devid);
    ADXL345_REG_READ(0x00, &devid);
    return devid;  
}


/* Determines the direction of movement based on the x-axis value.
Receives the x-axis and return an integer corresponding to the movement direction. */
int movement(int x){
    int16_t m; 

    // threshold values chosen 
    int16_t right_movement = 90;   
    int16_t left_movement = -90;

    if (x > right_movement) {
        m = 1; // right
    } else if (x < left_movement) {
        m = 2; // left
    } else {
        m = 0; // center
    }
    return m; 
}


