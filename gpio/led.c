#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>

static volatile uint32_t *gpio;
#define CICLOS 10

/* GPIO controller for raspberry 4 */
#define GPIO_BASE_PI4 0xfe200000
/* GPIO controller for raspberry 3 */
#define GPIO_BASE_PI3 0x3f200000

/* Para habilitar el GPIO #  */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
/* Habilita el PIN para la salida */
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
/* Habilita el PIN de manera alterna */
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
/* Cambia el PIN a 1 lógico, entrada es 1<<(gpio) */
#define GPIO_SET *(gpio+7)
/* Cambia el PIN a 0 lógico, entrada 1<<(gpio) */
#define GPIO_CLR *(gpio+10)
/* Verifica si el gpio es un valor 0 o 1 lógico */
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

int main(void){
  int fd;
  register int c=0;
	/* Se obtiene un manejador a la memoria física
	 * /dev/mem es un device driver de caracter que contiene una imagen de la
	 * memoria principal de la computadora.
	 *
     	 * Los Bytes de direcciones en /dev/mem son interpretados como direcciones de
	 * la memoria física.
	 * O_RDWR - Bandera para lectura y escritura
	 * O_SYNC - Bandera para sincronia. El archivo es abierto para acceso de I/O sincrono
	 * Cualquier escritura en el despriptor de archivo bloqueara la llamada a proceso hasta
	 * que el dato se haya escrito fisicamente.
	 */

	fd=open("/dev/mem", O_RDWR | O_SYNC);
	if(fd==-1){
		printf("Error al abrir /dev/mem %s \n",strerror(errno)),exit( EXIT_FAILURE );
  }

	/* Se obtiene una pagina de memoria en el espacio de direcciones virtuales
	 * a partir del offset 0x3f200000. En este offset se encuentran
	 * mapeados los perifericos. Consultar en el FHS en /proc/interrupts
	 * El primer parametro indica que la Direccion de inicio del mapa es cero
	 * El segundo parametro es el número de bytes a mapear
	 * El tercer parametro indica que la pagina puede ser de lectura y escritura
	 * El cuarto parametro indica que las modificaciones al mapa seran visibles a otros procesos
	 * El quinto parametro es el manejador de lamemoria fisica
	 * El sexto parametro es la dirección de inicio dell segmento
  **/
  gpio=
  (uint32_t *)mmap(
    0,
    getpagesize(),
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    fd,
    GPIO_BASE_PI4
  );
  if(gpio==MAP_FAILED){
    printf("Error al obtener direcion virtual: %s\n", strerror(errno));
    return -1;
  }
  printf("La direccion virtual es: %x\n", gpio);
  /* Se configura el BCM GPIO16 como salida */
  int arr[]={5,1,12,6,13,19,26,16,20,21};
  register int i;
  for(i=0;i<10;i++){
    INP_GPIO(arr[i]);
    OUT_GPIO(arr[i]);
  }
  while(c<CICLOS){
    /* Se pone en uno el GPIO 16 */
    for(i=0;i<10;i++){
      GPIO_SET=1<<arr[i];
      usleep(500000/2);
    }

    /* Se pone en cero el GPIO 16 */
    for(i=0;i<10;i++){
      GPIO_CLR=1<<arr[i];
      usleep(500000/2);
    }
    c++;
  }
  close(fd);
	return 0;
}
