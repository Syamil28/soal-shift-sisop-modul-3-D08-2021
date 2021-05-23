#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#define PORT 8080
char buffer[1024] = {0};
int angka;

int connection = 0;
int currentConnection = 0;
int id_socket[1000];

void *loginRegis(void *args) {
    int new_socket = *(int *)args;
    int valread;

    char buffer[1024] = {0};
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);

    if (strcmp(buffer, "exit") == 0) {
    }

    if (strcmp(buffer, "login") == 0 || strcmp(buffer, "register") == 0) {
        char id[1024] = {0};
        valread = read(new_socket, id, 1024);
        char password[1024] = {0};
        valread = read(new_socket, password, 1024);

        printf("id: %s, password: %s\n", id, password);

        if (strcmp(buffer, "register") == 0) {
            FILE *f;
            f = fopen("akun.txt", "a+");
            fprintf(f, "%s:%s\n", id, password);

            char authMessage[100];
            sprintf(authMessage, "2");
            send(new_socket, authMessage, strlen(authMessage), 0);

            fclose(f);

            loginRegis(&new_socket);
        }

        if (strcmp(buffer, "login") == 0) {
            FILE *f;
            f = fopen("akun.txt", "a+");
            char buffer[1024] = "";
            int isValid = 0;

            while (fgets(buffer, 1024, f) != NULL && !isValid) {
                char compare_id[1025], compare_pw[1025];


                char *token = strtok(buffer, ":");
                strcpy(compare_id, token);

                token = strtok(NULL, "\n");
                strcpy(compare_pw, token);

                if (strcmp(compare_id, id) == 0 && strcmp(compare_pw, password) == 0) {
                    isValid = 1;
                }
            }

            char authMessage[500];
            sprintf(authMessage, "%d", isValid);
            send(new_socket, authMessage, strlen(authMessage), 0);
            fclose(f);

            if (isValid) {
                afterLogin(new_socket, id, password);
            } else {
                loginRegis(&new_socket);
            }
        }
    } else {
        loginRegis(&new_socket);
        pthread_cancel(pthread_self());
    }
}

void afterLogin(int sock, char *id, char *password){
    int valread;
    char buffer[1024] = {0};
    valread = read(sock, buffer, 1024);
    if (strcmp(buffer, "add") == 0) {
        char publikasi[120] = {0};
        valread = read(sock, publikasi, 1024);
        char tahunPublikasi[120] = {0};
        valread = read(sock, tahunPublikasi, 1024);
        char filename[120] = {0};
        valread = read(sock, filename, 1024);

        FILE *fp;
        fp = fopen("files.tsv", "a+");
        fprintf(fp, "%s\t%s\t%s\n", publikasi, tahunPublikasi, filename);
        fclose(fp);

        fp = fopen("running.log", "a+");

        char *withoutFolder = filename + 6;
        fprintf(fp, "Tambah: %s (%s:%s)\n", withoutFolder, id, password);
        fclose(fp);


        write_file(sock, filename);

    }
}

void write_file(int sockfd, char *filename) {
    int n;
    FILE *fp;
    char buffer[1024];

    fp = fopen(filename, "w");
    bzero(buffer, 1024);
    while (1) {
        n = recv(sockfd, buffer, 1024, 0);
        fprintf(fp, "%s", buffer);
        bzero(buffer, 1024);
    }
    fclose(fp);
    return;
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    mkdir("FILES", 0777);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }



    pthread_t thread[55];

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        id_socket[currentConnection] = new_socket;

        if (connection > 0) {
            send(id_socket[currentConnection], "1", strlen("1"), 1024);
            pthread_create(&thread[currentConnection], NULL, loginRegis, &new_socket);
        } else {
            send(id_socket[currentConnection], "2", strlen("2"), 1024);
            pthread_create(&thread[currentConnection], NULL, loginRegis, &new_socket);
        }
        connection++;
        currentConnection++;



        //    valread = read(new_socket , buffer, 1024);
        //    // printf("%s\n",buffer );

        //    //ketika command tambah
        //    if(strcmp(buffer, "tambah")==0)
        //    {
        //        angka++;
        //        printf("%d\n", angka);
        //        send(new_socket, berhasilt, strlen(berhasilt), 0);
        //    }

        //    //ketika command kurang
        //    else if(strcmp(buffer, "kurang")==0)
        //    {
        //        angka--;
        //        printf("%d\n", angka);
        //        send(new_socket, berhasilk, strlen(berhasilk), 0);
        //    }

        //    //command cek
        //    else if(strcmp(buffer, "cek")==0)
        //    {
        //        sprintf(jumlah, "%d", angka);
        //        strcat(message, jumlah);
        //        send(new_socket, message, strlen(message), 0);
        //    }

        //    else
        //    {
        //        send(new_socket, errcommand, strlen(errcommand), 0);
        //    }

        //membersihkan command
        //    memset(buffer, 0, 1024);
    }
    return 0;
}
