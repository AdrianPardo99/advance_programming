#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("1.0");
MODULE_INFO(about, "Parametros 2");

static char *comando = "derecha";
module_param( comando, charp, 0660 );
MODULE_PARM_DESC( comando, "Comando del motor" );



/*
 * Es necesario crear funciones set y get de los parametros por cada
 *	parametro a utilizar
**/
static int irq = 10;
int set_param(const char *v,const struct kernel_param *kp){
	int param_arg=*(int*)kp->arg,ret;
	printk(KERN_INFO"Función Call Back 'set' ejecutada\n");
	/* Modifica el param de lado del usuario y verifica
	 * int param_set_int(const char*, const struct kernel_param);
	**/
	ret=param_set_int(v,kp);
	if(ret==0){
		printk(KERN_INFO"Param str: %s\n",v);
		printk(KERN_INFO"Param arg: %d\n",param_arg);
		printk(KERN_INFO"Int irq: %d\n",irq);
	}
	return ret;
}

int get_param(char *buffer, const struct kernel_param *kp){
	int param_arg=*(int*)kp->arg,ret;
	printk(KERN_INFO"Función Call Back 'get' ejecutada\n");
	ret=param_get_int(buffer,kp);
	if(ret>0){
		printk(KERN_INFO"Param buffer: %s\n",buffer);
		printk(KERN_INFO"Param arg: %d\n",param_arg);
		printk(KERN_INFO"Ret info: %d\n",ret);
		return ret;
	}
	return ret;
}

static const struct kernel_param_ops my_op_params={
	.set=set_param,
	.get=get_param
};

module_param_cb( irq, &my_op_params, &irq, 0660 );
MODULE_PARM_DESC( irq, "Número de interrupción" );

static int __init funcion_init(void){
	printk(KERN_INFO"Iniciando el ejemplo 1 de parametros callback\n");
	printk(KERN_INFO"La irq en la funcion inicio es: %d \n", irq);
	printk(KERN_INFO"El comando en la funcion inicio es: %s \n", comando);
	return 0;
}

static void __exit funcion_exit(void){
	printk(KERN_INFO"Terminando la ejecucion del ejemplo 1 de parametros callback\n");
	printk(KERN_INFO"La irq en la funcion exit es: %d \n", irq);
	printk(KERN_INFO"El comando en la funcion exit es: %s \n", comando);
}

module_init( funcion_init );
module_exit( funcion_exit );
