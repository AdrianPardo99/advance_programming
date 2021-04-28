#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

/*
 *
 * @author Adrian González Pardo
 * @date   28/04/2021
 * @email  gozapaadr@gmail.com
 * @github AdrianPardo99
 *
**/

/* GPIO controller for raspberry 4 */
#define GPIO_BASE_PI4 0xfe200000
/* GPIO controller for raspberry 3 */
#define GPIO_BASE_PI3 0x3f200000

void config_gpio(volatile uint32_t *gpio,int v_gpio,int mode){
  int GPFSEL=v_gpio/10,
      range=(v_gpio%10);
  range=(range<<1)+range;
  if(mode==0){
    /*
     * As Output
    **/
    *(gpio+GPFSEL)=(*(gpio+GPFSEL)&~(7<<range))|(1<<range);
  }else{
    /*
     * As input
    **/
    *(gpio+GPFSEL)=(*(gpio+GPFSEL)&~(7<<range));
  }
}

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

void value_gpio(volatile uint32_t *gpio,int v_gpio,int val){
  int bit=(v_gpio>>5)&1,
      GPSET_CLR;
  if(val==0){
    /*
     * As clr the value of gpio
    **/
    GPSET_CLR=bit+10;
  }else{
    /*
     * As set the value of gpio 1
    **/
    GPSET_CLR=bit+7;
  }
  *(gpio+GPSET_CLR)=(1<<(v_gpio%32));

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
