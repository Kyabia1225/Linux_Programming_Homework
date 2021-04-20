#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
int lines, words, bytes, flag;
int lines_total = 0, words_total = 0, bytes_total = 0;      //处理有文件的情况
bool wc_file(char *filename);                               //处理键盘输入
void wc();

int main(int argc, char *argv[]){
    if(argc == 1) {
        wc();
        printf(" %d %d %d\n", lines, words, bytes);
    } else {
        while(--argc){
            if(wc_file(argv[argc])){
                lines_total += lines;
                words_total += words;
                bytes_total += bytes;
                printf(" %d %d %d\t%s\n", lines, words, bytes, argv[argc]);
                lines = 0, words = 0, bytes = 0, flag = 0;  //初始化
            }
        }
        if(argc>2)
            printf(" %d %d %d\t%s\n", lines_total, words_total, bytes_total, "total");
    }
    return 0;
}

//对单个字符进行处理
void process_ch(char ch) {
    if(ch == '\n')
        lines ++;
    if(ch == '\t' || ch == ' ' || ch == '\n'){
        flag = 0;
    }else if(flag == 0){ //ch不为空白字符且flag==0时，代表新单词的开始
        flag = 1;
        words++;
    }
}

void wc() {
    lines=0, words=0, bytes=0;
    char ch;
    flag = 0;
    while((ch = getchar())!=EOF){
        bytes += sizeof(ch);
        process_ch(ch);
    }
}

bool wc_file(char *filename) {
    struct stat buf;
    FILE *fp;
    if(stat(filename,&buf) == -1){
        cerr<<"wc: "<<filename<<": No such file or directory\n";
        return false;
    }
    lines=0, words=0, bytes=buf.st_size;
    if(S_ISDIR(buf.st_mode)){
        //如果是目录，输出错误
        cerr<<"wc: "<<filename<<" Is a directory\n\t0\t0\t0 "<<filename<<endl;
        return false;
    }
    else{
        fp = fopen(filename,"r");
        char ch;
        flag = 0;
        while((ch = fgetc(fp))!=EOF){
            process_ch(ch);
        }
    }
    fclose(fp);
    return true;
}