#include "real_address.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>

const char * real_address(const char *address, struct sockaddr_in6 *rval){
  struct addrinfo hints={0};
  struct addrinfo *res;
  // hints.ai_family=AF_INET6;
  hints.ai_flags= (AI_V4MAPPED | AI_ADDRCONFIG);
  // hints.ai_protocol = 0;
  // hints.ai_socktype = 0;
  int err = getaddrinfo(address,NULL,&hints, &res);
  if (err!=0){
    printf("probleme getaddrinfo\n");
    printf("%s\n",gai_strerror(err));
    return gai_strerror(err);
  }
  struct sockaddr_in6 *t = (struct sockaddr_in6 *) res->ai_addr;
     *rval=*t;
  // memcpy(rval, res->ai_addr, res->ai_addrlen);
  //rval->sin6_family=AF_INET6;
  freeaddrinfo(res);

  return 0;
}
