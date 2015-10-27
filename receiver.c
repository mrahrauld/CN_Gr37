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
int count_tot=0;
pkt_t *pkt_buf[31];
int max_window= 5;
int c_window=5; //place dans la window actuelle
uint8_t c_seqnum=0; //prochain seqnum attendu

//attend la reception d'un message pour créer un connection
int wait_connection (char *hostname, int port)
{
  if (port <= 0) 
    {
      fprintf(stderr, "Invalid port!\n");
      return EXIT_FAILURE;
    }
  struct sockaddr_in6 *addr = malloc(sizeof(struct sockaddr_in6 *));
  real_address(hostname,addr);
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

//permet de décaler le buffer de pkt 
void move_pkt_buf(int decalage){
  int i;
  c_window=max_window;
  for(i=0;i<=10;i++){
    if(pkt_buf[decalage+i]!=NULL){
      c_window--;
    }
    pkt_buf[i]=pkt_buf[i+decalage];
  }

}
int send_ack(int seqnum, int sfd){
  char *ack=malloc(sizeof(char)*10);
  create_packet(PTYPE_ACK, c_window, seqnum+1 % 255 , 0, NULL, ack);
  ssize_t write_count = write(sfd,(void *) ack,8);
  if (write_count==-1){
    printf("erreur\n");
    perror(NULL);
    return -1;
  } 
  return 0;
}

//envoi un nack du seqnum dont des données sont manquantes
int send_nack(int seqnum, int sfd){
  char *nack=malloc(sizeof(char)*10);
  create_packet(PTYPE_NACK, c_window, seqnum % 256 , 0, NULL, nack);
  ssize_t write_count = write(sfd,(void *) nack,8);
  if (write_count==-1){
    printf("erreur\n");
    perror(NULL);
    return -1;
  } 
  return 0;
}

//Gere la reception d'un packet
//lorsque c'est celui attendu, il l'écrit et vide les éléments consécutifs dans le buffer de pkt
//lorsqu'il appartient à la fenetre mais n'est pas encore dans le buffer, il l'insère dedans
//S'il n'appartient pas à la fenetre ou qu'il est déjà dans le buffer, il le néglige
int recieve_packet(char *buf,int sfd,FILE *fichier){
  ssize_t read_count = read(sfd,(void *) buf,SIZE+8);
  if (read_count==-1){
    perror(NULL);
    return -1;
  }   
  pkt_t *pkt = pkt_new();
  pkt_status_code err= pkt_decode(buf,read_count,pkt);
  memset((void *) buf,0,sizeof(char)*SIZE);
  if(err!=0){
    printf("erreur decode %d\n", pkt_get_seqnum(pkt)); 
    send_nack(pkt_get_seqnum(pkt),sfd);
  }
  int count=pkt_get_seqnum(pkt)-c_seqnum;
  printf("pkt_seqnum: %d   c_seqnum: %d  \n",pkt_get_seqnum(pkt),c_seqnum);
  count_tot++;
  printf("\n %d \n", count_tot);
  if(count==0){ //si il s'agit du packet attendu
    if(pkt_get_length(pkt)==0){ // si c'est le dernier packet
      return 0;
    }
    printf("packet attendu\n");
    ssize_t write_count =  fwrite ((void *) pkt_get_payload(pkt), sizeof(char),read_count-8,fichier);//ssize_t write_count = fwrite(fichier,(void *) pkt_get_payload(pkt),read_count);
    if (write_count==-1){
      printf("erreur\n");
      perror(NULL);
    }
    while(pkt_buf[count]!=NULL){ //on vide le buffer des éléments consécutifs
      printf("vidage buffer\n");
      if(pkt_get_length(pkt_buf[count])==0){ // si c'est le dernier packet
	return 0;
      }
      c_window++;
      ssize_t write_count =  fwrite ((void *) pkt_get_payload(pkt_buf[count]), sizeof(char),read_count-8,fichier);
     
      if (write_count==-1){
	printf("erreur\n");
	perror(NULL);
      }
      count++;
      //pkt_del(pkt_buf[count]);
    }
    if(send_ack(c_seqnum+count,sfd)==-1){
      printf("erreur ack");
    } //on envoi un ack
    move_pkt_buf(count);
    c_seqnum=c_seqnum+1+count;
  }
  else if(count>0 && count < max_window &&  pkt_buf[count-1]==NULL){ //si il est dans la window et qu'il n'a pas deja été recu
    printf("packet pas attendu\n");
    pkt_buf[count-1]=pkt; //on le met dans le buffer
    c_window--;
  }
  else { //sinon, on le neglige // ENVOYER UN ACK?????
    printf("packet pas attendu du tout\n");
    //send_nack(s, int sfd)
  }
  
 
  return 1;

}

// boucle while attrapant les données du sfd pour les écrire dans le fichier
int recieve_data(int sfd,char * file){
  int boucle=1;
  char buf[SIZE];
  struct pollfd ptrfd[2];
  FILE *fichier = NULL;
  fichier  = fopen(file, "w");
  if (fichier == NULL)
    {
      printf("Impossible d'ouvrir le fichier\n");
    }
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
      int err = recieve_packet(buf,sfd,fichier);
      //printf("\n seqnum: %d \n", pkt_get_seqnum(pkt));
      if (err==-1){
	return err;
      }
      if (err==0){
	fclose(fichier);
	return 0;
      }
    }
  }
  fclose(fichier);
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
      //recieve_data(sfd);
    }
  else
    {
      hostname = argv[i+1];
      sfd = wait_connection(hostname, atoi(argv[i+2]));
      recieve_data(sfd,filename);
    }
  if (sfd == -1)
    {
       exit(EXIT_FAILURE);
    }
return EXIT_SUCCESS;
}
