/*
 * Initialize.c
 *
 *  Created on: Mar 22, 2018
 *      Author: fg7fww
 */
#include "../include/app.h"
#include "../include/app_init.h"

/* ----------------------------------------------------------------------------
 * Function      : void Initialize(void)
 * ----------------------------------------------------------------------------
 * Description   : Initialize the system by disabling interrupts, switching to
 *                 the 8 MHz clock (divided from the 48 MHz crystal) and
 *                 configuring the required DIOs to use the UART.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Initialize(void)
{
	Ts_NewMsgTemp();
	rx_end          = 0;
	tx_end			= 1;
	start_tx        = 0;
	nbr_tx_send     = 0;
	tx_buffer_index = 0;
	nbr_rx_receive  = 0;
	rx_buffer_index = 0;

	//Msg[0] = Ts_NewMsgGen;
	//Msg[1] = Ts_MsgFIFOEvent;

	/* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Test DIO12 to pause the program to make it easy to re-flash */
    DIO->CFG[RECOVERY_DIO] = DIO_MODE_INPUT  | DIO_WEAK_PULL_UP |
                             DIO_LPF_DISABLE | DIO_6X_DRIVE;
    while (DIO_DATA->ALIAS[RECOVERY_DIO] == 0);

    /* Prepare the 48 MHz crystal
     * Start and configure VDDRF */
    ACS_VDDRF_CTRL->ENABLE_ALIAS = VDDRF_ENABLE_BITBAND;
    ACS_VDDRF_CTRL->CLAMP_ALIAS  = VDDRF_DISABLE_HIZ_BITBAND;

    /* Wait until VDDRF supply has powered up */
    while (ACS_VDDRF_CTRL->READY_ALIAS != VDDRF_READY_BITBAND);

    /* Enable RF power switches */
    SYSCTRL_RF_POWER_CFG->RF_POWER_ALIAS   = RF_POWER_ENABLE_BITBAND;

    /* Remove RF isolation */
    SYSCTRL_RF_ACCESS_CFG->RF_ACCESS_ALIAS = RF_ACCESS_ENABLE_BITBAND;

    /* Start the 48 MHz oscillator without changing the other register bits */
    RF->XTAL_CTRL = ((RF->XTAL_CTRL & ~XTAL_CTRL_DISABLE_OSCILLATOR) |
                     XTAL_CTRL_REG_VALUE_SEL_INTERNAL);

    /* Enable 48 MHz oscillator divider at desired prescale value */
    RF_REG2F->CK_DIV_1_6_CK_DIV_1_6_BYTE = CK_DIV_1_6_PRESCALE_6_BYTE;

    /* Wait until 48 MHz oscillator is started */
    while (RF_REG39->ANALOG_INFO_CLK_DIG_READY_ALIAS !=
           ANALOG_INFO_CLK_DIG_READY_BITBAND);

    /* Switch to (divided 48 MHz) oscillator clock */
    Sys_Clocks_SystemClkConfig(JTCK_PRESCALE_1   |
                               EXTCLK_PRESCALE_1 |
                               SYSCLK_CLKSRC_RFCLK);

    /* Setup DIO5 as a GPIO input with interrupts on transitions, DIO6 as a
     * GPIO output. Use the integrated debounce circuit to ensure that only a
     * single interrupt event occurs for each push of the pushbutton.
     * The debounce circuit always has to be used in combination with the
     * transition mode to deal with the debounce circuit limitations.
     * A debounce filter time of 50 ms is used. */
    Sys_DIO_Config(BUTTON_DIO, DIO_MODE_GPIO_IN_0 | DIO_WEAK_PULL_UP |
                   DIO_LPF_DISABLE);
    Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_0);
    Sys_DIO_IntConfig(0, DIO_EVENT_TRANSITION | DIO_SRC(BUTTON_DIO) |
                      DIO_DEBOUNCE_ENABLE,
                      DIO_DEBOUNCE_SLOWCLK_DIV1024, 49);

    /* Configure UART DIOs */
    Sys_UART_DIOConfig(DIO_6X_DRIVE | DIO_LPF_DISABLE | DIO_WEAK_PULL_UP,
                       UART_TX_DIO,
                       UART_RX_DIO);

    /* Configure and enable UART at 115200 bps*/
    Sys_UART_Enable(SystemCoreClock,
                    BAUDRATE,
                    UART_DMA_MODE_DISABLE);

    Ts_Initialize();

    /* Enable interrupts */
    NVIC_EnableIRQ(DIO0_IRQn);
    NVIC_EnableIRQ(UART_RX_IRQn);
    NVIC_EnableIRQ(UART_TX_IRQn);

    /* Stop masking interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

