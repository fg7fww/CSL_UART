/*
 * timer.c
 *
 *  Created on: Mar 20, 2018
 *      Author: fg7fww
 */

#include "../include/app.h"
#include "../include/Timer.h"
#include "../include/CSL.h"

uint8_t Msg_Out[BUFFER_SIZE];
uint8_t Msg_Tmp[BUFFER_SIZE];
uint32_t msgNumPut=0;

aTstate_t Ts_FIFO_ST;
csl_ret_t Ts_FIFO_OP;

void Ts_Initialize(void)
{
     /* Timer 0 test
     */
    Sys_Timer_Set_Control(0,  T0_CFG1S);
    Sys_Timers_Stop(SELECT_TIMER0);

    /* Timer 1 test
     */
    Sys_Timer_Set_Control(1,  T1_CFGLESS1S);
    Sys_Timers_Stop(SELECT_TIMER1);

    /* Timer 2 test
     */
    Sys_Timer_Set_Control(2,  T2_CFGLESS1S);
    Sys_Timers_Stop(SELECT_TIMER2);

    /* Timer 3 test
      */
     Sys_Timer_Set_Control(3,  T3_CFGLESS1S);
     Sys_Timers_Stop(SELECT_TIMER3);

    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_EnableIRQ(TIMER3_IRQn);

     cbpf_t fncE = NULL;
    CSL_ATTx_p = NULL;
    Ts_FIFO_OP = CSL_FIFOInit((aT_t**) &CSL_ATTx_p);
/*
	if(CSL_ATTx_p!=NULL)
	{
		Ts_MsgTestPass();
	}
	else
	{
		Ts_MsgTestFail();
	}
*/

    if(Ts_FIFO_OP == CSL_OK)
    {
    	Ts_FIFO_ST = AT_READY;
    	if(CSL_ATTx_p!=NULL)
    	{
    		Ts_MsgTestPass();
    	}
    	else
    	{
    		Ts_MsgTestFail();
    	}
    }
    else
    {
    	Ts_FIFO_ST = AT_BUSY;
    	Ts_MsgTestFail();
    }

    uint32Max = 1000000;//4294967295=2^32 - 1;

}

void Ts_Start(void)
{
	Sys_Timers_Start(SELECT_TIMER0);
	//Sys_Timers_Start(SELECT_TIMER1);
}

void Ts_UI2ASCII(uint32_t data_var)
{
	uint32_t tmp;
	uint8_t* d_p;
	uint32_t s=data_var;
	char const ascii[] = "0123456789";

	d_p=&Msg_Out[7];
	do{
		s=(uint32_t) (s/10);
		d_p++;
	}while(s);
	s=data_var;
	while(s)
	{
		tmp=(uint32_t)s/10;
		*(d_p--)=(uint8_t)ascii[(int)(s-10*tmp)];
		s=tmp;
	}
}

void Ts_NewMsgTemp(void)
{
	uint8_t tmp[BUFFER_SIZE] = UART_TX_DATA;
	const uint8_t* tmp_p = &tmp[0];
	memcpy((uint8_t*)&Msg_Out[0], (uint8_t*)tmp_p, BUFFER_SIZE);
}

void Ts_NewMsgGen(void)
{
	Ts_NewMsgTemp();
	Ts_UI2ASCII(msgNumPut++);
	const uint8_t* s_p=&Msg_Out[0];
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE);
	if(msgNumPut>=uint32Max){msgNumPut=0;}
}

void Ts_MsgFIFOPut(void)
{
	csl_ret_t ret;
	const uint8_t* s_p=&Msg_Out[0];
	int len = BUFFER_SIZE;
	ret = CSL_FIFOStateGet(&Ts_FIFO_ST, CSL_ATTx_p);
	if((ret==CSL_OK)&&(Ts_FIFO_ST==AT_READY))
	{
		//ret = CSL_FIFOPacketPutTx(s_p, len, Ts_MsgFIFOEvent);
		ret = CSL_FIFOPacketPut(s_p, len, Ts_MsgFIFOEvent, CSL_ATTx_p);
	}
}

