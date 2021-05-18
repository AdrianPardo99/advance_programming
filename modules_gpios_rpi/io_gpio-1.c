/** @brief: Este programa muestra solicita las direcciones virtuales
 *  de el GPIO de la raspberry, se usan la función kthread_create para
 *  crear un hilo y la función wake_up_process para iniciar la ejecución
 *  del hilo
 */

#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>
#include <asm/io.h>

//#define ADDR_BASE_GPIO 0x3f200000		//Direccion base para Raspberry Pi3 B+
#define ADDR_BASE_GPIO 0xFE200000		//Direccion base para Raspberry Pi4 B

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("Adrian González Pardo");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Hilo de kernel con GPIO");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");				/* Versión del módulo 				*/

struct task_struct *khilo;
volatile uint32_t *gpio_virtual;
/*
 * La funcion que es ejecutada por el hilo del kernel
 * Valor de retorno:
 * Regresa id
 */

static int hilo_kernel(void *data){
	int id=*(int *)data;
	printk(KERN_INFO "Hilo en ejecucion con id: %d...\n", id);
	while( !kthread_should_stop()){
		//schedule();	//bloquea el hilo por un intervalo preestablecido de tiempo.
		//ssleep(2);	//bloquea el hilo por un numero de segundos = 2
		//msleep(500);	//bloquea el hilo por un numero de milisegundos = 500
		//Se pone en uno el GPIO 16
		*(gpio_virtual + 7) |= (1 << 16);
		ssleep(2);
		//Se pone en cero el GPIO 16
		*(gpio_virtual + 10) |= (1 << 16);
		ssleep(2);
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

	printk( KERN_INFO "\n--------------------------------------------\n" );
   	khilo = kthread_create( hilo_kernel, (void *)&id_thread, "kmi_hilo" );

	if(IS_ERR(khilo)){
		printk("KERN_ERR Error en la creación del hilo... \n");
		return PTR_ERR(khilo);
	}
	//RANGO DE DIRECCIONES DE LOS REGISTROS A PARTIR DE LA DIRECCION BASE
	gpio_virtual=(uint32_t *)ioremap(ADDR_BASE_GPIO, 0X30);
	*(gpio_virtual + 1)=(*(gpio_virtual + 1) & ~(7 << 18))|(1 << 18);

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
}

module_init(kernel_init);
module_exit(kernel_exit);
