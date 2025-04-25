/**
******************************************************************************
 * @file    API_MRF24J40.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo MRF24J40
 *******************************************************************************
 * @attention Driver independiente de la plataforma de uso y del compilardor.
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "compatibility.h"
#include "API_MRF24J40.h"
#include "API_MRF24J40_port.h"

/* Definiciones de la configuración por defecto ------------------------------*/
#define	DEFAULT_CHANNEL		CH_11
#define DEFAULT_SEC_NUMBER	(0X01)
#define	MY_DEFAULT_PAN_ID	(0x1234)
#define	MY_DEFAULT_ADDRESS	(0x1111)

#define HEAD_LENGTH			(0X08)
#define WRITE_16_BITS		(0X8010)
#define READ_16_BITS		(0X8000)
#define WRITE_8_BITS		(0x01)
#define READ_8_BITS			(0x7E)
#define SHIFT_LONG_ADDR		(0X05)
#define SHIFT_SHORT_ADDR	(0X01)
#define SHIFT_BYTE			(0X08)
#define FCS_LQI_RSSI		(0x04)

/* Variables privadas --------------------------------------------------------*/
static MRF24_State_t estadoActual;
/* MAC address por defecto del dispositivo */
static const uint8_t default_mac_address[] = {0x11,
                                              0x28,
                                              0x35,
                                              0x44,
                                              0x56,
                                              0x66,
                                              0x77,
                                              0x01};

/* Security key por defecto del dispositivo */
static const uint8_t default_security_key[] = {0x00,
                                               0x10,
                                               0x25,
                                               0x37,
                                               0x04,
                                               0x55,
                                               0x06,
                                               0x79,
                                               0x08,
                                               0x09,
                                               0x10,
                                               0x11,
                                               0x12,
                                               0x13,
                                               0x14,
                                               0x15};

/* Estructura con la información del dispositivo */
static struct {	bool_t get_new_msg;
				bool_t put_new_msg;
				uint8_t sequence_number;
				uint8_t my_channel;
				uint8_t security_key[16];
				uint8_t my_mac[8];
				uint16_t my_panid;
				uint16_t my_address;
				uint16_t intervalo;
}mrf24_data_config;

/* Estructura con la información de transmisión */
static struct {	uint16_t destinity_panid;
				uint16_t destinity_address;
				uint8_t largo_mensaje;
				const char * buffer_salida;
}mrf24_data_out;

/* Estructura con la información de recepción */
static struct {	uint16_t source_panid;
				uint16_t source_address;
				uint8_t tamano_mensaje;
				uint8_t rssi;
				uint8_t buffer_entrada[50];
}mrf24_data_in;

enum {	EADR0 = 0x05,
		EADR1,
		EADR2,
		EADR3,
		EADR4,
		EADR5,
		EADR6,
		EADR7
} long_mac_address;

/* Direcciones de los registros de 8 bits ------------------------------------*/
#define	RXMCR			(0x00)
#define	PANIDL			(0x01)
#define	PANIDH			(0x02)
#define	SADRL			(0x03)
#define	SADRH			(0x04)
#define	RXFLUSH			(0x0D)
#define	ORDER			(0x10)
#define	TXMCR			(0x11)
#define	ACKTMOUT		(0x12)
#define	ESLOTG1			(0x13)
#define	SYMTICKL		(0x14)
#define	SYMTICKH		(0x15)
#define	PACON0			(0x16)
#define	PACON1			(0x17)
#define	PACON2			(0x18)
#define	TXBCON0			(0x1A)
#define	TXNCON			(0x1B)
#define	TXG1CON			(0x1C)
#define	TXG2CON			(0x1D)
#define	ESLOTG23		(0x1E)
#define	ESLOTG45		(0x1F)
#define	ESLOTG67		(0x20)
#define	TXPEND			(0x21)
#define	WAKECON			(0x22)
#define	FRMOFFSET		(0x23)
#define	TXSTAT			(0x24)
#define	TXBCON1			(0x25)
#define	GATECLK			(0x26)
#define	TXTIME			(0x27)
#define	HSYMTMRL		(0x28)
#define	HSYMTMRH		(0x29)
#define	SOFTRST			(0x2A)
#define	SECCON0			(0x2C)
#define	SECCON1			(0x2D)
#define	TXSTBL			(0x2E)
#define	RXSR			(0x30)
#define	INTSTAT			(0x31)
#define MRFINTCON		(0x32)
#define	GPIO			(0x33)
#define	TRISGPIO		(0x34)
#define	SLPACK			(0x35)
#define	RFCTL			(0x36)
#define	SECCR2			(0x37)
#define	BBREG0			(0x38)
#define	BBREG1			(0x39)
#define	BBREG2			(0x3A)
#define	BBREG3			(0x3B)
#define	BBREG4			(0x3C)
#define	BBREG6			(0x3E)
#define	CCAEDTH			(0x3F)

