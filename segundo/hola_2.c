#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init funcion_inicio(void){
  printk(KERN_INFO "Hola mundo, Iniciando el modulo 2.\n");
  return 0;
}

static void __exit funcion_exit(void){
  printk(KERN_INFO "Terminando la ejecución del modulo 2.\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
