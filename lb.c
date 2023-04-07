#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <poll.h>

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
void sendtooo(char buf[], int newsockfd)
{

    // printf("sending||\n");
    int j = 0;
    char a[20];
    bzero(a, 20);
    while (j < strlen(buf))
    {
        bzero(a, 20);
        for (int i = 0; i < 15 && j < strlen(buf); i++)
        {
            a[i] = buf[j];
            j++;
        }
        if (j == strlen(buf))
        {
            send(newsockfd, a, strlen(a) + 1, 0);
            return;
        }
        else
        {
            send(newsockfd, a, strlen(a), 0);
        }
    }
}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
    int s1_sock,
        s2_sock, clnt_sock;

    int clilen;
    int newsockfd;

    char buffer[MAX_SIZE];
    char ans[MAX_SIZE];
    bzero(ans, MAX_SIZE);
    struct pollfd fds;
    int portnumber;
    int s1_load = 0, s2_load = 0;
    struct sockaddr_in s1_addr, s2_addr, clnt_addr;
    int clnt_addr_size;
    // Create sockets for communication with S1 and S2
    s1_sock = socket(AF_INET, SOCK_STREAM, 0);
    s2_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&s1_addr, 0, sizeof(s1_addr));

    portnumber = atoi(argv[2]);
    s1_addr.sin_family = AF_INET;
    s1_addr.sin_port = htons(portnumber);
    inet_aton("127.0.0.1", &s1_addr.sin_addr);
    // s1_addr.sin_addr.s_addr = INADDR_ANY;
    connect(s1_sock, (struct sockaddr *)&s1_addr, sizeof(s1_addr));
    memset(&s2_addr, 0, sizeof(s2_addr));
    portnumber = atoi(argv[3]);
    s2_addr.sin_family = AF_INET;
    s2_addr.sin_port = htons(portnumber);
    // s2_addr.sin_addr.s_addr = INADDR_ANY;
    inet_aton("127.0.0.1", &s2_addr.sin_addr);
    connect(s2_sock, (struct sockaddr *)&s2_addr, sizeof(s2_addr));
    bzero(buffer, MAX_SIZE);
    strcpy(buffer, "Send Load");
    send_Message(s1_sock, ans, buffer);
    recieve_Message(s1_sock, ans, buffer);
    s1_load = atoi(buffer);
    bzero(buffer, MAX_SIZE);
    strcpy(buffer, "Send Load");
    send_Message(s2_sock, ans, buffer);
    recieve_Message(s2_sock, ans, buffer);

    s2_load = atoi(buffer);
    close(s1_sock);
    close(s2_sock);

    // Create a socket for incoming client connections
    clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&clnt_addr, 0, sizeof(clnt_addr));
    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_addr.s_addr = INADDR_ANY;
    clnt_addr.sin_port = htons(atoi(argv[1]));
    bind(clnt_sock, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));

    char *p = inet_ntoa(s1_addr.sin_addr);
    char *x = inet_ntoa(s2_addr.sin_addr);
    // Listen for incoming client connections
    listen(clnt_sock, 5);

    fds.fd = clnt_sock;
    fds.events = POLLIN;
    int pollResult;
    int prev = time(NULL);
    int curr;
    int t1;
    int flag = 0;
    int diff = 0;
    while (1)
    {
        // Periodically ask S1 and S2 for their load
        // printf("IIiiiii\n");

        if (flag == 1)
        {
            pollResult = poll(&fds, 1, 5000 - diff * 1000);
        }
        else
        {
            curr = time(NULL);
            pollResult = poll(&fds, 1, 5000);
            // printf("sssss\n");
        }

        // printf("%d-poll result\n", pollResult);
        if (pollResult == -1)
        {
            error("ERROR in poll");
        }
        else if (pollResult == 0)
        {
            // connecting S1 and S2

            s1_sock = socket(AF_INET, SOCK_STREAM, 0);
            s2_sock = socket(AF_INET, SOCK_STREAM, 0);
            memset(&s1_addr, 0, sizeof(s1_addr));
            portnumber = atoi(argv[2]);
            s1_addr.sin_family = AF_INET;
            s1_addr.sin_port = htons(portnumber);
            inet_aton("127.0.0.1", &s1_addr.sin_addr);
            connect(s1_sock, (struct sockaddr *)&s1_addr, sizeof(s1_addr));
            memset(&s2_addr, 0, sizeof(s2_addr));
            portnumber = atoi(argv[3]);
            s2_addr.sin_family = AF_INET;
            s2_addr.sin_port = htons(portnumber);
            inet_aton("127.0.0.1", &s2_addr.sin_addr);
            connect(s2_sock, (struct sockaddr *)&s2_addr, sizeof(s2_addr));
            bzero(buffer, MAX_SIZE);
            strcpy(buffer, "Send Load");
            send_Message(s1_sock, ans, buffer);
            recieve_Message(s1_sock, ans, buffer);
            s1_load = atoi(buffer);
            printf("Load received from %s : %d\n", p, s1_load);
            bzero(buffer, MAX_SIZE);
            strcpy(buffer, "Send Load");
            send_Message(s2_sock, ans, buffer);
            recieve_Message(s2_sock, ans, buffer);
            s2_load = atoi(buffer);
            printf("Load received from %s : %d\n", x, s2_load);
            flag = 0;
            // closing the sockets of s1 and s2
            close(s1_sock);
            close(s2_sock);
        }
        else
        {

            t1 = time(NULL);
            diff = t1 - curr;
            flag = 1;
            clilen = sizeof(clnt_addr);
            newsockfd = accept(clnt_sock, (struct sockaddr *)&clnt_addr,
                               &clilen);

            if (newsockfd < 0)
            {
                printf("Accept error\n");
                exit(0);
            }
            printf("client accepted\n");
            if (fork() == 0)
            {
                if (s1_load < s2_load)
                {
                    // connecting S1
                    s1_sock = socket(AF_INET, SOCK_STREAM, 0);
                    memset(&s1_addr, 0, sizeof(s1_addr));

                    portnumber = atoi(argv[2]);
                    s1_addr.sin_family = AF_INET;
                    s1_addr.sin_port = htons(portnumber);
                    inet_aton("127.0.0.1", &s1_addr.sin_addr);
                    connect(s1_sock, (struct sockaddr *)&s1_addr, sizeof(s1_addr));
                    bzero(buffer, MAX_SIZE);
                    strcpy(buffer, "Send Time");
                    send_Message(s1_sock, ans, buffer);
                    printf("Sending client request to %s\n", p);
                }
                else
                {
                    // connecting S2
                    s2_sock = socket(AF_INET, SOCK_STREAM, 0);
                    memset(&s2_addr, 0, sizeof(s2_addr));
                    portnumber = atoi(argv[3]);
                    s2_addr.sin_family = AF_INET;
                    s2_addr.sin_port = htons(portnumber);
                    // s2_addr.sin_addr.s_addr = INADDR_ANY;
                    inet_aton("127.0.0.1", &s2_addr.sin_addr);
                    connect(s2_sock, (struct sockaddr *)&s2_addr, sizeof(s2_addr));
                    bzero(buffer, MAX_SIZE);
                    strcpy(buffer, "Send Time");
                    send_Message(s2_sock, ans, buffer);
                    printf("Sending client request to %s\n", x);
                }
                bzero(buffer, MAX_SIZE);
                int selected_sock = (s1_load < s2_load) ? s1_sock : s2_sock;
                recieve_Message(selected_sock, ans, buffer);
                close(selected_sock);
                send_Message(newsockfd, ans, buffer);
                exit(0);
            }
        }
    }
    close(s1_sock);
    close(s2_sock);
    close(clnt_sock);

    return 0;
}