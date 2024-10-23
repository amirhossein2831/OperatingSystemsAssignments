#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>

struct birthday {
    int day;
    int month;
    int year;
    struct list_head list;
};

LIST_HEAD(my_list);

int my_module_init(void) {
    struct birthday *b1;

    b1 = kmalloc(sizeof(struct birthday), GFP_KERNEL);
    if (!b1) {
        return -ENOMEM;
    }

    b1->day = 2;
    b1->month = 8;
    b1->year = 1995;

    INIT_LIST_HEAD(&b1->list);
    list_add_tail(&b1->list, &my_list);
    pr_info("Birthday added: %d/%d/%d\n", b1->day, b1->month, b1->year);
    return 0;
}

void  my_module_exit(void) {
    struct birthday *b1;
    struct list_head *pos, *q;

    // Iterate through the list and free allocated memory
    list_for_each_safe(pos, q, &my_list) {
        b1 = list_entry(pos, struct birthday, list);
        pr_info("Removing birthday: %d/%d/%d\n", b1->day, b1->month, b1->year);
        list_del(pos);
        kfree(b1);
    }
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday List Module");
MODULE_AUTHOR("OS-LAB-Group");

