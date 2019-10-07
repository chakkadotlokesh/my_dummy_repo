//gcc two.c -lpthread

#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<semaphore.h>
#include<stdbool.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>

const uint16_t SERVER_PORT=1000;
const uint16_t THREAD_COUNT = 10;
struct thread_param
{
	_Atomic _Bool available;
	int clt_conn_fd;
	pthread_t thread;
	sem_t sem;
}*head;

void error_occurred( char const * const msg )
{
	char error_string[200] = "";
	strerror_r( errno, error_string, 200 );
	fprintf( stderr, "Error: %s\n", msg );
	fprintf( stderr, "Message indicated by errno(%d) : %s\n\n", errno, error_string );
	raise(SIGABRT);
}

_Bool flag = true;

void *thread_function( void *p )
{
	struct thread_param *param = p;
	while( flag )
	{
		if( sem_wait( &param->sem ) )
			error_occurred( "Unable to wait on semaphore" );
//actual processing comes here.....
		close( param->clt_conn_fd );
		param->available = true;
	}
	return NULL;
}

void initialize_thread_params()
{
	head = calloc( sizeof(struct thread_param), THREAD_COUNT );
	if( head == NULL )
		error_occurred( "Unable to allocate memory" );
	for( uint16_t i=0; i<THREAD_COUNT; i++ )
	{
		if( sem_init( &head[i].sem, 0, 0 ) )
			error_occurred( "Unable to initialize semaphore" );
		head[i].available = true;
		if( pthread_create( &head[i].thread, NULL, thread_function, &head[i] ) )
			error_occurred( "Unable to create thread" );
	}
}

int main()
{
	initialize_thread_params();
	const int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sockfd == -1 )
		error_occurred( "Unable to create socket" );

	struct sockaddr_in servaddr =
	{
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(SERVER_PORT)
	}; 
	if( bind( sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) ) == -1 )
		error_occurred( "Unable to bind the socket" );

	if( listen( sockfd, 10 ) == -1 )
		error_occurred( "Unable to listen on the socket" );

	struct sockaddr_in cliaddr;
	int cliaddr_len = sizeof(cliaddr);
	while(flag)
	{
		const int connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cliaddr_len );
		if( connfd == -1 )
			error_occurred( "Unable to accept the socket" );
		for( uint16_t i=0; flag; i = ((i+1)%THREAD_COUNT) )
		{
			if( head[i].available )
			{
				head[i].clt_conn_fd = connfd;
				head[i].available = false;
				if( sem_post( &head[i].sem ) == -1 )
					error_occurred( "Unable to sem_post" );
				break;
			}
		}
	}
	return 0;
}
