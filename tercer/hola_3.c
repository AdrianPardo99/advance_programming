#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo a.k.a d3vcr4ck");
MODULE_DESCRIPTION("Ejemplo simple");
MODULE_INFO(about,"Modulo 3");

static int __init funcion_inicio(void){
  printk(KERN_INFO "Hola mundo, Iniciando el modulo 3.\n");
  return 0;
}

static void __exit funcion_exit(void){
  printk(KERN_INFO "Terminando la ejecución del modulo 3.\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
