#include <stdio.h>
#include <assert.h>
#include<getopt.h>  //getopt_long()函数头文件：函数用于读取命令行
#include <sys/types.h>
#include <dirent.h>//使用opendir和readdir函数
#include <string.h>

int nflag=0,pflag=0,vflag=0;

struct option long_options[]={
  {"show-pids", no_argument, &pflag, 1},
	{"numeric-sort", no_argument, &nflag, 1},
	{"version", no_argument, &vflag, 1},
	{0, 0, 0, 0}
};

void pstree_n(void);
void pstree_p(void);
void pstree_V(void);

void Print(void);

int main(int argc, char *argv[]) {
  char c;
  if (strcmp(argv[1], "pstree") != 0) {
    printf("If you want to query the process tree, output the 'pstree' command.\nIf it's not querying the process tree, then I can't help you.\n");
    //如果你想查询进程树，请输出‘pstree’指令。\n如果不是查询进程树，那我帮不了你。
    return 1;
}

  while((c = getopt_long(argc, argv, "npV", long_options, NULL)) != -1){
    switch(c){
      case 'n':
        nflag=1;
        break;
      case 'p':
        pflag=1;
        break;
      case 'V':
        vflag=1;
        break;
      case 0:
        break;//若是flag字段表示NULL时，getopt_long()会返回0，啥也不做。
      case '?':
        printf("not this option:%s\n",argv[optind-1]);//当getopt_long()函数遇到一个未知的选项时，它会返回'?'
        //这时输出未知选项.
        return 1;
      default:
        printf("not this program\n");
        break;
    }
  }
  Print();

  DIR *dirp;
  struct dirent *dp,*ptr;
  FILE *fp;
  char filepath[512];
  char procname[256];

  
  dirp = opendir("/proc");
  if (dirp == NULL) {
      perror("Failed to open directory \"/proc\"");
      return 1;
  }

  printf("PID\n");
  while ((dp = readdir(dirp)) != NULL) {
      if (dp->d_type == DT_DIR && dp->d_name[0] >= '0' && dp->d_name[0] <= '9') {
          //printf("%s\n", dp->d_name);
          // 打开/proc/[PID]/status文件
          snprintf(filepath, sizeof(filepath), "/proc/%s/status", dp->d_name); 
          fp = fopen(filepath, "r");
          if (fp != NULL) {
              // 读取进程名称
              fscanf(fp, "Name:\t%s", procname);
              printf("%s(%s)\n", procname, dp->d_name);
              fclose(fp);
          }
      }
  }

  closedir(dirp);

  assert(argv[argc] == NULL);
  return 0;
}

void Print(){
  if(nflag){
    pstree_n();
  }
  if(pflag){
    pstree_p();
  }
  if(vflag){
    pstree_V();
  }
}

void pstree_n(void){
  printf("pstree -n\n");
}
void pstree_p(void){
  printf("pstree -p\n");
}
void pstree_V(void){
  printf("pstree -V\n");
}