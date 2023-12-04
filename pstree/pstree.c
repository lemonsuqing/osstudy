#include <stdio.h>
#include <assert.h>
#include<getopt.h>  //getopt_long()函数头文件：函数用于读取命令行
#include <sys/types.h>
#include <dirent.h>//使用opendir和readdir函数
#include <string.h>

int main(int argc, char *argv[]) {
  char* key_s[2];
  key_s[0] = argv[1];
  key_s[1] = argv[2];
  char c = argv[2][1];
  if(strcmp(key_s[0],"pstree") == 0){
    //printf("the argv[1] = %s\n", key_s[0]);
    //printf("the argv[2] = %s\n", key_s[1]);
    switch(c){
      case 'n':
        printf("pstree -n\n");
        break;
      case 'p':
        printf("pstree -p\n");
        break;
      case 'V':
        printf("pstree -V\n");
        break;
      default:
        printf("not this program\n");
        break;
    }
  }
  assert(!argv[argc]);
  return 0;
}
