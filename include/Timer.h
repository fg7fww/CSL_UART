/*
 * timer.h
 *
 *  Created on: Mar 20, 2018
 *      Author: fg7fww
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "app.h"
//#include "stdint.h"

#define PERIOD_1S		40000
#define PERIOD_2S		80000
#define PERIOD_LESS1S	8000

#define T0_CFG1S		TIMER_MULTI_COUNT_1 | \
						TIMER_FREE_RUN | \
						TIMER_SLOWCLK_DIV2 | \
						TIMER_PRESCALE_32 | \
						PERIOD_1S

#define T1_CFGLESS1S     TIMER_MULTI_COUNT_1 | \
						TIMER_SHOT_MODE    | \
                        TIMER_SLOWCLK_DIV2 | \
                        TIMER_PRESCALE_32  | \
                        PERIOD_LESS1S

#define T2_CFGLESS1S	TIMER_MULTI_COUNT_1 | \
						TIMER_SHOT_MODE | \
						TIMER_SLOWCLK_DIV2 | \
						TIMER_PRESCALE_32 | \
						PERIOD_LESS1S

#define T3_CFGLESS1S	TIMER_MULTI_COUNT_1 | \
						TIMER_SHOT_MODE | \
						TIMER_SLOWCLK_DIV2 | \
						TIMER_PRESCALE_32 | \
						PERIOD_LESS1S

#define T3_CFG1S		TIMER_MULTI_COUNT_1 | \
						TIMER_SHOT_MODE | \
						TIMER_SLOWCLK_DIV2 | \
						TIMER_PRESCALE_32 | \
						PERIOD_1S

#define TS_MSG_EVENT                    "Msg FIFO Put Event  "
#define BUFFER_SIZE_MSG                (sizeof(TS_MSG_EVENT))
#define TS_MSG_PUT                    	"Msg FIFO Put Done   "
#define TS_MSG_GET	                    "Msg FIFO Get NotDone"
#define TS_MSG_TEST_PASS				"Msg Test Pass       "
#define TS_MSG_TEST_FAIL				"Msg Test Fail       "
#define TS_MSG_FIFO_AT_READY			"Msg AT_READY        "
#define TS_MSG_FIFO_AT_BUSY				"Msg AT_BUSY         "
#define TS_MSG_FIFO_AT_FULL				"Msg AT_FULL         "
#define TS_MSG_ECELL					"E.Cells:____________"


uint32_t uint32Max;

//void Ts_MsgFIFOEvent(void);

#endif /* TIMER_H_ */
