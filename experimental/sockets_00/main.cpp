//#undef _WIN32

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#define SOCKETS_INIT(x,y) WSAStartup( x , y )
#define SOCKETS_FREE WSACleanup
#define SOCKET_CLOSE(x) closesocket( x )

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

#define SOCKETS_INIT(x,y) 0
#define SOCKETS_FREE() 
#define SOCKET_CLOSE(x) close( x )

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PORT "27015"
#define END_ERROR 1

int main ( int argc, char *argv[] )
{
#ifdef _WIN32
	WSADATA wsaData;
#endif
	// Initialize sockets
	int t_initResult = SOCKETS_INIT( MAKEWORD(2,2), &wsaData );
	if ( t_initResult != 0 ) {
		printf( "Failure in sockets to initialize: %d\n", t_initResult );
		return END_ERROR;
	}

	int t_input;
	//printf( "0 server, 1 client" );
	//scanf( "%d", &t_input );
	if ( argc >= 2 && strlen( argv[1] ) > 1 ) {
		t_input = 1;
	}
	else {
		t_input = 0;
	}

	//t_input = getc(stdin) - '0';
	if ( t_input == 0 ) // Server
	{
		printf( "STARTING SERVER...\n" );

		struct addrinfo *result = NULL, *ptr = NULL, hints;

		memset( &hints, 0, sizeof(addrinfo) );
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		/* // UDP
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;
		*/

		// Resolve the local address and port to be used by the server
		if ( getaddrinfo( NULL, DEFAULT_PORT, &hints, &result ) != 0 ) {
			SOCKETS_FREE();
			return END_ERROR;
		}

		// Open a socket
		int listen_fd;
		listen_fd = socket( result->ai_family, result->ai_socktype, result->ai_protocol );

		// Bind the socket to open it for listening
		if ( bind( listen_fd, result->ai_addr, (int)result->ai_addrlen ) != 0 ) {
			freeaddrinfo( result );
			SOCKET_CLOSE( listen_fd );
			SOCKETS_FREE();
			return END_ERROR;
		}

		// Free address information
		freeaddrinfo( result );

		// Now we listen on the socket
		if ( listen( listen_fd, SOMAXCONN ) != 0 ) {
			//printf( "Listen failed with error: %ld\n", WSAGetLastError() );
			SOCKET_CLOSE( listen_fd );
			SOCKETS_FREE();
			return END_ERROR;
		} // not used for udp

		while ( true )
		{
			// Now, spin and tell people to bitch
			int client_fd;
			client_fd = accept( listen_fd, NULL,NULL );

			// Receive until the peer shuts down the connection
			int t_recvResult;
			do
			{
				char t_recvbuffer[512];
				int t_recvsize = 512;
				t_recvResult = recv( client_fd, t_recvbuffer, 512, 0 );
				if ( t_recvResult > 0 )
				{
					t_recvsize = t_recvResult;
					printf( "Bytes RX: %d\n", t_recvsize );
					// Print the received data to the screen
					t_recvbuffer[t_recvsize] = 0;
					printf( "%s\n", t_recvbuffer );

					// Send information back to the client
					char t_sendbuffer[512];
					sprintf( t_sendbuffer, "Server received %d bytes of data\n", t_recvsize );

					int t_sendResult = send( client_fd, t_sendbuffer, strlen(t_sendbuffer), 0 );
					if ( t_sendResult == -1 )
					{
						printf( "Send failed!\n" );
						SOCKET_CLOSE( client_fd );
						SOCKETS_FREE();
						return END_ERROR;
					}
					printf( "Bytes TX: %d\n", t_sendResult );
				}
				else if ( t_recvResult == 0 ) 
				{
					printf( "Connection closing...\n" );
				}
				else
				{
					//printf( "RX failed with error: %ld\n", WSAGetLastError() );
					printf( "Failure in RX\n" );
					SOCKET_CLOSE( client_fd );
					SOCKETS_FREE();
					return END_ERROR;
				}

			} while ( t_recvResult > 0 );

			SOCKET_CLOSE( client_fd );
		}
	}
	else if ( t_input == 1 ) // Client
	{
		printf( "STARTING CLIENT...\n" );

		struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

		memset( &hints, 0, sizeof(addrinfo) );
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		/* // UDP
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		*/

		// Resolve the server address and port
		getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
		// Attempt to connect to the first address returned by
		// the call to getaddrinfo
		ptr = result;

		// Open a socket
		int socket_fd;
		socket_fd = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
		
		// Connect to server.
		if ( connect( socket_fd, ptr->ai_addr, (int)ptr->ai_addrlen) != 0 ) {
			SOCKET_CLOSE(socket_fd);
			SOCKETS_FREE();
			return END_ERROR;
		}

		// Should really try the next address returned by getaddrinfo
		// if the connect call failed
		// But for this simple example we just free the resources
		// returned by getaddrinfo and print an error message
		freeaddrinfo(result);


		// Send initial buffer
		// Send information back to the client
		char t_sendbuffer[512];
		sprintf( t_sendbuffer, "Testing packet of data" );

		int t_sendResult = send( socket_fd, t_sendbuffer, strlen(t_sendbuffer), 0 );
		if ( t_sendResult < 0 )
		{
			printf( "Send failed!\n" );
			SOCKET_CLOSE( socket_fd );
			SOCKETS_FREE();
			return END_ERROR;
		}
		printf( "Bytes TX: %d\n", t_sendResult );

		// shutdown the connection for sending since no more data will be sent
		// the client can still use the ConnectSocket for receiving data
		/*if ( shutdown( socket_fd, SD_SEND ) != 0 ) {
			printf( "shutdown() failed\n" );
			SOCKET_CLOSE( socket_fd );
			SOCKETS_FREE();
			return 1;
		}*/

		// Receive data until the server closes the connection
		int t_recvResult;
		do
		{
			char t_recvbuffer[512];
			int t_recvsize = 512;
			t_recvResult = recv( socket_fd, t_recvbuffer, 512, 0 );
			if ( t_recvResult > 0 )
			{
				t_recvsize = t_recvResult;
				printf( "Bytes RX: %d\n", t_recvsize );
				// Print the received data to the screen
				t_recvbuffer[t_recvsize] = 0;
				printf( "%s\n", t_recvbuffer );
			}
			else if ( t_recvResult == 0 )
			{
				printf( "Connection closed\n" );
			}
			else
			{
				printf( "recv failed\n" );
			}

		} while ( t_recvResult > 0 );

		// Close the client
		SOCKET_CLOSE( socket_fd );
	}

	// Free sockets
	SOCKETS_FREE();

	return 0;
}
