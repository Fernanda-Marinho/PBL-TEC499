#include "ADXL345.h"
#include "address_map_arm.h"
#include "physical.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h> 
#include <fcntl.h>

static unsigned int * i2c0_base_ptr, * sysmgr_base_ptr;
static void * i2c0base_virtual, * sysmgrbase_virtual;
static int fd_i2c0base = -1, fd_sysmgr = -1;

//recebe x e retorna o movimento (centro, direita, esquerda)
int moviment(x){
    int16_t right_moviment = 90;  
    int16_t left_moviment = -90;
    int16_t m; 
    if (x > right_moviment) {
        m = 1; //1 = direita 
    } else if (x < left_moviment) {
        m = 2; //2 = esquerda
    } else {
        m = 0; 
    }
    return m; 
}

/* void teste(void){
    printf("aaaa\n"); 
} */

uint8_t configureAccel(void){

    uint8_t devid;
    int16_t mg_per_lsb = 4;
    int16_t XYZ[3];
    int16_t direction; 

    //Configure MUX to connect I2C0 controller to ADXL345
	if ((fd_sysmgr = open_physical(fd_sysmgr)) == -1) {
		return(-1);
	}

	if ((sysmgrbase_virtual = map_physical(fd_sysmgr, SYSMGR_BASE, SYSMGR_SPAN)) == NULL) {
		return(-1);
	}

	sysmgr_base_ptr = (unsigned int *) sysmgrbase_virtual;

    if ((fd_i2c0base = open_physical(fd_i2c0base)) == -1) {
		return(-1);
	}

	if ((i2c0base_virtual = map_physical(fd_i2c0base, I2C0_BASE, I2C0_SPAN)) == NULL) {
		return(-1);
	}

	i2c0_base_ptr = (unsigned int *) i2c0base_virtual;
	printf("Starting I2C0_Init()\n");

	I2C0_Init();
	printf("Getting ID\n");

	ADXL345_IdRead(&devid);
	//printf("%#x\n", devid);
    
    // Configure Pin Muxing
    //Pinmux_Config();
    
    // Initialize I2C0 Controller
    I2C0_Init();
    
    // 0xE5 is read from DEVID(0x00) if I2C is functioning correctly
    ADXL345_REG_READ(0x00, &devid);

    return devid; 
    
    // Correct Device ID
    /* if (devid == 0xE5){
        // Initialize accelerometer chip
        ADXL345_Init();
        
        while(1){
            if (ADXL345_WasActivityUpdated()){
                ADXL345_XYZ_Read(XYZ);
                direction = moviment((XYZ[0]*mg_per_lsb)); 
                printf("%d\n",direction); 
                //printf("X=%d mg, Y=%d mg, Z=%d mg\n", XYZ[0]*mg_per_lsb, XYZ[1]*mg_per_lsb, XYZ[2]*mg_per_lsb);
            }
        }
    } else {
        printf("Incorrect device ID\n");
    } */

}


void Pinmux_Config(){
    // Configurar pin muxing (no sysmgr) para conectar os fios do ADXL345 ao I2C0
    *(sysmgr_base_ptr + SYSMGR_I2C0USEFPGA) = 0;
    *(sysmgr_base_ptr + SYSMGR_GENERALIO7) = 1;
    *(sysmgr_base_ptr + SYSMGR_GENERALIO8) = 1;
}

// Inicializa o controlador I2C0 para uso com o chip ADXL345
void I2C0_Init(){

    // Abort any ongoing transmits and disable I2C0.
    *(i2c0_base_ptr + I2C0_ENABLE) = 2;
    
    // Esperar até que o I2C0 esteja desabilitado
    while(((*(i2c0_base_ptr + I2C0_ENABLE_STATUS))&0x1) == 1){}
    
    // Configurar o registrador de configuração com as configurações desejadas (atuar como
    // mestre, usar endereçamento de 7 bits, modo rápido (400kb/s)).
    *(i2c0_base_ptr + I2C0_CON) = 0x65;
    
    // Definir endereço alvo (desabilitar comandos especiais, usar endereçamento de 7 bits)
    *(i2c0_base_ptr + I2C0_TAR) = 0x53;
    
    // Definir contagens altas/baixas do SCL (Assumindo clock de entrada de 100MHz no Controlador I2C0).
    // O período mínimo alto do SCL é 0.6us, e o período mínimo baixo do SCL é 1.3us,
    // No entanto, o período combinado deve ser de 2.5us ou maior, então adiciona-se 0.3us a cada.
    *(i2c0_base_ptr + I2C0_FS_SCL_HCNT) = 60 + 30; // 0.6us + 0.3us
    *(i2c0_base_ptr + I2C0_FS_SCL_LCNT) = 130 + 30; // 1.3us + 0.3us
    
    // Habilitar o controlador
    *(i2c0_base_ptr + I2C0_ENABLE) = 1;
    
    // Esperar até que o controlador esteja habilitado
    while(((*(i2c0_base_ptr + I2C0_ENABLE_STATUS))&0x1) == 0){}
    
}

