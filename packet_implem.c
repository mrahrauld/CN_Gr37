#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
    /* Your code will be inserted here */
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
  
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
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
   //*(buf+4+i)= b; //*(c); 
   while ( i< (pkt->length)){ //RAJOUTER CONDITION LEN 
      printf("k\n"); 
      *(buf+4+i)=*((pkt->data)+i); 
      i++;
   } 
   
   
   
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
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
  if (window <= (uint8_t) 31 && window >= (uint8_t) 0){
  pkt->window = window;
  }
  else {
    return E_WINDOW;
  }
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
		if (seqnum >(uint8_t)0 && seqnum <=(uint8_t)255) // seqnum varie sur [0,255]
	{
		pkt->seqnum = seqnum ;
	}
	else 
	{
		return E_SEQNUM ;
	}
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
  if (length >= (uint16_t) 0 && length <= (uint16_t) 512){
    pkt->length = length;
  }
  else{
    return E_LENGTH;
  }
}

pkt_status_code pkt_set_crc(pkt_t *pkt, const uint32_t crc)
{
  pkt->crc = crc;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
							    const char *data,
								const uint16_t length)
{
  if (length> 512){
    return E_LENGTH;
  }
  else {
    pkt->data= data;
    pkt->length= length;
  }
  // REJOUTER PADDLING
}
