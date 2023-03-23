#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
void error_msg(char*msg);
int setup_socket(int IP_ver);
void setup_address(char*SERVER_IP, int SERVER_PORT, struct sockaddr_in6*storeAddr);
void interact_with_server(int serverSock);
int getLink_local_addrSCOPEID(char* LINK_LOCAL_ADDR);
//error message
void error_msg(char*msg){
	fprintf(stderr, "%s %s\n",msg,strerror(errno));
	exit(1);
}

//setup socket with specified SERVER IP, port
int setup_socket(int IP_ver){
	int serverSock;
	serverSock = socket(IP_ver==4 ? PF_INET : PF_INET6 , SOCK_STREAM, 0);
	return serverSock;
}

//setup server address
void setup_address(char*SERVER_IP, int SERVER_PORT, struct sockaddr_in6*storeAddr){
	char SERVER_IP_CHR[50]={0};
	strncpy(SERVER_IP_CHR,SERVER_IP,(size_t)(strchr(SERVER_IP,'%')-SERVER_IP));
	storeAddr->sin6_family = AF_INET6;
	inet_pton(AF_INET6,SERVER_IP_CHR,&storeAddr->sin6_addr);
	storeAddr->sin6_port = htons(SERVER_PORT);
	storeAddr->sin6_scope_id= getLink_local_addrSCOPEID(SERVER_IP);
	return;
}

//interact with server
void interact_with_server(int serverSock){
	char data[100] = {0};
	while(1){
		memset(data,0,sizeof(data));

		//read message from user
		printf("Please enter message : ");
		fgets(data,sizeof(data)-1, stdin);
		data[strlen(data)-1]='\0';

		//send message to server
		send(serverSock, data, strlen(data), 0);
		if(recv(serverSock, data, sizeof(data), 0)<=0){
			break;
		}
		printf("[SERVER] %s\n", data);
	}

	return;
}

int main(int argc, char*argv[]){
	char SERVER_IP[50]={0};
	int SERVER_PORT = 0;

	int serverSock;
	struct sockaddr_in6 serverAddr;
	if(argc!=3){
		error_msg("[Usage] TCP_client SERVER_IP SERVER_PORT");
	}
	strcpy(SERVER_IP, argv[1]);
	SERVER_PORT = atoi(argv[2]);

	serverSock = setup_socket(6);
	setup_address(SERVER_IP,SERVER_PORT,&serverAddr);
	if(connect(serverSock,(struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
		error_msg("[ERROR] Failed to connetc to server. ");
	}

	interact_with_server(serverSock);

	close(serverSock);

	return 0;
}
int getLink_local_addrSCOPEID(char* LINK_LOCAL_ADDR){
	int scope_ID= -1;
	struct addrinfo hints,*info;
	memset(&hints,0,sizeof(hints));
	hints.ai_flags=AI_NUMERICHOST;
	if(getaddrinfo(LINK_LOCAL_ADDR,NULL,&hints,&info)==0){
		struct sockaddr_in6 *sin6_info = (struct sockaddr_in6*)info->ai_addr;
		scope_ID = sin6_info->sin6_scope_id;
		freeaddrinfo(info);
	}
	return scope_ID;
}
