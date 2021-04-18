#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Esta es la tarea 2 de modulos de kernel");
MODULE_VERSION("1.0");

static int op=0;

module_param(op, int, 0660);
MODULE_PARM_DESC(op, "Número de operación\n\t\t\t1)Cuenta cuantas veces el "
  "caracter esta en la cadena\n\t\t\t2)Cuenta la longitud total de cadena");

static char *cadena="esta es la optativa de advanced computing systems programming";
module_param(cadena, charp, 0660);
MODULE_PARM_DESC(cadena, "Cadena de caracteres");

static char *ch="e";
module_param(ch, charp, 0660);
MODULE_PARM_DESC(ch, "Variable que contiene el caracter específico a"
  " buscar en la op 1");

static int res=0;
module_param(res, int, 0660);
MODULE_PARM_DESC(res, "Variable que contiene el valor resultante de la operación");

struct task_struct *khilo;


static int hilo_kernel(void* arg){
	int id = *(int*)arg;
  register int i;
  printk(KERN_INFO "Iniciando hilo");
  while(!kthread_should_stop()){
    schedule();
    if(op==1 || op==2){
      i=0;
      res=0;
      while(cadena[i]){
        if(op==2){
          res++;
        }else{
          if(cadena[i]==*ch){
            res++;
          }
        }
        i++;
      }
      printk(KERN_INFO"El resultado de la operacion %s es: %d",
        (op==1)?"la cantidad del caracter contenido en la cadena":
        "la cantidad totla de caracteres",res);
      op=0;
    }
  }
  return id;
}

static int __init funcion_init(void){
  static int id_thread = 10;
  printk(KERN_INFO "Inicializando modulo de tarea 2\n");

	khilo = kthread_run(hilo_kernel,(void*)&id_thread,"kmi_hilo");
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
  printk(KERN_INFO "Terminando la ejecucion del modulo de tarea 2\n");
  ret_khilo=kthread_stop(khilo);
  if(ret_khilo==EINTR){
    printk(KERN_ERR"Error trying to stop thread\n");
  }else{
    printk(KERN_INFO"Stop thread with value %d\n",ret_khilo);
  }
}

module_init( funcion_init );
module_exit( funcion_exit );
