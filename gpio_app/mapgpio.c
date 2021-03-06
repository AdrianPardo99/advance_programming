#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "mapgpio.h"

extern volatile uint32_t *gpio;

/* GPIO controller for raspberry 4 */
#define GPIO_BASE_PI4 0xfe200000
/* GPIO controller for raspberry 3 */
#define GPIO_BASE_PI3 0x3f200000

/* Para habilitar el GPIO #  */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
/* Habilita el PIN para la salida */
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
/* Habilita el PIN de manera alterna */
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
/* Cambia el PIN a 1 lógico, entrada es 1<<(gpio) */
#define GPIO_SET *(gpio+7)
/* Cambia el PIN a 0 lógico, entrada 1<<(gpio) */
#define GPIO_CLR *(gpio+10)
/* Verifica si el gpio es un valor 0 o 1 lógico */
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

uint32_t *create_offset_gpio(int fd,int base){
    return (uint32_t *)mmap(
      0,
      getpagesize(),
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      fd,
      (base==0)?(GPIO_BASE_PI3):(GPIO_BASE_PI4)
    );
}

int *character_to_led(char x){
  int *arr;
  arr=(int*)malloc(sizeof(int)*10);
  if(arr==NULL){
    perror("Error allocating memory"),exit(EXIT_FAILURE);
  }
  register int i;
  for(i=0;i<10;i++){
    arr[i]=0;
  }
  if(x>='a'&&x<='j'){
    arr[('j'-x)]=1;
  }else if(x>='k'&&x<='s'){
    arr[('s'-x)+1]=
      arr[('s'-x)]=1;
  }else if(x>='t'&&x<='z'){
    arr[('z'-x)+1]=
      arr[('z'-x)+2]=
      arr[('z'-x)+3]=1;
  }else if(x==' '){
    arr[0]=arr[1]=arr[2]=1;
  }else if(x>='A'&&x<='G'){
    arr[('G'-x)]=
    arr[('G'-x)+1]=
    arr[('G'-x)+2]=
    arr[('G'-x)+3]=1;
  }else if(x>='H'&&x<='M'){
    arr[('M'-x)]=
    arr[('M'-x)+1]=
    arr[('M'-x)+2]=
    arr[('M'-x)+3]=
    arr[('M'-x)+4]=1;
  }else if(x>='N'&&x<='R'){
    arr[('R'-x)]=
    arr[('R'-x)+1]=
    arr[('R'-x)+2]=
    arr[('R'-x)+3]=
    arr[('R'-x)+4]=
    arr[('R'-x)+5]=1;
  }else if(x>='S'&&x<='V'){
    arr[('V'-x)]=
    arr[('V'-x)+1]=
    arr[('V'-x)+2]=
    arr[('V'-x)+3]=
    arr[('V'-x)+4]=
    arr[('V'-x)+5]=
    arr[('V'-x)+6]=1;
  }else if(x>='W'&&x<='Y'){
    arr[('Y'-x)]=
    arr[('Y'-x)+1]=
    arr[('Y'-x)+2]=
    arr[('Y'-x)+3]=
    arr[('Y'-x)+4]=
    arr[('Y'-x)+5]=
    arr[('Y'-x)+6]=
    arr[('Y'-x)+7]=1;
  }else if(x=='Z'){
    arr[('Z'-x)+1]=
    arr[('Z'-x)+2]=
    arr[('Z'-x)+3]=
    arr[('Z'-x)+4]=
    arr[('Z'-x)+5]=
    arr[('Z'-x)+6]=
    arr[('Z'-x)+7]=
    arr[('Z'-x)+8]=
    arr[('Z'-x)+9]=1;
  }
  return arr;
}

void set_gpio(int *gp,int *vals,int a){
  register int i;
  for(i=0;i<a;i++){
    if(vals[i]==0){
      GPIO_CLR=1<<gp[i];
    }else{
      GPIO_SET=1<<gp[i];
    }
  }
}
