#include "wait_for_client.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


int wait_for_client(int sfd)
{
  int len =512;
  char *buf = malloc(sizeof(char *)*len);

  //struct sockaddr_in6 address;
  // address.sin6_family = AF_INET6;
  socklen_t address_len = sizeof ( struct sockaddr_in6);
  struct sockaddr_in6 *address = malloc(sizeof(struct sockaddr_in6));
  int err = recvfrom(sfd,(void *) buf, 1024, MSG_PEEK,(struct sockaddr *) address, &address_len);
  if (err == -1)
    {
      fprintf(stderr, "%s\n", strerror(errno));
      printf("erreur recvfrom\n");
      fprintf(stderr, "%s\n", strerror(errno));
      perror(NULL);
	  return -1;
    }
  err = connect(sfd, (struct sockaddr *) address, address_len);
  if (err == -1)
    {
      printf("erreur connect");
      perror(NULL);
      return -1;
    }
  return 0;
}