/* Direcciones de los registros de 16 bits -----------------------------------*/
#define	RFCON0			(0x200)
#define	RFCON1			(0x201)
#define	RFCON2			(0x202)
#define	RFCON3			(0x203)
#define	RFCON5			(0x205)
#define	RFCON6			(0x206)
#define	RFCON7			(0x207)
#define	RFCON8			(0x208)
#define	SLPCAL0			(0x209)
#define	SLPCAL1			(0x20a)
#define	SLPCAL2			(0x20b)
#define	RFSTATE			(0x20f)
#define	RSSI			(0x210)
#define	SLPCON0			(0x211)
#define	SLPCON1			(0x220)
#define	WAKETIMEL		(0x222)
#define	WAKETIMEH		(0x223)
#define	REMCNTL			(0x224)
#define	REMCNTH			(0x225)
#define	MAINCNT0		(0x226)
#define	MAINCNT1		(0x227)
#define	MAINCNT2		(0x228)
#define	MAINCNT3		(0x229)
#define	TESTMODE		(0x22f)
#define	ASSOEADR0		(0x230)
#define	ASSOEADR1		(0x231)
#define	ASSOEADR2		(0x232)
#define	ASSOEADR3		(0x233)
#define	ASSOEADR4		(0x234)
#define	ASSOEADR5		(0x235)
#define	ASSOEADR6		(0x236)
#define	ASSOEADR7		(0x237)
#define	ASSOSADR0		(0x238)
#define	ASSOSADR1		(0x239)

/* Direcciones de los registros de las FIFOs ---------------------------------*/
#define	TX_NORMAL_FIFO	(0x000)
#define	TX_BEACON_FIFO	(0x080)
#define	TX_GTS1_FIFO	(0x100)
#define	TX_GTS2_FIFO	(0x180)
#define	RX_FIFO			(0x300)
#define	TX_SEC_KEY		(0x280)
#define RX_SEC_KEY		(0x2B0)

/* Definiciones del registro RXMCR -------------------------------------------*/
#define	NOACKRSP		(0X20)
#define	PANCOORD		(0X08)
#define	COORD			(0X04)
#define	ERRPKT			(0X02)
#define	PROMI			(0X01)

/* Definiciones del registro RXFLUSH -----------------------------------------*/
#define WAKEPOL_HIGH	(0X40)
#define WAKEPAD_EN		(0X20)
#define CMDONLY			(0X08)
#define DATAONLY		(0X04)
#define BCNONLY			(0X02)
#define RXFLUSH_RESET	(0X01)

/* Definiciones del registro TXMCR -------------------------------------------*/
#define	NOCSMA			(0X80)
#define	BATLIFEXT		(0X40)
#define	SLOTTED			(0X20)
#define	MACMINBE1		(0X10)
#define	MACMINBE0		(0X08)
#define	CSMABF2			(0X04)
#define	CSMABF1			(0X02)
#define	CSMABF0			(0X01)

