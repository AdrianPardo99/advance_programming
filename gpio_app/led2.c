#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "mapgpio.h"

volatile uint32_t *gpio;
int main(int argc, char **argv){
  if(argc<2){
    printf("Error\n\tUsage %s <message>\n",*argv),exit(EXIT_FAILURE);
  }
  int fd;
  fd=open("/dev/mem", O_RDWR | O_SYNC);
	if(fd==-1){
		printf("Error al abrir /dev/mem %s \n",strerror(errno)),exit(EXIT_FAILURE);
  }
  gpio=create_offset_gpio(fd,1);
  register int i,j;
  int l=strlen(*(argv+1)),*arr,
    arr_gpio[]={5,1,12,6,13,19,26,16,20,21};
  for(i=0;i<10;i++){
    INP_GPIO(arr_gpio[i]);
    OUT_GPIO(arr_gpio[i]);
  }
  for(i=0;i<l;i++){
    arr=character_to_led(argv[1][i]);
    set_gpio(arr_gpio,arr,10);
    printf("%c\t|",argv[1][i]);
    for(j=0;j<10;j++){
      printf("%d|",arr[j]);
    }
    printf("\n");
    sleep(2);
    free(arr);
  }
  arr=(int*)malloc(sizeof(int)*10);
  if(arr==NULL){
    perror("Error allocating memory"),exit(EXIT_FAILURE);
  }
  for(i=0;i<10;i++){
    arr[i]=0;
  }

  set_gpio(arr_gpio,arr,10);

  free(arr);
  close(fd);
  return 0;
}
