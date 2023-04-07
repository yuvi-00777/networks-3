/*
            NETWORK PROGRAMMING WITH SOCKETS

In this program we illustrate the use of Berkeley sockets for interprocess
communication across the network. We show the communication between a server
process and a client process.


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
/* THE SERVER PROCESS */
#define BUF_SIZE 15
#define MAX_SIZE 1000
void send_Message(int newsockfd, char buf[], char command[])
{
    // strcat(command, "\0");
    int l = strlen(command), i = 0, j = 0;
    while (j < l)
    {
        bzero(buf, BUF_SIZE);
        i = 0;
        while (i < BUF_SIZE && j < l)
        {
            buf[i++] = command[j++];
        }
        if (j == l)
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
int main(int argc, char *argv[])
{
    srand(time(0));
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;
    int load;
    int i;
    char buf[MAX_SIZE]; /* We will use this buffer for communication */
    char ans[MAX_SIZE];
    /* The following system call opens a socket. The first parameter
       indicates the family of the protocol to be followed. For internet
       protocols we use AF_INET. For TCP sockets the second parameter
       is SOCK_STREAM. The third parameter is set to 0 for user
       applications.
    */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    /* The structure "sockaddr_in" is defined in <netinet/in.h> for the
       internet family of protocols. This has three main fields. The
       field "sin_family" specifies the family and is therefore AF_INET
       for the internet family. The field "sin_addr" specifies the
       internet address of the server. This field is set to INADDR_ANY
       for machines having a single IP address. The field "sin_port"
       specifies the port number of the server.
    */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    /* With the information provided in serv_addr, we associate the server
       with its port using the bind() system call.
    */
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    listen(sockfd, 5); /* This specifies that up to 5 concurrent client
                  requests will be queued up while the system is
                  executing the "accept" system call below.
               */

    /* In this program we are illustrating an iterative server -- one
       which handles client connections one by one.i.e., no concurrency.
       The accept() system call returns a new socket descriptor
       which is used for communication with the server. After the
       communication is over, the process comes back to wait again on
       the original socket descriptor.
    */
    while (1)
    {

        /* The accept() system call accepts a client connection.
           It blocks the server until a client request comes.

           The accept() system call fills up the client's details
           in a struct sockaddr which is passed as a parameter.
           The length of the structure is noted in clilen. Note
           that the new socket descriptor returned by the accept()
           system call is stored in "newsockfd".
        */
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           &clilen);

        if (newsockfd < 0)
        {
            perror("Accept error\n");
            exit(0);
        }

        /* We initialize the buffer, copy the message to it,
            and send the message to the client.
        */

        recieve_Message(newsockfd, ans, buf);
        if (strcmp(buf, "Send Load") == 0)
        {
            // Generate a random dummy load
            bzero(buf, MAX_SIZE);
            load = rand() % 100 + 1;
            printf("Load sent: %d\n", load);
            sprintf(buf, "%d", load);
            // Send the load to the load balancer
            sendtooo(buf, newsockfd);
        }
        else if (strcmp(buf, "Send Time") == 0)
        {
            // Fulfill the service request and send the date and time to the load balancer
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char *time_str = asctime(tm);
            sendtooo(time_str, newsockfd);
        }
        close(newsockfd);

        // continue;
        /* We now receive a message from the client. For this example
           we make an assumption that the entire message sent from the
           client will come together. In general, this need not be true
           for TCP sockets (unlike UDPi sockets), and this program may not
           always work (for this example, the chance is very low as the
           message is very short. But in general, there has to be some
           mechanism for the receiving side to know when the entire message
          is received. Look up the return value of recv() to see how you
          can do this.
        */
        // recv(newsockfd, buf, 100, 0);
        // printf("%s\n", buf);

        // close(newsockfd);
    }
    return 0;
}