// Função para permitir que componentes do lado FPGA (ex. processadores Nios II) possam
// acessar o controlador I2C0 através da ponte F2H. Esta função
// precisa ser chamada de um programa ARM, e para permitir que um programa Nios II
// acesse o controlador I2C0.
void I2C0_Enable_FPGA_Access(){

    // Desafastar resets da ponte FPGA
    *(RSTMGR_BRGMODRST) = 0;
    
    // Habilitar mestres não seguros para acessar o I2C0
    *(L3REGS_L4SP) = *(L3REGS_L4SP) | 0x4;
    
    // Habilitar mestres não seguros para acessar registradores de pinmuxing (no sysmgr)
    *(L3REGS_L4OSC1) = *(L3REGS_L4OSC1) | 0x10;
}

// Escrever valor no registrador interno no endereço especificado
void ADXL345_REG_WRITE(uint8_t address, uint8_t value){
    
    // Enviar endereço do registrador (+0x400 para enviar sinal START)
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;
    
    // Enviar valor
    *(i2c0_base_ptr + I2C0_DATA_CMD) = value;
    
}

// Ler valor do registrador interno no endereço especificado
void ADXL345_REG_READ(uint8_t address, uint8_t *value){

    // Enviar endereço do registrador (+0x400 para enviar sinal START)
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;
    
    // Enviar sinal de leitura
    *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;
    
    // Ler a resposta (primeiro esperar até que o buffer RX contenha dados)  
    while (*(i2c0_base_ptr + I2C0_RXFLR) == 0){}
    *value = *(i2c0_base_ptr + I2C0_DATA_CMD);
}

// Ler múltiplos registradores internos consecutivos
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len){

    // Enviar endereço do registrador (+0x400 para enviar sinal START)
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;
    
    // Enviar sinal de leitura len vezes
    int i;
    for (i=0;i<len;i++)
        *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;

    // Ler os bytes
    int nth_byte=0;
    while (len){
        if ((*(i2c0_base_ptr + I2C0_RXFLR)) > 0){
            values[nth_byte] = *(i2c0_base_ptr + I2C0_DATA_CMD);
            nth_byte++;
            len--;
        }
    }
}

// Inicializar o chip ADXL345
void ADXL345_Init(){

    // Faixa de +- 16g, resolução total
    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, XL345_RANGE_16G | XL345_FULL_RESOLUTION);
    
    // Taxa de Dados de Saída: 200Hz
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, XL345_RATE_200);

    // NOTA: O bit DATA_READY não é confiável. Ele é atualizado em uma taxa muito maior do que a Taxa de Dados
    // Use as interrupções de Atividade e Inatividade.
    //----- Habilitando interrupções -----//
    ADXL345_REG_WRITE(ADXL345_REG_THRESH_ACT, 0x04);	// limiar de atividade
    ADXL345_REG_WRITE(ADXL345_REG_THRESH_INACT, 0x02);	// limiar de inatividade
    ADXL345_REG_WRITE(ADXL345_REG_TIME_INACT, 0x02);	// tempo para inatividade
    ADXL345_REG_WRITE(ADXL345_REG_ACT_INACT_CTL, 0xFF);	// Habilita acoplamento AC para limiares
    ADXL345_REG_WRITE(ADXL345_REG_INT_ENABLE, XL345_ACTIVITY | XL345_INACTIVITY );	// habilitar interrupções
    //-------------------------------//
    
    // Parar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);
    
    // Iniciar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);
}

