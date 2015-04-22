/************
 * get and parse json program
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <jansson.h>

#define CMD_WGET "/usr/bin/wget" 
#define JSON_TARGET_URL "http://192.168.22.30/site/api/rest/diary/find/"
#define JSON_DEST_PATH "/tmp/myjson.tmp"
#define JSON_REQ_PATH "req.json"

int get_json_wget_post(const char *path, const char* postfile_path);
int get_json_http(char **result, size_t *result_len);
int loadfile_json(const char *json_file, char **buf);


int main(int argc, char *argv[])
{
    int ret = 0;
    char *json_buf = NULL;

    ret = get_json_wget_post(JSON_DEST_PATH, JSON_REQ_PATH);
    if(ret < 0){
        fprintf(stderr, "fail get_json_wget_post() : ret = %d\n", ret);
    }
    else{
        /* import json file. */
        ret = loadfile_json(JSON_DEST_PATH, &json_buf);
        if(ret < 0){
            fprintf(stderr, "fail loadfile_json() : ret = %d\n", ret);
            return 1;
        }
        else{
            fprintf(stderr, "success loadfile_json()\n");
            printf("%s\n", json_buf);
            free(json_buf);
            json_buf = NULL;
        }
    }

	return EXIT_SUCCESS;
}


/*
 * get json by wget, and load json-file.
 */
int get_json_wget_post(const char *path, const char *postfile_path)
{
    int ret = 0;
    char cmd[4096];
    char postdata[4096];

    sprintf(cmd, "%s --post-file=\"%s\" -O \"%s\" \"%s\"",
            CMD_WGET, postfile_path, path, JSON_TARGET_URL);

    fprintf(stderr, "postdata: \n%s\n", postdata);
    
    ret = system(cmd);
    if(ret == 0){
        fprintf(stderr, "success wget.\n");
    }
    else if(ret < 0){
        fprintf(stderr, "fail wget. errno=%d.", errno);
        return -1;
    }
    else{
        fprintf(stderr, "wget ret = %d", ret);
    }   

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
