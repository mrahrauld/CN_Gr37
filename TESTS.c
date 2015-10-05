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
  printf("coucou\n");
  pkt_t *test;
  test = pkt_new();
  
  int i = pkt_get_window(test);
  
  printf("%d\n",i);
  printf("hey\n");
  pkt_set_window(test,(uint8_t) 8);
  i = pkt_get_window(test);
  
  printf("%d\n",i);
}
