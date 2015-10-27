#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>
#include <math.h>
#include <string.h>
#define SIZE 512
struct __attribute__((__packed__)) pkt {
	ptypes_t type;
	uint8_t window;
	uint8_t seqnum;
	uint16_t length;
	char *data;
	uint32_t crc;
	};
	
/*
* Allocation en mémoire d'un pkt
*/
pkt_t* pkt_new()
{
	pkt_t *new;
	new = malloc(sizeof(struct pkt));
	return new;
}

/*
* free la mémoire du pkt
*/
void pkt_del(pkt_t *pkt)
{
	free(pkt->data);
	free(pkt);
}

/*
* Decode 
*/
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
	if( len < 4)
	{
		printf("E_NOHEADER\n");
		return E_NOHEADER;
	}
		pkt_status_code err = PKT_OK;
		
		//header
		char *header=malloc(4*sizeof(char));
		memcpy((void *)header, (void *) data, 4);
		
		//type
		int type = header[0] >> 5;
		pkt_status_code err1 = pkt_set_type(pkt,(ptypes_t) type);
	
		//window
		uint8_t w = header[0] << 3;
		uint8_t window= w >> 3;
		pkt_status_code err2 = pkt_set_window(pkt,window);
	
		//seqnum
		uint8_t seqnum= header[1] ;
		pkt_status_code err3 = pkt_set_seqnum(pkt,seqnum);
	
		//length
		uint16_t length;
		memcpy(&length,&header[2],2);

		pkt_status_code err4 = pkt_set_length(pkt,length);

		//payload
		char *buf = malloc (sizeof(char)*length+1);
		memcpy(buf,(data+4),length);
		pkt_status_code err5 = pkt_set_payload(pkt,buf, length);

		//crc
		int crc =0;
		memcpy((void *)&crc,(void *)(data+4+length),4);
		
		//calcul crc
		int crc2 = (int) crc32(0, (void *)data, 4+length);

		err = pkt_set_crc(pkt, (const uint32_t) crc);
		free(header);
		free(buf);
		return err;
	
	// Gestion des erreurs
	if(err1==E_TYPE)	
	{
		printf("erreur TYPE");
		return err1;
	}
		
	if(err2==E_WINDOW)
	{
		printf("erreur WINDOW");
		return err2;
	}
		
	if(err3==E_SEQNUM)
	{
		printf("erreur SEQNUM");
		return err3;
	}
			
	if (length != (uint16_t) len - 8 || length>512)
	{
		printf("erreur LENGTH");
		return E_LENGTH;
	}
	
	if(err4==E_LENGTH)
	{
		printf("erreur LENGTH2");
		return err4;
	}
	
	if(err5==E_LENGTH)
	{
		printf("erreur LENGTH3");
		return err5;
	}
	
	if ( crc != crc2)
	{
		printf("crc différents");
		return E_CRC;
	}
}
/*
* Encode
*/
pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
	if(pkt_get_length(pkt)>512)
	{
		printf("erreur length\n");
		return E_LENGTH;
	}
	
	if(pkt_get_type(pkt)!=1 && pkt_get_type(pkt)!=2 && pkt_get_type(pkt)!=4)
	{
	printf("erreur TYPE\n");
	return E_TYPE;
	}
	
	if(pkt_get_window(pkt)>31)
	{
	printf("erreur WINDOW\n");
	return E_WINDOW;
	}
	
	uint16_t length = pkt_get_length(pkt);
	int padding=0;
	
	if(length % 4 !=0)
	{
	padding = (4- (length % 4));
	}
	
	if (pkt->length+padding+8 > (uint16_t) *len+8)
	{ 
		return E_NOMEM;
	}
	//HEADER
	uint8_t type = pkt_get_type(pkt) << 5;
	uint8_t window = pkt_get_window(pkt);
	uint8_t tw = type | window;
	memcpy((void *) buf, (void *)&tw,1);
	uint8_t seqnum = pkt_get_seqnum(pkt);
	memcpy((void *) (buf+1), (void *)&seqnum, 1);
	int lengthpad= length + padding;
	memcpy((void *) (buf+2), (void *)&lengthpad, 2);
	const char *data = pkt_get_payload(pkt);

	if(data==NULL)
	{
		return E_NOPAYLOAD;
	}

	//PAYLOAD	
	memcpy((void *)&buf[4],data,length);
	//PADDING

	if(length % 4 !=0)
	{
		uint8_t pad= 0;
		int i;
		for(i=0;i<padding;i++)
		{
			memcpy((void *)(buf+4+length+i),&pad,1);
		}
	}
	
	//CRC
	int crc = (int) crc32(0, (const void *) buf, 4+length+padding);	
	memcpy((void *)(buf+4+length+padding),(void *) &crc, 4);
	
	//len
	*len = 8+length+padding;
	return PKT_OK;
}

ptypes_t pkt_get_type (const pkt_t* pkt)
{
	return pkt->type;
}

uint8_t pkt_get_window(const pkt_t* pkt)
{
	return pkt->window;
}

uint8_t pkt_get_seqnum(const pkt_t* pkt)
{
	return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
	return pkt->length;
}

uint32_t pkt_get_crc (const pkt_t* pkt)
{
	return pkt->crc;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
	return pkt->data;
}

pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
	if (type!=1&&type!=2&&type!=4)
	{
		return E_TYPE;
	}
	pkt->type = type;
	return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
	if (window <= (uint8_t) 31 ){
		pkt->window = window;
	}
	else {
		return E_WINDOW;
	}
	return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
	pkt->seqnum = seqnum ;
	return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
	if (length <= (uint16_t) 512){
		pkt->length = length;
	}
	else{
		return E_LENGTH;
	}
	return PKT_OK;
}

pkt_status_code pkt_set_crc(pkt_t *pkt, const uint32_t crc)
{
	pkt->crc = crc;
	return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,const char *data,const uint16_t length)
{
	if (length> 512){
		return E_LENGTH;
	}
	else {
		char *dat= malloc(sizeof(char)*(length+1));
		memcpy((void *)dat,(void *) data, length);
		pkt->data= dat;
		pkt->length= length;
	}
	return PKT_OK;
}

void print_pkt(pkt_t *pkt){
	printf("type: %d\n",(int) pkt_get_type(pkt));
	printf("window: %d\n",(int) pkt_get_window(pkt));
	printf("seqnum: %d\n",(int) pkt_get_seqnum(pkt));
	printf("length: %d\n",(int) pkt_get_length(pkt));
	printf("data: %s\n", pkt_get_payload(pkt));
	printf("CRC: %u\n", pkt_get_crc(pkt));
	}
	
int create_packet(uint8_t type, uint8_t window, uint8_t seqnum, uint16_t length, char *data, char *packet){
	pkt_t *pkt = pkt_new();
	size_t len =(size_t ) 512;
	int err;
	pkt_set_type(pkt,(ptypes_t) type);
	pkt_set_window(pkt, window);
	pkt_set_seqnum(pkt, seqnum);
	pkt_set_length(pkt, length);
	pkt_set_payload(pkt, data, length);
	err=pkt_encode(pkt,packet,&len);
	if (err!=0){
		printf("erreur encode");	
		return err;
	}
	int lenght_packet=length+8;
	if (lenght_packet % 4 != 0){
		lenght_packet= lenght_packet + 4 - (lenght_packet % 4);
	}
	pkt_del(pkt);
	return lenght_packet;
}
