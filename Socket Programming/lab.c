#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#define RESPONSELEN 100000
#define HYPERLINKLEN 1000

//char cmp[20]="a href=\"";
char target1[] = "<a";
char *target1_ptr = &target1[0];
char target2[] = "href=";
char *target2_ptr = &target2[0];
char *match_pos[105];


int main(int argc , char const* argv[]){
    
    
    char *receive = calloc(RESPONSELEN, sizeof(char));
    char *hyperlink = calloc(HYPERLINKLEN, sizeof(char));
    printf("Please enter the URL:\n");
    char hostname[10000];
    scanf("%s",hostname);
    int idx=-1;
    for(int i=0;i<strlen(hostname);i++){
        if(hostname[i]=='/'){
            idx=i;
            break;
        }
    }
    
    char str[10240];
    char str2[10240];
    if(idx==-1){
    	for(int i=0;i<strlen(hostname);i++)
    		str[i]=hostname[i];
    	str2[0]='/';
    }else{
    	for(int i=0;i<idx;i++)
    	str[i]=hostname[i];
    	for(int i=idx;i<strlen(hostname);i++)
    	str2[i-idx]=hostname[i];
    }
    struct hostent *h=gethostbyname(str);
    //printf("%s\n%s\n",str,str2);
    //if(h==NULL) printf("null");

    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    //if(server_fd==-1) printf("failed");

    struct sockaddr_in serverAddress;
    //memset(&serverAddress,0,sizeof(struct sockaddr_in));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(80);
    memcpy(&serverAddress.sin_addr,h->h_addr_list[0],h->h_length);
    
    if ( connect(server_fd,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) <0){
        printf("connection failed");
        return -1;
    }
    
    printf("========== Socket ==========\n");

    char message[10240];
    strcpy(message,"GET ");
    int len_hostname=strlen(hostname);
    if(idx==-1) strcat(message,"/");
    else strncat(message,str2,len_hostname-idx);
    strcat(message," HTTP/1.1\r\nHost: ");
    strncat(message,str,idx);
    strcat(message,"\r\n\r\n");

    printf("Sending HTTP request\n");
    send(server_fd,message,strlen(message),0);

    int valread=0;
    
    int cnt=0;
    //char receive[102400];                          !!!!!!!!!!!!!!!!!!!!~
    printf("Receiving the response\n");
    while(1){
        char buffer[10240];
        for(int i=0;i<10240;i++)
        buffer[i]='\0';
        valread=read(server_fd,buffer,10240);
        if(valread<=0) break;
        else{
            cnt+=strlen(buffer);
            strncat(receive,buffer,strlen(buffer));
        }
    }
    
    printf("======== Hyperlinks ========\n");
    
    /*int len=strlen(receive);
    int cnt_link;
    for(int i=0;i<len;i++){
        if(receive[i]=='<'){
            int ok=1;
            for(int j=0;j<8;j++){
                if(i+1+j>=len){
                    ok=0;
                    break;
                }
                else{
                    if(receive[i+1+j]!=cmp[j]){
                        ok=0;
                        break;
                    }
                }
            }
            if(ok==0) continue;
            char link[10000]; //從i+9開始印，接著判斷 ' " '
            int link_len;
            for(int j=i+9;j<len;j++){
                if(j<=len-2 && j>i+9){
                    if(receive[j]=='"'){
                        for(int k=i+9;k<j;k++){
                            printf("%c",receive[k]);
                        }
                        printf("\n");
                        cnt_link++;
                        break;
                    }
                }
            }
        }
    }
    printf("============================\n");
    printf("We have found %d hyperlinks\n",cnt_link);*/
    
    // Find all match positions
    int hyper_count = 0;
    //char hyperlink[1000];
    receive = strstr(receive, target1_ptr);
    receive = strstr(receive, target2_ptr);
    while (receive != NULL) {
        match_pos[hyper_count++] = receive;
        receive = strstr(receive + strlen(target2), target2_ptr);
    }

    // Print the hyperlinks at match positions
    for (int i = 0; i < hyper_count; i++) {
        hyperlink = strtok(match_pos[i] + strlen(target2), "\"");
        printf("%s\n", hyperlink);
    }
    
    printf("============================\n");
    printf("We have found %d hyperlinks\n", hyper_count);
}
