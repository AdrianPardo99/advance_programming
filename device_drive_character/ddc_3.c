/*
 *  ddc-3.c - Asignacion de numero mayor y numero menor
 *	(se usa asignación dinámica).
 * 	Se realiza la creación de clase y archivo de dispositivo
 *	(se usa creación automática).
 * 	Para un device driver de caracter.
 */
#include <linux/module.h>			/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>			/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>				/* Necesaria para las macros			*/
#include <linux/kdev_t.h>			/* Necesaria para las macros MAJOR, MINOR	*/
#include <linux/fs.h>				/* Necesaria para las funciones de registro	*/
#include <linux/device.h>			/* Necesaria para el dispositivo		*/

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("VICTOR H GARCIA O");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Manejo de macro __init");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");				/* Versión del módulo 				*/
MODULE_INFO(driver, "PULSO CARDIACO"); 		/* Información personalizada del usuario	*/

dev_t dispositivo;
static struct class * dev_class;
static struct device * dev_file;

static int __init funcion_inicio(void){
	int ret;

	printk(KERN_INFO "Iniciando el DDC \n");
	printk(KERN_INFO "Registrando el device driver de caracter... \n");

	ret = alloc_chrdev_region( &dispositivo, 0, 1, "ESCOM_DDC" );
	if( ret < 0 ){
		printk(KERN_ERR "Error al registrar el device driver de caracter \n");
		return ret;
	}
	printk(KERN_INFO "Dispositivo Registrado exitosamente... \n");
	printk(KERN_INFO "Numero mayor asignado: %d \n", MAJOR(dispositivo));
	printk(KERN_INFO "Numero menor asignado: %d \n", MINOR(dispositivo));

	dev_class = class_create(THIS_MODULE, "ESCOM_class");
	if( IS_ERR( dev_class ) ){
		printk(KERN_ERR "Error al crear la clase del dispositivo \n");
		unregister_chrdev_region( dispositivo, 1 );

		return PTR_ERR(dev_class);
	}
	printk(KERN_INFO "Clase creada exitosamente... \n");

	dev_file = device_create( dev_class, NULL, dispositivo, NULL, "ESCOM_device");
	if( IS_ERR( dev_file ) ){
		printk(KERN_ERR "Error al crear el dispositivo \n");
		class_destroy( dev_class );
		unregister_chrdev_region( dispositivo, 1 );

		return PTR_ERR(dev_file);
	}
	printk(KERN_INFO "Dispositivo creado exitosamente... \n");

	return 0;
}

static void __exit funcion_exit(void){
	printk(KERN_INFO "Terminando la ejecucion del DDC \n");

	device_destroy( dev_class, dispositivo );
	class_destroy( dev_class );
	unregister_chrdev_region( dispositivo, 1 );
}

module_init(funcion_inicio);
module_exit(funcion_exit);
