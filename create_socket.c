#include <sys/types.h>    
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "create_socket.h"

int create_socket(struct sockaddr_in6 *source_addr,
                 int src_port,
                 struct sockaddr_in6 *dest_addr,
                 int dst_port){
  int so = socket(AF_INET6,SOCK_DGRAM,0);
  socklen_t address_len = sizeof ( struct sockaddr_in6);
  if (so==-1){
    perror(NULL);
    return -1;
  }
  if (source_addr != NULL){  
    if (src_port > 0){
      source_addr->sin6_port= (in_port_t) src_port;
    }
    if (source_addr->sin6_family== AF_INET6){
    }
    if (bind(so, (struct sockaddr *) source_addr , address_len)==-1){
      printf("probleme bind\n");
      perror(NULL);
      return -1;
    }
  }
  if (dest_addr != NULL){
    if (dst_port>0){
      dest_addr->sin6_port = (in_port_t) dst_port;
    }
    if(connect(so,(struct sockaddr *) dest_addr,address_len)==-1){
      perror(NULL);
      return -1;
    }
  }
  return so;
}
