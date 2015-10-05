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
	/* Your code will be inserted here */
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
	/* Your code will be inserted here */
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
	/* Your code will be inserted here */
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
  return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
	/* Your code will be inserted here */
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
	/* Your code will be inserted here */
}

uint32_t pkt_get_crc   (const pkt_t* pkt)
{
	/* Your code will be inserted here */
}

const char* pkt_get_payload(const pkt_t* pkt)
{
	/* Your code will be inserted here */
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
	/* Your code will be inserted here */
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
  pkt->window= window;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
	/* Your code will be inserted here */
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
	/* Your code will be inserted here */
}

pkt_status_code pkt_set_crc(pkt_t *pkt, const uint32_t crc)
{
	/* Your code will be inserted here */
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
							    const char *data,
								const uint16_t length)
{
	/* Your code will be inserted here */
}