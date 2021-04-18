#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos 			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
#include <linux/cpumask.h>

#define NUM_PROCS 4
#define MAX 16
/*
*  thread-5.c - Demostracion de la ejecución de un hilo en espacio de kernel
*  se usa la función kthread_create() y wake_up_process(). Se vinvula el hilo
*  a un procesador con la funcion kthread_bind()
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("1.0");

static int A[MAX]={12, 45, 87, 38, 97, -45, 44, 88, 22, 43, -5,
  -23, -34, -8, 17, 20},B[MAX]={66, 3, 48, 23, 21, 9, 4, 8, 2,
  -4, -8, -20, -10, -56, 88, 54},C[MAX];


struct task_struct *khilo[NUM_PROCS];

static int hilo_kernel(void *arg){
	int id=*(int*)arg;
	register int i;
  printk(KERN_INFO"Init thread: %d\n",id);
  for(i=id;i<MAX;i++){
    C[i]=A[i]*B[i];
  }
	while(!kthread_should_stop()){
		schedule();
	}
  printk(KERN_INFO"End thread %d\n",id);;
	return id;
}

static int __init funcion_inicio(void){
	int id_thread[NUM_PROCS];
  register int i;
	printk(KERN_INFO "Iniciando el ejemplo 1 de hilos de kernel\n");
  for(i=0;i<NUM_PROCS;i++){
    *(id_thread+i)=i;
    khilo[i]=kthread_create(hilo_kernel,(void *)&id_thread[i], "kmi_hilo" );
    if(IS_ERR(khilo[i])){
      printk(KERN_ERR "Error al crear el hilo de kernel\n");
      return PTR_ERR(khilo[i]);
    }
    //kthread_bind(khilo[i],i);
    wake_up_process(khilo[i]);
    printk(KERN_INFO "Hilo creado con pid: %d y nombre: %s\n",
      khilo[i]->pid, khilo[i]->comm);
  }
	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

static void __exit funcion_exit(void){
  register int i;
	int ret_hilok[NUM_PROCS];
  for(i=0;i<NUM_PROCS;i++){
     ret_hilok[i]=kthread_stop(khilo[i]);
     if( ret_hilok[i] == -EINTR ){
       printk(KERN_ERR "Error en la terminacion del hilo\n");
     }else{
       printk(KERN_INFO "Terminacion del hilo con valor: %d \n", ret_hilok[i]);
     }
  }

	printk(KERN_INFO "Terminando la ejecucion resultado:\n");
  for(i=0;i<MAX;i++){
    printk(KERN_INFO"C[%d]=%d",i,C[i]);
  }
}

module_init( funcion_inicio );
module_exit( funcion_exit );
