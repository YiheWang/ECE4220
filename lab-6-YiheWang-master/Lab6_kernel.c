/*
 ============================================================================
 Name        : c.c
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
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define MSG_SIZE 50
#define CDEV_NAME "Lab6"	// "YourDevName"

MODULE_LICENSE("GPL");

static int major; 
static char msg[MSG_SIZE];

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/

int mydev_id;	// variable needed to identify the handler
unsigned long *GPFSEL; // Select register to set it to input/output mode
unsigned long *GPSET; // Set register
unsigned long *GPCLR; // Clear register, clear a pin
unsigned long *GPEDS; // Eevet detect register
unsigned long *GPAREN; // Asynochronous rising edge detection
unsigned long *GPPUD; // pull up and pull down enable 
unsigned long *GPPUDCLK; // clock for pull up and down

int flag = 1;

// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.
unsigned long timer_interval_ns = 1e6;	// timer interval length (nano sec part)
static struct hrtimer hr_timer;			// timer structure
static int count = 0, dummy = 0;

// Function called when the user space program reads the character device.
// Some arguments not used here.
// buffer: data will be placed there, so it can go to user space
// The global variable msg is used. Whatever is in that string will be sent to userspace.
// Notice that the variable may be changed anywhere in the module...
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
	// Whatever is in msg will be placed into buffer, which will be copied into user space
	ssize_t dummy = copy_to_user(buffer, msg, length);	// dummy will be 0 if successful

	// msg should be protected (e.g. semaphore). Not implemented here, but you can add it.
	msg[0] = '\0';	// "Clear" the message, in case the device is read again.
					// This way, the same message will not be read twice.
					// Also convenient for checking if there is nothing new, in user space.
	
	return length;
}

// Function called when the user space program writes to the Character Device.
// Some arguments not used here.
// buff: data that was written to the Character Device will be there, so it can be used
//       in Kernel space.
// In this example, the data is placed in the same global variable msg used above.
// That is not needed. You could place the data coming from user space in a different
// string, and use it as needed...
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
	ssize_t dummy;
	
	if(len > MSG_SIZE)
		return -EINVAL;
	
	// unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
	dummy = copy_from_user(msg, buff, len);	// Transfers the data from user space to kernel space
	if(len == MSG_SIZE)
		msg[len-1] = '\0';	// will ignore the last character received.
	else
		msg[len] = '\0';
	
	switch(msg[1]){
		case 'A':
			timer_interval_ns = 1000000;
			break;
		case 'B':
			timer_interval_ns = 800000;
			break;
		case 'C':
			timer_interval_ns = 600000;
			break;
		case 'D':
			timer_interval_ns = 400000;
			break;
		case 'E':
			timer_interval_ns = 200000;
			break;
	}
	
	return len;		// the number of bytes that were written to the Character Device.
}

static struct file_operations fops = {
	.read = device_read, 
	.write = device_write,
};


static irqreturn_t button_isr(int irq, void *dev_id)
{
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);

	//0000 0001 0000 0000 0000 0000 button1 
	if((*GPEDS & 0x10000) == 0x10000){
		printk("Pressing Button 1");
		timer_interval_ns = 1000000;
	}
	//0000 0010 0000 0000 0000 0000 button2 
	else if((*GPEDS & 0x20000) == 0x20000){
		printk("Pressing Button 2");
		timer_interval_ns = 800000;
	}
	//0000 0100 0000 0000 0000 0000 button3
	else if((*GPEDS & 0x40000) == 0x40000){
		printk("Pressing Button 3");
		timer_interval_ns = 600000;
	}
	//0000 1000 0000 0000 0000 0000 button4
	else if((*GPEDS & 0x80000) == 0x80000){
		printk("Pressing Button 4");
		timer_interval_ns = 400000;
	} 
	//0001 0000 0000 0000 0000 0000 button5
	else if((*GPEDS & 0x100000) == 0x100000){
		printk("Pressing Button 5");
		timer_interval_ns = 200000;
	}
	
	*GPEDS = 0xFFFFFFFF; // clear the event detect status
	// This same handler will be called regardless of what button was pushed,
	// assuming that they were properly configured.
	// How can you determine which button was the one actually pushed?
		
	// DO STUFF (whatever you need to do, based on the button that was pushed)

	// IMPORTANT: Clear the Event Detect status register before leaving.	
	
	printk("Interrupt handled\n");	
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

// Timer callback function: this executes when the timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
  	ktime_t currtime, interval;	// time type, in nanoseconds
	unsigned long overruns = 0;

	// Re-configure the timer parameters (if needed/desired)
  	currtime  = ktime_get();
  	interval = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)

	// Advance the expiration time to the next interval. This returns how many
	// intervals have passed. More than 1 may happen if the system load is too high.
  	overruns = hrtimer_forward(timer_for_restart, currtime, interval);


	if(flag){
		*GPSET = *GPSET | 0x40;
		flag = 0; //reverse the flag
	}// set
	else{
		*GPCLR = *GPCLR | 0x40;
		flag = 1;
	}// clear
	//switch between high and low


	return HRTIMER_RESTART;	// Return this value to restart the timer.
							// If you don't want/need a recurring timer, return
							// HRTIMER_NORESTART (and don't forward the timer).
}

int timer_init(void)
{
	major = register_chrdev(0, CDEV_NAME, &fops);
	if (major < 0) {
     		printk("Registering the character device failed with %d\n", major);
	     	return major;
	}
	printk("Lab6_cdev_kmod example, assigned major: %d\n", major);
	printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);

	// Configure and initialize timer
	ktime_t ktime = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)

	// CLOCK_MONOTONIC: always move forward in time, even if system time changes
	// HRTIMER_MODE_REL: time relative to current time.
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	// Attach callback function to the timer
	hr_timer.function = &timer_callback;

	
	int dummy = 0;

	// Map GPIO registers
	GPFSEL = (unsigned long *)ioremap(0x3F200000, 4096);
	GPSET = (unsigned long *)ioremap(0x3F20001C, 4096);
	GPCLR = (unsigned long *)ioremap(0x3F200028, 4096);
	GPEDS = (unsigned long *)ioremap(0x3F200040, 4096);	
	GPAREN = (unsigned long *)ioremap(0x3F20007C, 4096);
	GPPUD = (unsigned long *)ioremap(0x3F200094, 4096);
	GPPUDCLK = (unsigned long *)ioremap(0x3F200098, 4096);	
			
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers
	
	*GPFSEL = *GPFSEL | 0x40000; // pin number for speaker is 6
	++GPFSEL;// button 1-4 is in next register
	//*GPFSEL &= 0xF3DBFFFF;
	*GPFSEL = *GPFSEL & 0x0;
	++GPFSEL;
	//*GPFSEL &= 0xFFFFFFF8;
	//button 5 is in next register
	*GPFSEL = *GPFSEL & 0x0;// initialize button to input
	
	
	// Don't forget to configure all ports connected to the push buttons as inputs.
	*GPPUD = *GPPUD | 0x1; // enable pull down control
	udelay(100);// wait for 150 cycles
	*GPPUDCLK = *GPPUDCLK | 0x1F0000; //bits 16, 17 ,18, 19, 20
	udelay(100);
	*GPPUD = *GPPUD & 0x0; //remote the control signal
	udelay(100);
	*GPPUDCLK = *GPPUDCLK & 0x0; // remove the clock

	// You need to configure the pull-downs for all those ports. There is
	// a specific sequence that needs to be followed to configure those pull-downs.
	// The sequence is described on page 101 of the BCM2835-ARM-Peripherals manual.
	// You can use  udelay(100);  for those 150 cycles mentioned in the manual.
	// It's not exactly 150 cycles, but it gives the necessary delay.
	// WiringPi makes it a lot simpler in user space, but it's good to know
	// how to do this "by hand".
		
	*GPAREN = *GPAREN | 0x1F0000; //bits 16, 17, 18, 19, 20
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
			
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	printk("Button Detection enabled.\n");
	// Start the timer
 	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);

	return 0;
}

void timer_exit(void)
{
	*GPEDS = *GPEDS | 0xFFFFFFFF;
	*GPAREN = *GPAREN | 0x0;
	// Remove the interrupt handler; you need to provide the same identifier
    	free_irq(79, &mydev_id);
	
	printk("Button Detection disabled.\n");
	int ret;
  	ret = hrtimer_cancel(&hr_timer);	// cancels the timer.
  	if(ret)
		printk("The timer was still in use...\n");
	else
		printk("The timer was already canceled...\n");	// if not restarted or
														// canceled before

  	printk("HR Timer module uninstalling\n");

	unregister_chrdev(major, CDEV_NAME);
	printk("Char Device /dev/%s unregistered.\n", CDEV_NAME);	
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "timer_init" will execute when you install your
// module. "timer_exit" will execute when you remove your module.
// You can give different names to those functions.
module_init(timer_init);
module_exit(timer_exit);