void Ts_MsgFIFOGet(void)
{
	uint8_t Msg_Event[BUFFER_SIZE_MSG]=TS_MSG_GET;
	csl_ret_t ret;
	uint8_t* d_p=&Msg_Tmp[0];
	const uint8_t* s_p=&Msg_Event[0];
	int len;
	ret = CSL_FIFOReadable((aT_t*) CSL_ATTx_p);
	if(ret==CSL_OK)
	{
		ret = CSL_FIFOPacketGet((uint8_t*) d_p, &len, CSL_ATTx_p);
	}
	if(ret!=CSL_OK)
	{
		s_p=&Msg_Tmp[0];
		d_p=&Msg_Event[0];
	}
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
    /* Restart indexes */
    tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

int Ts_EmptyCellCountGet(void)
{
	csl_ret_t ret;
	int emptyCells;
	ret = CSL_FIFOEmptyCellCountGet((aT_t*) CSL_ATTx_p, &emptyCells );
	if(ret==CSL_OK)
	{
		return emptyCells;
	}
	return 0;
}

/*
 *  Test callback of Put package Event
 */

void Ts_MsgFIFOEvent(void)
{
	uint8_t Msg_Event[BUFFER_SIZE_MSG]=TS_MSG_EVENT;
	const uint8_t* s_p = &Msg_Event[0];
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
    /* Restart indexes */
    tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

void Ts_MsgTestPass(void)
{
	uint8_t Msg_Test[BUFFER_SIZE_MSG]=TS_MSG_TEST_PASS;
	const uint8_t* s_p = &Msg_Test[0];
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
    tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

void Ts_MsgTestFail(void)
{
	uint8_t Msg_Test[BUFFER_SIZE_MSG]=TS_MSG_TEST_FAIL;
	const uint8_t* s_p = &Msg_Test[0];
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
    tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

void Ts_MsgEmptyCellCount(void)
{
	uint8_t Msg_Out[BUFFER_SIZE_MSG]=TS_MSG_ECELL;
	int cellEmptyCount =Ts_EmptyCellCountGet();
	Ts_UI2ASCII((uint32_t) cellEmptyCount);
	const uint8_t* s_p = &Msg_Out[0];
	memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
    tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

void Ts_MsgFIFOStateGet()
{
	aTstate_t aTS;
	csl_ret_t ret;
	const uint8_t* s_p;
	uint8_t Msg_FIFO_State [AT_NUMS] [BUFFER_SIZE_MSG+1] = {TS_MSG_FIFO_AT_READY, TS_MSG_FIFO_AT_BUSY, TS_MSG_FIFO_AT_FULL};
	ret = CSL_FIFOStateGet(&aTS, (aT_t*) CSL_ATTx_p);
	if(ret==CSL_OK)
	{
		s_p=&Msg_FIFO_State[aTS][0];
		memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
	}
	else
	{
		uint8_t Msg_Test[BUFFER_SIZE_MSG]=TS_MSG_TEST_FAIL;
		const uint8_t* s_p = &Msg_Test[0];
		memcpy((uint8_t*)tx_buffer, s_p, BUFFER_SIZE_MSG);
	}
	tx_buffer_index = 0;
	UART->TX_DATA = LF;
}

void Ts_FIFOFlow(void)
{
	aTstate_t aTS;
	csl_ret_t ret;
	int cellEmptyCount =Ts_EmptyCellCountGet();
	ret = CSL_FIFOStateGet(&aTS, (aT_t*) CSL_ATTx_p);


	if(cellEmptyCount<=(CSL_AT_SIZE-4))
	{
		Sys_Timers_Stop(SELECT_TIMER3);
	    Sys_Timer_Set_Control(3,  T3_CFGLESS1S);

	}

	if(cellEmptyCount==(CSL_AT_SIZE-0))
	{
		Sys_Timers_Stop(SELECT_TIMER3);
		Sys_Timer_Set_Control(3,  T3_CFG1S);
	}
	//Ts_MsgFIFOGet();

}

