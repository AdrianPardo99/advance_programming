#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void){
  int fd=open("/sys/module/param_3/parameters/irq",O_RDWR),n;
  char *irq="2";
  if(fd==-1){
    perror("Error opening the fd "),exit(EXIT_FAILURE);
  }
  printf("El descriptor de archivo es: %d\n",fd);
  if((n=write(fd,irq,strlen(irq)*sizeof(char)))==-1){
    perror("Error trying to write in irq"),exit(EXIT_FAILURE);
  }
  printf("Bytes escritos: %d\n",n);
  close(fd);
  return 0;
}
