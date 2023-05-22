#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include<windows.h>
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData ); 
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}
	#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
	#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include<unistd.h>
	
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization();
int connection( int internet_socket );
void logs( char * );
void parse( char * );
void execution();
void sendGarbage( int internet_socket );
void cleanup( int );
char * strsep (char **stringp, const char *delim);

char ip_address[16];

int main( int argc, char * argv[] )
{
	//////////////////
	//Initialization//
	//////////////////
	int internet_socket = 0;
	int client_internet_socket = 0;
	
	
	OSInit();
	internet_socket = initialization();
	//vraag 2
	while (1)
	{
		//////////////
		//Connection//
		//////////////

		client_internet_socket = connection( internet_socket );

		/////////////
		//Execution//
		/////////////
		
		execution();
		sendGarbage( client_internet_socket );
		// deel van vraag 11
		cleanup(client_internet_socket);
		//break;
	}	
		////////////
		//Clean up//
		////////////

		cleanup( internet_socket );

		OSCleanup();
	
	return 0;
}

int initialization()
{
	
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	// Vraag 1
	int getaddrinfo_return = getaddrinfo( NULL, "22", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				perror( "bind" );
				close( internet_socket );
			}
			else
			{
				
				int listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					close( internet_socket );
					perror( "listen" );
				}
				else
				{
					break;
				}
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}

	return internet_socket;
}

int connection( int internet_socket )
{
	
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( client_socket == -1 )
	{
		perror( "accept" );
		close( internet_socket );
		exit( 3 );
	}
	else
	{
		// Vraag 3 en 4
		char ipAddress[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *)&client_internet_address;
        void *addr = &(sockaddr_ipv6->sin6_addr);
        inet_ntop(AF_INET6, addr, ipAddress, INET6_ADDRSTRLEN);
		logs( ipAddress);
		strcpy (ip_address, ipAddress);
		
	}
	return client_socket;
}


void execution()
{

    struct addrinfo HTTP_socket;
    struct addrinfo *res;
	int sockfd;
	
    char get_request[100000];
    char received[100000];
    //int byte_count;
	
	//get host info, make socket and connect it
    memset(&HTTP_socket, 0,sizeof HTTP_socket);
	
    HTTP_socket.ai_family=AF_UNSPEC;
    HTTP_socket.ai_socktype = SOCK_STREAM; //TCP connection!
	// vraag 5 en 6
    int result = getaddrinfo("ip-api.com","80", &HTTP_socket, &res);
	
    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if( sockfd == -1 )
	{
		perror( "Connection socket error" );
		close( sockfd );
		exit( 1 );
	}
	
    result = connect(sockfd,res->ai_addr,res->ai_addrlen);
    if( result == -1 )
	{
		perror( "Connection error" );
		close( sockfd );
		exit( 1 );
	}
	// send get request
	char getRequest[1000000];
	
	// vraag 7
	snprintf(get_request, 100000,
        "GET /json/%s HTTP/1.1\r\n"
        "Host: www.ip-api.com\r\n"
        //"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n"
        "\r\n", ip_address
    );
	
	printf("\n%s\n", getRequest);
	
	// receive the getrequest
	// Vraag 8
	result = send(sockfd, get_request, strlen(get_request), 0);
	if( result == -1 )
	{
		perror( "send error" );
		close( sockfd );
		exit( 1 );
	}
	
	result = recv(sockfd, received, 100000 - 1, 0);
	if( result == -1 )
	{
		perror( "receive error" );
		close( sockfd );
		exit( 1 );
	}
	result = recv(sockfd, received, 100000 - 1, 0);
	if( result == -1 )
	{
		perror( "receive error" );
		close( sockfd );
		exit( 1 );	
	}
	// vraag 9, 10, 11???
	logs( received );
	printf("%s", received);
	
	// free the allocated memory
	
	freeaddrinfo(res);
    close(sockfd);
	
}

// vraag 13
void sendGarbage( int internet_socket )
{
	
	
	
	int bytes_send = 0;

	while (bytes_send != -1)
	{
		printf("sending_bytes\n");
		bytes_send = send( internet_socket, "The sun shone down on the lush green fields, casting long shadows as the gentle breeze rustled through the trees. A few birds chirped in the distance, adding to the peaceful ambiance. In the nearby town, people went about their daily business, with some stopping to chat with their neighbors or grab a bite to eat at a local cafe. Meanwhile, a few kids played a game of catch in the park, laughing and shouting as they ran after the ball. As the day wore on, the sun began to set, painting the sky with hues of orange, pink, and purple. The air grew cooler, and the stars began to twinkle in the night sky. In the quiet of the evening, the only sounds were the occasional cricket chirp and the distant hum of a passing car. It was a simple, yet beautiful day, full of the joys of life.", 1000, 0 );
		printf("bytes_send = %d\n", bytes_send);
		
		if( bytes_send == -1 )
		{
			perror( "send" );
		}
		sleep(1);
		
	}	
	
}


void cleanup( int socket)
{
	int shutdown_return = shutdown( socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	
	close( socket );
}

void logs( char * ip_address)
{
	FILE *filePointer = fopen( "logs.txt", "a" ); // Ask Operating System to open a file on the file system to write
	fprintf(filePointer, "IP address from client = %s\n", ip_address);
	fclose( filePointer );                        // Let de Operating System know we are no longer needing access to the file
}

void parse( char * received )
{
	char *parsePT = NULL;
	char *startToken = strtok(&parsePT[0], "{");
	if (startToken != NULL) 
	{
		char result[10000];
		//result = startToken;
		  	
	}

	
}

/* Copyright (C) 1992-2023 Free Software Foundation, Inc.
      This file is part of the GNU C Library.
    
       The GNU C Library is free software; you can redistribute it and/or
       modify it under the terms of the GNU Lesser General Public
       License as published by the Free Software Foundation; either
       version 2.1 of the License, or (at your option) any later version.
    
       The GNU C Library is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
      Lesser General Public License for more details.
   
      You should have received a copy of the GNU Lesser General Public
      License along with the GNU C Library; if not, see
      <https://www.gnu.org/licenses/>.  */

char * strsep (char **stringp, const char *delim)
   {
     char *begin, *end;
   
     begin = *stringp;
     if (begin == NULL)
       return NULL;
   
     /* Find the end of the token.  */
     end = begin + strcspn (begin, delim);
   
     if (*end)
       {
         /* Terminate the token and set *STRINGP past NUL character.  */
         *end++ = '\0';
         *stringp = end;
       }
     else
       /* No more delimiters; this is the last token.  */
       *stringp = NULL;
   
     return begin;
   }
