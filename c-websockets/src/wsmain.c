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
#include <jansson.h>

/* macro */
/*
#define MY_USE_STATIC_BUF
*/
#define RECV_BUF_SIZE  (1024)
#define MSG_RECV_POLLING_SPAN  (1 * 1000)


/* typedef */
typedef enum  {
    WSPROTO_CHAT,
    WSPROTO_MAX,
} MY_PROTOCALS;

typedef struct my_config {
    char server_address[256];
    int is_use_ssl;
    unsigned short server_port;
    char ws_pass[256];
} MY_CONFIG;


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
struct my_config g_config = {"192.168.22.102", 0, 8888, "12345678"};


struct libwebsocket_protocols g_wsproto[] = {
    {
        "chat",
        callback_chat,
        0,
        RECV_BUF_SIZE,
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
    size_t jsonmsg_len = 0;
#ifdef MY_USE_STATIC_BUF
    char msgbuf[LWS_SEND_BUFFER_PRE_PADDING + RECV_BUF_SIZE + LWS_SEND_BUFFER_POST_PADDING];
#else
    char *sendmsg = NULL;
    char *jsonmsg = NULL;
#endif

    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        fprintf(stderr, "LWS_CALLBACK_CLIENT_ESTABLISHED\n");

        /* websocket application authentication. */
#ifdef MY_USE_STATIC_BUF
        {
            jsonmsg_len = snprintf(&msgbuf[LWS_SEND_BUFFER_PRE_PADDING],
                     sizeof(msgbuf) - LWS_SEND_BUFFER_PRE_PADDING,
                     "{\"data\": {\"token\": \"%s\"}, \"func\": \"auth\"}",
                     "12345678");

            fprintf(stderr, "send message:\n%s\n", &msgbuf[LWS_SEND_BUFFER_PRE_PADDING]);
        
            ret = libwebsocket_write(wsi,
                                     (unsigned char*)&msgbuf[LWS_SEND_BUFFER_PRE_PADDING],
                                     jsonmsg_len,
                                     LWS_WRITE_TEXT);
        }
#else
        {
            json_t *root_dict = json_object();
            json_t *data_dict = json_object();

            json_object_set_new(root_dict, "func", json_string("auth"));

            json_object_set_new(data_dict, "token", json_string(g_config.ws_pass));
            json_object_set_new(root_dict, "data", data_dict);

            jsonmsg = json_dumps(root_dict, JSON_COMPACT);
            if(jsonmsg){
                jsonmsg_len = strlen(jsonmsg);
            
                sendmsg = (char*)malloc(LWS_SEND_BUFFER_PRE_PADDING + jsonmsg_len +
                                    LWS_SEND_BUFFER_POST_PADDING);
                if(sendmsg){
                    memcpy(&sendmsg[LWS_SEND_BUFFER_PRE_PADDING], jsonmsg, jsonmsg_len);

                    fprintf(stderr, "send message:\n%s\n", jsonmsg);

                    ret = libwebsocket_write(wsi,
                                             (unsigned char*)&(sendmsg[LWS_SEND_BUFFER_PRE_PADDING]),
                                             jsonmsg_len,
                                             LWS_WRITE_TEXT);
                }
                else{
                    fprintf(stderr, "fail malloc(). [sendmsg]\n");   
                }
            }
            else{
                fprintf(stderr, "fail malloc(). [jsonmsg]\n");
            }

            free(jsonmsg);
            free(sendmsg);

            json_decref(data_dict);
            json_decref(root_dict);                                    
        }
#endif   
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
        g_config.server_address,
        g_config.server_port,
        g_config.is_use_ssl,
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
        ret = libwebsocket_service(context, MSG_RECV_POLLING_SPAN);
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
