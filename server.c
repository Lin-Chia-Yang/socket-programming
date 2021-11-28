#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 48763

char* guess(char clientnumber[], char guessnumber[]){
    static char result[1024]={0};
    for(int i=0;i<4;i++){
        result[i]='0';
    }
    char A='0';
    char B='0';
    for(int i=0;i<4;i++){
        if(clientnumber[i]==guessnumber[i]){
            A=A+1;
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(clientnumber[i]==guessnumber[j]){
                if(i==j){
                    break;
                }
                else{
                    B=B+1;
                    break;
                }
            }
        }
    }
    result[0]=A;
    result[1]='A';
    result[2]=B;
    result[3]='B';
    return result;
}
int main()
{
    char client1number[1024]={0};
    char client2number[1024]={0};
    char guessmessage[1024]={0};
    // message buffer
    char buf[1024] = {0};
    int socket_fd = socket(PF_INET , SOCK_DGRAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(serverPort)
    };
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    printf("Server ready!\n");
    struct sockaddr_in clientAddr;
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    int len = sizeof(clientAddr);
    if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0){
    }
    //client1 name
    else{
        printf("client1: %s\n",buf);
        client1=clientAddr;
    }
    if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0){
    }
    //client2 name
    else{
        printf("client2: %s\n",buf);
        client2=clientAddr;
        strcpy(guessmessage,"choose 4 numbers");
        sendto(socket_fd, guessmessage, sizeof(guessmessage), 0, (struct sockaddr *)&client1, sizeof(client1));
        memset(guessmessage, 0, sizeof(guessmessage));
    }
    //client1 enter number
    char buf1[1024]={0};
    char buf2[1024]={0};
    if (recvfrom(socket_fd, buf1, sizeof(buf1), 0, (struct sockaddr *)&clientAddr, &len) < 0){
    }
    else{
        strcpy(client1number,buf1);
        printf("client1 number: %s\n",client1number);
        strcpy(guessmessage,"choose 4 numbers");
        sendto(socket_fd, guessmessage, sizeof(guessmessage), 0, (struct sockaddr *)&client2, sizeof(client2));
        memset(guessmessage, 0, sizeof(guessmessage));
    }
    //client2 enter number
    if (recvfrom(socket_fd, buf2, sizeof(buf2), 0, (struct sockaddr *)&clientAddr, &len) < 0){
    }
    else{
        strcpy(client2number,buf2);
        printf("client2 number: %s\n",client2number);
        strcpy(guessmessage,"guess:");
        sendto(socket_fd, guessmessage, sizeof(guessmessage), 0, (struct sockaddr *)&client1, sizeof(client1));
    }
    char AB[1024]={0};
    char end[1024]={0};
    while (1){
        if(recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0){
            break;
        }
        else{
            //client1 guess
            if(clientAddr.sin_port==client1.sin_port){
                strcpy(AB,guess(client2number,buf));
                memset(buf, 0, sizeof(buf));
                sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client1 , sizeof(client1));
                if(strcmp(AB,"4A0B") == 0){
                    strcpy(end,"client1 win!");
                    sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client1, sizeof(client1));
                    sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client2, sizeof(client2));
                    printf("AB=%s end=%s\n",AB,end);
                }
                else{
                    sendto(socket_fd, guessmessage, sizeof(guessmessage), 0, (struct sockaddr *)&client2, sizeof(client2));
                }
            }
            //client2 guess
            else if(clientAddr.sin_port==client2.sin_port){
                strcpy(AB,guess(client1number,buf));
                memset(buf, 0, sizeof(buf));
                sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client2 , sizeof(client2));
                if(strcmp(AB,"4A0B") == 0){
                    strcpy(end,"client2 win!");
                    sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client1, sizeof(client1));
                    sendto(socket_fd, AB, sizeof(AB), 0, (struct sockaddr *)&client2, sizeof(client2));
                }
                else{
                    sendto(socket_fd, guessmessage, sizeof(guessmessage), 0, (struct sockaddr *)&client1, sizeof(client1));
                }
            }
        }
        if (strcmp(AB, "4A0B") == 0){
            printf("game over, %s\n",end);
            break;
        }
        memset(buf, 0, sizeof(buf));
    }
    if (close(socket_fd) < 0){
        perror("close socket failed!");
    }
    return 0;
}
