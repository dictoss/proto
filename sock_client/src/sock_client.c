/*
  socket client program
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 macro
 */
#define MY_BUF_SIZE (32 * 1024)
#define MY_ARRAY_SIZE(a) (sizeof(a)/(sizeof(a[0])))


/*
 typedef
 */
struct mysockdata {
	int fd;
	struct sockaddr_storage sockaddr;
};

/*
  prototype
 */
void print_hexdump(const char *s, size_t size);


/*
 global var
 */
int g_argc;
char **g_argv;

/*
 main
 */
int main(int argc, char *argv[])
{
	int ret = 0;
    int fd = 0;
    struct mysockdata sockdata = {0};
    struct addrinfo in_addrinfo = {0};
    struct addrinfo *paddrres = NULL;
    struct addrinfo *paddr = NULL;
    char *host = "127.0.0.1";
    char *port = "80";
    char sendbuf[MY_BUF_SIZE];
    char recvbuf[MY_BUF_SIZE];

	g_argc = argc;
	g_argv = argv;

    /* prepare socket */
    in_addrinfo.ai_flags = 0;
    in_addrinfo.ai_family = AF_INET;
    in_addrinfo.ai_socktype = SOCK_STREAM;
    in_addrinfo.ai_protocol = 0;
    
    ret = getaddrinfo(host, port, &in_addrinfo, &paddrres);
    if(ret){
        fprintf(stderr, "fail getaddrinfo(). ret = %d, errno=%d\n", ret, errno);
        return EXIT_FAILURE;
    }

    for(paddr = paddrres; paddr != NULL; paddr = paddrres->ai_next){
        fprintf(stderr, "sockaddr : ai_family = %d, ai_socktype = %d, ai_protocol = %d\n", paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol);

        fd = socket(paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol);
        if(fd < 0){
            return EXIT_FAILURE;
        }
        else{
            fprintf(stderr, "success socket() fd=%d.\n", fd);
        }
        
        ret = connect(fd, (struct sockaddr*)paddr->ai_addr, paddr->ai_addrlen);
        if(ret < 0){
            fprintf(stderr, "fail connect(ret=%d, errno=%d)\n",
                ret, errno);
            close(ret);
        }
        else{
            /* success conect */
            sockdata.fd = fd;
            memcpy(&(sockdata.sockaddr), &(paddr->ai_addr), sizeof(sockdata.sockaddr));
            break;
        }
    }

    freeaddrinfo(paddrres);

    if(sockdata.fd < 1){
        fprintf(stderr, "fail connect all. abort.\n");
        return EXIT_FAILURE;
    }

    /* data send and recv */
    sprintf(sendbuf, "GET / HTTP/1.0\r\n\r\n");

    ret = send(sockdata.fd, sendbuf, strlen(sendbuf), 0);
    if(ret < 0){
        fprintf(stderr, "fail send(ret=%d, errno=%d). abort.\n",
                ret, errno);
        return EXIT_FAILURE;
    }
    else{
        fprintf(stderr, "success send(ret=%d)\n", ret);
    }

    memset(recvbuf, 0, sizeof(recvbuf));
    ret = recv(sockdata.fd, recvbuf, sizeof(recvbuf), 0);
    if(ret < 0){
        fprintf(stderr, "fail recv(ret=%d, errno=%d). abort.\n",
                ret, errno);
        return EXIT_FAILURE;
    }
    else{
        fprintf(stderr, "success recv(ret=%d)\n", ret);
        fprintf(stderr, "----\n");
        fprintf(stdout, recvbuf);
        fprintf(stderr, "----\n");
        print_hexdump(recvbuf, ret);
        fprintf(stderr, "\n----\n");  
    }
    
    /* close */
    shutdown(sockdata.fd, SHUT_RDWR);
    close(sockdata.fd);

	fprintf(stderr, "finish.\n");
	
	return 0;
}

/*
 hex dump tool.
 */
void print_hexdump(const char *s, size_t size)
{
    const char *p = NULL;
    int i = 0;

    for(p = s, i = 0; p < s + size; p ++, i++){
        if(i % 8 == 0){
            fprintf(stderr, "\n");
        }

        fprintf(stderr, "%02X ", *p);
    }
}
