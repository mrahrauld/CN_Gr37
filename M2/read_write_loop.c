#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

#include "read_write_loop.h"

#define SIZE 1024
int send_message(char *buf, int sfd){
      ssize_t read_count = read(STDIN_FILENO,(void *) buf,SIZE);
      if (read_count==-1){
	perror(NULL);
	return -1;
      } 
      ssize_t write_count = write(sfd,(void *) buf,read_count);
      if (write_count==-1){
	perror(NULL);
	return -1;
      } 
      memset((void *) buf,0,sizeof(char)*SIZE);
      return write_count;
}
int recieve_message(char *buf, int sfd){
  ssize_t read_count = read(sfd,(void *) buf,SIZE);
      if (read_count==-1){
	perror(NULL);
	return -1;
      } 
      ssize_t write_count = write(STDOUT_FILENO,(void *) buf,read_count);
      if (write_count==-1){
	perror(NULL);
	return -1;
      } 
      memset((void *) buf,0,sizeof(char)*SIZE);
      return write_count;

}
void read_write_loop(int sfd){
  int truevalue = 1;
  char buf[SIZE];
  struct pollfd ptrfd[2];
  ptrfd[0].fd = sfd;
  ptrfd[0].events = POLLIN;
  ptrfd[1].fd = STDIN_FILENO;
  ptrfd[1].events = POLLIN;
  int err;

  while(truevalue){
    //printf("w");
    err = poll(ptrfd,2,-1);
    if(err == -1){
      perror(NULL);
      return;
    }
    if( ptrfd[1].revents == POLLIN){
      //printf("send\n");
      err=send_message(buf,sfd);
      if (err==-1){
	printf("error send_message\n");
	return;
      }
    }
    else if(ptrfd[0].revents == POLLIN){
      //printf("recieve\n");
      err=recieve_message(buf,sfd);
      if (err==-1){
	return;
      }
    }
  }
  
}

