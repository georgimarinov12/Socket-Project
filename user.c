#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
   
void commands();
void *receive();
   
int sock = 0;
struct sockaddr_in serv_addr;
   
int main(int argc, char const *argv[]){ 
    pthread_t receiving_thread;    
    
        
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(8080); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    
    pthread_create(&receiving_thread, NULL, receive, NULL);
    commands();
    
    return 0; 
} 

void *receive(){
	while(1){
		char message[500];
		char username[50];
		char warning[30];
		char warning_num[15];
	
		read(sock, warning_num, 2);
		sleep(1);
		
		if(warning_num[0] == '0'){
			read(sock, warning, 30);
			printf("%s\n", warning);
		} else{
			read(sock, username, 50);
			printf("%s:\n", username);
		
			sleep(1);
			read(sock, message, 500);
			printf("%s\n", message);	
		}
		
		memset(message, 0, strlen(message));
		memset(username, 0, strlen(message));
		
	}
	
	pthread_exit(NULL);
	return NULL;
}

void commands(){
	char option[2];
	while(1){
		printf("Command list:\nu - pick username\nm - send message\nq - quit\n");
		scanf("%s", option);
		if(option[0] == 'u'){
			send(sock, option, strlen(option)+1, 0);
			
			char newsurname[50];
	   		
		   	printf("Username (Character limit 50):\n");
		   	scanf("%s", newsurname);
			
			send(sock, newsurname, strlen(newsurname)+1, 0);
		
		} else if(option[0] == 'm'){
			send(sock, option, strlen(option)+1, 0);
		
			char message[500];
			char recipient[50];
			
			printf("Write the username of the recipient, new line, then write your message:\n");
			
			scanf("%s", recipient);
			scanf("%s", message);
			
			send(sock, recipient, strlen(recipient)+1, 0);
			sleep(1);
			send(sock, message, strlen(message)+1, 0);
		
		} else if(option[0] == 'q'){
			break;
		
		} else printf("Invalid command!\n");
	}
	
	pthread_exit(NULL);
	return;
}
