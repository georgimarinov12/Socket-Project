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
int client_count = 0;

pthread_mutex_t mutex;

void *commands(void *);
void add_client(client_t *);

int main(int argc, char const *argv[]){ 
	int server_fd, client_fd;
	int opt; 
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t *tid = malloc(sizeof(pthread_t) * 100);
    int i = 0; 
    
	pthread_mutex_init(&mutex, NULL); 
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
     
        exit(EXIT_FAILURE); 
    } 
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){ 
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
    
    printf("Server start\n");
    
	while(1){
		socklen_t address_length = sizeof(client_address);
		if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &address_length))<0){ 
		    perror("accept"); 
		    exit(EXIT_FAILURE); 
		}
		
		client_t *client = (client_t*)malloc(sizeof(client_t));
		client->address = client_address;
		client->fd = client_fd;
		strcpy(client->username, "Anonymous");
		
		add_client(client);
		pthread_create(&tid[i], NULL, &commands, (void*)client);
    	++i;
    }
    
    pthread_mutex_destroy(&mutex);
    return 0; 
} 

void add_client(client_t *client){
	pthread_mutex_lock(&mutex);
	for(int i = 0; i < 100; ++i){
		if(!clients[i]){
			clients[i] = client;
			client_count++;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
}

void *commands(void *arg){
	client_t *client = (client_t*)arg;
	 
	while(1){
		char option[2];
		read(client->fd, option, 2);
		
		if(option[0] == 'u'){
			char newsurname[50];
	   		
		   	read(client->fd, newsurname, 50);
			strcpy(client->username, newsurname);
			printf("New user: %s\n", client->username);

		} else if(option[0] == 'm'){
			int check_num = 1;
			
			while(check_num == 1){
				char cli_username[50];
				strcpy(cli_username, client->username);
				
				char message[500];
				char recipient[50];
				char *warning = "No such user exists!";
				char warning_num[2];
				
				read(client->fd, recipient, 50);
				sleep(1);
				read(client->fd, message, 500);
				
				pthread_mutex_lock(&mutex);
				for(int i = 0; i < 100; ++i){
					if(strcmp(clients[i]->username, recipient) == 0){
						strcpy(warning_num, "1");
						send(clients[i]->fd, warning_num, strlen(warning_num), 0);
						sleep(1);
						send(clients[i]->fd, cli_username, strlen(cli_username), 0);
						sleep(1);
						send(clients[i]->fd, message, strlen(message), 0);
						check_num = 0;
						break;
					}
					if(i == client_count){
						strcpy(warning_num, "0");
						send(clients[i]->fd, warning_num, strlen(warning_num), 0);
						sleep(1);
						send(client->fd, warning, strlen(warning), 0);
						break;
					}
				}
				pthread_mutex_unlock(&mutex);
			}
		} else if(option[0] == 'q') break;
	}
    
    pthread_exit(NULL);
    return NULL;
}
