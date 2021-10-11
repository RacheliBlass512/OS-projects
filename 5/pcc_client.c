#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <assert.h>


int main(int argc, char const *argv[]) 
{ 
	if(argc<4){
		printf("Need 3 main arguments\n");
		return 1;
	}
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	char num_to_send[10];
	const char* ip_adrres = argv[1];
	int server_port = atoi(argv[2]);
	const char* file_to_open=argv[3];

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(server_port); 
	
// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, ip_adrres, &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	FILE* file1 = fopen(file_to_open, "r");
	if (NULL == file1)
	{
		perror("error in opening file");
		exit(1);
	}	
	fseek(file1, 0L, SEEK_END);
	int sz = ftell(file1);
	fseek(file1, 0L, SEEK_SET);
//convert sz to char, num_to_send
	sprintf(num_to_send, "%d", sz);
	
	send(sock , num_to_send , strlen(num_to_send) , 0 ); 

//write to buffer the cintent of the file
	char* bufdata = (char*)malloc(sz*sizeof(char));
	char* res = fgets(bufdata,sz+1, (FILE*)file1); 
	if (NULL == res && !feof(file1))
 	{
 		perror("Reading error occurred.\n");
 		exit(1);
	}

	fclose(file1);
	int nsent;
	int totalsent = 0;
	int notwritten = sz;
	
	while( notwritten > 0 ){
		nsent = write(sock, bufdata + totalsent, notwritten);
		assert( nsent >= 0);
      	totalsent  += nsent;
      	notwritten -= nsent;
	}
	free(bufdata);
	char printable[10];
	int nread = read(sock,printable, 10);
	printable[nread]='\0';
	int num_printable = atoi(printable);
	printf("# of printable characters: %u\n",num_printable);

	return 0; 
} 