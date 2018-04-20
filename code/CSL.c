/*
 * CSL.c
 *
 *  Created on: Mar 15, 2018
 *      Author: fg7fww
 */

#include "../include/CSL.h"

/*
 *
 */
csl_ret_t CSL_FIFOInit(aT_t** aT_pp)
{
	csl_ret_t ret=CSL_ERROR;
	aT_t* aT_p = NULL;
	if(*aT_pp==NULL)
	{
		*aT_pp=(aT_t*) malloc(sizeof(aT_t));
		aT_p=(aT_t*) (*aT_pp);
		if(*aT_pp!=NULL)
		{
			aT_p->pck_p = 0;
			aT_p->pck_p = 0;
			aT_p->pck_g = 0;
			aT_p->aPT_p = (aPT_t *) malloc(CSL_AT_SIZE * sizeof(aPT_t));
			if(aT_p->aPT_p!=NULL)
			{
				aT_p->aTS = AT_READY;
				ret = CSL_OK;
			}
			else
			{
				ret = CSL_ERROR;
			}
		}
		else
		{
			ret = CSL_ERROR;
		}
	}
	else
	{
		ret=CSL_ERROR;
	}
	return ret;
}

/*
 *
 */
csl_ret_t CSL_FIFOFree(aT_t** aT_pp)
{
	csl_ret_t ret=CSL_ERROR;
	aT_t* aT_p = NULL;
	aT_p=(aT_t*) (*aT_pp);
	if(aT_p==NULL)
	{
		ret = CSL_OK;
	}
	else
	{
		if(aT_p->aPT_p!=NULL)
		{
			free((aPT_t*) aT_p->aPT_p);
			aT_p->aPT_p=NULL;
		}
		free(*aT_pp);
		*aT_pp=NULL;
		ret=CSL_OK;
	}
	return ret;
}

/*
 *
 */
csl_ret_t CSL_FIFOReadable(aT_t* aT_p)
{
	csl_ret_t ret=CSL_ERROR;
	if(aT_p==NULL){return ret;}
	if(aT_p->pck_g==0){return CSL_NO_DATA;}
	if(aT_p->pck_g>0){return CSL_OK;}
	return CSL_ERROR;
}

/*
 *
 */
csl_ret_t CSL_FIFOEmptyCellCountGet(aT_t* aT_p, int* cellsCount )
{
	csl_ret_t ret=CSL_ERROR;
	int tmp=CSL_AT_SIZE;
	if((aT_p==NULL)||((aT_p->aPT_p==NULL)||(aT_p->aTS!=AT_READY)))
	{
		return ret;
	}
	else
	{
		for(int i=0;i<CSL_AT_SIZE;i++)
		{
			if(!aT_p->aPT_p[i].res){tmp--;}
		}
		*cellsCount=tmp;
		ret = CSL_OK;
	}
	return ret;
}

/*
 *
 */
csl_ret_t CSL_FIFOStateGet(aTstate_t* aTS, aT_t* aT_p)
{
	csl_ret_t ret = CSL_ERROR;
	if((aT_p==NULL)||(aT_p->aPT_p==NULL)){ret=CSL_ERROR;}
	else
	{
		*aTS=aT_p->aTS;
		ret = CSL_OK;
	}
	return ret;
}

csl_ret_t CSL_EmptyCellGet(int* row, aT_t* aT_p)
{
	csl_ret_t ret = CSL_ERROR;
	if((aT_p==NULL)||(aT_p->aPT_p==NULL)){ret=CSL_ERROR;}
	else
	{
		for(int i=0;i<CSL_AT_SIZE;i++)
		{
			if (aT_p->aPT_p[i].res==false)
			{
				*row =(int) i;
				return CSL_OK;
			}
		}
	}
	return CSL_ERROR;
}

/*
 * csl_ret_t CSL_FIFOPacketPut(const char* buff_p, int len, cbpf_t fncE, aT_t* aT_p)
 * packet writing to AT
 */
#ifdef FCHAR
csl_ret_t CSL_FIFOPacketPut(const char* buff_p, int len, cbpf_t fncE, aT_t* aT_p)
#else
csl_ret_t CSL_FIFOPacketPut(const uint8_t* buff_p, int len, cbpf_t fncE, aT_t* aT_p)
#endif
{
	int row;
	csl_ret_t ret=CSL_ERROR;
#ifdef FCHAR
	char* d_p;
#else
	uint8_t* d_p;
#endif

	if((aT_p==NULL)||(aT_p->aTS !=AT_READY))
	{
		return ret;
	}
	else
	{
		aT_p->aTS=AT_BUSY;
		d_p = &aT_p->aPT_p[row].csl_packet[0];
		if(CSL_EmptyCellGet(&row, aT_p)==CSL_OK)
		{
			memcpy(d_p, buff_p, len);
			aT_p->aPT_p[row].pck_len=len;
			aT_p->aPT_p[row].res=true;
			aT_p->aPT_p[row].pck_order=++(aT_p->pck_p);
			if(aT_p->pck_g==0){aT_p->pck_g++;}
			if(fncE != NULL)
			{
				fncE();
			}
			aT_p->aTS=AT_READY;
			ret = CSL_OK;
		}
		else
		{
			aT_p->aTS=AT_FULL;
			ret = CSL_ERROR;
		}
	}

	return ret;
}

