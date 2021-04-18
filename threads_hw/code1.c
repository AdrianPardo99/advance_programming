#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/cpumask.h>

#define NUM_PROCS num_online_cpus()
#define MAX 8

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Esta es la tarea 1 de modulos de kernel");
MODULE_VERSION("1.0");

static int irq = 0;
module_param(irq, int, 0660);
MODULE_PARM_DESC(irq, "Número de operación\n\t\t\t1)Promedio del arreglo"
  "\n\t\t\t2)Ordenamiento burbuja\n\t\t\t3)Pares del arreglo");
static int datos[MAX] = {5, 78, -23, 97, 12, -5, 7, 44};
static int numEle = MAX,numEle2=0;
module_param_array(datos, int, &numEle, 0660);
MODULE_PARM_DESC(datos, "Arreglo de datos");

static int promedio=0;
module_param(promedio, int , 0660);
MODULE_PARM_DESC(promedio,"Promedio de los datos del arreglo");

static int pares[MAX]={-1, -1, -1, -1, -1, -1, -1, -1};
module_param_array(pares, int, &numEle2, 0660);
MODULE_PARM_DESC(datos, "Arreglo de números pares");

struct task_struct *khilo;

void promedio_todo(void){
  register int i;
  promedio=0;
  for(i=0;i<numEle;i++){
    promedio+=datos[i];
  }
  promedio/=numEle;
}

void burbuja_f(void){
  register int i,j,aux;
  for(i=0;i<numEle;i++){
    for(j=i;j<numEle;j++){
      if(datos[i]>datos[j]){
        aux=datos[j];
        datos[j]=datos[i];
        datos[i]=aux;
      }
    }
  }
}

void pares_f(void){
  register int i,j=0;
  for(i=0;i<MAX;i++){
    pares[i]=-1;
  }
  for(i=0;i<numEle;i++){
    if(datos[i]%2==0){
      pares[j]=datos[i];
      j++;
    }
  }
  numEle2=j;
}
static int hilo_kernel(void* arg){
	int id = *(int*)arg;
  register int i;
	printk(KERN_INFO "Iniciando hilo");
	while(!kthread_should_stop()){
    schedule();
    if(irq==1){
      promedio_todo();
      printk(KERN_INFO "\tEl promedio entero es: %d\n",promedio);
      irq=0;
    }else if(irq==2){
      burbuja_f();
      printk(KERN_INFO "\tOrdenamiento\n");
      for(i=0;i<numEle;i++){
        printk(KERN_INFO "\tdatos[%d]=%d\n",i,datos[i]);
      }
      irq=0;
    }else if(irq==3){
      pares_f();
      printk(KERN_INFO "\tPares\n");
      for(i=0;i<numEle2;i++){
        printk(KERN_INFO"\tPar[%d]=%d",i,pares[i]);
      }
      irq=0;
    }
  }
  return id;
}

static int __init funcion_init(void){
  static int id_thread = 10;
  printk(KERN_INFO "Inicializando modulo de tarea 1\n");

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
  printk(KERN_INFO "Terminando la ejecucion del modulo de tarea 1\n");
  ret_khilo=kthread_stop(khilo);
  if(ret_khilo==EINTR){
    printk(KERN_ERR"Error trying to stop thread\n");
  }else{
    printk(KERN_INFO"Stop thread with value %d\n",ret_khilo);
  }
}

module_init( funcion_init );
module_exit( funcion_exit );
