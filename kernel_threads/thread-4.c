#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos 			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
#define MAX 5
/*
*  thread-4.c - Demostracion de la ejecución de un hilo en espacio de kernel
*  se usa la función kthread_create() y wake_up_process(). Se vinvula el hilo
*  a un procesador con la funcion kthread_bind()
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VICTOR HUGO GARCIA ORTEGA");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("4.0");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");


struct task_struct *khilo;

static int hilo_kernel(void *arg){
	int id = *(int*)arg;
	int cont = 0;
	int cpu;

	printk(KERN_INFO "Iniciando la ejecucion de la funcion hilo_kernel\n");

	while( !kthread_should_stop() ){
		schedule();
		//ssleep(1);
		cpu = smp_processor_id();
		printk(KERN_INFO "Ejecucion en CPU: %d contador: %d \n", cpu, cont++);
	}

	return id;
}

static int __init funcion_inicio(void){
	static int id_thread = 10;
	printk(KERN_INFO "Iniciando el ejemplo 1 de hilos de kernel\n");
	khilo = kthread_create( hilo_kernel, (void *)&id_thread, "kmi_hilo" );
	if( IS_ERR(khilo) ){
		printk(KERN_ERR "Error al crear el hilo de kernel\n");
		return PTR_ERR(khilo);
	}
	kthread_bind(khilo,3);
	wake_up_process(khilo);
	printk(KERN_INFO "Hilo creado con pid: %d y nombre: %s\n", khilo->pid, khilo->comm);

	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

static void __exit funcion_exit(void){
	int ret_hilok = kthread_stop( khilo );

	printk(KERN_INFO "Terminando la ejecucion del ejemplo 1 de hilos\n");

	if( ret_hilok == -EINTR )
		printk(KERN_ERR "Error en la terminacion del hilo\n");
	else
		printk(KERN_INFO "Terminacion del hilo con valor: %d \n", ret_hilok);
}

module_init( funcion_inicio );
module_exit( funcion_exit );
