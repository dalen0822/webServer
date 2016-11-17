#include "unit.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>


/*************************************************
Function:     trim_string
Description: ȥ�����׿ո�\n \ r \t �ַ�
Input: value �޸ĵ��ַ���
	    
	    
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
Input: fileName �ļ�·��
	    name ��ȡ�ַ�����
Output: value ����name����Ӧ��value

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

