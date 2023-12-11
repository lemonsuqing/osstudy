#include <stdio.h>
#include <assert.h>
#include<getopt.h>  //getopt_long()函数头文件：函数用于读取命令行
#include <sys/types.h>
#include <dirent.h>//使用opendir和readdir函数
#include <string.h>
#include <stdlib.h>

int nflag=0,pflag=0,vflag=0;

struct option long_options[]={
  {"show-pids", no_argument, &pflag, 1},
	{"numeric-sort", no_argument, &nflag, 1},
	{"version", no_argument, &vflag, 1},
	{0, 0, 0, 0}
};

typedef struct Proc{
  char procName[256];
  int Pid;
  int PPid;
  struct Proc *next;
  struct Proc *child;
}Proc;

#define MAX_PROCS  791621423  // 最大进程数
int flag = 1;
Proc* procs[MAX_PROCS];  // 存储所有进程的哈希表

Proc* createProc(char* procName, int Pid, int PPid) {
    Proc* proc = (Proc*)malloc(sizeof(Proc));
    strcpy(proc->procName, procName);
    proc->Pid = Pid;
    proc->PPid = PPid;
    proc->next = NULL;
    proc->child = NULL;
    return proc;
}

void insertProc(Proc* proc) {
    procs[proc->Pid] = proc;
    if (proc->PPid != 0 && procs[proc->PPid] != NULL) {
        Proc* current = procs[proc->PPid]->child;
        Proc* prev = NULL;
        while (current != NULL && current->Pid < proc->Pid) {
            prev = current;
            current = current->next;
        }
        if (prev == NULL) {
            proc->next = procs[proc->PPid]->child;
            procs[proc->PPid]->child = proc;
        } else {
            proc->next = prev->next;
            prev->next = proc;
        }
    }
}


void printProc(Proc* proc, int level) {
    if (flag) {
        flag = 0;
    } else {
        for (int i = 0; i < level; i++) {
            printf("       ");
        }
        printf("|\n");
    }
    for (int i = 0; i < level; i++) {
        printf("       ");
    }
    if (level > 0) {
        printf("+----");
    }
    if(pflag)
      printf("%s(%d)\n", proc->procName, proc->Pid);
    else
      printf("%s\n", proc->procName);
    Proc* child = proc->child;
    while (child != NULL) {
        printProc(child, level + 1);
        child = child->next;
    }
}

int main(int argc, char *argv[]) {
  char c;
  if (strcmp(argv[1], "pstree") != 0) {
    printf("If you want to query the process tree, output the 'pstree' command.\n");
    return 1;
}

  while((c = getopt_long(argc, argv, "npV", long_options, NULL)) != -1){
    switch(c){
      case 'n':
        nflag=1;  break;
      case 'p':
        pflag=1;  break;
      case 'V':
        vflag=1;  break;
      case 0:
        break;//若是flag字段表示NULL时，getopt_long()会返回0
      case '?':
        printf("not this option:%s\n",argv[optind-1]);//当getopt_long()函数遇到一个未知的选项时，它会返回'?'
        //这时输出未知的那个选项.
        return 1;
      default:
        printf("not this program\n");
        break;
    }
  }
  if(vflag){
    printf("pstree -V\n");
    return 0;
  }

  DIR *dirp;
  struct dirent *dp,*ptr;
  FILE *fp;
  char filepath[512];
  char procname[256];
  char line[256];
  int ppid;
  int count = 0;
  
  dirp = opendir("/proc");
  if (dirp == NULL) {
      perror("Failed to open directory \"/proc\"");
      return 1;
  }

  while ((dp = readdir(dirp)) != NULL) {
      if (dp->d_type == DT_DIR && dp->d_name[0] >= '0' && dp->d_name[0] <= '9') {
          // 打开/proc/[PID]/status文件
          snprintf(filepath, sizeof(filepath), "/proc/%s/status", dp->d_name); 
          fp = fopen(filepath, "r");
          if (fp != NULL) {
                while (fgets(line, sizeof(line), fp)) {
                    if (strncmp(line, "Name:", 5) == 0) {
                        sscanf(line, "Name:\t%s", procname);
                    } else if (strncmp(line, "PPid:", 5) == 0) {
                        sscanf(line, "PPid:\t%d", &ppid);
                    }
                }
                Proc* proc = createProc(procname, atoi(dp->d_name), ppid);
                insertProc(proc);
                //printf("%s(%s)\t%d\n", procname, dp->d_name, ppid);
                count ++;
                fclose(fp);
            }
      }
  }
  

  closedir(dirp);
  printf("Total proc: %d\n",count);
  printProc(procs[1], 0);
  assert(argv[argc] == NULL);
  return 0;
}
