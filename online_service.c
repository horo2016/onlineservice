#include"stdio.h"
#include "string.h"
#include "cJSON.h"
#include <sys/time.h>
#include "stdlib.h"
#include "time.h"
#include "stdarg.h"
#include  <string.h>
#include "md5.h"

#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

//需要的时间
#define MINS 2 //3分钟
#define DELAY_TIME (60*MINS)
#define FILEBUFFER_LENGTH 5000
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
char chargename[17]="000dbc1239846328";

void get_str_md5(const char *str, char *md5_str)
{
	MD5_CTX md5;
	unsigned char md5_value[MD5_SIZE];
	int i=0;
 
	MD5Init(&md5);
	MD5Update(&md5,(unsigned char *) str, strlen(str));
	
	MD5Final(&md5, md5_value);
 
	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; // add end
}
static void process_qrcodeText(const void *p, size_t length)
{
    FILE *save_fd  = fopen("qrcode.txt", "w");
    fputc ('.', stdout);
    fwrite(p, sizeof(char), length, save_fd);
    fflush (stdout);
    fclose(save_fd);
}
int CreatQrcodejson()
{
	char tmp_buf[0xff]={0};
	char topic_buf[0xff]={0};
	unsigned char  value_buf[0xff]={0};
	 char  send_buf[0xff]={0};
	 char md5_str[MD5_STR_LEN + 1];
	 char md5_str_id[MD5_STR_LEN + 1];

	 char md5_str_license[12];

//	memcpy(topic_buf,"online",sizeof("online"));
	 time_t cur_time;
	struct tm *tm_buf = NULL;

	time (&cur_time);
	tm_buf = localtime (&cur_time);
	memset (tmp_buf, 0, sizeof (tmp_buf));
	sprintf ((char *)tmp_buf, "%d-%d-%d %02d:%02d:%02d", tm_buf->tm_year + 1900, tm_buf->tm_mon + 1, tm_buf->tm_mday, tm_buf->tm_hour, tm_buf->tm_min, tm_buf->tm_sec);

	memset(md5_str_license,0,12);
	get_str_md5((const char *)tmp_buf, md5_str);
	memcpy(md5_str_license,md5_str,5);
	//printf("time %s \n",md5_str);
	
	get_str_md5((const char *)chargename, md5_str_id);
	//printf("id:%s-%s\n",chargename,md5_str_id);
	memcpy(md5_str_license+5,md5_str_id,5);


	sprintf(topic_buf,"{\"TIME\":\"%s\",\"SN\":\"%s\",\"LICENSE\":\"%s\"}",tmp_buf,chargename,md5_str_license);
	//printf("value_buf %s \n",topic_buf);

	//sprintf(send_buf,"echo -e \"%s\"  > qrcode.txt",topic_buf);
	process_qrcodeText(topic_buf,strlen(topic_buf));
	system("sudo cp qrcode.txt /usr/local/boa/qrcodejs/");


   

    return 0;
}

/*******************************************************************************
 * function name	: get_strchr_len
 * description	: Get the migration length of the characters in a string
 * param[in] 	: str-string, c-char
 * param[out] 	: none
 * return 		: >0-len, -1-fail
 *******************************************************************************/
int get_strchr_len(char *str, char c)
{
	int i = 0;
	int len = 0;

	if(!str)
		return -1;

	while(*(str+i++) != c)
		len ++;

	return len;
}
//打开fileName指定的文件，从中读取第lineNumber行
//返回值：成功返回1，失败返回0
int get_file_line(char *fileName,int lineNumber)
{
    FILE *filePointer;
    int i=0,j=0;
    char buffer[FILEBUFFER_LENGTH];
    char *temp;
    char *sub_str = NULL ;

    if((fileName==NULL))
    {
        return 0;
    }

    if(!(filePointer=fopen(fileName,"rb")))
    {return 0;}


    while((!feof(filePointer))&&(i<lineNumber))
    {
        if(!fgets(buffer,FILEBUFFER_LENGTH,filePointer))
        {
            return 0;
        }
        i++;//差点又忘记加这一句了
    }



    fgets(buffer,sizeof(buffer),filePointer); //读第bai10行
    printf("%s", buffer );
    sub_str = strstr(buffer, "Serial");

    if(sub_str != NULL){
        sub_str += (strlen("Serial") + 4); //
        printf("%s\n", sub_str);
       int value_len = get_strchr_len(sub_str, '\n');
      

         for(j=0;j<value_len;j++){
             chargename[j]= *(sub_str+j);
            }
         chargename[16]='\0';


    }


    fclose(filePointer);
    return 1;
}
int CreatOnlinejson()
{
	 
	char topic_buf[0xff]={0};
	unsigned char  value_buf[0xff]={0};
	char  send_buf[0xff]={0};
    char namebuffer[0xff]={0};
	
	
	memcpy(topic_buf,"online",sizeof("online"));
    sprintf(namebuffer,"\"%s\"",chargename);

    cJSON * root =  cJSON_CreateObject();
  if(!root) {
         printf("get root faild !\n");
     }else printf("get root success!\n");

    cJSON_AddItemToObject(root, "\"type\"", cJSON_CreateString("\"1\""));
    cJSON_AddItemToObject(root, "\"sn\"", cJSON_CreateString(namebuffer));
    
    
    memcpy(value_buf,cJSON_Print(root),strlen(cJSON_Print(root)));
      

	sprintf(send_buf,"mosquitto_pub -h www.woyilian.com -t %s  -m \"%s\"  ",topic_buf,value_buf);
	system(send_buf);
	 //printf("%s\n", send_buf);
        //printf("%s\n", publishstring.pMessage);

    cJSON_Delete(root);

    return 0;
}
void send_Online2Monitor()
{
   CreatOnlinejson();
}
void main()
{
	memset(chargename,0,17);
	get_file_line("/proc/cpuinfo",42);
    CreatQrcodejson();

	printf("device id name:%s\n",chargename);
	while(1){
	  send_Online2Monitor();
	  sleep(DELAY_TIME);
	}
}