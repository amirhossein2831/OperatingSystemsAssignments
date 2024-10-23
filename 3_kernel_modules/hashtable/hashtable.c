#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/hashtable.h>
#include <linux/string.h>

struct person {
    int id;
    char firstname[30];
    struct hlist_node node;
};

static DEFINE_HASHTABLE(person_table, 5);

static int __init my_module_init(void) {
    struct person *new_person;

    new_person = kmalloc(sizeof(struct person), GFP_KERNEL);
    if (!new_person) {
        printk(KERN_ERR "Memory allocation failed for person\n");
        return -ENOMEM;
    }

    new_person->id = 1;
    strncpy(new_person->firstname, "AmirHossein Motaghian", sizeof(new_person->firstname) - 1);
    new_person->firstname[sizeof(new_person->firstname) - 1] = '\0';

    hash_add(person_table, &new_person->node, new_person->id);

    struct person *entry;
    hash_for_each_possible(person_table, entry, node, new_person->id) {
        if (entry->id == new_person->id) {
            printk(KERN_INFO "Person ID: %d, First Name: %s\n", entry->id, entry->firstname);
        }
    }

    return 0;
}

static void __exit my_module_exit(void) {
    struct person *entry;
    struct hlist_node *tmp;
    int bkt;

    hash_for_each_safe(person_table, bkt, tmp, entry, node) {
        hash_del(&entry->node);
        kfree(entry);
    }

    printk(KERN_INFO "Module unloaded and memory freed\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");

