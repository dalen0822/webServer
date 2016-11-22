#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "threadpool.h"
#include "safe.h"
#include "httpd.h"
#include "errno.h"
#include "http.h"
#include "unit.h"

#define TRHEAD_SIZE 100
#define QUEUE_SIZE 3000

#if 1
#define DEBUGP(fmt, args...) do{		/* show debug*/	\
    printf("[%s:%d]  "fmt, __func__, __LINE__, ## args);    \
    }while(0)
#else
    #define DEBUGP(fmt, args...)
#endif

typedef struct {
    char server_addr[32];
    int  server_port;
}server_config;

threadpool_t *g_pool;
/* The internal web server */
httpd * webserver = NULL;
server_config config;

void thread_httpd(void *args) {
	void	**params;
	httpd	*webserver;
	request	*r;
	
	params = (void **)args;
	webserver = *params;
	r = *(params + 1);
	free(params); /* XXX We must release this ourselves. */
	
	if (httpdReadRequest(webserver, r) == 0) {
		/*
		 * We read the request fine
		 */
		DEBUGP("Processing request from %s\n", r->clientAddr);
		DEBUGP("Calling httpdProcessRequest() for %s\n", r->clientAddr);
		httpdProcessRequest(webserver, r);
		DEBUGP("Returned from httpdProcessRequest() for %s\n", r->clientAddr);
	}
	else {
		DEBUGP("No valid request received from %s\n", r->clientAddr);
	}
	DEBUGP("Closing connection with %s\n", r->clientAddr);
	httpdEndRequest(r);
}

void wait_for_connect() {
    while(1) {
        void ** p ;
        request *r;
        r = httpdGetConnection(webserver, NULL);

        /* We can't convert this to a switch because there might be
              * values that are not -1, 0 or 1. */
        if (webserver->lastError == -1) {
            /* Interrupted system call */
            DEBUGP("Interrupted system call\n");
            if (r) {
                free(r);
            }
            continue; /* restart loop */
        }
        else if (webserver->lastError < -1) {
            /*
                     * FIXME
                     * An error occurred - should we abort?
                     * reboot the device ?
                     */
            DEBUGP("FATAL: httpdGetConnection returned unexpected value %d, exiting.\n", webserver->lastError);
            //termination_handler(0);
        }
        else if (r != NULL) {
            /*
                     * We got a connection
                     *
                     * We should create another thread
                     */
            DEBUGP("Received connection from %s, spawning worker thread\n", r->clientAddr);
            /* The void**'s are a simulation of the normal C
                     * function calling sequence. */
            p = safe_malloc(2 * sizeof(void *));
            *p = webserver;
            *(p + 1) = r;

            if(threadpool_add(g_pool, &thread_httpd, (void *)p, 0) < 0) {
                DEBUGP("Thread poll full, ignore request\n");
                safe_free(p);
                httpdEndRequest(r);
            }
        }
        else {
            /* webserver->lastError should be 2 */
            /* XXX We failed an ACL.... No handling because
                    * we don't set any... */
        }
    }

}

void init_config(const char *file_config) {
    char tmpbuf[32] = "";
    get_file_value(file_config,"server_addr",config.server_addr);
    get_file_value(file_config,"server_port",tmpbuf);
    config.server_port = atoi(tmpbuf);
    return ;
}

void init_webServer(const char *config_file) {
    if(!webserver) {
        DEBUGP("Could not init web server!\n");
    }
    char tmpbuf[BUFF_LEN_256] = "";
    FILE *fp = NULL;
    get_file_value(config_file,"web_path",tmpbuf);
    //set basePath
    httpdSetFileBase(webserver,tmpbuf);
    //set log
    get_file_value(config_file,"err_log",tmpbuf);
    if((fp = fopen(tmpbuf,"w+")) != NULL) {
        httpdSetErrorLog(webserver,fp);
    }
    get_file_value(config_file,"access_log",tmpbuf);
    if((fp = fopen(tmpbuf,"w+")) != NULL) {
        httpdSetAccessLog(webserver,fp);
    }

    //设置可访问根目录下的所有文件
    httpdAddWildcardContent(webserver,"/",NULL,"html");
    httpdAddWildcardContent(webserver,"/pic",NULL,"html/pic");
    //设置默认页
    httpdAddFileContent(webserver,"/","",HTTP_TRUE,NULL,"html/index.html");
    //
    httpdAddCContent(webserver,"/", "about", HTTP_FALSE,NULL,http_callback_about);//get /about
}
void register_callback_fun() {
    httpdAddC404Content(webserver, http_callback_404);
}
int main(int argc,char *argv[]) 
{
    if(argc != 2) {
        printf("argv err !\n");
        return 0;
    }
    init_config(argv[1]);
    
    if(!(g_pool = threadpool_create(TRHEAD_SIZE, QUEUE_SIZE, 0))) {
        DEBUGP("create thread pool failed\n");
        //termination_handler(0);
    }
    if ((webserver = httpdCreate(config.server_addr, config.server_port)) == NULL) {
        DEBUGP("Could not create web server: %s\n", strerror(errno));
        exit(1);
    }
    init_webServer(argv[1]);
    register_callback_fun();
    DEBUGP("[%s:%d ] web server start: %s:%d.\n",__func__,__LINE__,config.server_addr,config.server_port);
    wait_for_connect();
    return 0;
}
