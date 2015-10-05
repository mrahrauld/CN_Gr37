#include "packet_interface.h"
#include "packet_implem.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
int main ( int argc, char *argv[]){
  test_window();
}

int test_window(){
  pkt_t *test;
  test = pkt_new();
  
  int i = pkt_get_window(test);
  
  pkt_set_window(test,(uint8_t) 8);
  int b = pkt_get_window(test);

  if(i==0 && b==8){
    printf("test_window reussi");
  }
  
}

int test_type(){

}
