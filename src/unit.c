#include "unit.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "execinfo.h"
#include "signal.h"
#include "threadpool.h"

extern threadpool_t *g_pool;
/*************************************************
Function:     trim_string
Description: 去掉行首空格；\n \ r \t 字符
Input: value 修改的字符串
	    
	    
Output: value

Return:

Others:
*************************************************/

static void trim_string(char *value){

    int i,len,ti,tag;

	len = strlen(value);
	ti = 0;
	tag = 0;
	for (i = 0; i < len; i ++) {
		if((value[i] == ' ') && (tag == 0)) {

		}else if((value[i] == '\n') || (value[i] == '\r') || (value[i] == '\t')) {

		}else{
			value[ti] = value[i];
			ti ++;
            tag = 1;
		}
	}
	value[ti] = 0;
}

/*************************************************
Function:     get_file_value
Description: 
Input: fileName 文件路径
	    name 获取字符串名
Output: value 返回name所对应的value

Return:

Others:
*************************************************/

int get_file_value(const char *file_name,char *name, char *value){

    FILE *fp = NULL;
	char buf[BUFF_LEN_256 + 4] = {'\0'};
    int len = 0,flag = 0;

	if (name == NULL){
		return -1;
	}
	if((fp = fopen(file_name, "r")) == NULL){
        printf("open %s fail!\n",file_name);    
		return -1;
	}
	while(fgets(buf, BUFF_LEN_256, fp) > 0){
		trim_string(buf);
		if(strncmp(buf,name,strlen(name)) == 0){
			strncpy(value,buf + strlen(name) + 1,strlen(buf) - strlen(name) - 1);
			*(value + (strlen(buf) - strlen(name) - 1))  = '\0';

			fclose(fp);
			return 0;
		}
	}

	fclose(fp);
	return -1;
}


void stack_dump(int sig) {
    #define SIZE 32
    void *buffer[SIZE];
    char **stacks = NULL;
    int size = 0,i;

    size = backtrace(buffer,SIZE);
    stacks = backtrace_symbols(buffer,size);
    if(stacks) {
        printf("receive signal %d.\n",sig);
        for(i = 0;i != size; ++i) {
            printf("[ stack info ] : %s.\n",stacks[i]);
        }
        free(stacks);
    }
}

void ignore_signal(int sig){
    stack_dump(sig);
}

void exit_signal(int sig) {
    stack_dump(sig);
    //do clean work
    threadpool_destroy(g_pool,1);
    exit(0);
}
void init_signal() {
    signal(SIGINT,exit_signal);
    signal(SIGSEGV,exit_signal);
    signal(SIGALRM,ignore_signal);
    signal(SIGHUP,stack_dump);
    signal(SIGPIPE,ignore_signal);
    signal(SIGTERM,exit_signal);
    signal(SIGQUIT,exit_signal);
    signal(SIGILL,ignore_signal);
    signal(SIGCHLD,exit_signal); 
}
