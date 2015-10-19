#include "packet_interface.h"
#include "packet_implem.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <zlib.h>
int main ( int argc, char *argv[]){
  test_window();
  test_length();
  test_type();
  test_seqnum();
  test_payload();
  test_encode();
  test_decode();
}

int test_window(){
  pkt_t *test;
  test = pkt_new();
  
  int i = pkt_get_window(test);
  
   pkt_set_window(test,(uint8_t) 8);
  int b = pkt_get_window(test);

  if(i==0 && b==8 && pkt_set_window(test,(uint8_t) 34)==E_WINDOW){
    printf("test_window reussi\n");
  }
  
}

int test_type()
{
	pkt_t *test; 
	test = pkt_new();
	pkt_set_type(test,(ptypes_t) 2);
	int i = pkt_get_type(test);
	if(i==2 && pkt_set_type(test,(ptypes_t) 3) == E_TYPE)
	{
		printf("Test_type reussi\n");
	}
}

int test_seqnum()
{
	pkt_t *test; 
	test = pkt_new();
	pkt_set_seqnum(test,(uint8_t) 47);
	int i = pkt_get_seqnum(test);
	if(i==47 && pkt_set_seqnum(test,(uint8_t) 256) == E_SEQNUM)
	{
		printf("Test_seqnum reussi\n");
	}
}
int test_length(){
    pkt_t *test;
  test = pkt_new();

  pkt_set_length(test, (uint16_t) 45);
  if (pkt_get_length(test)== (uint16_t) 45 && pkt_set_length(test, (uint16_t) 4500)==E_LENGTH){
    printf("test_length reussi\n");
  }
}
int test_payload(){
  pkt_t *test;
  test = pkt_new();
  char *data;
  data= malloc(sizeof(char)*5);
  data = "Hello";
  uint16_t length= 5;
  pkt_set_payload(test, data, length);
  char *dat=  pkt_get_payload(test);
  printf("%s\n", dat);
}
int print_pkt(pkt_t *pkt){
  printf("type: %d\n",(int) pkt->type);
  printf("window: %d\n",(int) pkt->window); 
  printf("length: %d\n",(int) pkt->length);
  printf("data: %s\n",pkt->data);
  printf("CRC: %u\n",pkt->crc);
}

int test_encode(){
  pkt_t *test;
  test = pkt_new();
  

  char *data;
  data= malloc(sizeof(char)*5);
  data = "Hello";
  uint16_t length= 5;
  pkt_set_payload(test, data, length);

  pkt_set_window(test,(uint8_t) 3);
  pkt_set_type(test,(ptypes_t) 2);
  pkt_set_seqnum(test,(uint8_t) 70);
  print_pkt(test);
  
  char *buf = (char*) malloc(sizeof(char*)*2000);
  size_t len;
  len =(size_t)2000;
   printf("1\n"); 
    pkt_encode(test,buf,&len);
  printf("%s\n", buf+4);
  // printf("%d\n", (int) *(buf+3));
}

int test_decode(){
   pkt_t *test;
  test = pkt_new();
  char *data;
  data= malloc(sizeof(char)*5);
  data = "Hello";
  uint16_t length= 8;
  pkt_set_payload(test, data, length);
  pkt_set_window(test,(uint8_t) 3);
  pkt_set_type(test,(ptypes_t) 2);
  pkt_set_seqnum(test,(uint8_t) 70);
  
  char *buf = (char*) malloc(sizeof(char*)*2000);
  size_t len;
  len =(size_t)200;
  pkt_encode(test,buf,&len);
  
  pkt_t *deco;
  deco = pkt_new();
  size_t leng;
  leng =(size_t)16;
  pkt_decode(buf,leng,deco);
  print_pkt(deco);
}
  
