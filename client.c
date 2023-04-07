#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 30
#define MAX_SIZE 1000
void send_Message(int newsockfd, char buf[], char command[])
{
    strcat(command, "\0");
    int l = strlen(command), i = 0, j = 0;
    while (j < l)
    {
        bzero(buf, BUF_SIZE);
        i = 0;
        while (i < BUF_SIZE && j <= l)
        {
            buf[i++] = command[j++];
        }
        if (j >= l)
            send(newsockfd, buf, strlen(buf) + 1, 0);
        else
            send(newsockfd, buf, strlen(buf), 0);
    }
}
void recieve_Message(int newsockfd, char buf[], char command[])
{
    bzero(command, MAX_SIZE);
    bzero(buf, BUF_SIZE);
    int t;
    while ((t = recv(newsockfd, buf, BUF_SIZE, 0)) > 0)
    {
        strcat(command, buf);
        if (buf[t - 1] == '\0')
            break;
    }
}
void receivefrom(int sockfd, char buf[])
{
    // char *buf = malloc(sizeof(char) * 200);
    bzero(buf, strlen(buf));
    char a[20];
    bzero(a, 20);
    int t = 0;
    while ((t = recv(sockfd, a, 12, 0)) > 0)
    {

        if (a[t - 1] == '\0')
        {
            strcat(buf, a);
            break;
        }
        strcat(buf, a);
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; /* Socket descriptors */
    socklen_t servlen;
    struct sockaddr_in cli_addr, serv_addr;
    char *ip = "127.0.0.1";
    int port = 3000;
    int i;
    char buf[MAX_SIZE];
    char ans[MAX_SIZE];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    printf("[+]TCP server socket created.\n");
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(atoi(argv[1]));
    servlen = sizeof(serv_addr);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                 sizeof(serv_addr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }

    // receiving time from load balncer
    recieve_Message(sockfd, ans, buf);
    printf("%s\n", buf);
    close(sockfd);
    return 0;
}