/* Definiciones del registro ACKTMOUT -----------------------------------------*/
#define	DRPACK			(0X80)
#define	MAWD6			(0X40)
#define	MAWD5			(0X20)
#define	MAWD4			(0X10)
#define	MAWD3			(0X08)
#define	MAWD2			(0X04)
#define	MAWD1			(0X02)
#define	MAWD0			(0X01)

/* Definiciones del registro TXBCON0 -----------------------------------------*/
#define	TXBSECEN		(0X02)
#define	TXBTRIG			(0X01)

/* Definiciones del registro TXNCON ------------------------------------------*/
#define FPSTAT			(0X10)
#define INDIRECT		(0X08)
#define TXNACKREQ		(0X04)
#define TXNSECEN		(0X02)
#define TXNTRIG			(0X01)

/* Definiciones del registro WAKECON -----------------------------------------*/
#define IMMWAKE			(0X80)
#define REGWAKE			(0X40)

/* Definiciones del registro TXBCON1 -----------------------------------------*/
#define TXBMSK			(0X80)
#define WU_BCN			(0X40)
#define RSSI_8_SYM		(0X30)
#define RSSI_4_SYM		(0X20)
#define RSSI_2_SYM		(0X10)
#define RSSI_1_SYM		(0X00)

/* Definiciones del registro GATECLK -----------------------------------------*/
#define GTSON			(0X08)

/* Definiciones del registro SOFTRST -----------------------------------------*/
#define RSTPWR			(0X04)
#define RSTBB			(0X02)
#define RSTMAC			(0X01)

/* Definiciones del registro SECCON0 -----------------------------------------*/
#define SECIGNORE		(0X80)
#define SECSTART		(0X40)
#define RX_AES_CBC_32	(0X38)
#define RX_AES_CBC_64	(0X30)
#define RX_AES_CBC_128	(0X28)
#define RX_AES_CCM_32	(0X20)
#define RX_AES_CCM_64	(0X18)
#define RX_AES_CCM_128	(0X10)
#define RX_AES_CTR		(0X08)
#define TX_AES_CBC_32	(0X07)
#define TX_AES_CBC_64	(0X06)
#define TX_AES_CBC_128	(0X05)
#define TX_AES_CCM_32	(0X04)
#define TX_AES_CCM_64	(0X03)
#define TX_AES_CCM_128	(0X02)
#define TX_AES_CTR		(0X01)

/* Definiciones del registro SECCON1 -----------------------------------------*/
#define TXB_AES_CBC_32	(0X70)
#define TXB_AES_CBC_64	(0X60)
#define TXB_AES_CBC_128	(0X50)
#define TXB_AES_CCM_32	(0X40)
#define TXB_AES_CCM_64	(0X30)
#define TXB_AES_CCM_128	(0X20)
#define TXB_AES_CTR		(0X10)

/* Definiciones del registro PACON2 ------------------------------------------*/
#define	FIFOEN			(0X80)
#define	TXONTS3			(0X20)
#define	TXONTS2			(0X10)
#define	TXONTS1			(0X08)
#define	TXONTS0			(0X04)
#define	TXONT8			(0X02)
#define	TXONT7			(0X01)

/* Definiciones del registro TXSTBL ------------------------------------------*/
#define	RFSTBL3			(0X80)
#define	RFSTBL2			(0X40)
#define	RFSTBL1			(0X20)
#define	RFSTBL0			(0X10)
#define	MSIFS3			(0X08)
#define	MSIFS2			(0X04)
#define	MSIFS1			(0X02)
#define	MSIFS0			(0X01)

/* Definiciones del registro RXSR --------------------------------------------*/
#define	UPSECERR		(0X40)
#define	BATIND			(0X20)
#define	SECDECERR		(0X04)

/* Definiciones del registro INTSTAT -----------------------------------------*/
#define SLPIF			(0X80)
#define WAKEIF			(0X40)
#define HSYMTMRIF		(0X20)
#define SECIF			(0X10)
#define RXIF			(0X08)
#define TXG2IF			(0X04)
#define TXG1IF			(0X02)
#define TXNIF			(0X01)

