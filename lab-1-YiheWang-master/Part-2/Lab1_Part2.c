Name        : Lab1_Part2.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

MODULE_LICENSE("GPL");


int init_module(void){
	printk("Hello world\n");

	unsigned long *GPFSEL0;
	unsigned long *GPSET0;
	GPFSEL0 = (unsigned long *)ioremap(0x3F200000,4096);
	GPSET0 = (unsigned long *)ioremap(0x3F20001C,4096);

	*GPFSEL0 = * GPFSEL0 | (1 << 6);
	*GPSET0 = * GPSET0| (1 << 2);//bit masking, turn on red LED

	*GPFSEL0 = * GPFSEL0 | (1 << 9);
	*GPSET0 = * GPSET0| (1 << 3);//bit masking, turn on yellow LED

	*GPFSEL0 = * GPFSEL0 | (1 << 12);
	*GPSET0 = * GPSET0| (1 << 4);//bit masking, turn on yellow LED

	*GPFSEL0 = * GPFSEL0 | (1 << 15);
	*GPSET0 = * GPSET0| (1 << 5);//bit masking, turn on yellow LED

	return 0;
}

void cleanup_module(void){
	printk("Have a good day\n");

	unsigned long *GPCLR0;
	GPCLR0 =  (unsigned long *)ioremap(0x3F200028,4096);

	*GPCLR0 = *GPCLR0 | (1 << 2);//bit masking, turn off red LED

	*GPCLR0 = *GPCLR0 | (1 << 3);//bit masking, turn off red LED

	*GPCLR0 = *GPCLR0 | (1 << 4);//bit masking, turn off red LED

	*GPCLR0 = *GPCLR0 | (1 << 5);//bit masking, turn off red LED

}


