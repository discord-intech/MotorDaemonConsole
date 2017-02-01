#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <zconf.h>

#define SOCKET_PORT 56987
#define BUFFER_MAX_SIZE 2048

void connecting ( in_port_t port, const char * hostname );
void* readerWorker(void*);

int sock;
pthread_t t;


void signalHandler(int sign)
{
    if(sign == SIGINT)
    {
        pthread_cancel(t);
        close(sock);
        exit(0);
    }
}

int main()
{

    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        printf("\ncan't catch SIGINT\n");
    }

    connecting (htons(SOCKET_PORT), "localhost") ;

    pthread_create(&t, NULL, readerWorker, NULL);

    printf("MotorDaemon detected on localhost:%d\nMotorDaemonConsole is ready\n", SOCKET_PORT);

    for( ; ; )
    {
        char string[BUFFER_MAX_SIZE];
        fgets (string, BUFFER_MAX_SIZE, stdin);

        if(string[strlen(string)-1] == '\n') //Remove the newline (MotorDeamon's reading it like an idiot)
        {
            string[strlen(string)-1] = '\0';
        }

        if(string[strlen(string)-1] != '\r')
        {
            string[strlen(string)-1] = '\r';
        }

        write(sock, string, sizeof(string));

        if (strstr(string, "exit") != NULL)
        {
            signalHandler(SIGINT);
            exit(0);
        }
    }

}

void* readerWorker(void* null)
{

    for( ; ; )
    {
        char buf[BUFFER_MAX_SIZE];
        ssize_t bytes = recv(sock, buf, sizeof(buf), 0);
        if(bytes < BUFFER_MAX_SIZE)
        {
            buf[bytes] = '\0';
        }

        printf("\n%s\n", buf);
    }

    return NULL;
}


void connecting ( in_port_t port, const char * hostname )
{
    sock = socket ( AF_INET, SOCK_STREAM, 0 ) ;
    if ( sock == -1 ) {
        perror ( "socket" ) ;
        exit ( 1 ) ;
    }

    // Search for the host name.
    struct hostent * hostent ;
    hostent = gethostbyname ( hostname ) ;
    if ( ! hostent ) {
        fprintf ( stderr, "Problem with 'gethostbyname'.\n" ) ;
        exit ( 1 ) ;
    }

    // Initialisation of the sockaddr_in data structure.
    struct sockaddr_in addr ;
    memset ( & addr, 0, sizeof ( struct sockaddr_in ) ) ;
    addr.sin_family = AF_INET ;
    addr.sin_port = port;
    addr.sin_addr.s_addr = ( ( struct in_addr * ) ( hostent -> h_addr ) ) -> s_addr ;

    // Name the socket.
    int code ;
    code = connect ( sock, ( struct sockaddr * ) & addr, sizeof ( struct sockaddr_in ) ) ;
    if ( code == -1 ) {
        perror ( "connect" ) ;
        exit ( 1 ) ;
    }
}