/* Definiciones del registro INTCON ------------------------------------------*/
#define SLPIE_DIS		(0X80)
#define WAKEIE_DIS		(0X40)
#define HSYMTMRIE_DIS	(0X20)
#define SECIE_DIS		(0X10)
#define RXIE_DIS		(0X08)
#define TXG2IE_DIS		(0X04)
#define TXG1IE_DIS		(0X02)
#define TXNIE_DIS		(0X01)

/* Definiciones del registro RFCTL -------------------------------------------*/
#define	WAKECNT8		(0X10)
#define	WAKECNT7		(0X08)
#define	RFRST_HOLD		(0X04)
#define	RFTXMODE		(0X02)
#define	RFRXMODE		(0X01)

/* Definiciones del registro BBREG0 ------------------------------------------*/
#define	TURBO			(0X01)

/* Definiciones del registro BBREG1 ------------------------------------------*/
#define	RXDECINV		(0X04)

/* Definiciones del registro BBREG2 ------------------------------------------*/
#define	CCA_MODE_3		(0XC0)
#define	CCA_MODE_1		(0X80)
#define	CCA_MODE_2		(0X40)
#define	CCACSTH3		(0X20)
#define	CCACSTH2		(0X10)
#define	CCACSTH1		(0X08)
#define	CCACSTH0		(0X04)

/* Definiciones del registro BBREG3 ------------------------------------------*/
#define	IEEE_802_15_4	(0XD0)
#define	TURBO_MODE		(0X30)
#define	PREDETTH2		(0X08)
#define	PREDETTH1		(0X04)
#define	PREDETTH0		(0X02)

/* Definiciones del registro BBREG4 ------------------------------------------*/
#define	IEEE_250KBPS	(0X80)
#define	TURBO_625KBPS	(0X40)
#define	PRECNT2			(0X10)
#define	PRECNT1			(0X08)
#define	PRECNT0			(0X04)

/* Definiciones del registro BBREG6 ------------------------------------------*/
#define	RSSIMODE1		(0X80)
#define	RSSIMODE2		(0X40)

/* Definiciones del registro CCAEDTH -----------------------------------------*/
#define	CCAEDTH7		(0X80)
#define	CCAEDTH6		(0X40)
#define	CCAEDTH5		(0X20)
#define	CCAEDTH4		(0X10)
#define	CCAEDTH3		(0X08)
#define	CCAEDTH2		(0X04)
#define	CCAEDTH1		(0X02)
#define	CCAEDTH0		(0X01)

/* Definiciones del registro RFCON1 ------------------------------------------*/
#define VCOOPT7			(0X80)
#define VCOOPT6			(0X40)
#define VCOOPT5			(0X20)
#define VCOOPT4			(0X10)
#define VCOOPT3			(0X08)
#define VCOOPT2			(0X04)
#define VCOOPT1			(0X02)
#define VCOOPT0			(0X01)

/* Definiciones del registro RFCON2 ------------------------------------------*/
#define PLLEN			(0X80)

/* Definiciones del registro RFCON3 ------------------------------------------*/
#define	P30dBm			(0XC0)
#define	P20dBm			(0X80)
#define	P10dBm			(0X40)
#define	P6_3dBm			(0X38)
#define	P4_9dBm			(0X30)
#define	P3_7dBm			(0X28)
#define	P2_8dBm			(0X20)
#define	P1_9dBm			(0X18)
#define	P1_2dBm			(0X10)
#define	P0_5dBm			(0X08)
#define	P0dBm			(0X00)

/* Definiciones del registro RFCON5 ------------------------------------------*/
#define	BATTH_3_5		(0XE0)
#define	BATTH_3_3		(0XD0)
#define	BATTH_3_2		(0XC0)
#define	BATTH_3_1		(0XB0)
#define	BATTH_2_8		(0XA0)
#define	BATTH_2_7		(0X90)
#define	BATTH_2_6		(0X80)
#define	BATTH_2_5		(0X70)

/* Definiciones del registro RFCON6 ------------------------------------------*/
#define	TXFIL			(0X80)
#define	_20MRECVR		(0X10)
#define	BATEN			(0X08)

