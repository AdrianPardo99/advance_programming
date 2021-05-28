#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include<linux/delay.h>
#include <asm/io.h>

//#define ADDR_BASE_GPIO 0x3F200000		//Direccion base para Raspberry Pi3 B+
#define ADDR_BASE_GPIO 0xFE200000		//Direccion base para Raspberry Pi4 B

#define N 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo, Samuel Alejandro Jurado Macias,"
  " Raymundo Pulido Bejarano");
MODULE_DESCRIPTION("Hilo de kernel con GPIO a 7 segmentos con funciones callback");
MODULE_VERSION("1.0");


volatile uint32_t *gpio_virtual;
int arr_gpio[]={17,18,27,22,23,24,10},size=7,segment[7];

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

void seven_seg_number(char x,int *arr){
  /* Este funciona para un GND comun en caso contrario solo invertir bits */
  register int i;
  for(i=0;i<7;i++){
    *(arr+i)=0;
  }
  if(x=='0'){
    for(i=0;i<6;i++){
      *(arr+i)=1;
    }
  }else if(x=='1'){
    *(arr+1)=*(arr+2)=1;
  }else if(x=='2'){
    *(arr)=*(arr+1)=*(arr+3)=*(arr+4)=*(arr+6)=1;
  }else if(x=='3'){
    for(i=0;i<4;i++){
      *(arr+i)=1;
    }
    *(arr+6)=1;
  }else if(x=='4'){
    *(arr+1)=*(arr+2)=*(arr+5)=*(arr+6)=1;
  }else if(x=='5'){
    *(arr)=*(arr+2)=*(arr+3)=*(arr+5)=*(arr+6)=1;
  }else if(x=='6'){
    for(i=2;i<7;i++){
      *(arr+i)=1;
    }
    *(arr)=1;
  }else if(x=='7'){
    *(arr)=*(arr+1)=*(arr+2)=1;
  }else if(x=='8'){
    for(i=0;i<7;i++){
      *(arr+i)=1;
    }
  }else if(x=='9'){
    for(i=0;i<4;i++){
      *(arr+i)=1;
    }
    *(arr+5)=*(arr+6)=1;
  }
}

static char boleta[N]="2015140309";


int set_boleta(const char *v,const struct kernel_param *k){
  int len=strlen(v);
  printk(KERN_INFO"Callback 'set' boleta\n");
  if(len>N){
    printk(KERN_ERR"Error the string it's too large\n");
    return -ENOSPC;
  }else if(len<=1){
    printk(KERN_ERR"Error the string it's empty\n");
    return -EINVAL;
  }
  strcpy(k->arg,v);
  printk(KERN_INFO"Param set: %s\n",boleta);
  register int i=0,j=0;
  printk(KERN_INFO "Boleta: %s\n",boleta);
  printk(KERN_INFO"Posicion[]\t#\tSegmento de bits\n");
  while(boleta[i]){
    seven_seg_number(boleta[i],&*segment);
    for(j=0;j<7;j++){
      value_gpio(gpio_virtual,arr_gpio[j],segment[j]);
    }
    printk(KERN_INFO"boleta[%d]\t%c\t|%d|%d|%d|%d|%d|%d|%d|\n",
      i,boleta[i],segment[0],segment[1],segment[2],segment[3],segment[4],
      segment[5],segment[6]);
    ssleep(1);
    i++;
  }
  printk(KERN_INFO"Fin de escritura de Boleta: %s\n",boleta);
  for(j=0;j<7;j++){
    value_gpio(gpio_virtual,arr_gpio[j],0);
  }
  ssleep(1);
  return 0;
}

int get_boleta(char *b, const struct kernel_param *k){
  printk(KERN_INFO"Callback 'get' boleta\n");
  strcpy(b,(char*)k->arg);
  return strlen(b)+1;
}

static const struct kernel_param_ops boleta_kp={
  .set=set_boleta,
  .get=get_boleta
};
module_param_cb(boleta,&boleta_kp,&boleta,0660);
MODULE_PARM_DESC(boleta, "Boleta de datos del alumno");
static int __init kernel_init(void){
  register int i;
	//RANGO DE DIRECCIONES DE LOS REGISTROS A PARTIR DE LA DIRECCION BASE
	gpio_virtual=(uint32_t *)ioremap(ADDR_BASE_GPIO, 0X30);
  for(i=0;i<size;i++){
    config_gpio(gpio_virtual,arr_gpio[i],0);
  }
	return 0;
}
/*
 * La funcion kthread_stop realiza lo siguiente:
 * 1. Activa kthread->kthread_should_stop = true
 * 2. Despierta al hilo
 * 3. Espera a que el hilo termine
 * 4. Valor de retorno:
 * 		Regresa el resultado de hilo_kernel ó
 * 		-EINTR si no se llama a la funcion wake_up_process()
 */
static void __exit kernel_exit(void){
  printk(KERN_INFO"Bye bye");
}

module_init(kernel_init);
module_exit(kernel_exit);
