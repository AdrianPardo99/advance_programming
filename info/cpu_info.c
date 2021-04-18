#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cpumask.h>

#define NUM_PROCS num_online_cpus()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Ejemplo del numero de cpus de la arquitectura");
MODULE_INFO(about,"Modulo CPU Info");

static int __init funcion_inicio(void){
  printk(KERN_INFO "CPUs: %d\n",NUM_PROCS);
  return 0;
}

static void __exit funcion_exit(void){
  printk(KERN_INFO "Terminando la ejecución modulo.\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
