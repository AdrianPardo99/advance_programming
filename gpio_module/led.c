#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include "mmap.h"

/*
 *
 * @author Adrian González Pardo
 * @date   28/04/2021
 * @email  gozapaadr@gmail.com
 * @github AdrianPardo99
 *
**/

#define CICLOS 5

int main(void){
  int fd;
  volatile uint32_t *gpio;
  register int c=0,i;
	fd=open("/dev/mem", O_RDWR | O_SYNC);
	if(fd==-1){
		printf("Error al abrir /dev/mem %s \n",strerror(errno)),exit( EXIT_FAILURE );
  }
  gpio=create_offset_gpio(fd,1);
  if(gpio==MAP_FAILED){
    printf("Error al obtener direcion virtual: %s\n", strerror(errno));
    return -1;
  }
  printf("La direccion virtual es: 0x%08x\n", gpio);
  int arr_gpio[]={5,1,12,6,13,19,26,16,20,21},size=10;
  for(i=0;i<size;i++){
    config_gpio(gpio,arr_gpio[i],0);
  }
  for(i=0;i<size;i++){
    value_gpio(gpio,arr_gpio[i],0);
  }
  while(c<CICLOS){
    /* Se pone en uno los GPIOs */

    for(i=0;i<size;i++){
      value_gpio(gpio,arr_gpio[i],1);
      usleep(500000/2);
    }
    /* Se pone en cero los GPIOs */
    for(i=0;i<size;i++){
      value_gpio(gpio,arr_gpio[i],0);
      usleep(500000/2);
    }
    c++;
  }
  close(fd);
	return 0;
}
