#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

// openSSL
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/bio.h"

#include "http_praser.h"

#define BUFFER_SIZE 20480
#define HOST "api.telegram.org"
#define PATH "bot591458604:AAHOF6mhG6ft9Zyvr2bCciFWKiXsuSJmD9Q/getMe"

int request(){
    BIO * request_bio;
    char read_buffer[BUFFER_SIZE];

    SSL_CTX *request_ctx = SSL_CTX_new(SSLv23_client_method());
    if (request_ctx == NULL) {
        printf("errored; unable to load context.\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    SSL *request_ssl;

    request_bio = BIO_new_ssl_connect(request_ctx);
    BIO_get_ssl(request_bio, &request_ssl);
    SSL_set_mode(request_ssl, SSL_MODE_AUTO_RETRY);

    BIO_set_conn_hostname(request_bio, HOST":443");

    if(BIO_do_connect(request_bio) <= 0){
        printf("Error connection\n");
    }

    struct http_request test_request;

    char request[] = "GET /"PATH" HTTP/1.1\r\n"
    "Host: "HOST"\r\n"
    "User-Agent: Wget/1.19.1 (linux-gnu)\r\n"
    "Connection: Keep-Alive\r\n\r\n";

    // prase_request(&test_request, request);

    int test;
    printf("[Request to "HOST"]\n%s\n", request);

    if((test = BIO_write(request_bio, request, strlen(request))) <= 0){
        printf("errored; unable to write.\n");
        BIO_free_all(request_bio);
        
        ERR_print_errors_fp(stderr);
        return -1;
    }

    while(1){
        int res = BIO_read(request_bio, read_buffer, BUFFER_SIZE);
        if(res < 0){
            if(!BIO_should_retry(request_bio)){
                printf("Error: read failed");
                ERR_print_errors_fp(stderr);
                break;
            }
            printf("Res is %d. that < 0", res);
        }
        else{
            printf("[Response from "HOST"]\n%s\n", read_buffer);
            int tee = search(read_buffer, "\r\n\r\n");
            char http_body[1000];
            get_body(http_body, read_buffer);
            printf("This is body =>\n%s\n", http_body);
            // printf("Search found => %d\n", tee);
            // printf("Found Char => %c\n", read_buffer[tee]);
            break;
        }
    }
}

int main() {
    request();
}