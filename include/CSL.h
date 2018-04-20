/*
 * CSL.h
 *
 *  Created on: Mar 15, 2018
 *      Author: fg7fww
 */

#ifndef CSL_H_
#define CSL_H_

#include <rsl10.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CSL_PACKET_LEN	20
#define CSL_AT_SIZE	5
//#define FCHAR

/* Return values */
typedef enum csl_ret
{
    CSL_OK = 0,
    CSL_ERROR,
    CSL_NO_DATA,
} csl_ret_t;

typedef enum aTstate
{
	AT_READY,	// ready to use
	AT_BUSY,	// wait for pending operation finish
	AT_FULL,	// full, must be read first
	AT_NUMS
} aTstate_t;

typedef struct aPT {
#ifdef FCHAR
	char csl_packet[CSL_PACKET_LEN];	// CS packet
#else
	uint8_t csl_packet[CSL_PACKET_LEN];
#endif
	bool res;							// reserved by value or empty
	int pck_len;						// length of stored packet in byte
	int pck_order;						// valid time sequence index range (2*CSL_AT_SIZE >= index >= 1)
} aPT_t;

typedef struct aT
{
	aPT_t* aPT_p;		// Packet allocation table
	aTstate_t aTS;		// AT current state
	int pck_g;
	int pck_p;
} aT_t;

typedef aT_t* aT_p_t;
typedef aT_p_t* aT_pp_t;

// Callback function for the event put to FIFO
typedef void (*cbpf_t)(void);
//cbpf_t fncE = NULL;

/*
 * 	Allocated system peripherals FIFOs implementation example
 */
// Periphery -> System
static aT_p_t CSL_ATRx_p = NULL;
// System -> Periphery
static aT_p_t CSL_ATTx_p;// = NULL;

/*
 * 	FIFO APIs
 */
csl_ret_t CSL_FIFOInit(aT_t** aT_p);
csl_ret_t CSL_FIFOFree(aT_t** aT_p);
csl_ret_t CSL_FIFOReadable(aT_t* aT_p);
csl_ret_t CSL_FIFOStateGet(aTstate_t* aTS, aT_t* aT_p);
csl_ret_t CSL_FIFOEmptyCellCountGet(aT_t* aT_p, int* cellsCount );
#ifdef FCHAR
csl_ret_t CSL_FIFOPacketPut(const char* buff, int len, cbpf_t fncE, aT_t* aT_p);
csl_ret_t CSL_FIFOPacketGet(char* buff, int* len_ptr, aT_t* aT_p);
#else
csl_ret_t CSL_FIFOPacketPut(const uint8_t* buff, int len, cbpf_t fncE, aT_t* aT_p);
csl_ret_t CSL_FIFOPacketGet(uint8_t* buff, int* len_ptr, aT_t* aT_p);
#endif
/*
 * 	Implementation examples
 */
#ifdef FCHAR
csl_ret_t CSL_FIFOPacketPutRx(const char* buff, int len, cbpf_t);
csl_ret_t CSL_FIFOPacketGetRx(char* buff, int* len_ptr);
csl_ret_t CSL_FIFOPacketPutTx(const char* buff, int len, cbpf_t);
csl_ret_t CSL_FIFOPacketGetTx(char* buff, int* len_ptr);
#else
csl_ret_t CSL_FIFOPacketPutRx(const uint8_t* buff, int len, cbpf_t);
csl_ret_t CSL_FIFOPacketGetRx(uint8_t* buff, int* len_ptr);
csl_ret_t CSL_FIFOPacketPutTx(const uint8_t* buff, int len, cbpf_t);
csl_ret_t CSL_FIFOPacketGetTx(uint8_t* buff, int* len_ptr);
#endif

#endif /* CSL_H_ */
