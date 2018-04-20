/* ----------------------------------------------------------------------------
 * Copyright (c) 2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * app.c
 * - Simple application using a DIO5 input to send data through UART
 * - DIO6 is configured as output and connected to a LED.
 * - If data received is correct, DIO6 toggle.
 * - The core run on a clock from the 48 MHz crystal. The UART needs a clock
 *   with a good accuracy and precision.
 * ----------------------------------------------------------------------------
 * $Revision: 1.5 $
 * $Date: 2017/12/05 16:19:52 $
 * ------------------------------------------------------------------------- */
#include "include/app.h"

Msg_t Msg[3] = {Ts_NewMsgGen, Ts_MsgFIFOEvent, Ts_MsgFIFOStateGet};

/* ----------------------------------------------------------------------------
 * Function      : void DIO0_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Start UART transfer.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void DIO0_IRQHandler(void)
{
    static uint8_t ignore_next_dio_int = 0;
    if (ignore_next_dio_int)
    {
        ignore_next_dio_int = 0;
    }
    else if (DIO_DATA->ALIAS[BUTTON_DIO] == 0)
    {
        /* Button is pressed: Ignore next interrupt.
         * This is required to deal with the debounce circuit limitations. */
        ignore_next_dio_int = 1;

        /* start transmission */
        //start_tx = 1;
    }
}

/* ----------------------------------------------------------------------------
 * Function      : void UART_TX_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Sends the next byte.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_TX_IRQHandler(void)
{
    nbr_tx_send++;
    tx_end=0;
    if (tx_buffer_index < (BUFFER_SIZE))
    {
        UART->TX_DATA = tx_buffer[tx_buffer_index++];
    }
    else
    {
     	nbr_tx_send=0;
    	tx_end=1;
    }
}

/* ----------------------------------------------------------------------------
 * Function      : void UART_RX_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Receive data, waits to receive an 'R' before filling
 *                 the buffer.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_RX_IRQHandler(void)
{
    nbr_rx_receive++;
    rx_buffer[rx_buffer_index] = UART->RX_DATA;

    /* Wait to receive the first letter of the expected string to increase the
     * index */
    if (rx_buffer_index == 0)
    {
        if (rx_buffer[0] == UART_TX_DATA[0])
        {
            rx_buffer_index = 1;
        }
    }
    else
    {
        rx_buffer_index++;
    }

    /* The correct number of bytes were received, flag rx_end is set */
    if (rx_buffer_index >= BUFFER_SIZE)
    {
        rx_buffer_index = 0;
        rx_end = 1;
    }
}


void TIMER0_IRQHandler(void)
{
	if(tx_end==1)
	{
		Msg[0]();
		/* Restart indexes */
		tx_buffer_index = 0;
		UART->TX_DATA = LF;
		Sys_Timers_Start(SELECT_TIMER1);
	}
 }

void TIMER1_IRQHandler(void)
{
    if(tx_end==1)
    {
    	//Msg[1]();
    	Ts_MsgFIFOPut();
    	Sys_Timers_Start(SELECT_TIMER2);
    }
}

void TIMER2_IRQHandler(void)
{
    if(tx_end==1)
    {
    	Msg[2]();
    }
    Sys_Timers_Start(SELECT_TIMER3);
}

void TIMER3_IRQHandler(void)
{
    if(tx_end==1)
    {
    	Ts_FIFOFlow();
    }
}

/* ----------------------------------------------------------------------------
 * Function      : int main(void)
 * ----------------------------------------------------------------------------
 * Description   : Initialize the system, wait that the button is pressed to
 *                 start the transmission of an UART frame.
 *                 When receive a the correct number of characters verify the
 *                 value and toggle the led if correct.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int main(void)
{
    uint8_t rx_error;
    uint8_t i;

    /* Initialize the system */
    Initialize();
    Ts_Start();

    Sys_Timers_Start(SELECT_TIMER0);


    /* Spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        Sys_Watchdog_Refresh();



        /* RX ended, check the received value and if it is correct toggle the
         * LED. */
        if (rx_end == 1)
        {
            /* Check that the data received is as expected. If data is correct
             * toggle the GPIO6. */
            rx_error = 0;
            for (i = 0; i < BUFFER_SIZE; i++)
            {
                if (rx_buffer[i] != tx_buffer[i])
                {
                    rx_error = 1;
                    break;
                }
            }

            /* Reset flag to accept another RX frame */
            rx_end = 0;

            /* Blink the LED if no errors */
            if (rx_error == 0)
            {
                Sys_GPIO_Set_High(LED_DIO);
                Sys_Delay_ProgramROM(0.5 * SystemCoreClock);
                Sys_GPIO_Set_Low(LED_DIO);
            }
        }
    }
}
