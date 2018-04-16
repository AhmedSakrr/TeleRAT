#include "include/common.h"
#include "tcp_server.h"
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <pthread.h>

#define OP_START '\\x02'
#define OP_STOP "\x03"

Telegram_chat chat;

void *connect_handle(void * temp_struct);
void *telegram_serv(void *vargp);

struct sendto_function {
    int *client_soc;
    char *ip_client;
}sendto_function;

int wow;
pthread_mutex_t stopParin;


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c, *new_sock;
    struct sockaddr_in server , client;
    
    int potnumber_server;

    potnumber_server = atoi(argv[1]); 
    if (2 != argc) {

        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);

    } 

    pthread_t tid;
    if(pthread_create(&tid, NULL, telegram_serv, NULL) < 0){
        perror("Can't create thread for Telegram\n");
    }
    printf("Thread for telegram created.\n");
    
    //Create socket
    //If cannot create socket it return value of socket_desc valuable = -1
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(potnumber_server);

    if (pthread_mutex_init(&stopParin, NULL) != 0)
    {
        printf("\n Mutex init ERROR! \n");
        return 1;
    }

    //Bind
    //bind() associates the socket with its local address [that's why server side binds, so that clients can use that address to connect to server.] connect() is used to connect to a remote [server] address
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    printf("bind done");
     
    //Listen
    //Waiting connection form client
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    struct sendto_function send_to_function;
    
    while ( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        
        //accept connection form client complete
        pthread_t server_serv;
        send_to_function.ip_client = inet_ntoa(client.sin_addr);
        send_to_function.client_soc = malloc(sizeof *send_to_function.client_soc);
        *send_to_function.client_soc = client_sock;
        if( pthread_create( &server_serv , NULL ,  connect_handle , (void*) &send_to_function) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    } 
    if (client_sock<0)
    {
        printf("accept failed");
        return 1;
    }
    pthread_exit(NULL);
    pthread_mutex_destroy(&stopParin);

    return 0;
}


void *connect_handle(void * temp_struct){
    struct sendto_function socket_struct = *(struct sendto_function *) temp_struct;
    printf("Connection accepted ");
    int new_socket = *socket_struct.client_soc;
    char message[2000]="";
    int ret, read_socket;
    char buf[256], *ipclient = socket_struct.ip_client;

    int read_size;
    printf("\nChat id: %s\n",chat.id);
    printf("Chat text: %s\n\n\n\n\n",chat.text);
    if(ret = write(new_socket , chat.id , 50)<0)
	{
		printf("Sending Error!\n");
	}
    if(ret = write(new_socket , chat.text , 4000)<0)
	{
		printf("Sending Error!\n");
	}
<<<<<<< HEAD
    printf("Chat id: %s\n",chat.id);
=======

    
>>>>>>> 84ea7d4d0064667b3efe50714809cfaab5fee7ae
    printf("Client socket: %d\nIP: %s\n", new_socket, ipclient);
    while(read_socket = recv(new_socket, message, 2000, 0) > 0) {
    }
    if(read_socket == 0)
    {
        printf("Client %d disconnected \n", new_socket);
        fflush(stdout);
        close(new_socket);
        pthread_exit(NULL);
        return 0;
    }


}

void *telegram_serv(void *vargp){
    while(1){
        if(tcp_server(&chat) <= 0){
            printf("Retry to bind address in 15 second...\n");
            sleep(15);
        }
    }

    return NULL;
}

void *telegram_serv2(void *vargp){
    while(1){
        if(strlen(chat.id) > 0){
            printf("[Telegram chat]\n");
            printf("chat id: %s\n", chat.id);
            printf("chat message: %s\n", chat.text);
            printf("[Telegram end chat]\n");

            telegram_send_msg(chat.id, chat.text);

            bzero(chat.id, sizeof(chat.id));
            bzero(chat.text, sizeof(chat.text));
        }
    }

    return NULL;
}