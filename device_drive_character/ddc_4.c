/*
 *  ddc-4.c - Asignacion de numero mayor y numero menor
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
#include <linux/cdev.h>     /* Necesaria para hacer uso del cdev */

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("VICTOR H GARCIA O");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Manejo de macro __init");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");				/* Versión del módulo 				*/
MODULE_INFO(driver, "PULSO CARDIACO"); 		/* Información personalizada del usuario	*/

dev_t dispositivo;
static struct class *dev_class;
static struct device *dev_file;
static struct cdev dev_cdev;

static int driver_open(struct inode*, struct file*);
static int driver_release(struct inode*, struct file*);
static ssize_t driver_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t driver_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops={
  .owner=THIS_MODULE,
  .read=driver_read,
  .write=driver_write,
  .open=driver_open,
  .release=driver_release
};

static int driver_open(struct inode *inode, struct file *file){
  printk(KERN_INFO "Ejecutandose la operación open\n");
  return 0;
}

static int driver_release(struct inode *inode, struct file *file){
  printk(KERN_INFO "Ejecutandose la operación release\n");
  return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
  printk(KERN_INFO "Ejecutandose la operación driver read\n");
  return 0;
}
static ssize_t driver_write(struct file *filp, const char *buf, size_t len, loff_t *off){
  printk(KERN_INFO "Ejecutandose la operación driver write\n");
  return len;
}

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

  cdev_init(&dev_cdev,&fops);
  ret=cdev_add(&dev_cdev,dispositivo,1);
  if(ret<0){
    printk(KERN_ERR "Error al crear los file_operations");
    unregister_chrdev_region( dispositivo, 1 );
    return ret;
  }
  printk(KERN_INFO "File Operators creado exitosamente... \n");

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
  cdev_del(&dev_cdev);
	unregister_chrdev_region( dispositivo, 1 );
}

module_init(funcion_inicio);
module_exit(funcion_exit);
