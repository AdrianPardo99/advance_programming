#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#define N 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Esta es la tarea 2 de modulos de kernel y funciones callback");
MODULE_VERSION("1.0");

static char *ch="e";
module_param(ch, charp, 0660);
MODULE_PARM_DESC(ch, "Variable que contiene el caracter específico a"
  " buscar en la op 1");

static int res=0;
module_param(res, int, 0660);
MODULE_PARM_DESC(res, "Variable que contiene el valor resultante de la operación");

static char cadena[N]="esta es la optativa de advanced computing systems programming";

static int op=0;
int set_op(const char *v,const struct kernel_param *k){
  int ret;
  register int i;
  printk(KERN_INFO"Callback 'set' op\n");
  if((ret=param_set_int(v,k))==0){
    printk(KERN_INFO"Int op set: %d\n",op);
    if(op==1||op==2){
      res=0;
      i=0;
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
      printk(KERN_INFO"El resultado de la operacion %s es: %d\n",
        (op==1)?"la cantidad del caracter contenido en la cadena":
        "la cantidad totla de caracteres",res);
      op=0;
    }
  }
  return ret;
}

int get_op(char *b,const struct kernel_param *k){
  return 0;
}

static const struct kernel_param_ops op_kp={
  .set=set_op,
  .get=get_op
};

module_param_cb(op,&op_kp, &op, 0660);
MODULE_PARM_DESC(op, "Número de operación\n\t\t\t1)Cuenta cuantas veces el "
  "caracter esta en la cadena\n\t\t\t2)Cuenta la longitud total de cadena");

int set_cadena(const char *v,const struct kernel_param *k){
  int len=strlen(v);
  printk(KERN_INFO"Callback 'set' cadena\n");
  if(len>N){
    printk(KERN_ERR"Error the string it's too large\n");
    return -ENOSPC;
  }else if(len<=1){
    printk(KERN_ERR"Error the string it's empty\n");
    return -EINVAL;
  }
  strcpy(k->arg,v);
  printk(KERN_INFO"Param set: %s\n",cadena);
  return 0;
}

int get_cadena(char *b, const struct kernel_param *k){
  printk(KERN_INFO"Callback 'get' cadena\n");
  strcpy(b,(char*)k->arg);
  return strlen(b)+1;
}

static const struct kernel_param_ops cadena_kp={
  .set=set_cadena,
  .get=get_cadena
};
module_param_cb(cadena,&cadena_kp,&cadena,0660);
MODULE_PARM_DESC(cadena, "Cadena de caracteres");

static int __init funcion_init(void){
  printk(KERN_INFO "Inicializando modulo de tarea callback 2\n");
  return 0;
}

static void __exit funcion_exit(void){
  printk(KERN_INFO "Terminando la ejecucion del modulo de tarea callback 2\n");
}

module_init( funcion_init );
module_exit( funcion_exit );