// Calibrar o ADXL345. O DE1-SoC deve ser colocado em uma superfície plana,
// e deve permanecer estacionário durante a calibração.
void ADXL345_Calibrate(){
    
    int average_x = 0;
    int average_y = 0;
    int average_z = 0;
    int16_t XYZ[3];
    int8_t offset_x;
    int8_t offset_y;
    int8_t offset_z;
    
    // Parar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);
    
    // Obter os offsets atuais
    ADXL345_REG_READ(ADXL345_REG_OFSX, (uint8_t *)&offset_x);
    ADXL345_REG_READ(ADXL345_REG_OFSY, (uint8_t *)&offset_y);
    ADXL345_REG_READ(ADXL345_REG_OFSZ, (uint8_t *)&offset_z);
    
    // Usar taxa de 100 Hz para calibração. Salvar a taxa atual.
    uint8_t saved_bw;
    ADXL345_REG_READ(ADXL345_REG_BW_RATE, &saved_bw);
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, XL345_RATE_100);
    
    // Usar faixa de 16g, resolução total. Salvar o formato atual.
    uint8_t saved_dataformat;
    ADXL345_REG_READ(ADXL345_REG_DATA_FORMAT, &saved_dataformat);
    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, XL345_RANGE_16G | XL345_FULL_RESOLUTION);
    
    // Iniciar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);
    
    // Obter as médias das acelerações x, y, z sobre 32 amostras (LSB 3.9 mg)
    int i = 0;
    while (i < 32){
		// Nota: use DATA_READY aqui, não pode usar ATIVIDADE porque a placa está estacionária.
        if (ADXL345_IsDataReady()){
            ADXL345_XYZ_Read(XYZ);
            average_x += XYZ[0];
            average_y += XYZ[1];
            average_z += XYZ[2];
            i++;
        }
    }
    average_x = ROUNDED_DIVISION(average_x, 32);
    average_y = ROUNDED_DIVISION(average_y, 32);
    average_z = ROUNDED_DIVISION(average_z, 32);
    
    // Parar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_STANDBY);
    
    // printf("Average X=%d, Y=%d, Z=%d\n", average_x, average_y, average_z);
    
    // Calcular os offsets (LSB 15.6 mg)
    offset_x += ROUNDED_DIVISION(0-average_x, 4);
    offset_y += ROUNDED_DIVISION(0-average_y, 4);
    offset_z += ROUNDED_DIVISION(256-average_z, 4);
    
    // printf("Calibration: offset_x: %d, offset_y: %d, offset_z: %d (LSB: 15.6 mg)\n",offset_x,offset_y,offset_z);
    
    // Definir os registradores de offset
    ADXL345_REG_WRITE(ADXL345_REG_OFSX, offset_x);
    ADXL345_REG_WRITE(ADXL345_REG_OFSY, offset_y);
    ADXL345_REG_WRITE(ADXL345_REG_OFSZ, offset_z);
    
    // Restaurar a taxa de largura de banda original
    ADXL345_REG_WRITE(ADXL345_REG_BW_RATE, saved_bw);
    
    // Restaurar o formato de dados original
    ADXL345_REG_WRITE(ADXL345_REG_DATA_FORMAT, saved_dataformat);
    
    // Iniciar medição
    ADXL345_REG_WRITE(ADXL345_REG_POWER_CTL, XL345_MEASURE);
}

// Retorna verdadeiro se houve atividade desde a última leitura (verifica o bit ATIVIDADE).
bool ADXL345_WasActivityUpdated(){
	bool bReady = false;
    uint8_t data8;
    
    ADXL345_REG_READ(ADXL345_REG_INT_SOURCE,&data8);
    if (data8 & XL345_ACTIVITY)
        bReady = true;
    
    return bReady;
}

// Retorna verdadeiro se há novos dados (verifica o bit DATA_READY).
bool ADXL345_IsDataReady(){
    bool bReady = false;
    uint8_t data8;
    
    ADXL345_REG_READ(ADXL345_REG_INT_SOURCE,&data8);
    if (data8 & XL345_DATAREADY)
        bReady = true;
    
    return bReady;
}

// Ler dados de aceleração dos três eixos
void ADXL345_XYZ_Read(int16_t szData16[3]){
    uint8_t szData8[6];
    ADXL345_REG_MULTI_READ(0x32, (uint8_t *)&szData8, sizeof(szData8));

    szData16[0] = (szData8[1] << 8) | szData8[0]; 
    szData16[1] = (szData8[3] << 8) | szData8[2];
    szData16[2] = (szData8[5] << 8) | szData8[4];
}

// Ler o registrador de ID
void ADXL345_IdRead(uint8_t *pId){
    ADXL345_REG_READ(ADXL345_REG_DEVID, pId);
}


// Abrir /dev/mem, se ainda não estiver aberto, para dar acesso a endereços físicos
int open_physical (int fd)
{
	if (fd == -1)
		if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1)
		{
			printf ("ERRO: não foi possível abrir \"/dev/mem\"...\n");
			return (-1);
		}
	return fd;
}

// Fechar /dev/mem para dar acesso a endereços físicos
void close_physical (int fd)
{
	close (fd);
}

/*
 * Estabelecer um mapeamento de endereço virtual para os endereços físicos começando em base, e
 * estendendo-se por span bytes.
 */
void* map_physical(int fd, unsigned int base, unsigned int span)
{
	void *virtual_base;

	// Obter um mapeamento dos endereços físicos para endereços virtuais
	virtual_base = mmap (NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
	if (virtual_base == MAP_FAILED)
	{
		printf ("ERRO: mmap() falhou...\n");
		close (fd);
		return (NULL);
	}
	return virtual_base;
}

/*
 * Fechar o mapeamento de endereço virtual previamente aberto
 */
int unmap_physical(void * virtual_base, unsigned int span)
{
	if (munmap (virtual_base, span) != 0)
	{
		printf ("ERRO: munmap() falhou...\n");
		return (-1);
	}
	return 0;
}