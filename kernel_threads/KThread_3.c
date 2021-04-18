/*
	KThread_1.c Demostracion de uso de hilos de Kernel
	Se usa la funcion kthread_create() y wake_up_process()
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/delay.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raymundo Pulido");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL con hilos");
MODULE_INFO(driver,"PULSO CARDIACO");
MODULE_INFO(interface,"UART");
MODULE_VERSION("1.0");

struct task_struct *khilo;

static int hilo_kernel(void* datos){
	int id = *(int*)datos;
	register int con = 0;
	printk(KERN_INFO "Init kernel thread in function");
	while(!kthread_should_stop()){
		schedule();
		printk(KERN_INFO "Schedule number%d\n",con++);
	}
	return id;
}
static int __init funcion_inicio(void){
		
	static int id_thread = 10;
	
	printk(KERN_INFO "Ejemplo 3 hilos de kernel\n");
		
	khilo = kthread_run(hilo_kernel,(void*)&id_thread,"kmi_hilo");
	
	if( IS_ERR(khilo) ){
		printk(KERN_ERR "Error en la creacion del hilo... \n");
		return PTR_ERR(khilo);
	}
	
	printk( KERN_INFO "Hilo creado con pid: %d y nombre: %s\n",khilo->pid,khilo->comm ); 
	return 0;
}
static void __exit funcion_final(void){
	int retorno;
	printk(KERN_INFO "Terminando la ejecucion del ejemplo 3 de hilos\n");
	retorno = kthread_stop(khilo);
	if (retorno == -EINTR){
		printk(KERN_ERR "Error en la terminacion del hilo\n");
	}else{
		printk(KERN_INFO "Stop thread with value %d\n",retorno);
	}
}
module_init( funcion_inicio );
module_exit( funcion_final );
