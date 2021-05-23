#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

void loginRegis(int sock) {
    char option[120], id[100], pass[100];
    printf("login atau register\n> ");
    scanf("%s", option);

    if (strcmp(option, "exit") == 0) {
        send(sock, "exit", strlen("exit"), 0);
        exit(0);
    }

    if (!strcmp(option, "login") == 0 && !strcmp(option, "register") == 0) {
        loginRegis(sock);
        return;
    }

    send(sock, option, strlen(option), 0);
    printf("Id : ");
    getchar();
    scanf("%[^\n]s", id);
    send(sock, id, strlen(id), 0);
    printf("Password : ");
    getchar();
    scanf("%[^\n]s", pass);
    send(sock, pass, strlen(pass), 0);


    char logRegMsg[1024] = {0};
    int valread;
    valread = read(sock, logRegMsg, 1024);

    if (strcmp(logRegMsg, "1") == 0) {
        printf("Login Sukses\n");
    } else if (strcmp(logRegMsg, "2") == 0) {
        printf("Register Sukses, silahkan Login\n\n");
        loginRegis(sock);
    } else {
        printf("Login Gagal \n");
        loginRegis(sock);
    }

    afterLogin(sock);
}

void afterLogin(int sock) {
    char option[120], id[100], pass[100];
    printf("\nPilih add, download, delete, see, atau find\n");
    scanf("%s", option);

    if (strcmp(option, "add") == 0)Add(sock);
    if (strcmp(option, "download") == 0)Download(sock);
    if (strcmp(option, "delete") == 0)Delete(sock);
    if (strcmp(option, "see") == 0)See(sock);
    if (strcmp(option, "find") == 0)Find(sock);
}

void Add(int sock) {
    send(sock, "add", strlen("add"), 0);

    char publisher[100], tahunPublikasi[100], filename[100];
    printf("Publisher: ");
    getchar();
    scanf("%[^\n]s", publisher);
    send(sock, publisher, strlen(publisher), 0);
    printf("Tahun Publikasi: ");
    getchar();
    scanf("%[^\n]s", tahunPublikasi);
    send(sock, tahunPublikasi, strlen(tahunPublikasi), 0);
    printf("Filepath: ");
    getchar();
    scanf("%[^\n]s", filename);
    send(sock, filename, strlen(filename), 0);
    sleep(1);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("ERROR");
        exit(1);
    }

    sleep(1);
    send_file(fp, sock);
    printf("File terkirim.\n");
}

void send_file(FILE *f, int sockfd) {
    int n;
    char data[1024] = {0};

    while (fgets(data, 1024, f) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, 1024);
    }
    send(sockfd, "stop signal", sizeof("stop signal"), 0);
}


void Download(int sock){}
void Delete(int sock){}
void See(int sock){}
void Find(int sock){}


void write_file(int sock, char *filename) {
    int n;
    FILE *fp;
    char buffer[1024];

    fp = fopen(filename, "w");
    bzero(buffer, 1024);
    while (1) {
        n = recv(sock, buffer, 1024, 0);

        fprintf(fp, "%s", buffer);
        bzero(buffer, 1024);
    }
    fclose(fp);
    return;
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1){
        char authMessage[1024] = {0};
        int valread;
        valread = read(sock, authMessage, 1024);


        if (strcmp(authMessage, "wait") != 0) {
            loginRegis(sock);
        } else {
            printf("Waiting\n");
        }
        //   //memasukkan input
        //   char *input;
        //   scanf("%s", input);
        //   //kirim ke server
        //   send(sock , input , strlen(input) , 0 );
        //   //menerima balasan dari server
        //   valread = read(sock, buffer, 1024);
        //   printf("%s\n",buffer );
        //   memset(buffer, 0, 1024);

    }
    return 0;
}
