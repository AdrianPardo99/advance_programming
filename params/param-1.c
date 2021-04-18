#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/

/*
 *  param-1.c - Demostracion de parametros en el modulo de kernel
 *  se usa un parametro entero (int)
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Este es el ejemplo de parametros 1");
MODULE_VERSION("1.0");
MODULE_INFO(about, "Parametros 1");

static int irq = 10;
module_param( irq, int, 0660 );
MODULE_PARM_DESC( irq, "Número de interrupción" );

static int __init funcion_init(void){
 printk(KERN_INFO "Iniciando el ejemplo 1 de parametros\n");
 printk(KERN_INFO "La irq en la funcion inicio es: %d \n", irq);
 return 0;
}

static void __exit funcion_exit(void){
 printk(KERN_INFO "Terminando la ejecucion del ejemplo 3 de parametros\n");
 printk(KERN_INFO "La irq en la funcion exit es: %d \n", irq);
}

module_init( funcion_init );
module_exit( funcion_exit );
