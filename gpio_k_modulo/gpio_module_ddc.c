#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include<linux/delay.h>
#include <asm/io.h>

#define MEM_SIZE 1024
//#define ADDR_BASE_GPIO 0x3F200000		//Direccion base para Raspberry Pi3 B+
#define ADDR_BASE_GPIO 0xFE200000		//Direccion base para Raspberry Pi4 B

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian González Pardo, Samuel Alejandro Jurado Macias,"
  " Raymundo Pulido Bejarano");
MODULE_DESCRIPTION("GPIO a 7 segmentos con character device driver");
MODULE_VERSION("1.0");

volatile uint32_t *gpio_virtual;
int arr_gpio[]={17,18,27,22,23,24,10},size=7,segment[7];
void config_gpio(volatile uint32_t*,int,int);
void value_gpio(volatile uint32_t*,int,int);
void seven_seg_number(char,int*);

dev_t dispositivo;
static struct class *dev_class;
static struct device *dev_file;
static struct cdev dev_cdev;
static char *buffer;


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
  if((buffer=(char*)kmalloc(MEM_SIZE,GFP_KERNEL))==NULL){
    printk(KERN_ERR"Error al crear memoria en el driver");
    return -ENOMEM;
  }
  return 0;
}

static int driver_release(struct inode *inode, struct file *file){
  printk(KERN_INFO "Ejecutandose la operación release\n");
  kfree(buffer);
  return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
  int ret;
  ssize_t lon;
  printk(KERN_INFO "Ejecutandose la operación driver read\n");
  printk(KERN_INFO"Longitud %d, offset: %lld\n",len,*off);
  if((*off==0)&&len>0){
    strcpy(buffer,"Device escrito para uso de GPIO, sin uso en esta función\n");
    lon=strlen(buffer)+1;
    if((ret=copy_to_user(buf,buffer,lon))){
      return -EFAULT;
    }
    (*off)+=lon;
    return lon;
  }
  return 0;
}

static ssize_t driver_write(struct file *filp, const char *buf, size_t len, loff_t *off){
  int ret=0;
  register int i=0,j=0;
  printk(KERN_INFO "Ejecutandose la operación driver write\n");
  printk(KERN_INFO"Tamanio: %d, offset: %lld\n",len,*off);
  if((ret=copy_from_user(buffer,buf,len))){
    printk(KERN_ERR"Error copiando datos del usuario\n");
    return -EFAULT;
  }
  printk(KERN_INFO"Dato recibido: %s\n",buffer);
  /* Codigo de gpio */
  printk(KERN_INFO "Boleta: %s\n",buffer);
  printk(KERN_INFO"Posicion[]\t#\tSegmento de bits\n");
  while(buffer[i]){
    seven_seg_number(buffer[i],&*segment);
    for(j=0;j<7;j++){
      value_gpio(gpio_virtual,arr_gpio[j],segment[j]);
    }
    printk(KERN_INFO"boleta[%d]\t%c\t|%d|%d|%d|%d|%d|%d|%d|\n",
      i,buffer[i],segment[0],segment[1],segment[2],segment[3],segment[4],
      segment[5],segment[6]);
    ssleep(1);
    i++;
  }
  printk(KERN_INFO"Fin de escritura de Boleta: %s\n",buffer);
  for(j=0;j<7;j++){
    value_gpio(gpio_virtual,arr_gpio[j],0);
  }
  ssleep(1);
  return len;
}

void config_gpio(volatile uint32_t *gpio,int v_gpio,int mode){
  int GPFSEL=v_gpio/10,
      range=(v_gpio%10);
  range=(range<<1)+range;
  if(mode==0){
    /*
     * As Output
    **/
    *(gpio+GPFSEL)=(*(gpio+GPFSEL)&~(7<<range))|(1<<range);
  }else{
    /*
     * As input
    **/
    *(gpio+GPFSEL)=(*(gpio+GPFSEL)&~(7<<range));
  }
}

void value_gpio(volatile uint32_t *gpio,int v_gpio,int val){
  int bit=(v_gpio>>5)&1,
      GPSET_CLR;
  if(val==0){
    /*
     * As clr the value of gpio
    **/
    GPSET_CLR=bit+10;
  }else{
    /*
     * As set the value of gpio 1
    **/
    GPSET_CLR=bit+7;
  }
  *(gpio+GPSET_CLR)=(1<<(v_gpio%32));

}

void seven_seg_number(char x,int *arr){
  /* Este funciona para un GND comun en caso contrario solo invertir bits */
  register int i;
  for(i=0;i<7;i++){
    *(arr+i)=0;
  }
  if(x=='0'){
    for(i=0;i<6;i++){
      *(arr+i)=1;
    }
  }else if(x=='1'){
    *(arr+1)=*(arr+2)=1;
  }else if(x=='2'){
    *(arr)=*(arr+1)=*(arr+3)=*(arr+4)=*(arr+6)=1;
  }else if(x=='3'){
    for(i=0;i<4;i++){
      *(arr+i)=1;
    }
    *(arr+6)=1;
  }else if(x=='4'){
    *(arr+1)=*(arr+2)=*(arr+5)=*(arr+6)=1;
  }else if(x=='5'){
    *(arr)=*(arr+2)=*(arr+3)=*(arr+5)=*(arr+6)=1;
  }else if(x=='6'){
    for(i=2;i<7;i++){
      *(arr+i)=1;
    }
    *(arr)=1;
  }else if(x=='7'){
    *(arr)=*(arr+1)=*(arr+2)=1;
  }else if(x=='8'){
    for(i=0;i<7;i++){
      *(arr+i)=1;
    }
  }else if(x=='9'){
    for(i=0;i<4;i++){
      *(arr+i)=1;
    }
    *(arr+5)=*(arr+6)=1;
  }
}

static int __init funcion_inicio(void){
	int ret;
  register int i;
	printk(KERN_INFO "Iniciando el DDC \n");
	printk(KERN_INFO "Registrando el device driver de caracter... \n");

	ret = alloc_chrdev_region( &dispositivo, 0, 1, "GPIO_DDC" );
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

	dev_class = class_create(THIS_MODULE, "GPIO_class");
	if( IS_ERR( dev_class ) ){
		printk(KERN_ERR "Error al crear la clase del dispositivo \n");
		unregister_chrdev_region( dispositivo, 1 );

		return PTR_ERR(dev_class);
	}
	printk(KERN_INFO "Clase creada exitosamente... \n");

	dev_file = device_create( dev_class, NULL, dispositivo, NULL, "GPIO_device");
	if( IS_ERR( dev_file ) ){
		printk(KERN_ERR "Error al crear el dispositivo \n");
		class_destroy( dev_class );
		unregister_chrdev_region( dispositivo, 1 );

		return PTR_ERR(dev_file);
	}
	printk(KERN_INFO "Dispositivo creado exitosamente... \n");
  printk(KERN_INFO"Configurando GPIO\n");
  gpio_virtual=(uint32_t *)ioremap(ADDR_BASE_GPIO, 0X30);
  for(i=0;i<size;i++){
    config_gpio(gpio_virtual,arr_gpio[i],0);
  }
  printk(KERN_INFO"GPIO habilitado correctamente.\n");
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