/* Definiciones del registro RFCON7 ------------------------------------------*/
#define	SLPCLK100KHZ	(0X80)
#define	SLPCLK32KHZ		(0X40)

/* Definiciones del registro RFCON8 ------------------------------------------*/
#define	RFVCO			(0X10)

/* Definiciones del registro RFSTATE -----------------------------------------*/
#define	RTSEL2			(0XE0)
#define	RTSEL1			(0XC0)
#define	RX				(0XA0)
#define	TX				(0X80)
#define	CALVCO			(0X60)
#define	RF_SLEEP		(0X40)
#define	CALFIL			(0X20)
#define	RF_RESET		(0X00)

/* Definiciones del registro SLPCON0 -----------------------------------------*/
#define	INTEDGE_RISISNG	(0X02)
#define	SLPCLKDIS		(0X01)

/* Definiciones del registro SLPCON1 -----------------------------------------*/
#define	CLKOUTDIS		(0X20)
#define	SLPCLKDIV4		(0X10)
#define	SLPCLKDIV3		(0X08)
#define	SLPCLKDIV2		(0X04)
#define	SLPCLKDIV1		(0X02)
#define	SLPCLKDIV0		(0X01)

/* Definiciones del registro TESTMODE ----------------------------------------*/
#define	RSSIWAIT1		(0X10)
#define	RSSIWAIT0		(0X08)
#define	TESTMODE2		(0X04)
#define	TESTMODE1		(0X02)
#define	TESTMODE0		(0X01)

/* LSB */
#define	DATA			(0X01)
#define	ACK				(0X02)
#define	MAC_COMM		(0X03)
#define	SECURITY		(0X08)
#define	FRAME_PEND		(0X10)
#define	ACK_REQ			(0X20)
#define	INTRA_PAN		(0X40)

/* MSB */
#define TX_CTR			(0x01)
#define TX_CCM128		(0x02)
#define TX_CCM64		(0x03)
#define TX_CCM32		(0x04)
#define TX_CBC_MAC128	(0x05)
#define TX_CBC_MAC64	(0x06)
#define TX_CBC_MAC32	(0x07)
#define	SHORT_D_ADD		(0X08)
#define	LONG_D_ADD		(0X0C)
#define	SHORT_S_ADD		(0X80)
#define	LONG_S_ADD		(0XC0)

/* Prototipo de funciones privadas -------------------------------------------*/
static void InicializoVariables(void);
static MRF24_State_t InicializoMRF24(void);
static void SetShortAddr(uint8_t reg_address, uint8_t valor);
static void SetLongAddr(uint16_t reg_address, uint8_t valor);
static uint8_t GetShortAddr(uint8_t reg_address);
static uint8_t GetLongAddr(uint16_t reg_address);
static void SetDeviceAddress(void);
static void SetChannel(void);
static void SetDeviceMACAddress(void);

/* Funciones privadas --------------------------------------------------------*/
/**
 * @brief  Inicialización de variables de configuración por defecto.
 * @param  None
 * @retval None
 */
static void InicializoVariables(void) {

	for(int i = 0; i < SEC_KEY_SIZE; i++){

		if(i < LARGE_MAC_SIZE)
			mrf24_data_config.my_mac[i] = default_mac_address[i];
		mrf24_data_config.security_key[i] = default_security_key[i];
	}
	mrf24_data_config.sequence_number = DEFAULT_SEC_NUMBER;
	mrf24_data_config.my_channel = DEFAULT_CHANNEL;
	mrf24_data_config.get_new_msg = false;
	mrf24_data_config.put_new_msg = false;
	mrf24_data_config.my_panid = MY_DEFAULT_PAN_ID;
	mrf24_data_config.my_address = MY_DEFAULT_ADDRESS;
	mrf24_data_in.source_address = VACIO;
	mrf24_data_in.source_panid = VACIO;
	mrf24_data_in.tamano_mensaje = VACIO;
	mrf24_data_in.buffer_entrada[0] = VACIO;
	mrf24_data_out.destinity_panid = VACIO;
	mrf24_data_out.destinity_address = VACIO;
	mrf24_data_out.largo_mensaje = VACIO;
	mrf24_data_out.buffer_salida = NULL;
	return;
}

