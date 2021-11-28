#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define serverPort 48763
#define serverIP "127.0.0.1"

int legal(char number[]){
    if(strlen(number)!=4){
        return 0;
    }
    int count=1;
    for(int i=0;i<strlen(number);i++){
        for(int j=0;j<strlen(number);j++){
            if(i!=j && number[i]==number[j]){
                return 0;
            }
        }
    }
    for(int i=0;i<strlen(number);i++){
        if(number[i]-'0' < 0 && number[i]-'0' > 9){
            return 0;
        }
    }
    return 1;
}
int main()
{
    char buf[1024] = {0};
    char recvbuf[1024] = {0};
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        printf("Create socket fail!\n");
        return -1;
    }
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(serverIP),
        .sin_port = htons(serverPort)
    };
    int len = sizeof(serverAddr);
    printf("Please input your user name: ");
    scanf("%s", buf);
    sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    memset(buf, 0, sizeof(buf));
    printf("wait for client2...\n");
    if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0){
            printf("recvfrom data from %s:%d, failed!\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
        }
    else{
            printf("choose  numbers:\n");
            while(1){
                scanf("%s", buf);
                if(legal(buf)==1){
                    break;
                }
                else{
                    printf("input error\n");
                }
            }
            sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            printf("wait for client2 choosing numbers...\n");
            memset(buf, 0, sizeof(buf));
        }
    while (1){
        if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0){
            break;
        }
        else{
            if(strcmp(recvbuf,"4A0B") == 0){
                printf("you lose!\n");
                break;
            }
            printf("%s\n",recvbuf);
            while(1){
                scanf("%s", buf);
                if(legal(buf)==1){
                    break;
                }
                else{
                    printf("input error\n");
                }
            }
            //send to server
            sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            //wait result
            if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0){
                break;
            }
            //print result
            else{
                printf("result: %s\n",recvbuf);
            }
            if(strcmp(recvbuf,"4A0B") == 0){
                if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0){
                break;
                }
                else{
                    printf("you win!\n");
                }
                break;
            }
            else{
                printf("wait for client2...\n");
            }
            memset(buf, 0, sizeof(buf));

        }
    }
    if (close(socket_fd) < 0) {
        perror("close socket failed!");
    }
    return 0;
}
