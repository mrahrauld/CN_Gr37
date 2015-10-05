#include "packet_interface.h"
#include "packet_implem.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main ( int argc, char *argv[]){
  test_window();
  test_length();
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

int test_type(){

}
int test_length(){
    pkt_t *test;
  test = pkt_new();

  pkt_set_length(test, (uint16_t) 45);
  if (pkt_get_length(test)== (uint16_t) 45 && pkt_set_length(test, (uint16_t) 4500)==E_LENGTH){
    printf("test_length reussi\n");
  }
}
