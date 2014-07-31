/************
 * websocket client program
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <stdarg.h>

#include <libwebsockets.h>

/* proto type */ 
void log_callback(int, const char *, ...);
void signal_handler(int);
int callback_chat(struct libwebsocket_context *,
                  struct libwebsocket *,
                  enum libwebsocket_callback_reasons reason,
                  void *user, void *in, size_t len);


/* global variables */
volatile sig_atomic_t g_sigint = 0;
int deny_deflate = 0;
int deny_mux = 0;


typedef enum  {
    WSPROTO_CHAT,
    WSPROTO_MAX,
} MY_PROTOCALS;

struct libwebsocket_protocols g_wsproto[] = {
    {
        "chat",
        callback_chat,
        4096,
        4096,
        0,
        NULL,
        0,
    },
    { /* end of list */
        NULL,
        NULL,
        0,
        0,
        0,
        NULL,
        0,
    }
};


/* function */

void log_callback(int severity, const char *msg, ...)
{
    va_list va;
    va_start(va, msg);

    vfprintf(stderr, msg, va);
    fputc('\n', stderr);

    va_end(va);
}

void signal_handler(int sig){
    switch(sig){
    case SIGINT:
        g_sigint = 1;
        break;
    case SIGTERM:
        exit(1);
        break;
    case SIGKILL:
        exit(1);
        break;
    case SIGHUP:
        exit(1);
        break;
    case SIGPIPE:
        /* nothing */
        break;
    default:
        exit(1);
    }
}

int callback_chat(struct libwebsocket_context *this,
                  struct libwebsocket *wsi,
                  enum libwebsocket_callback_reasons reason,
                  void *user, void *in, size_t len)
{
    int ret = 0;
    char msgbuf[4096];

    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        fprintf(stderr, "LWS_CALLBACK_CLIENT_ESTABLISHED\n");

        /* websocket application authentication. */
        snprintf(msgbuf, sizeof(msgbuf), "{\"data\": {\"token\": \"%s\"}, \"func\": \"auth\"}", "12345678");

        fprintf(stderr, "send message:\n%s\n", msgbuf);
        
        ret = libwebsocket_write(wsi,
                                 (unsigned char*)msgbuf,
                                 strlen(msgbuf),
                                 LWS_WRITE_TEXT);
        if(ret < 0){
            fprintf(stderr, "libwebsocket write message failed. (%d)\n", ret);
        }
        break;
    case LWS_CALLBACK_ESTABLISHED:
        fprintf(stderr, "LWS_CALLBACK_ESTABLISHED\n");
        break;
    case LWS_CALLBACK_RECEIVE:
        fprintf(stderr, "LWS_CALLBACK_RECEIVE\n");
        break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
        ((char *)in)[len] = '\0';
        fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
        break;
    /* because we are protocols[0] ... */
    case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
        if ((strcmp(in, "deflate-stream") == 0) && deny_deflate) {
            fprintf(stderr, "denied deflate-stream extension\n");
            return 1;
        }
        if ((strcmp(in, "x-google-mux") == 0) && deny_mux) {
            fprintf(stderr, "denied x-google-mux extension\n");
            return 1;
        }
        break;
    case LWS_CALLBACK_CLOSED:
        fprintf(stderr, "LWS_CALLBACK_CLOSED\n");
        break;
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
        fprintf(stderr, "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER\n");
        break;
    default:
        fprintf(stderr, "LWS_CALLBACK_* = %d\n", reason);
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct libwebsocket_context *context = NULL;
    struct lws_context_creation_info wsinfo = {0};
    struct libwebsocket *wsi_chat = NULL;
    char *address = "192.168.22.102";
    int port = 8888;
    int use_ssl = 0;
    int ietf_version = -1;
    int ret = 0;

    fprintf(stderr, "hello wsmain.\n");

    /* set signal handler) */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGPIPE, signal_handler);

    /* libwebsockets_set_log_callback(log_callback);*/

    wsinfo.port = CONTEXT_PORT_NO_LISTEN;
    wsinfo.iface = NULL;
    wsinfo.protocols = g_wsproto;
    /*wsinfo.protocols = NULL;*/
    wsinfo.extensions = libwebsocket_get_internal_extensions();
    wsinfo.token_limits = NULL;
    wsinfo.ssl_private_key_password = NULL;
    wsinfo.ssl_cert_filepath = NULL;
    wsinfo.ssl_private_key_filepath = NULL;
    wsinfo.ssl_ca_filepath = NULL;
    wsinfo.ssl_cipher_list = NULL;
    wsinfo.http_proxy_address = NULL;
    wsinfo.http_proxy_port = 0;
    wsinfo.gid = -1;
    wsinfo.uid = -1;
    wsinfo.options = 0;
    wsinfo.user = NULL;
    wsinfo.ka_time = 0;
    wsinfo.ka_probes = 0;
    wsinfo.ka_interval = 0;

    fprintf(stderr, "try libwebsocket_create_context.\n");

    context = libwebsocket_create_context(&wsinfo);
    if (context == NULL) {
        fprintf(stderr, "Creating libwebsocket context failed\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "try libwebsocket_client_connect.\n");

    wsi_chat = libwebsocket_client_connect(
        context,
        address,
        port,
        use_ssl,
        "/",
        "pcdennokan.dip.jp",
        "http://pcdennokan.dip.jp",
        g_wsproto[WSPROTO_CHAT].name,
        ietf_version);
    if (wsi_chat == NULL) {
        fprintf(stderr, "libwebsocket chat connect failed\n");
        return EXIT_FAILURE;
    }

    /* loop */
    for(;;){
        ret = libwebsocket_service(context, 1000);
        fprintf(stderr, "service : %d\n", ret);

        if(0 < g_sigint){
            fprintf(stderr, "receive SIGINT\n");
            break;
        }
    }

    fprintf(stderr, "Exiting\n");

    libwebsocket_context_destroy(context);

	return EXIT_SUCCESS;
}

