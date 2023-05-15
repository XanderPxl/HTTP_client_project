#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
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
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization();
int connection( int internet_socket );
void logs( char * );
void execution( int internet_socket );
void cleanup( int internet_socket, int client_internet_socket );

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
	while (1)
	{
		//////////////
		//Connection//
		//////////////

		client_internet_socket = connection( internet_socket );

		/////////////
		//Execution//
		/////////////

		execution( client_internet_socket );

	}	
		////////////
		//Clean up//
		////////////

		cleanup( internet_socket, client_internet_socket );

		OSCleanup();
	
	return 0;
}

int initialization()
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
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
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			//Step 1.3
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				perror( "bind" );
				close( internet_socket );
			}
			else
			{
				//Step 1.4
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
	//Step 2.1
	
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
		
		char ipAddress[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *)&client_internet_address;
        void *addr = &(sockaddr_ipv6->sin6_addr);
        inet_ntop(AF_INET6, addr, ipAddress, INET6_ADDRSTRLEN);
        //printf("%s\n", ipAddress);
		logs( ipAddress);
		//ip_address = calloc( sizeof(char), strlen("24.48.0.1")/*(strlen(ipAddress))*/ );
		strcpy (ip_address, ipAddress);
		printf("ip_address = %s", ip_address);
	}
	return client_socket;
}

void execution( int internet_socket )
{
	//Step 3.2
	
	//Stream sockets and rcv()
    struct addrinfo HTTP_socket;
    struct addrinfo *res;
	int sockfd;
	
    char get_request[100000];
    char received[100000];
    int byte_count;
	
	//get host info, make socket and connect it
    memset(&HTTP_socket, 0,sizeof HTTP_socket);
	
    HTTP_socket.ai_family=AF_UNSPEC;
    HTTP_socket.ai_socktype = SOCK_STREAM;
	
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
	
	char getRequest[1000000];
	/*
	strcat (get_request, "GET /json/");
	strcat (get_request, ip_address);
	strcat (get_request, " HTTP/1.1\r\n");
	strcat (get_request, 
		"Host: www.ip-api.com\r\n"
        //"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n"
        "\r\n"
	);
	*/
	printf("%s", ip_address);
	snprintf(get_request, 100000,
        "GET /json/%s HTTP/1.1\r\n"
        "Host: www.ip-api.com\r\n"
        //"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n"
        "\r\n", ip_address
    );
	
	printf("\n%s\n", getRequest);
	
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
	printf("%s", received);
	freeaddrinfo(res);
    close(sockfd);
	
}

void cleanup( int internet_socket, int client_internet_socket )
{
	//Step 4.2
	int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	//Step 4.1
	close( client_internet_socket );
	close( internet_socket );
}

void logs( char * ip_address)
{
	FILE *filePointer = fopen( "logs.txt", "a" ); // Ask Operating System to open a file on the file system to write
	fprintf(filePointer, "%s\n", ip_address);
	fclose( filePointer );                        // Let de Operating System know we are no longer needing access to the file
}
