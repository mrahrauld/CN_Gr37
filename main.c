#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>

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

&int create_socket(struct sockaddr_in6 *source_addr,
                 int src_port,
                 struct sockaddr_in6 *dest_addr,
                 int dst_port){
  int so = socket(AF_INET6,SOCK_DGRAM,0);
  socklen_t address_len = sizeof ( struct sockaddr_in6);
  printf("so : %d\n",so);
  if (so==-1){
    perror(NULL);
    return -1;
  }
  if (source_addr != NULL){  
    if (src_port > 0){
      source_addr->sin6_port= (in_port_t) src_port;
       printf("port assigné\n");
    }
    if (source_addr->sin6_family== AF_INET6){
      printf("AF_INET6\n");
    }
    if (bind(so, (struct sockaddr *) source_addr , address_len)==-1){
      printf("probleme bind\n");
      perror(NULL);
      return -1;
    }
  }
  if (dest_addr != NULL){
    printf("2\n");
    if (dst_port>0){
      dest_addr->sin6_port = (in_port_t) dst_port;
    }
    if(connect(so,(struct sockaddr *) dest_addr,address_len)==-1){
      perror(NULL);
      return -1;
    }
  }
	printf("so : %d\n",so);
  return so;
}

#define SIZE 1024
int send_message(char *buf, int sfd){
      ssize_t read_count = read(STDIN_FILENO,(void *) buf,SIZE);
      if (read_count==-1){
	printf("error read\n");
	perror(NULL);
	return -1;
      } 
      ssize_t write_count = write(sfd,(void *) buf,read_count);
      if (write_count==-1){
	printf("error write\n");
	perror(NULL);
	return -1;
      } 
      //printf("message sended\n");
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
      //printf("message recieved\n");
      memset((void *) buf,0,sizeof(char)*SIZE);
      return write_count;
}
void read_write_loop(int sfd){
  int truevalue = 1;
  char buf[SIZE];
  struct pollfd ptrfd[3];
  ptrfd[0].fd = sfd;
  ptrfd[0].events = POLLIN;
  ptrfd[1].fd = STDIN_FILENO;
  ptrfd[1].events = POLLIN;
  ptrfd[2].fd = STDOUT_FILENO;
  ptrfd[2].events = POLLOUT;
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

int wait_for_client(int sfd)
{
 	struct sockaddr_in6 address;
	address.sin6_family = AF_INET6;
 	socklen_t address_len = sizeof ( struct sockaddr_in6);
	printf("%d\n",sfd);
	int err = recvfrom(sfd, NULL, 0, MSG_PEEK,(struct sockaddr *) &address, &address_len);
	if (err == -1)
	{
	  fprintf(stderr, "%s\n", strerror(errno));
	  printf("erreur recvfrom\n");
	  fprintf(stderr, "%s\n", strerror(errno));
	  perror(NULL);
	  return -1;
	}
	printf("%d\n",sfd);
	err = connect(sfd, (struct sockaddr *) &address, address_len);
	if (err == -1)
	{
	  printf("erreur connect");
	  perror(NULL);
	  return -1;
	}
	return 0;
}

int main(int argc, const char *argv[]){
    struct sockaddr_in6 *retour=malloc(sizeof(struct sockaddr_in6 *));
  real_address(argv[1], retour);
  printf("TRY\n");
  int socket = create_socket(retour,atoi(argv[2]),retour,atoi(argv[3]));
		printf("OK! socket:%d portecoute:%d portenvoi:%d \n",socket,atoi(argv[2]),atoi(argv[3]));
		read_write_loop(socket);
		close(socket);
}
