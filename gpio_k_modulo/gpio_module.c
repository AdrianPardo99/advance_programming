#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>
#include <asm/io.h>

//#define ADDR_BASE_GPIO 0x3F200000		//Direccion base para Raspberry Pi3 B+
#define ADDR_BASE_GPIO 0xFE200000		//Direccion base para Raspberry Pi4 B

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo, Samuel Alejandro Jurado Macias,"
  " Raymundo Pulido Bejarano");
MODULE_DESCRIPTION("Hilo de kernel con GPIO a 7 segmentos");
MODULE_VERSION("1.0");

static char *boleta="2015140309";
module_param(boleta,charp,0660);
MODULE_PARM_DESC(boleta, "Boleta de datos del alumno");

static int des=0;
module_param(des,int,0660);
MODULE_PARM_DESC(des,"Opción: 1 Muestra en display");

struct task_struct *khilo;
volatile uint32_t *gpio_virtual;
int arr_gpio[]={6,13,19,26,16,20,21},size=7,segment[7];

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

/*
 * La funcion que es ejecutada por el hilo del kernel
 * Valor de retorno:
 * Regresa id
 */
static int hilo_kernel(void *data){
	int id=*(int *)data;
  register int i=0,j=0;
	printk(KERN_INFO "Hilo en ejecucion con id: %d...\n", id);
	while( !kthread_should_stop()){
    if(des==1){
      i=0;
      printk(KERN_INFO "Boleta: %s\n",boleta);
      while(boleta[i]){
        seven_seg_number(boleta[i],&*segment);
        for(j=0;j<7;j++){
          value_gpio(gpio_virtual,arr_gpio[j],segment[j]);
        }
        ssleep(1);
        i++;
      }
      printk(KERN_INFO"Fin de escritura de Boleta: %s\n",boleta);
      des=0;
    }
    for(j=0;j<7;j++){
      value_gpio(gpio_virtual,arr_gpio[j],0);
    }
		ssleep(1);
	}
	return id;
}
/*
 * La funcion kthread_run crea y despierta a un hilo, lo pone en ejecucion.
 * Ejecuta una llamada a kthread_create() seguida de una llamada a wake_up_process()
 * Valor de retorno:
 * 		Regresa el hilo creado ó
 * 		ERR_PTR(-ENOMEM )
 */

static int __init kernel_init(void){
	static int id_thread = 10;
  register int i;
	printk( KERN_INFO "--------------------------------------------\n" );
   	khilo = kthread_create( hilo_kernel, (void *)&id_thread, "kmi_hilo" );

	if(IS_ERR(khilo)){
		printk("KERN_ERR Error en la creación del hilo... \n");
		return PTR_ERR(khilo);
	}
	//RANGO DE DIRECCIONES DE LOS REGISTROS A PARTIR DE LA DIRECCION BASE
	gpio_virtual=(uint32_t *)ioremap(ADDR_BASE_GPIO, 0X30);
  for(i=0;i<size;i++){
    config_gpio(gpio_virtual,arr_gpio[i],0);
  }

	wake_up_process( khilo );
	printk( KERN_INFO "Hilo de Kernel creado: %s con PID %d \n", khilo->comm, khilo->pid );

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
	int ret_hilo_kernel = kthread_stop( khilo );
	if( ret_hilo_kernel == -EINTR ){
		printk( KERN_INFO "Error en la terminación del hilo \n" );
	}else{
		printk( KERN_INFO "Hilo de Kernel con id: %d detenido\n", ret_hilo_kernel );
	}
	iounmap( gpio_virtual );
  printk( KERN_INFO "--------------------------------------------\n" );
}

module_init(kernel_init);
module_exit(kernel_exit);