/**
 * @brief  Inicialización del módulo MRF24J40MA
 * @param  None
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK)
 */
static MRF24_State_t InicializoMRF24(void) {

	uint8_t lectura;
	delayNoBloqueanteData delay_time_out;
	DelayInit(&delay_time_out, MRF_TIME_OUT);
	SetShortAddr(SOFTRST, RSTPWR | RSTBB | RSTMAC);
	DelayReset(&delay_time_out);

	do {

		lectura = GetShortAddr(SOFTRST);
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRIDO;
	}while((lectura & (RSTPWR | RSTBB | RSTMAC)) != VACIO);
	delay_t(WAIT_1_MS);
	SetShortAddr(RXFLUSH, RXFLUSH_RESET);
	SetDeviceAddress();
	SetDeviceMACAddress();
	SetLongAddr(RFCON2, PLLEN);
	SetLongAddr(RFCON3, P30dBm | P6_3dBm);
	SetLongAddr(RFCON6, TXFIL | _20MRECVR);
	SetLongAddr(RFCON7, SLPCLK100KHZ);
	SetLongAddr(RFCON8, RFVCO);
	SetLongAddr(SLPCON1, CLKOUTDIS | SLPCLKDIV0);
	SetShortAddr(BBREG2, CCA_MODE_1);
	SetShortAddr(BBREG6, RSSIMODE2);
	SetShortAddr(CCAEDTH, CCAEDTH2 | CCAEDTH1);
	SetShortAddr(PACON2, FIFOEN | TXONTS2 | TXONTS1);
	SetShortAddr(TXSTBL, RFSTBL3 | RFSTBL0 | MSIFS2 | MSIFS0);
	DelayReset(&delay_time_out);

	do {

		lectura = GetLongAddr(RFSTATE);
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRIDO;
	}while(lectura != RX);
	SetShortAddr(MRFINTCON, SLPIE_DIS | WAKEIE_DIS | HSYMTMRIE_DIS | SECIE_DIS | TXG2IE_DIS | TXNIE_DIS);
	SetShortAddr(ACKTMOUT, DRPACK | MAWD5 | MAWD4 | MAWD3 | MAWD0);
	SetLongAddr(RFCON1, VCOOPT1 | VCOOPT0);
	SetChannel();
	SetShortAddr(RXMCR, VACIO);
	(void)GetShortAddr(INTSTAT);
	return INICIALIZACION_OK;
}

/**
 * @brief  Escribo en registro de 1 byte un dato de 1 byte
 * @param  Dirección del registro - 1 byte
 * @param  Dato - 1 byte
 * @retval None
 */
static void SetShortAddr(uint8_t reg_address, uint8_t valor) {

	// Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza con el MSB
	// en 0 indicando una dirección corta, 6 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura.
	reg_address = (uint8_t) (reg_address << SHIFT_SHORT_ADDR) | WRITE_8_BITS;
	SetCSPin(DISABLE);
	WriteByteSPIPort(reg_address);
	WriteByteSPIPort(valor);
	SetCSPin(ENABLE);
	return;
}

/**
 * @brief  Leo en registro de 1 byte un dato de 1 byte
 * @param  Dirección del registro - 1 byte
 * @param  Dato - 1 byte
 * @retval Valor devuelto por el módulo - 1 byte
 */
static uint8_t GetShortAddr(uint8_t reg_address) {

	// Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza con el MSB
	// en 0 indicando una dirección corta, 6 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura.
	uint8_t leido_spi = VACIO;
	reg_address = (uint8_t) (reg_address << SHIFT_SHORT_ADDR) & READ_8_BITS;
	SetCSPin(DISABLE);
	WriteByteSPIPort(reg_address);
	leido_spi = ReadByteSPIPort();
	SetCSPin(ENABLE);
	return leido_spi;
}

