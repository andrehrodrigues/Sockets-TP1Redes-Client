#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>

int main(int argc, char *argv[]) {

    FILE *requestedFile;
    int clientSocket, serverPort, n;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    int bufferSize;
    char* nomeArquivo;

    //Parameter check.
    if (argc < 5) {
        fprintf(stderr, "ERROR: Number of parameters incorrect. Expected: %s <hostname> <port> <file_name> <buffer_size>\n", argv[0]);
        exit(0);
    }

    //Server port for connection.
    serverPort = atoi(argv[2]);

    //File name to request from server.
    nomeArquivo = argv[3];

    //Buffer size.
    bufferSize = atoi(argv[4]);

    char buffer[bufferSize];

    //Defition of client socket.
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        perror("ERROR: Problema ao tentar abrir o socket.");
        exit(1);
    }

    //Get the server info based on parameters passed.
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR: Host nao encontrado.\n");
        exit(0);
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(serverPort);

    //Begin connection to the server.
    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("ERROR: Nao foi possivel criar a conexao.");
        exit(1);
    }

    //Send file name to host
    n = write(clientSocket, nomeArquivo, strlen(nomeArquivo));

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    //Create/Open file ordered
    requestedFile = fopen(nomeArquivo, "w+");

    //Read response
    do {

        //Get package from the host.
        bzero(buffer, sizeof(buffer));
        n = read(clientSocket, buffer, bufferSize);

        if (n < 0) {
            perror("ERROR reading from socket");
            fclose(requestedFile);
            exit(1);
        }

        //If the request was succesful write to file.
        if ( n > 0 && buffer != NULL && strlen(buffer) > 0){
            fputs(buffer, requestedFile);
        }

    } while (n > 0);

    close(clientSocket);

    fclose(requestedFile);

    return 0;
}