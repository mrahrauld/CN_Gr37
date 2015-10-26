#include "packet_interface.h"
#include "packet_implem.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <zlib.h>
int main ( int argc, char *argv[]){
  // test_window();
  /* test_length(); */
  /* test_type(); */
  /* test_seqnum(); */
  /* test_payload(); */
  test_encode();
  //test_decode();
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
  pkt_del(test);
  
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
	pkt_del(test);
}

int test_seqnum()
{
	pkt_t *test; 
	test = pkt_new();
	pkt_set_seqnum(test,(uint8_t) 47);
	int i = pkt_get_seqnum(test);
	if(i==47)
	{
		printf("Test_seqnum reussi\n");
	}
	else{
	  printf("Test_seqnum raté\n");
	}
	pkt_del(test);
}
int test_length(){
    pkt_t *test;
  test = pkt_new();

  pkt_set_length(test, (uint16_t) 45);
  if (pkt_get_length(test)== (uint16_t) 45 && pkt_set_length(test, (uint16_t) 513)==E_LENGTH){
    printf("test_length reussi\n");
  }
  pkt_del(test);
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
  printf("Hello == %s\n", dat);
  pkt_del(test);
  free(data);
}

int test_encode(){
  pkt_t *test;
  test = pkt_new();
  

  char *data;
  data= malloc(sizeof(char)*5);
  data = "talle";
  uint16_t length= (6);
  pkt_set_payload(test, data, length);

  pkt_set_window(test,(uint8_t) 6);
  pkt_set_type(test,(ptypes_t) 2);
  pkt_set_seqnum(test,(uint8_t) 9);
  print_pkt(test);
  
  char *buf = (char*) malloc(sizeof(char*)*20000000);
  size_t len;
  len =(size_t)20000000;
   printf("1\n"); 
    pkt_encode(test,buf,&len);
    affichebin((unsigned) *(buf+4));
    //affichebin((unsigned) *(buf+3));
    printf("\n");
  printf("%s\n", buf);

  // printf("%d\n", (int) *(buf+3));
  pkt_del(test);
  free(buf);
}
void affichebin(uint8_t n)
{
	unsigned bit = 0 ;
	unsigned mask = 1 ;
	int i;
	for (i = 0 ; i < 8 ; i++)
	{
		bit = (n & mask) >> i ;
		printf("%d", bit) ;
		mask <<= 1 ;
	}
}

int test_decode(){
   pkt_t *test;
  test = pkt_new();
  char *data;
  data= malloc(sizeof(char)*5);
  data = "Hellos";
  uint16_t length= 6;
  pkt_set_payload(test, data, length);
  pkt_set_window(test,(uint8_t) 3);
  pkt_set_type(test,(ptypes_t) 2);
  pkt_set_seqnum(test,(uint8_t) 70);
  
  char *buf = (char*) malloc(sizeof(char*)*2000);
  size_t len;
  len =(size_t)2000;
  pkt_encode(test,buf,&len);
  print_pkt(test);
  pkt_t *deco;
  deco = pkt_new();
  size_t leng;
  leng =(size_t)16;
  if(pkt_decode(buf,leng,deco)!=PKT_OK){
    printf("erreur decode");
  }
  print_pkt(deco);
}
  
