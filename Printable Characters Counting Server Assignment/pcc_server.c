#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <signal.h>  

int busy = 0;
int statistics[95];


void sigint_handler(){
	while(busy){}
	for(int i=0;i<95;i++){
	printf("char ’%c’ : %u times\n", i+32,statistics[i]);
	}
	exit(0);
}

int main(int argc, char const *argv[]) 
{ 
	signal(SIGINT, &sigint_handler);   
	if(argc<2){
		printf("Need 1 main arguments\n");
		return 1;
	}
	int server_fd, new_socket; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	int num_will_get;
	char buffer[1024] = {0}; 
	int server_port = atoi(argv[1]);
	int num_of_printable_char = 0;
	
	for(int i=0;i<95;i++){
		statistics[i]=0;
	}
	
	
// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
// Forcefully attaching socket to the port we want 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( server_port ); 
	
// Forcefully attaching socket to the port we want  
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	while(1){
		busy = 0;
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
						(socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		busy = 1;
		num_of_printable_char=0;
		int valread = read( new_socket , buffer, 1024);
		if (valread==0){
			//do nothing!
		}
		num_will_get = atoi(buffer);
		char* recieve_buff = (char*)malloc(num_will_get*sizeof(char));
		int bytes_to_read = num_will_get;
		int totalread = 0;
		char current_char;
		int ascci;
		int bytes_read;
		while( bytes_to_read>0) {
			bytes_read = read(new_socket,recieve_buff+totalread, bytes_to_read);
			if( bytes_read <= 0 )
				break;
			bytes_to_read-=bytes_read;
			totalread+=bytes_read;
		}
		for(int i=0;i<num_will_get;i++){
			current_char=recieve_buff[i];
			ascci = (int)current_char;
			if(ascci>=32 && ascci<=126){
				num_of_printable_char++;
				statistics[ascci-32]=statistics[ascci-32]+1;
			}
		}
		free(recieve_buff);
		char sendback[24];
		sprintf(sendback,"%d", num_of_printable_char);
		send(new_socket , sendback , strlen(sendback) , 0 ); 
	}
} 