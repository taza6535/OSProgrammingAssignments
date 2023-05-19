#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h> 

#define BUFFER_SIZE 756 


/* Define device_buffer and other global data structures you will need here */

int numTimesOpened=0;
int numTimesClosed=0;
char  * device_buffer;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	printk(KERN_ALERT "Read called with length: %d and offset: %d", length, *offset);
	if((*offset+length) > BUFFER_SIZE){
		printk(KERN_ALERT "Trying to read out of bounds.\n");
		return -1; 
	}
	int couldNotBeRead = copy_to_user(buffer, device_buffer + *offset, length); 
	if(couldNotBeRead==0){
		*offset= *offset + length;
		printk(KERN_ALERT "Number of bytes read: %d\n", length);
	}
	else{
		*offset = *offset + (length-couldNotBeRead);
		printk(KERN_ALERT "Number of bytes read: %d\n", length-couldNotBeRead);
		printk(KERN_ALERT "Number of bytes that could not be read: %d\n", couldNotBeRead);
	}


	return (length - couldNotBeRead);
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	printk(KERN_ALERT "Write called with length: %d and offset: %d", length, *offset);
	if(*offset + length > BUFFER_SIZE){
		printk(KERN_ALERT "Trying to write out of bounds.\n");
		return -1;
	}
	int couldNotWrite = copy_from_user(device_buffer+*offset, buffer, length);
	if(couldNotWrite==0){
		*offset = *offset + length; 
		printk(KERN_ALERT "Number of bytes written: %d\n", length);
	}
	else{
		*offset = *offset + (length-couldNotWrite);
		printk(KERN_ALERT "Number of bytes written: %d\n", length-couldNotWrite);
		printk(KERN_ALERT "Number of bytes that could not be written: %d\n", couldNotWrite);

	}
	return (length - couldNotWrite);
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	numTimesOpened = numTimesOpened + 1;
	printk(KERN_ALERT "The device is opened. The number of times the device has been opened: %d\n", numTimesOpened);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	numTimesClosed = numTimesClosed + 1;
	printk(KERN_ALERT "The device is closed. The number of times this device has been closed: %d\n", numTimesClosed);
	return 0; 
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t position=0;
	if(whence==0){ //position set to offset
		position = offset;
	}
	else if(whence==1){ //current position incremented by offset
		position = pfile->f_pos + offset;
	}
	else if(whence==2){ //position of offset bytes from end of the file
		position = BUFFER_SIZE + offset; 
	}
	if(position < 0){
		printk(KERN_ALERT "position less than 0: %d", position);
		position = BUFFER_SIZE - (-position); //wrap around to end of the buffer
	}
	position = position % BUFFER_SIZE;
	//while(position >= BUFFER_SIZE){ //wrap to the beginning of the buffer
		//int wrapAround = position - BUFFER_SIZE;
		//position = wrapAround;
	//}
	pfile->f_pos = position;
	printk(KERN_ALERT "The position has been set to: %d", position);
	return position;
}

struct file_operations pa2_char_driver_file_operations = { 

	.owner   = THIS_MODULE,
	.open = pa2_char_driver_open,
    .release = pa2_char_driver_close, 
    .read = pa2_char_driver_read,  
    .write = pa2_char_driver_write,  
    .llseek = pa2_char_driver_seek
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */

	printk(KERN_ALERT "Init function has been called\n");
	int opened = register_chrdev(240, "simple_character_device", &pa2_char_driver_file_operations);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if(opened<0){
		printk(KERN_ALERT "Device could not be opened.\n");
		return -1;
	}
	else{
		printk(KERN_ALERT "Device successfully registered.\n");
		return 0;
	}

	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_ALERT "The exit function has been called.\n");
	kfree(device_buffer);
	unregister_chrdev(240, "simple_character_device");
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
