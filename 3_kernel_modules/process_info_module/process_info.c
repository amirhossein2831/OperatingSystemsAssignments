#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>

int my_module_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;

    pr_info("Loading process info...");

    for_each_process(task) {
        mm = task->mm;
        pr_info("Process: PID=%d | Name=%s\n", task->pid, task->comm);
        if (mm) {
            pr_info("Memory Usage (RSS): %lu KB\n", get_mm_rss(mm) * PAGE_SIZE / 1024);
        }

        pr_info("  CPU Time: User Mode=%llu | Kernel Mode=%llu\n",task->utime, task->stime);
    }

    pr_info("All Data of process shared.\n");
    return 0;
}

void my_module_exit(void) {
    pr_info("Process Info Module Exiting\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Module to Display Process Information");
MODULE_AUTHOR("OS-LAB-Group");