/**
 * @brief  Escribo de en registro de 2 bytes un dato de 1 byte
 * @param  Dirección del registro - 2 bytes
 * @param  Dato - 1 byte
 * @retval None
 */
static void SetLongAddr(uint16_t reg_address, uint8_t valor) {

	// Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza con el MSB
	// en 1 indicando una dirección larga, 10 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura. En los 4 bits restantes (LSB) no importa el valor.
	reg_address = (reg_address << SHIFT_LONG_ADDR) | WRITE_16_BITS;
	SetCSPin(DISABLE);
	Write2ByteSPIPort(reg_address);
	WriteByteSPIPort(valor);
	SetCSPin(ENABLE);
	return;
}

/**
 * @brief  Leo en registro de 2 bytes un dato de 1 byte
 * @param  Dirección del registro - 2 bytes
 * @param  Dato - 1 byte
 * @retval Valor devuelto por el módulo - 1 byte
 */
static uint8_t GetLongAddr(uint16_t reg_address) {

	// Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza con el MSB
	// en 1 indicando una dirección larga, 10 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura. En los 4 bits restantes (LSB) no importa el valor.
	uint8_t respuesta;
	reg_address = (reg_address << SHIFT_LONG_ADDR) | READ_16_BITS;
	SetCSPin(DISABLE);
	Write2ByteSPIPort(reg_address);
	respuesta = ReadByteSPIPort();
	SetCSPin(ENABLE);
	return respuesta;
}

/**
 * @brief  Seteo en el módulo en canal guardado en mrf24_data_config
 * @param  None
 * @retval None
 */
static void SetChannel(void) {

	SetLongAddr(RFCON0, mrf24_data_config.my_channel);
	SetShortAddr(RFCTL, RFRST_HOLD);
	SetShortAddr(RFCTL, VACIO);
	delay_t(WAIT_1_MS);
	return;
}

/**
 * @brief  Seteo en el módulo la dirección corta guardada en mrf24_data_config
 * @param  None
 * @retval None
 */
static void SetDeviceAddress(void) {

	SetShortAddr(SADRH, (uint8_t) (mrf24_data_config.my_address >> SHIFT_BYTE));
	SetShortAddr(SADRL, (uint8_t) (mrf24_data_config.my_address));
	SetShortAddr(PANIDH, (uint8_t) (mrf24_data_config.my_panid >> SHIFT_BYTE));
	SetShortAddr(PANIDL, (uint8_t) (mrf24_data_config.my_panid));
	return;
}

/**
 * @brief  Seteo en el módulo la dirección mac guardada en mrf24_data_config.
 * @param  None.
 * @retval None.
 */
static void SetDeviceMACAddress(void) {

	long_mac_address = EADR0;

	for(int i = 0; i < 8; i++) {

		SetShortAddr(long_mac_address++, mrf24_data_config.my_mac[i]);
	}
	return;
}

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief  Inicialización del módulo MRF24J40MA.
 * @param  None.
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK).
 */
MRF24_State_t MRF24J40Init(void) {

	InicializoVariables();
	InicializoPines();
	delay_t(WAIT_1_MS);
	SetResetPin(1);
	delay_t(WAIT_1_MS);
	estadoActual = InicializoMRF24();
	return estadoActual;
}

/**
 * @brief   Paso por referencia la dirección del mensaje a enviar.
 * @param   Puntero al mensaje.
 * @retval  Estado de la operación (ERROR_INESPERADO, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetMensajeSalida(const char * mensaje) {

	if(strlen(mensaje) == VACIO || estadoActual != INICIALIZACION_OK)
		return ERROR_INESPERADO;
	mrf24_data_out.buffer_salida = mensaje;
	mrf24_data_out.largo_mensaje = (uint8_t) strlen(mensaje);
	return OPERACION_REALIZADA;
}

/**
 * @brief   Configuro la dirección corta del dispositivo con el que me comunicaré.
 * @param   Dirección corta del dispositivo - 2 bytes.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetDireccionDestino(uint16_t direccion) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	mrf24_data_out.destinity_address = direccion;
	return OPERACION_REALIZADA;
}

/**
 * @brief   Configuro la PANID del dispositivo con el que me comunicaré.
 * @param   Dirección PANID de dos bytes.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetPANIDDestino(uint16_t panid) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	mrf24_data_out.destinity_panid = panid;
	return OPERACION_REALIZADA;
}

/**
 * @brief   Envío la información almacenada en la estructura de salida.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, TRANSMISION_REALIZADA).
 */
