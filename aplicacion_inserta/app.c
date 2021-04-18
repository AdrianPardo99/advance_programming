#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#define N 1024

/*
 *
 * @author Adrian González Pardo
 * @date   28/03/2021
 * @email  gozapaadr@gmail.com
 *
**/

int create_fd(char*);
void read_p(int,char*,int);
void write_p(int,char*);

int main(int argc,char **argv){
  if(geteuid()!=0){
    printf("Please run the app as root\n");
    exit(1);
  }
  if(argc<2){
    printf("Error\n\tUsage: %s <op=path_variable=val>\n\n"
      "\tIf op equal 0 means read value in path_variable\n\t"
      "or equal 1 means write the path_variable\n\t"
      "or equal 2 means read and write the path_variable\n\t"
      //"or equal 3 means write and read the path_variable\n\t"
      "The path_variable is the full path where the kernel module variable"
      " is located\n\tThe val are the values or the single value where the "
      "kernel module storage the variables in the path_variable\n",
      *argv),exit(EXIT_FAILURE);
  }

  int *fd=(int*)malloc(sizeof(int)*(argc-1));
  printf("Descriptores a crear: %d\n",argc-1);
  if(fd==NULL){
    perror("Error allocating memory "),exit(EXIT_FAILURE);
  }
  char *split,d[N],path[N],val[N],buffer[N];
  register int i,c=0;
  for(i=1;i<argc;i++){
    split=strtok(*(argv+i),"=");
    c=0;
    memset(d,0,N);
    memset(path,0,N);
    memset(val,0,N);
    memset(buffer,0,N);
    while(split){
      if(c==0){
        strcpy(d,split);
      }else if(c==1){
        strcpy(path,split);
      }else{
        strcpy(val,split);
      }
      split=strtok(NULL,"=");
      c++;
    }
    fd[i-1]=create_fd(path);
    if(*d=='0'){
      printf("Modo lectura para los valores alojados en %s\n",path);
      read_p(fd[i-1],buffer,N);
    }else if(*d=='1'){
      printf("Modo escritura para los valores alojados en %s para insertar "
        "valores %s\n",path,val);
      write_p(fd[i-1],val);
    }else if(*d=='2'){
      printf("Modo de lectura y escritura para los valores alojados en %s para"
              "primero leer y despues escribir valores\n",path);
      read_p(fd[i-1],buffer,N);
      write_p(fd[i-1],val);
    }/*else if(*d=='3'){
      printf("Modo de escritura y lectura para los valores alojados en %s para"
              "primero escribir y despues leer valores\n",path);
      write_p(fd[i-1],val);
      read_p(fd[i-1],buffer,N);
    }*/
    puts("");
    close(fd[i-1]);
  }
  free(fd);
  return 0;
}

int create_fd(char *path){
  int fd=open(path,O_RDWR);
  if(fd==-1){
    perror("Error opening the fd "),exit(EXIT_FAILURE);
  }
  printf("Descriptor de archivo con numero: %d\n",fd);
  return fd;
}

void read_p(int fd, char *dst, int size){
  int n=read(fd,dst,size);
  if(n==-1){
    perror("Error trying to read a parameter "),exit(EXIT_FAILURE);
  }
  *(dst+(n-1))=0;
  printf("Number of bytes read: %d\nValue of parameter %s\n",n,dst);
}

void write_p(int fd,char *src){
  int n=write(fd,src,(sizeof(char)*strlen(src))+1);
  if(n==-1){
    perror("Error trying to write a parameter "),exit(EXIT_FAILURE);
  }
  printf("Number of bytes write: %d\n",n);
}
