#include <stdint.h>

#ifndef MAPGPIO_H

#define MAPGPIO_H

/* En caso de que no funcione teclear
* sudo cat /proc/iomem | grep gpio
* Copiar la primera dirección como hex
* Para definir el banco de memoria de acceso al GPIO
**/
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

/*
 * @brief: Create the structure for use the RaspberryPi gpio
 * @args:
 *  int indicate the filedescriptor
 *  int indicate the index of the PI_Base 0->Pi4 or 1->Pi3
**/
uint32_t *create_offset_gpio(int,int);

/*
 * @brief: Create character to array that size 10 bits
 * @args: char indicate the caracter between a to z in ascii code
**/
int *character_to_led(char);

/*
 * @brief: Function that recive two array
 * @args:
 *  int* indicate the gpio pins for use
 *  int* indicate the values for assing in the gpio
 *  int  indicate the size of the array
**/
void set_gpio(int*,int*,int);

#endif
