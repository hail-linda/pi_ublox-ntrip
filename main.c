#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main()
{

int tcp_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (tcp_socket == -1) {
        printf("不能打开套接字");
	return 0;
    }

printf("%d\n",23);
return 0;
}
