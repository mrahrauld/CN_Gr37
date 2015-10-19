#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>
#include <math.h>
/* Extra #includes */
/* Your code will be inserted here */

struct __attribute__((__packed__)) pkt {
 ptypes_t type;
 uint8_t window;
 uint8_t seqnum;
 uint16_t length;

char *data;

 uint32_t crc;
};

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
  pkt_t *new;
  new = malloc(sizeof(struct pkt));

  new->window=0;
  return new;
}

void pkt_del(pkt_t *pkt)
{
  free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
  if( len <8){
    return E_NOHEADER;
  } 
  pkt->window=(uint8_t) *data & 31;
  char t = *data >> 5;
  ptypes_t type = (ptypes_t) t & 7;
  if (type !=(ptypes_t)  1 && type !=(ptypes_t) 2 && type!=(ptypes_t) 4){
    return E_TYPE;
  }
  pkt->type= type;
  pkt->seqnum=(uint8_t )*(data+1);
  char l =*(data+2) << 8;
  pkt->length = l |  *(data+3);
  int length = (int)len -8 ;
  if (pkt->length != length){
    return E_LENGTH;
  }
  printf("??%d\n",length);
  char *buf = malloc (sizeof(char)*length);
  int i=0;
  while(i<length){
    *(buf+i)=*(data+4+i);
    i++;
  }
  pkt->data = buf;
  uint32_t a = (uint32_t) *(data+4+i) << 24;
  uint32_t b = (uint32_t) *(data+5+i) << 16;
  uint32_t c = (uint32_t) *(data+6+i) << 8;
  uint32_t d = (uint32_t) *(data+7+i);
  printf("%u\n",d);
  pkt->crc = a | b | c | d;

  uLong crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc,(const Bytef*) data,length+4);
  if ( pkt->crc != crc) 
    {
      return E_CRC;
    }
  return 0;
  
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
  if (pkt->length+8 > (uint16_t) *len){
    return E_NOMEM;
  }
  unsigned char b;
  int type = pkt->type << 5;
  int window = pkt->window;
  b= (char) window | type;
  *buf = b;
  *(buf+1)= (char) pkt->seqnum;
  
   int l =pkt->length; 
   *(buf+3)=(char) l & 255;  
   l = l>>8; 
   *(buf+2)=(char) l & 255;  
   printf("2\n"); 
   int i=0;  
   while ( i< (pkt->length)){ 
      *(buf+4+i)=*((pkt->data)+i); 
      i++;
   }
   
   int len_POST = 4+i;
   int c = pkt->crc;
   int j;
   int k;
   uLong crc = crc32(0L, Z_NULL, 0);
	 crc = crc32(crc,(const Bytef *) buf,len_POST);
   for (j=4 ; j>0 ; j--)
   {
    k = 8*j;
   	*(buf+4+i+j) =(char) c & (int) pow((int) 2,(int) k);
	 	c = c >> 8*(j-1);
   }
   return 0;
   
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
  return pkt->type;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
  return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
  return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
  return pkt->length;
}

uint32_t pkt_get_crc   (const pkt_t* pkt)
{
	return pkt->crc;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
  return pkt->data;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
		if(type == (ptypes_t)1 || type == (ptypes_t)2 || type == (ptypes_t)4 )
	{
		pkt->type = type;
	}
	else 
	{
		return E_TYPE;
	}
		return 0;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
  if (window <= (uint8_t) 31 ){
  pkt->window = window;
  }
  else {
    return E_WINDOW;
  }
  return 0;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
  pkt->seqnum = seqnum ;
  return 0;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
  if (length <= (uint16_t) 512){
    pkt->length = length;
  }
  else{
    return E_LENGTH;
  }
  return 0;
}

pkt_status_code pkt_set_crc(pkt_t *pkt, const uint32_t crc)
{
  pkt->crc = crc;
  return 0;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
							    const char *data,
								const uint16_t length)
{
   int j;
  if (length> 511){
    return E_LENGTH;
  }
  else {
    int mod = (length % 4);
    if (mod == 0){
      char *dat= malloc(sizeof(char)*length);
      for (j=0;j<length;j++){
	*(dat+j)=*(data+j);
      }
    pkt->data= dat;
    pkt->length= length;
    }
    else{
      char *dat= malloc(sizeof(char)*(length+4-mod));
     
      for (j=0;j<length;j++){
	*(dat+j)=*(data+j);
      }
      printf("paddling...\n");
      int i;
      for(i=0;i<4-mod;i++){
	*(dat+length+i)=(char) 0;
      }
      pkt->data= dat;
      pkt->length= length+4-mod;
    }
  }
  return 0;
  
}
