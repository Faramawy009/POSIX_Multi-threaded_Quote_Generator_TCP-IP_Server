
/***********************************************************************************************

  CSci 4061 Fall 2017
 Assignment# 5:   Networking

 Student1 name: Abdelrahman Elfaramawy
 Student1 ID:   5171605
 X500 id: elfar009

Student2 name: Hanei Moubarak
Student2 ID:   5215277
X500 id: mouba005


 Operating system on which you tested your code: Linux
 CSELABS machine: <KH1260 - 18> <KH1260 - 19>

 GROUP INSTRUCTION:  Please make only ONLY one  submission when working in a group.

***********************************************************************************************/
/*

*/
/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define PORT "6789"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold
#define BUFFER_SIZE 1024
#define MAX_NUMBER_OF_CATEGORIES 1000
struct socket_context
{
    int sock_fd;
    struct sockaddr_storage sock_addr;
};
typedef struct socket_context client_sock_context;

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *handle_request(void * arg){
  client_sock_context *client_sock;
  client_sock =(client_sock_context*)arg;
  char s[INET6_ADDRSTRLEN];
  inet_ntop(client_sock->sock_addr.ss_family,
    get_in_addr((struct sockaddr *)&client_sock->sock_addr),
    s, sizeof s);

  printf("%d: got connection from %s\n", pthread_self(),s);

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char* tokenized;
  char* categories[MAX_NUMBER_OF_CATEGORIES];
  FILE* files[MAX_NUMBER_OF_CATEGORIES];
  int size = 0;

  fp = fopen("config.txt", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);
  /**
  * read all categories from config.txt
  * For each line, save the category and the filename in categories[] and Files[] respectively
  **/
  while ((read = getline(&line, &len, fp)) != -1) {
    categories[size] = (char*) malloc (BUFFER_SIZE*sizeof(char));
    strcpy(categories[size],strtok(line,": \n"));
    FILE* temp = fopen(strtok(NULL, ": \n"), "r");
    if (temp == NULL){
    printf("NULL pointer exception\n");
    pthread_exit(0);
    }
    files[size] = (FILE*) malloc (sizeof(FILE*));
    files[size] = temp;
    size++;
  }
  fclose(fp);
  char currentSelec[BUFFER_SIZE];  // current selection from the client
  int flag;
  int finished = 0;
  while (!finished){
  if (flag = recv(client_sock->sock_fd, currentSelec, BUFFER_SIZE, 0) < 0){
  perror("while reading from socket");
  break;
  }

  if (!strcmp(currentSelec, "BYE")){
    printf("Terminating thread\n");
    finished = 1;
  }
  // If user entered any, send a quote from a random category.
  else if (!strcmp(currentSelec, "ANY\n")){
    time_t t;
    srand((unsigned) time(&t));
    int randomNum = rand()%size;
    char result [BUFFER_SIZE ]= "";
    if(read = getline(&line, &len, files[randomNum]) == -1){
    rewind(files[randomNum]);
    read = getline(&line, &len, files[randomNum]);
    }
    strcat(result, line);
    strcat(result, "\n");
    if (read = getline(&line, &len, files[randomNum]) == -1){

    }
    strcat(result, line);
    strcat(result, "\n");

    if (flag = send(client_sock->sock_fd, result, BUFFER_SIZE, 0) < 0)
    perror("while sendding to customer");
  }
  // if user input is list, send a list of all categories.
  else if (!strcmp(currentSelec, "LIST\n")){
    char result [BUFFER_SIZE ]= "";
    for (int i=0; i<size; i++){
    //printf("%s\n\n\n", categories[i]);
    strcat(result, categories[i]);
    strcat(result, "\n");

    }
    if (flag = send(client_sock->sock_fd, result, BUFFER_SIZE, 0) < 0)
    perror("while sendding to customer");
  }
  // This mean the user entered a category, check if it exists or not
  // If it exists, send the next quote from that category.
  // If it does not exist, send an undefined input error message to the client.
  else{
    int exist = 0;
    int index = 0;
    char result [BUFFER_SIZE ]= "";
    char * currentTrimmed = (char*) malloc (BUFFER_SIZE*sizeof(char));
    strcpy(currentTrimmed, strtok(currentSelec,"\n"));
    for (int i=0; i<size; i++){
    if (!strcmp(currentTrimmed, categories[i])){
      exist = 1;
      if(read = getline(&line, &len, files[i]) == -1){
      rewind(files[i]);
      read = getline(&line, &len, files[i]);
      }
      strcat(result, line);
      strcat(result, "\n");
      if (read = getline(&line, &len, files[i]) == -1){
      printf("Error reading from empty file");
      perror("Aborting");
      }
      strcat(result, line);
      break;
    }

    }
    if (!exist){
    if (flag = send(client_sock->sock_fd, "undefined input!!! \n", BUFFER_SIZE, 0) < 0)
    perror("while sendding to customer");
    }
    else {
    strcat(result, "\n");
    if (flag = send(client_sock->sock_fd, result, BUFFER_SIZE, 0) < 0)
      perror("while sendding to customer");
    }
  }
  memset(currentSelec, 0, BUFFER_SIZE);
  }

    close(client_sock->sock_fd);
    free(client_sock);
    pthread_exit(0);
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    int rv;
    fd_set rfds;
    struct timeval tv;
    int retval;

    pthread_t th;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
    FD_ZERO(&rfds);
    FD_SET(sockfd,&rfds);
    tv.tv_sec=5;
    tv.tv_usec=0;

    retval = select(sockfd+1,&rfds,NULL,NULL,&tv);

    if(retval){
      printf("New connection...\n");
      new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

      if (new_fd == -1) {
        perror("accept");
          continue;
      }

      client_sock_context* client_sock = (client_sock_context*) malloc(sizeof(client_sock_context));
      client_sock->sock_fd = new_fd;
      client_sock->sock_addr = their_addr;

      //create a new thread to handle this request
      //logic for child is present in handle_request() function defined above
      pthread_create(&th,NULL,handle_request,client_sock);
      }
      else{
      printf("No connection request...timeout..set timer again..\n");
    }
    }
    return 0;
}