/*
 * 	csl_ret_t CSL_FIFOCellGet(int* row, aT_t* aT_p)
 * 	First reserved cell get
 */
csl_ret_t CSL_FIFOCellGet(int* row, aT_t* aT_p)
{
	for(int i=0;i<CSL_AT_SIZE;i++)
	{
		if(aT_p->aPT_p[i].pck_order==aT_p->pck_g)
		{
			*row=i;
			return CSL_OK;
		}
	}
	return CSL_ERROR;
}

/*
 * csl_ret_t CSL_FIFOPacketGet(char* buff_p, int* len_ptr, aT_t* aT_p)
 * packet reading from AT
 */
#ifdef FCHAR
csl_ret_t CSL_FIFOPacketGet(char* buff_p, int* len_ptr, aT_t* aT_p)
#else
csl_ret_t CSL_FIFOPacketGet(uint8_t* buff_p, int* len_ptr, aT_t* aT_p)
#endif
{
	int row;
#ifdef FCHAR
	const char* s_p;
#else
	const uint8_t* s_p;
#endif
	csl_ret_t ret=CSL_ERROR;
	if((aT_p==NULL)||((aT_p->aTS!=AT_READY)&&(aT_p->pck_g<1)))
	{
		return ret;
	}
	else
	{
		s_p = &aT_p->aPT_p[row].csl_packet[0];
		aT_p->aTS=AT_BUSY;
		if(CSL_FIFOCellGet(&row, aT_p)==CSL_OK)
		{
			memcpy(buff_p, s_p, aT_p->aPT_p[row].pck_len);
			*len_ptr=aT_p->aPT_p[row].pck_len;
			aT_p->aPT_p[row].pck_order=0;
			aT_p->aPT_p[row].res=false;
			if(aT_p->pck_p > aT_p->pck_g)
			{
				aT_p->pck_g++;
			}
			if(aT_p->pck_p == aT_p->pck_g)
			{
				aT_p->pck_p = aT_p->pck_g = 0;
			}
			// Time sequence index upper limit
			if(aT_p->pck_p >= 2*CSL_AT_SIZE)
			{
				for(int i=0;i<CSL_AT_SIZE;i++)
				{
					if(aT_p->aPT_p[row].pck_order>0)
					{
						aT_p->aPT_p[row].pck_order-=CSL_AT_SIZE;
					}
				}
				aT_p->pck_p-=CSL_AT_SIZE;
				aT_p->pck_g-=CSL_AT_SIZE;
			}
			ret = CSL_OK;
		}
		else
		{
			ret = CSL_ERROR;
		}
	}
	aT_p->aTS=AT_READY;
	return ret;
}

/*
 * 	Implementation examples
 */
#ifdef FCHAR
csl_ret_t CSL_FIFOPacketPutRx(const char* buff, int len, cbpf_t fncE)
#else
csl_ret_t CSL_FIFOPacketPutRx(const uint8_t* buff, int len, cbpf_t fncE)
#endif
{
	return CSL_FIFOPacketPut(buff,len, fncE, CSL_ATRx_p);
}

#ifdef FCHAR
csl_ret_t CSL_FIFOPacketGetRx(char* buff, int* len_ptr)
#else
csl_ret_t CSL_FIFOPacketGetRx(uint8_t* buff, int* len_ptr)
#endif
{
	return CSL_FIFOPacketGet(buff, len_ptr, CSL_ATRx_p);
}

#ifdef FCHAR
csl_ret_t CSL_FIFOPacketPutTx(const char* buff, int len, cbpf_t fncE)
#else
csl_ret_t CSL_FIFOPacketPutTx(const uint8_t* buff, int len, cbpf_t fncE)
#endif
{
	return CSL_FIFOPacketPut(buff,len, fncE, CSL_ATTx_p);
}

#ifdef FCHAR
csl_ret_t CSL_FIFOPacketGetTx(char* buff, int* len_ptr)
#else
csl_ret_t CSL_FIFOPacketGetTx(uint8_t* buff, int* len_ptr)
#endif
{
	return CSL_FIFOPacketGet(buff, len_ptr, CSL_ATTx_p);
}
