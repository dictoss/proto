/************
 * get and parse json program
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <jansson.h>
#include <curl/curl.h>

#define DEFAULT_REQUEST_URL  "https://github.com/"
#define JSON_DEST_PATH "/tmp/myjson.tmp"
#define JSON_REQ_PATH "req.json"

/* must not too smaller than CURL_MAX_WRITE_SIZE */
#define MY_ALLOC_SIZE CURL_MAX_WRITE_SIZE

struct mycurlbuffer
{
    char*  buf;
    size_t size;    
    size_t pos;
    int first;
    CURL *pc;
};

int test_curl_call(const char*);
int get_http_file(const char *, struct mycurlbuffer*);
int get_http_json(const char *url, json_t **);
int post_http_json(const char *url, json_t *req);
int loadfile_json(const char *json_file, char **buf);

size_t curl_recv_cb(char *ptr, size_t size, size_t nmemb, void *userdata);

/*
 main
 */
int main(int argc, char *argv[])
{
    int exitcode = EXIT_SUCCESS;

    if(1 < argc){
        exitcode = test_curl_call(argv[1]);
    }
    else{
        exitcode = test_curl_call(NULL);
    }
    
	return exitcode;
}

/*
  curl call
 */
int test_curl_call(const char *url)
{
    int exitcode = 0;
    int ret = 0;
    CURLcode cres;
    const char* target_url = NULL;

    cres = curl_global_init(CURL_GLOBAL_ALL);
    if(cres){
        fprintf(stderr, "fail curl_global_init(), ret=%d\n", cres);
        exitcode = EXIT_FAILURE;
    }

    if(url){
        target_url = url;
    }
    else{
        target_url = DEFAULT_REQUEST_URL;
    }

    fprintf(stderr, "target_url: %s\n", target_url);

#if 1
    {
        struct mycurlbuffer d = {0};
        size_t i = 0;

        ret = get_http_file(target_url, &d);
        if(ret < 0){
            /* fail */
            exitcode = 1;
        }
        else{
            exitcode = 0;
            
            /* print data */
            for(i = 0; i < d.pos; i ++){
                fprintf(stdout, "%c", *(d.buf + i));
            }
            
            free(d.buf);
        }
    }
#endif

#if 0
    {
        json_t *json = NULL;

        ret = get_http_json(target_url, &json);
        if(ret < 0){
            /* fail */
            exitcode = 1;
        }
        else{
            const char *key;
            json_t *value;
            void *iter;
            
            /* print data */
            fprintf(stderr, "got json\n\n");
            
            iter = json_object_iter(json);
            while(iter){
                key = json_object_iter_key(iter);
                value = json_object_iter_value(iter);
                
                fprintf(stderr, "key = %s\n", key);
                
                if(JSON_STRING == json_typeof(value)){
                    fprintf(stderr, "  value = %s\n", json_string_value(value));
                }
                else{
                    fprintf(stderr, "  value is not string.\n");
                }
                
                iter = json_object_iter_next(json, iter);
            }
            
            json_decref(json);
            
        exitcode = 0;
        }
    }
#endif
    
    curl_global_cleanup();

    return exitcode;
}


size_t curl_recv_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    CURLcode cres;
    double content_length = 0.0;
    struct mycurlbuffer *pd = (struct mycurlbuffer*)userdata;

    fprintf(stderr, "recv-size is %ld\n", size * nmemb);

    if(!pd->first){
        cres = curl_easy_getinfo(
            pd->pc,
            CURLINFO_CONTENT_LENGTH_DOWNLOAD,
            &content_length);
        fprintf(stderr, "CURLcode is %d\n", cres);

        /* get or guess response length */
        if(content_length == -1){
            fprintf(stderr, "Content-Length is not defined.\n");
            pd->size = MY_ALLOC_SIZE;
        }
        else{
            fprintf(stderr, "Content-Length is %ld\n", (size_t)content_length);
            pd->size = (size_t)content_length;            
        }

        /* add null terminate size. */
        pd->buf = (char*)malloc(pd->size + sizeof(wchar_t));
        pd->pos = 0;
        pd->first = 1;
    }

    if(pd->size < (pd->pos + (size * nmemb))){
        /* buffer too small  */
        char *tmp = NULL;

        fprintf(stderr, "buffer too small. re-malloc.\n");
        pd->size = pd->size + MY_ALLOC_SIZE;
        
        tmp = (char*)malloc(pd->size);
        memcpy(tmp, pd->buf, pd->pos);
        free(pd->buf);
        pd->buf = tmp;
    }

    memcpy(pd->buf + pd->pos, ptr, size * nmemb);    
    pd->pos += (size * nmemb);
    /* for json-string responsed. */
    *(pd->buf + pd->pos) = '\0';
    
    return size * nmemb;
}


/*
 * get file from http.
 */
int get_http_file(const char *url, struct mycurlbuffer *pdata)
{
    CURLcode cres;
    CURL *curl = NULL;
    long http_code = 0L;
    double dlsize = 0.0;
    char* content_type = NULL;
    
    curl = curl_easy_init();
    if(NULL == curl){
        return -1;
    }

    /* prepare */
    cres = curl_easy_setopt(curl, CURLOPT_URL, url);
    cres = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    pdata->pc = curl;
    cres = curl_easy_setopt(curl, CURLOPT_WRITEDATA, pdata);
    cres = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_recv_cb);

    /* request and response */
    fprintf(stderr, "start request\n");
    cres = curl_easy_perform(curl);
    if(cres){
        curl_easy_cleanup(curl);
        fprintf(stderr, "fail request\n");
        return -2;
    }
    else{        
        fprintf(stderr, "response done\n");
    }
    
    /* response */
    cres = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    cres = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dlsize);
    /* must not free. free called curl_easy_cleanup() */
    cres = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);

    fprintf(stderr, "HTTP Status code : %ld\n", http_code);
    fprintf(stderr, "HTTP Download size : %ld\n", (long)dlsize);
    fprintf(stderr, "HTTP Content-type : %s\n", content_type);
    
    fprintf(stderr, "recv data-size : %ld\n", pdata->size);
    fprintf(stderr, "recv data-pos  : %ld\n", pdata->pos);
    
    fprintf(stderr, "----\n\n");

    curl_easy_cleanup(curl);
    pdata->pc = NULL;
    
    return 0;
}


int get_http_json(const char *url, json_t **res)
{
    int ret = 0;
    json_error_t error;
    struct mycurlbuffer d = {0};
    
    ret = get_http_file(url, &d);
    if(0 < ret){
        return -1;
    }
    else{
        *res = json_loads(d.buf, 0, &error);

        free(d.buf);

        return 0;
    }
}

int post_http_json(const char *url, json_t *req)
{
    return 0;
}


int loadfile_json(const char *json_file, char **buf)
{
    json_t *json = NULL;
    json_error_t error;
 
    json = json_load_file(json_file, 0, &error);
    if(NULL == json){
        fprintf(stderr, "error json_load_file(). error=%d\n", 0);
        return -1;
    }

    *buf = json_dumps(json, 0);
    if(NULL == *buf){
        fprintf(stderr, "error json_dumps().\n");
        json_decref(json);
        return 0;
    }

    json_decref(json);

    return 0;
}
