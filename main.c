#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 9000
int port = 0;
char *path;
void loadInfo(){
    printf("File is loaded!! :)\n");
    printf("Port: %d\n", port);
    printf("Path: %s\n", path);
}
// load the configuration
void loadConfig(){
    FILE *filePointer;   
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    filePointer = fopen("secret.txt", "r");
    if (filePointer == NULL)
        exit(EXIT_FAILURE);
      while ((read = getline(&line, &len, filePointer)) != -1) {
        if(strstr(line, "PORT=") != NULL){
            port = atoi(line + 5);
        }
        if(strstr(line, "PATH=") != NULL){
            path = line + 5;
        }
    }
    fclose(filePointer);
    loadInfo();
}
void serverSetup(char httpHeader[]){
    FILE *fp;
    fp = fopen(strcat(path, "index.html"), "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
        strcat(httpHeader, line);
    }
    fclose(fp);
}
int main(int argc, char const *argv[]){
    // initialise the essential socket creation variables
    int server_fd, new_socket; 
    long valread;
    struct sockaddr_in address;
    // get the length of the socket address
    int addrlen = sizeof(address);
    // httpHeader
    char httpHeader[100000] = "HTTP/1.1 200 OK\r\n\n";

    // load the port number and the type of loading from the secrett file 
    loadConfig();
    // setup the server by initialising the header 
    serverSetup(httpHeader);

    // print the header after loading index.html file 
    printf("Header: %s\n", httpHeader);
    // if the socket can't be created show the error

     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("%s","socket created");
    }
    
    // set the address and port numbers  
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    // set the sinzero value to '/0' for a full length
    memset(address.sin_zero, '\0', sizeof address.sin_zero);


   // once socket has been created , try binding it ,
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    // after bind step , try to listen for the server to accept the connection.
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // listen for the client to get connected until it get's connected to the server
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        // if failure ocurs while accepting the socket connection exit the program
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , httpHeader , strlen(httpHeader));
        printf("---Hello message sent---");
        close(new_socket);
    }
    return 0;
}
