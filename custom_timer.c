#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julia Kozel");
MODULE_VERSION("0.01");

static struct timer_list custom_timer;
static int delay = 7;
static int times_left = 4;
static struct kobject *customtimer_kobject;

static void print_text(unsigned long data)
{
	if (times_left > 0) {
		printk(KERN_INFO "U r in lab 6\n");
		times_left--;
	} 
	//jiffies определяет количество временных тиков (тактов)
	//, прошедших после загрузки системы	
	mod_timer(&custom_timer, jiffies + delay * HZ);
}

static ssize_t customtimer_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", times_left);
}

static ssize_t customtimer_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%d", &times_left);
	return count;
}

static struct kobj_attribute times_left_attribute =__ATTR(times_left, 0660, customtimer_read, customtimer_write);

static int customtimer_init(void)
{
	int error = 0;	

	//create a kobject and place it in sysfs in the location
	//underneath the specified parent kobject
	customtimer_kobject = kobject_create_and_add("customtimer", kernel_kobj);
	if (!customtimer_kobject) {
		return -ENOMEM;
	}

	//associate the attribute structure pointed at by attr 
	//with the kobject pointed at by kobj
	error = sysfs_create_file(customtimer_kobject, &times_left_attribute.attr);
	if (error) {
		printk(KERN_INFO "failed to create the times_left file in /sys/kernel/customtimer \n");
		return error;
	}
	init_timer_on_stack(&custom_timer);
	custom_timer.expires = jiffies + delay * HZ;
	custom_timer.data = 0;
	custom_timer.function = print_text;
	add_timer(&custom_timer);
	return 0;
}

static void customtimer_exit(void)
{	
	kobject_put(customtimer_kobject);
	del_timer(&custom_timer);
}

module_init(customtimer_init);
module_exit(customtimer_exit);
