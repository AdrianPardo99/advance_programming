#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#define MAX 8

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Esta es la tarea 1 de modulos de kernel y funciones callback");
MODULE_VERSION("1.0");

static int promedio=0;
module_param(promedio, int , 0660);
MODULE_PARM_DESC(promedio,"Promedio de los datos del arreglo");
static int datos[MAX] = {5, 78, -23, 97, 12, -5, 7, 44};
static int numEle = MAX,numEle2=0;
module_param_array(datos, int, &numEle, 0660);
MODULE_PARM_DESC(datos, "Arreglo de datos");
static int pares[MAX]={-1, -1, -1, -1, -1, -1, -1, -1};
module_param_array(pares, int, &numEle2, 0660);
MODULE_PARM_DESC(datos, "Arreglo de números pares");

void promedio_todo(void);
void burbuja_f(void);
void pares_f(void);

static int irq = 0;
int set_irq(const char *v,const struct kernel_param *k){
  int ret;
  register int i;
  printk(KERN_INFO"Callback 'set' irq\n");
  if((ret=param_set_int(v,k))==0){
    printk(KERN_INFO"Int irq set: %d\n",irq);
    if(irq==1){
      promedio_todo();
      printk(KERN_INFO"\tEl promedio entero es: %d\n",promedio);
    }else if(irq==2){
      burbuja_f();
      printk(KERN_INFO "\tOrdenamiento\n");
      for(i=0;i<numEle;i++){
        printk(KERN_INFO "\tdatos[%d]=%d\n",i,datos[i]);
      }
    }else if(irq==3){
      pares_f();
      printk(KERN_INFO "\tPares\n");
      for(i=0;i<numEle2;i++){
        printk(KERN_INFO"\tPar[%d]=%d\n",i,pares[i]);
      }
    }
  }
  return ret;
}

int get_irq(char *b,const struct kernel_param *k){
  int ret;
  printk(KERN_INFO"Callback 'get' irq\n");
  if((ret=param_get_int(b,k))>0){
    printk(KERN_INFO"Numero de bytes de salida: %d\n",ret);
  }

  return ret;
}
static const struct kernel_param_ops irq_kp={
  .set=set_irq,
  .get=get_irq
};

module_param_cb(irq,&irq_kp,&irq,0660);
MODULE_PARM_DESC(irq, "Número de operación\n\t\t\t1)Promedio del arreglo"
  "\n\t\t\t2)Ordenamiento burbuja\n\t\t\t3)Pares del arreglo");

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

static int __init funcion_init(void){
  printk(KERN_INFO "Inicializando modulo de tarea callback 1\n");
  return 0;
}

static void __exit funcion_exit(void){
  printk(KERN_INFO "Terminando la ejecucion del modulo de tarea callback 1\n");
}

module_init( funcion_init );
module_exit( funcion_exit );
