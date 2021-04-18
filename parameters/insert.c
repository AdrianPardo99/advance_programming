#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#define N 1024

int create_fd(char*);
void read_p(int,char*,int);
void write_p(int,char*);

int main(void){
  int fdIrq=create_fd("/sys/module/param_3/parameters/irq"),
    fdCmd=create_fd("/sys/module/param_3/parameters/comando"),
    fdData=create_fd("/sys/module/param_3/parameters/datos");
  char irq[N],cmd[N],data[N];
  memset(irq,0,N);
  memset(cmd,0,N);
  memset(data,0,N);
  read_p(fdIrq,irq,N);
  read_p(fdCmd,cmd,N);
  read_p(fdData,data,N);
  strcpy(irq,"8");
  strcpy(cmd,"atras");
  strcpy(data,"1,3,5,9");
  write_p(fdIrq,irq);
  write_p(fdCmd,cmd);
  write_p(fdData,data);
  close(fdIrq);
  close(fdCmd);
  close(fdData);
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
