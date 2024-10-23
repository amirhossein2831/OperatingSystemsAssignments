#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

// Parameters that will be passed during module insertion
static char *my_string = "default_string";
static long my_long = 100;
static int my_int = 42;
static short my_short = 12;

module_param(my_string, charp, 0000);
module_param(my_long, long, 0000);
module_param(my_int, int, 0000);
module_param(my_short, short, 0000);

// Descriptions for the parameters
MODULE_PARM_DESC(my_string, "A string parameter");
MODULE_PARM_DESC(my_long, "A long integer parameter");
MODULE_PARM_DESC(my_int, "An integer parameter");
MODULE_PARM_DESC(my_short, "A short integer parameter");

int my_module_init(void) {
    pr_info("printing params....");
    pr_info("Module parameters: string=%s, long=%ld, int=%d, short=%d\n", my_string, my_long, my_int, my_short);

    return 0;
}


void my_module_exit(void) {
    pr_info("exiting from your module...");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday List Module with Parameters");
MODULE_AUTHOR("OS-LAB-Group");

