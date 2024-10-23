#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>

// Define the birthday structure
struct birthday {
    int day;
    int month;
    int year;
    struct list_head list;
};

// Initialize the list head
LIST_HEAD(my_list);

int my_module_init(void) {
    struct list_head *pos;
    struct birthday *b1;
    int i;

    // Create and add 5 birthday elements to the list
    for (i = 0; i < 5; i++) {
        struct birthday *new_birthday;
        new_birthday = kmalloc(sizeof(struct birthday), GFP_KERNEL);
        if (!new_birthday) {
            pr_err("Memory allocation failed\n");
            continue;
        }

        // Assign arbitrary date values (you can change them as needed)
        new_birthday->day = 2 + i;
        new_birthday->month = 8;
        new_birthday->year = 1995 + i;

        // Add each element to the list
        list_add_tail(&new_birthday->list, &my_list);
    }

    // Print the list (forward iteration)
    pr_info("Forward Iteration: \n");
    list_for_each(pos, &my_list) {
        b1 = list_entry(pos, struct birthday, list);
        pr_info("Birthday: %d/%d/%d\n", b1->day, b1->month, b1->year);
    }

    return 0;
}


void my_module_exit(void) {
    struct birthday *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &my_list, list) {
        list_del(&entry->list);
        kfree(entry);  // Free the memory for the entry
        printk(KERN_INFO "Deleted successfully\n");
    }
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday List Module");
MODULE_AUTHOR("OS-LAB-Group");

