/*
 *  ddc_2.c - Asignación de numero mayor y numero menor en un
 *		device driver de caracter de manera dinamica
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("Uso y manejo de asignación numeral");
MODULE_VERSION("1.0");
MODULE_INFO(driver, "PULSO CARDIACO");

/* Revisar disponibilidad en /proc/devices */
dev_t dispositivo;

static int __init funcion_inicio(void){
	int ret;
	printk(KERN_INFO "Init Device Driver Character\n");
	ret=alloc_chrdev_region(&dispositivo,0,1,"DDC_GENETIC");
	if(ret<0){
		printk(KERN_ERR"Error trying to resgistry the module");
		return ret;
	}
	printk(KERN_INFO"Major number %d\n",MAJOR(dispositivo));
	printk(KERN_INFO"Minor number %d\n",MINOR(dispositivo));
	return 0;
}

static void __exit funcion_exit(void){
	printk(KERN_INFO "End Device Driver Character\n");
	unregister_chrdev_region(dispositivo,1);
}

module_init(funcion_inicio);
module_exit(funcion_exit);
