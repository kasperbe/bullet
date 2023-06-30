#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>


#define BUFFER_SIZE = 1024;

int sock_connect(char *url, in_port_t port) { 

  struct sockaddr_in addr;
  struct hostent *hp;
  int on = 1;

  if((hp = gethostbyname(url)) == NULL) {
    printf("gethostbyname: %s", url);
    return -1;
  }

  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

  if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    printf("connect -1");
    return -1;
  }

  return sock;
}

int main(int argc, char *argv[]) {

  int t = 10;
  int n = 10;
  char *url = argv[1];

  for(int i = 2; i < argc; i++) {
    if(!strcmp(argv[i], "-t")) {
      printf("Hit T\n");
      t = atoi(argv[i+1]);
    }

    if(!strcmp(argv[i], "-n")) {
      n = atoi(argv[i+1]);
    }
  }

  printf("flags: n: %d t: %d, url: %s", n, t, url);


  char buffer[1024];

  for(int i=0; i<t; i++) {
    int sock = sock_connect(url, 80);
    if(sock == -1) {
      printf("can't connect to socket");
      exit(1);
    }
    printf("\n\nSTARTING NEW REUQEST \n\n\n");
    struct timeval tv = {n/t, 0}; 
    if (select(0, NULL, NULL, NULL, &tv) < 0) perror("select");

    write(sock, "GET /\r\n", strlen("GET /\r\n"));
    bzero(buffer, 1024);

    while(read(sock, buffer, 1024-1) != 0) {
//      fprintf(stderr, "%s", buffer);
        printf("Received response");
      bzero(buffer, 1024);
    }


    shutdown(sock, SHUT_RDWR);
    close(sock);
  }


  return 0;
}

