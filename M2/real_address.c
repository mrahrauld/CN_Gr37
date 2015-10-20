#include "real_address.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>

const char * real_address(const char *address, struct sockaddr_in6 *rval){
  struct addrinfo hints;
  struct addrinfo *res;
  hints.ai_family=AF_INET6;
  hints.ai_flags= (AI_V4MAPPED | AI_ADDRCONFIG);
  hints.ai_protocol = 0;
  hints.ai_socktype = 0;
  int err = getaddrinfo(address,0,&hints, &res);
  if (err!=0){
    printf("%s\n",gai_strerror(err));
    return gai_strerror(err);
  }
  /* struct sockaddr_in6 *t = (struct sockaddr_in6 *) res->ai_addr; */
  /*    *rval=*t; */
  /*    rval->sin6_family=AF_INET6; */
  memcpy(rval, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
 printf("probleme\n");
  return 0;
}
