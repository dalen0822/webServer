#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "httpd.h"
#include "safe.h"
#include "http.h"
#include "threadpool.h"

void http_callback_404(httpd *webserver, request *r) {
    send_http_page(r,"xxx","ooooo");
}


/** @brief http template page read only once
 */
static char* get_http_page()
{
    struct stat stat_info;
    int fd;
    ssize_t written;
    char htmlmsgfile[64] = "/dalen/GitHub/webServer/html/index.html";
    static struct {
        char    buff[10240];
        int     len;
        int     loaded;
    } page = {
        .len = 0,
        .loaded = 0,
    };

    if(page.loaded) {
        if((page.len > 0)) {
            return safe_strdup(page.buff);
        } else {
            printf("HTML has loaded, but return size 0, maybe file %s can not find\n",
                  htmlmsgfile);
            return NULL;
        }
    }

    fd = open(htmlmsgfile, O_RDONLY);
    if (fd == -1) {
        page.loaded = 1;
        page.len= -1;

        return NULL;
    }

    if (fstat(fd, &stat_info) == -1) {
        printf("Failed to stat HTML message file: %s\n", strerror(errno));
        close(fd);

        page.loaded = 1;
        page.len= -1;

        return NULL;
    }

    written = read(fd, page.buff, (size_t) stat_info.st_size);
    if (written == -1) {
        printf("Failed to read HTML message file: %s\n", strerror(errno));
        close(fd);

        page.loaded = 1;
        page.len= -1;
        return NULL;
    }

    page.buff[written] = 0;
    page.len= written;

    close(fd);
    return safe_strdup(page.buff);
}

void send_http_page(request * r, const char *title, const char *message)
{
    // read http page from memory direct, only load once
    char *buffer = get_http_page();
    //s_config *config = config_get_config();

    if(!buffer)
        return;

    httpdAddVariable(r, "title", title);
    httpdAddVariable(r, "message", message);
    httpdAddVariable(r, "nodeID", "flb");
    httpdOutput(r, buffer);
    free(buffer);
}

