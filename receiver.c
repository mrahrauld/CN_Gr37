#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>

#include "real_address.h"
#include "create_socket.h"
#include "read_write_loop.h"
#include "wait_for_client.h"

#define SIZE 1024


int wait_connection (char *hostname, int port)
{
  if (port <= 0) 
    {
      fprintf(stderr, "Invalid port!\n");
      return EXIT_FAILURE;
    }
  printf("wait_connection\n");
  struct sockaddr_in6 *addr = malloc(sizeof(struct sockaddr_in6 *));
  printf("real_adress\n");
  real_address(hostname,addr);
  printf("real_adress\n");
  int sfd = create_socket(addr,port,NULL,-1);
  if (sfd < 0)
    {
      fprintf(stderr, "Failed to create the socket!\n");
      return EXIT_FAILURE;
    }
  int err = wait_for_client(sfd);
  if (err == -1)
    {
      fprintf(stderr, "Failed to wait_for_client!\n");
      return EXIT_FAILURE;
    }
  return sfd;	
}

/* int set_Ack() */
/* {} */

/* int read_message(char *buf, int sfd) */
/* {} */

/* int read_file(char *file, int sfd) */
/* {} */
/* void move_window() */
/* {} */

/* int read_loop(int sfd) */
/* {} */


int read_packet(char *packet, int sfd)
{
  ssize_t read_count = read(sfd,(void *) packet,SIZE);
  if (read_count==-1)
    {
      perror(NULL);
      printf("error read\n");
      return -1;
    } 
  return read_count;
}      
int recieve_data(int sfd){
  int boucle=1;
  char buf[SIZE];
  struct pollfd ptrfd[2];
  ptrfd[0].fd = sfd;
  ptrfd[0].events = POLLIN;
  ptrfd[1].fd = STDOUT_FILENO;
  ptrfd[1].events = POLLOUT;
  int err;
  while(boucle){
    err = poll(ptrfd,2,-1);
    if(err == -1){
      perror(NULL);
      return err;
    }
    if(ptrfd[0].revents == POLLIN){
      err=recieve_message(buf,sfd);
      if (err==-1){
	return err;
      }
    }
  }
  return 0;
}



int main(int argc,char *argv[])
{

   char *filename = NULL;
  int i;
  for(i=1; i<argc;i++)
    {
      if ( strcmp("-f", argv[i])==0 && argc >= i+1)
      	{
      	  filename=argv[i+1];
      	  i++;
      	  break;
      	}
      else if ( strcmp("--filename", argv[i])==0 && argc>=i+1)
      	{
      	  filename=argv[i+1];
      	  i++;
      	  break;
      	}
    }
  char *hostname;
  int sfd;
  if ( filename == NULL)
    {
      hostname = argv[1];
      sfd = wait_connection(hostname, atoi(argv[2]));
       recieve_data(sfd);
    }
  else
    {
      printf("filename\n");
      hostname = argv[i+1];
      printf("filename\n");
      sfd = wait_connection(hostname, atoi(argv[i+2]));
    }
  if (sfd == -1)
    {
       exit(EXIT_FAILURE);
    }
return EXIT_SUCCESS;
}
