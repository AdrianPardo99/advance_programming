#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void){
  printk(KERN_INFO "Hola mundo, Iniciando el modulo 1.\n");
  return 0;
}

void cleanup_module(void){
  printk(KERN_INFO "Terminando la ejecución del modulo 1.\n");
}
