#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

void error_msg(char*msg);

int setup_socket(int IP_ver);

int getLink_local_addrSCOPEID(char* LINK_LOCAL_ADDR);

void setup_address(char*SERVER_IP, int SERVER_PORT, struct sockaddr_in6*storeAddr);

void interact_with_client(int serverSock);

int main(int argc, char*argv[]){
	//arguments
	char SERVER_IP[50] = {0};
	int SERVER_PORT = 0;

	//socket
	int serverSock;
	struct sockaddr_in6 serverAddr;

	if(argc!=3){
		error_msg("[Usage] TCP_server SERVER_IP SERVER_PORT");
	}
	strcpy(SERVER_IP, argv[1]);
	SERVER_PORT = atoi(argv[2]);

	serverSock = setup_socket(6);
	setup_address(SERVER_IP, SERVER_PORT, &serverAddr);
	if(bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) <0){
		error_msg("[ERROR] Failed to bind.");
	}
	listen(serverSock, 2);
	printf("server setup\n");

	interact_with_client(serverSock);

	close(serverSock);
	return 0;
}

void error_msg(char*msg){
	fprintf(stderr,"%s %s\n", msg, strerror(errno));
	exit(1);
}

int setup_socket(int IP_ver){
	int socketFd;
	int sockopt= 1;
	socketFd = socket(IP_ver==4 ? PF_INET : PF_INET6 , SOCK_STREAM, 0);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
	return socketFd;
}



void setup_address(char*SERVER_IP, int SERVER_PORT, struct sockaddr_in6*storeAddr){
	char SERVER_IP_CHR[50]={0};
	strncpy(SERVER_IP_CHR,SERVER_IP,(size_t)(strchr(SERVER_IP,'%')-SERVER_IP));
	
	storeAddr->sin6_family = AF_INET6; // AF_INET6
	inet_pton(AF_INET6,SERVER_IP_CHR,&storeAddr->sin6_addr);
	storeAddr->sin6_port =htons(SERVER_PORT);
	storeAddr->sin6_scope_id=getLink_local_addrSCOPEID(SERVER_IP);
	
	return;
}

void interact_with_client(int serverSock){
	int clientSock = 0;
	struct sockaddr_in6 clientAddr;
	int clientAddrLength = sizeof(clientAddr);
	
	char clientAddr_str[50]={0};
	
	char msg[BUFSIZ] = {0};

	while(1){

		clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLength);
		printf("[INFO] Connection from %s[%d]\n", inet_ntop(AF_INET6,&clientAddr.sin6_addr,clientAddr_str,sizeof(clientAddr_str)), ntohs(clientAddr.sin6_port));
		
		while(1){
			memset(msg, 0, sizeof(msg));
			if(recv(clientSock, &msg, sizeof(msg),0) <= 0){
				printf("[INFO] Client disconnected.\n");
				break;
			}
			printf("[CLIENT] %s\n", msg);

			strcat(msg, "[server readed]");
			send(clientSock, msg, strlen(msg), 0);
		}
	}
	return;
}


int getLink_local_addrSCOPEID(char* LINK_LOCAL_ADDR){
	int scope_ID=-1;
	
	struct addrinfo hints,*info;
	
	memset(&hints,0,sizeof(hints));
	
	hints.ai_flags=AI_NUMERICHOST;
	
	if(getaddrinfo(LINK_LOCAL_ADDR,NULL,&hints,&info)==0){
		struct sockaddr_in6 *sin6_info=(struct sockaddr_in6*)info->ai_addr;
		
		scope_ID=sin6_info->sin6_scope_id;
		freeaddrinfo(info);
	}
	return scope_ID;
}





