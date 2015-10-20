#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>
#include <math.h>
#include <string.h>
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

  return new;
}

void pkt_del(pkt_t *pkt)
{
  free(pkt->data);
  free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
  if( len < 4){
    return E_NOHEADER;
  } 
  pkt_status_code err=0;
  //header
  int *header=malloc(4*sizeof(char));
  memcpy((void *)header, (void *) data, 4);

  //type
  int type = *header >> 29;
  err = pkt_set_type(pkt,(ptypes_t) type);
  if(err==E_TYPE){
    return err;
  }

  //window
  int w = *header << 3;
  uint8_t window=(uint8_t) (w >> 27);
  err = pkt_set_window(pkt,window);
  if(err==E_WINDOW){
    return err;
  }


  //seqnum
  int s = *header << 8;
  uint8_t seqnum=(uint8_t ) (s >> 24) ;
  err = pkt_set_seqnum(pkt,seqnum);
  if(err==E_SEQNUM){
    return err;
  }

  //length  
  int l =*header << 16;
  uint16_t length =(uint16_t) (l>>16);
  if (length != (uint16_t) len - 8 || length>512){
    return E_LENGTH;
  }
  err = pkt_set_length(pkt,length);
  if(err==E_LENGTH){
    return err;
  }
  
  //payload
  char *buf = malloc (sizeof(char)*length);
  int i=0;
  while(i<length){
    *(buf+i)=*(data+4+i);
    i++;
  }
  err = pkt_set_payload(pkt,buf, length);
  if(err==E_LENGTH){
    return err;
  }
  
  //crc
  int crc =0;
  memcpy((void *)&crc,(void *)&data[len-4],4);

  //calcul crc
  int crc2 = (int) crc32(0, (void *)data, len-4);

  if ( crc != crc2){
    return E_CRC;
  }
  err = pkt_set_crc(pkt, (const uint32_t) crc);
  
  return err;
  
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
  if (pkt->length+8 > (uint16_t) *len){
    return E_NOMEM;
  }
  //HEADER
  char *header=malloc(sizeof(char)*4);
  uint8_t type = pkt_get_type(pkt) << 5;
  uint8_t window = pkt_get_window(pkt);
  uint8_t tw = type | window;
  memcpy((void *) header, (void *)&tw,1);
  uint8_t seqnum = pkt_get_seqnum(pkt);
   memcpy((void *) (header+1), (void *)&seqnum,1);
  uint16_t length = pkt_get_length(pkt);
  memcpy((void *) (header+2), (void *) &length,2);
  const char *data = pkt_get_payload(pkt);
  
  //HEADER ET PAYLOAD
  memcpy((void *)buf,header,4);
  memcpy((void *)&buf[4],data,length);

  //PADDING
  int padding=0;
  if(length % 4 !=0){
    padding = 4- (length % 4);
    int *pad= 0;
    memcpy((void *)&buf[4+length],pad,padding);
  }

  //CRC
  int crc = (int) crc32(0, (const void *) buf, 4+length+padding);
  memcpy((void *)&buf[4+length+padding],(void *) &crc, 4);

  //len
  *len = 8+length+padding;

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
	if (type!=1&&type!=2&&type!=4){
	  //	return E_TYPE;
	}
	pkt->type = type;
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
  if (length> 512){
    return E_LENGTH;
  }
  else {
    int mod = (length % 4);
    if (mod == 0){
      char *dat= malloc(sizeof(char)*length+1);
 	memcpy((void *)dat,(void *) data, length);
    pkt->data= dat;
    pkt->length= length;
    }
    else{
              int i=0;
      char *dat= malloc(sizeof(char)*(length+4-mod));
     	
 		memcpy((void *)dat,(void *) data, length);
         memcpy((void *)&dat[length],(void *) &i, 4-mod);
        pkt->data= dat;
      pkt->length= length+4-mod;
    }
  }
  return 0;
  
}

