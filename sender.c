#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */

#include "real_address.h"
#include "create_socket.h"
//#include "packet_implem.h"

/* arguments: adresse ipv6 du receiver et port d'écoute. */
/* elle cree la connection et le socket  */
/* retourne: socket ou -1 si erreur  */
int create_connection(char *hostname,int port){
  struct sockaddr_in6 *retour=malloc(sizeof(struct sockaddr_in6 *));
  real_address(hostname, retour);
  int sfd = create_socket(NULL, -1, &retour, port);
  if (sfd < 0) {
    fprintf(stderr, "Failed to create the socket!\n");
    return EXIT_FAILURE;
  }
  return sfd;
}

int create_packet(uint8_t type, uint8_t window, uint8_t seqnum, uint16_t length, char *data, char *packet){
  struct pkt_t *pkt = pkt_new();
  pkt_set_type(pkt,(ptypes_t) type);
  pkt_set_window(pkt, window);
  pkt_set_seqnum(pkt, seqnum);
  pkt_set_length(pkt, length);
  pkt_set_payload(pkt, data, length);

  pkt_encode(pkt,packet,512);
  
  return 0;

}

int send_packet(char *packet, int sfd){
  ssize_t write_count = write(sfd,(void *) packet, read_count);
  if (write_count== -1){
    printf("error write\n");
    perror(NULL);
    return -1;
  } 
  return write_count;
}

int send_message(char *buf, int sfd){
  
}

int send_file(char *file, int sfd){

}

int write_loop(int sfd){

}

void main(int argc, const char *argv[]){

}
