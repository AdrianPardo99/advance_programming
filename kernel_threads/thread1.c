#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/sched.h>

/* thread1.c - Demostracion de la ejecución de un hilo en espacio de kernel
   se usa la funcion kthread_create y wake_up_process
*/

struct task_struct khilo;

static int hilo_kernel(void *arg){
  int id=*(int*)*arg;
  register int con=0;
  printk(KERN_INFO"Init kernel thread in function");
  while(!kthread_should_stop()){
    schedule();
    printk(KERN_INFO"Schedule number %d\n",con++);
  }
  return id;
}

static int __init funcion_inicio(void){
  int id_thread=10;
  printk(KERN_INFO "Iniciando el ejemplo 1 de hilos en kernel\n");
  khilo=kthread_create(hilo_kernel,(void*)&id_thread,"kmi_hilo");
  if(IS_ERR(khilo)){
    printk(KERN_ERR "Error trying to create thread...\n");
    return PTR_ERR(khilo);
  }
  wake_up_process(khilo);
  printk(KERN_INFO"Thread created by id %d and name %s\n",khilo->pid,khilo->comm);
  return 0;
}

static void __exit funcion_exit(void){
  int ret_khilo;
  printk(KERN_INFO "Terminando la ejecucion del ejemplo 1 de hilos\n");
  ret_khilo=kthread_stop(khilo);
  if(ret_khilo==_EINTR){
    printk(KERN_ERR"Error trying to stop thread\n");
  }else{
    printk(KERN_INFO"Stop thread with value %d\n",ret_khilo);
  }
}

module_init( funcion_inicio );
module_exit( funcion_exit );
