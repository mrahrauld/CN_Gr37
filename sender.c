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
#include "packet_interface.h"

#define SIZE 512

uint8_t s_seqnum = 0;//seqnum attendu
uint8_t s_window = 0;
uint8_t last_seqnum = 0;// dernier seqnum envoyé
char *buf_frame[31];


/* arguments: adresse ipv6 du receiver et port d'écoute. */
/* elle cree la connection et le socket  */
/* retourne: socket ou -1 si erreur  */
int create_connection(char *hostname,int port){
  struct sockaddr_in6 *retour=malloc(sizeof(struct sockaddr_in6 *));
  real_address(hostname, retour);
  int sfd = create_socket(NULL, -1, retour, port);
  if (sfd < 0) {
    fprintf(stderr, "Failed to create the socket!\n");
    return EXIT_FAILURE;
  }
  printf("sfd : %d \n", sfd);
  return sfd;
}

void move_buf_frame(int decalage){
  int i;
  for (i=0;i<=10;i++){
    buf_frame[i]=buf_frame[i+decalage];
  }
}

pkt_t *recieve_packet(char *buf,int sfd){
  ssize_t read_count = read(sfd,(void *) buf,SIZE+8);
  if (read_count==-1){
    perror(NULL);
    return NULL;
  }   
  
  pkt_t *pkt = pkt_new();
  pkt_status_code err= pkt_decode(buf,read_count,pkt);
  //print_pkt(pkt);
  if(err!=0){
     printf("erreur decode\n");
    return NULL;
  }

  ssize_t write_count = write(STDOUT_FILENO,(void *) pkt_get_payload(pkt),read_count);
  if (write_count==-1){
    printf("erreur\n");
    perror(NULL);
    return NULL;
  } 
 memset((void *) buf,0,sizeof(char)*SIZE);
  return pkt;

}
		  
int send_packet(char *buf, int sfd,ssize_t read_count,int place_in_buf_frame,int seqnum){

  char *packet = malloc(sizeof(char)*(read_count+11));
  int lenght_packet;
  if (read_count==512){
    lenght_packet = create_packet((uint8_t) 1, (uint8_t) s_window, (uint8_t) seqnum, (uint16_t) read_count, buf, packet);
  }
  else {
    lenght_packet = create_packet((uint8_t) 1, (uint8_t) s_window, (uint8_t) seqnum, (uint16_t) read_count-1, buf, packet);
  }
  if (lenght_packet <= 0){
    return lenght_packet;
  }
  ssize_t write_count = write(sfd,(void *) packet, lenght_packet);
  if (write_count== -1){
    printf("error write\n");
    perror(NULL);
    return -1;
  }
  buf_frame[place_in_buf_frame]=packet;
  memset((void *) buf,0,sizeof(char)*SIZE);
  return write_count;
}

int send_file(char *file, int sfd){
   char buf2[SIZE];
   //pkt_t *ack=malloc(sizeof(pkt_t *));
   FILE *fichier = NULL;
   fichier  = fopen(file, "r");
   if (fichier == NULL)
     {
       printf("Impossible d'ouvrir le fichier\n");
     }
  char buf[SIZE+8];

  struct pollfd ptrfd[2];
  ptrfd[0].fd = sfd;
  ptrfd[0].events = POLLIN;
  int err;
  
  //Envoi du premier paquet
  size_t fin=fread( buf , sizeof(char), SIZE , fichier );
  err=send_packet(buf,sfd,fin,0,0);
  if (err==-1){
    return err;
  }

  while(fin!=0){
    err = poll(ptrfd,(nfds_t) 1,200);
    //printf("poll\n");
    if(err <= -1){
      perror(NULL);
      return err;
    }
    //printf("  %d  ",err);
    if (err==0){ //si le timer s'écoule, on renvoi le dernier attendu
      ssize_t write_count = write(sfd,(void *)buf_frame[0], SIZE+8);
	if (write_count== -1){
	  printf("error write\n");
	  perror(NULL);
	  return -1;
	}
    }
    if(ptrfd[0].revents == POLLIN){ // de l'information est prête à être lue
      pkt_t *ack=recieve_packet(buf2,sfd);
      if (pkt_get_type(ack)==PTYPE_ACK){
	move_buf_frame(pkt_get_seqnum(ack)-s_seqnum);
	s_seqnum= pkt_get_seqnum(ack);
	printf("s_seqnum : %d\n",s_seqnum);
	s_window = pkt_get_window(ack);
	printf("s_window : %d\n",s_window);
	int i;
	for (i=0;i<=s_window-(last_seqnum-s_seqnum);i++){
	  fin=fread( buf , sizeof(char), SIZE , fichier );
	    if(fin==0){
	      break;
	    }
	    err=send_packet(buf,sfd,fin,last_seqnum-s_seqnum+1,last_seqnum+1);
	    if (err==-1){
	      return err;
	    }
	    last_seqnum++;
	}
      }
      else if (pkt_get_type(ack)==PTYPE_NACK){
	
      }
    }
  }

      /* err=send_packet(buf,sfd,fin); */
      /* if (err==-1){ */
      /* 	return err; */
      /* } */
      /* fin=fread( buf , sizeof(char), SIZE , fichier ); */
      /* printf("while\n"); */
}

/* int write_loop(int sfd){ */

/* } */
int send_data(int sfd){
  int boucle=1;
  char buf[SIZE];
  struct pollfd ptrfd[2];
  ptrfd[0].fd = STDIN_FILENO;
  ptrfd[0].events = POLLIN;
  ptrfd[1].fd = sfd;
  ptrfd[1].events = POLLOUT;
  int err;
  while(boucle){
    err = poll(ptrfd,2,-1);
    if(err == -1){
      perror(NULL);
      return err;
    }
    if(ptrfd[0].revents == POLLIN){
        ssize_t read_count = read(STDIN_FILENO,(void *) buf,SIZE);
	if (read_count==-1){
	  printf("error read\n");
	  perror(NULL);
	  return -1;
	}
	err=send_packet(buf,sfd,read_count,0,0);
      if (err==-1){
	return err;
      }
    }
  }
  return 0;
}

int main(int argc,char *argv[]){
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
      sfd = create_connection(hostname, atoi(argv[2]));
      send_data(sfd);
    }
  else
    {
      hostname = argv[i+1];
      sfd = create_connection(hostname, atoi(argv[i+2]));
      send_file(filename ,sfd);
    }
  if (sfd == -1)
    {
       exit(EXIT_FAILURE);
    }
return EXIT_SUCCESS;
}