MRF24_State_t MRF24TransmitirDato(void) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	uint8_t pos_memoria = 0;
	uint8_t largo_cabecera = HEAD_LENGTH;
	SetLongAddr(pos_memoria++, largo_cabecera);
	SetLongAddr(pos_memoria++, mrf24_data_out.largo_mensaje + largo_cabecera);
	SetLongAddr(pos_memoria++, DATA|ACK_REQ|INTRA_PAN);         // LSB.
	SetLongAddr(pos_memoria++, LONG_S_ADD|SHORT_D_ADD);         // MSB.
	SetLongAddr(pos_memoria++, mrf24_data_config.sequence_number++);
	SetLongAddr(pos_memoria++, (uint8_t) mrf24_data_out.destinity_panid);
	SetLongAddr(pos_memoria++, (uint8_t) (mrf24_data_out.destinity_panid >> SHIFT_BYTE));
	SetLongAddr(pos_memoria++, (uint8_t) mrf24_data_out.destinity_address);
	SetLongAddr(pos_memoria++, (uint8_t) (mrf24_data_out.destinity_address >> SHIFT_BYTE));

	for(int8_t i = 0; i < mrf24_data_out.largo_mensaje; i++) {

		SetLongAddr(pos_memoria++, mrf24_data_out.buffer_salida[i]);
	}
	SetLongAddr(pos_memoria++, VACIO);
	SetShortAddr(TXNCON, TXNACKREQ | TXNTRIG);
	return TRANSMISION_REALIZADA;
}

/**
 * @brief   Consulto si se levantó la bandera indicando la llegada de un mensaje.
 * @param   None.
 * @retval  Estado de la operación (ERROR_INESPERADO, MSG_PRESENTE, MSG_NO_PRESENTE).
 */
volatile MRF24_State_t MRF24IsNewMsg(void) {

	if(estadoActual != INICIALIZACION_OK)
		return ERROR_INESPERADO;

	if(!IsMRF24Interrup())
		return MSG_PRESENTE;
	return MSG_NO_PRESENTE;
}

/**
 * @brief   Recibir un paquete y dejarlo en el bufer de entrada de mrf24_data_config.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, MSG_LEIDO).
 */
MRF24_State_t MRF24ReciboPaquete(void) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	uint8_t index;
	uint8_t largo_mensaje;
	SetLongAddr(BBREG1, RXDECINV);
	SetShortAddr(RXFLUSH, DATAONLY);
	largo_mensaje = GetLongAddr(RX_FIFO);

	for(index = 0; index < largo_mensaje - FCS_LQI_RSSI; index++) {

		mrf24_data_in.buffer_entrada[index] = GetLongAddr(RX_FIFO + HEAD_LENGTH + index);
	}
	SetLongAddr(BBREG1, VACIO);
	(void)GetShortAddr(INTSTAT);
	return MSG_LEIDO;
}

/**
 * @brief   Devuelvo un puntero al mensaje recibido por RF.
 * @param   None.
 * @retval  Puntero a la cadena recibida.
 */
uint8_t * MRF24GetMensajeEntrada(void) {

	return mrf24_data_in.buffer_entrada;
}

/**
 * @brief   Obtengo el PANID en el que estoy.
 * @param   None.
 * @retval  La dirección de 2 bytes de mi PANID.
 */
uint16_t MRF24GetMiPANID(void) {

	return mrf24_data_config.my_panid;
}
