#include <stdint.h>
#ifndef MMAP_H

/*
 *
 * @author Adrian González Pardo
 * @date   28/04/2021
 * @email  gozapaadr@gmail.com
 * @github AdrianPardo99
 *
**/

#define MMAP_H

/*
 * @brief: Function that config a pin of GPIO
 * @args:
 *    uint32_t is the virtual direction of the GPIO
 *    int is the number of the GPIO
 *    int is the mode that we use the GPIO if 0 is output if 1 is input
**/
void config_gpio(volatile uint32_t*,int,int);

/*
 * @brief: Create the structure for use the RaspberryPi gpio
 * @args:
 *  int indicate the filedescriptor
 *  int indicate the index of the PI_Base 0->Pi4 or 1->Pi3
**/
uint32_t *create_offset_gpio(int,int);

/*
 * @brief: Set the gpio(first int) as the value of the second int
 * @args:
 *    uint32_t is the virtual direction of the GPIO
 *    int is the number of the GPIO
 *    int is value that obtain the GPIO
**/
void value_gpio(volatile uint32_t*,int,int);

/*
 * @brief: Create character to array that size 10 bits
 * @args: char indicate the caracter between a to z and A to Z,
 *  include space in ascii
**/
int *character_to_led(char);

/*
 * @brief: Create character to array that size 7 bits
 * @args:
 *    char indicate the caracter between 0 to 9,
 *      include space in ascii
 *    int is array where store the data
**/
void seven_seg_number(char,int*);
#endif
