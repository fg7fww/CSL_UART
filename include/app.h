/*
 * app.h
 *
 *  Created on: Mar 21, 2018
 *      Author: fg7fww
 */

#ifndef APP_H_
#define APP_H_

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <rsl10.h>

#define UART_TX_DATA                    "PCK num:____________"
#define BUFFER_SIZE                     (sizeof UART_TX_DATA)
#define CONCAT(x, y)                    x##y
#define DIO_SRC(x)                      CONCAT(DIO_SRC_DIO_, x)
#define UART_RX_DIO                     4	//2 UART RX for RSL10 QFN EVB V1.3
#define UART_TX_DIO                     5	//3	UART TX	for RSL10 QFN EVB V1.3
#define BUTTON_DIO                      7	//5 !!! CMSIS conflict on RSL10 QFN EVB V1.3 Button and UART_TX on the same wire
#define LED_DIO                         6	//when UART_TX_DIO is directly connected to the UART_RX_DIO, LED_DIO is blinking after message receiving
#define BAUDRATE                        115200
#define LF								0xA

/* Variable declarations and initializations */
volatile uint8_t rx_end;
volatile uint8_t tx_end;
volatile static uint8_t nbr_tx_send;
volatile uint8_t tx_buffer[BUFFER_SIZE]; // = UART_TX_DATA;
volatile uint8_t tx_buffer_index;
volatile uint8_t nbr_rx_receive;
volatile uint8_t rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_buffer_index;
volatile static uint8_t start_tx;

typedef void (*Msg_t)(void);

/* Function declarations */
extern void DIO0_IRQHandler(void);
extern void UART_TX_IRQHandler(void);
extern void UART_RX_IRQHandler(void);
void Initialize(void);
void Ts_Initialize(void);
void Ts_Start(void);
void Ts_NewMsgTemp(void);
void Ts_NewMsgGen(void);
void Ts_MsgFIFOPut(void);
void Ts_MsgFIFOGet(void);
void Ts_MsgFIFOEvent(void);
void Ts_MsgTestPass(void);
void Ts_MsgTestFail(void);
void Ts_MsgFIFOStateGet(void);
int Ts_EmptyCellCountGet(void);
void Ts_FIFOGetStart(void);
void Ts_FIFOFlow(void);
void Ts_MsgEmptyCellCount(void);

//extern void TIMER0_IRQ(void);
extern void TIMER0_IRQHandler(void);
extern void TIMER1_IRQHandler(void);
extern void TIMER2_IRQHandler(void);

#endif /* APP_H_ */
