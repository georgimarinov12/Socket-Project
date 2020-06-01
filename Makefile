all: server.c user.c
	gcc -pthread -Wall -pedantic -std=c11 server.c -o server && gcc -Wall -pedantic -std=c11 user.c -o user

server: server.c
	gcc -pthread -Wall -pedantic -std=c11 server.c -o server
	
user: user.c
	gcc -Wall -pedantic -std=c11 user.c -o user
