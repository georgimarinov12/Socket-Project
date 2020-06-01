#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

typedef struct {
	struct sockaddr_in address;
	int fd;
	char username[50];
	
} client_t;

client_t *clients[100];

pthread_mutex_t mutex;

void *commands(void *);
void add_client(client_t *);

int main(int argc, char const *argv[]){ 
	int server_fd, client_fd;
	int opt; 
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t tid; 
    
	pthread_mutex_init(&mutex, NULL); 
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
     
        exit(EXIT_FAILURE); 
    } 
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(8080); 
    
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0){ 
         
        exit(EXIT_FAILURE); 
    } 
    
    if (listen(server_fd, 100) < 0){ 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    
    printf("Server start");
    
	while(1){
		socklen_t address_length = sizeof(client_address);
		if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &address_length))<0){ 
		    perror("accept"); 
		    exit(EXIT_FAILURE); 
		}
		
		client_t *client = (client_t*)malloc(sizeof(client_t));
		client->address = client_address;
		client->fd = client_fd;
		strcpy(client->username, "");
		
		add_client(client);
		pthread_create(&tid, NULL, &commands, (void*)client);
    }
    
    pthread_mutex_destroy(&mutex);
    return 0; 
} 

void add_client(client_t *client){
	pthread_mutex_lock(&mutex);
	for(int i = 0; i < 100; ++i){
		if(!clients[i]){
			clients[i] = client;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
}

void *commands(void *arg){
	client_t *client = (client_t*)arg;
	
	char banner[200] = "Command list:\nu - pick username\nm - send message\n"; 
	write(client->fd, banner, strlen(banner));
	 
	char *option[2];
	read(client->fd, option, 1);
	if(strcmp(option[0], "u") == 0){
    	char newsurname[50];
   		
   		for(int i = 1; i == 1;){
	   		write(client->fd, "Username (Character limit 50):\n", 31);
	   		read(client->fd, newsurname, 50);
			
			for(int i = 0; i < 100; ++i){
    			if(strcmp(clients[i]->username, newsurname) == 0){
    				write(client->fd, "Username taken\n", 15);
    				break;
    			}
    			else{
    				strcpy(client->username, newsurname);
    				i = 0;
    			}
    		}
			
    	}
    } else if(strcmp(option[0], "m") == 0){
    	char message[500];
    	char recipient[50];
    	
    	write(client->fd, "Write the username of the recipient, new line, then write your message:\n", 73);
    	
    	read(client->fd, recipient, 50);
    	read(client->fd, message, 500);
    	
    	pthread_mutex_lock(&mutex);
    	for(int i = 0; i < 100; ++i){
    		if(strcmp(clients[i]->username, recipient) == 0){
    			write(clients[i]->fd, message, strlen(message));
    			break;
    		}
    	}
    	pthread_mutex_unlock(&mutex);
    
    }else write(client->fd, "Invalid command!\n", 17);
    
    return NULL;
}
