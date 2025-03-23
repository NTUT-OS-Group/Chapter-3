#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define PROC_NAME "task_info"
#define BUFFER_SIZE 10240

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A Kernel Module for displaying task information");

// 用於保存輸出的緩衝區
static char *message;
static struct task_struct *task;

// 當用戶讀取 /proc/task_info 時，執行此函數
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos) {
    int len;
    if (*pos > 0 || !message) {
        return 0; // 已讀取結束
    }

    len = strlen(message);
    if (copy_to_user(user_buf, message, len)) {
        return -EFAULT;
    }

    *pos = len;
    return len;
}

// 文件操作結構
static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
};

// 初始化模組
static int __init task_info_init(void) {
    struct task_struct *task_iter;
    int offset = 0;

    // 創建 /proc/task_info 檔案
    proc_create(PROC_NAME, 0, NULL, &proc_file_ops);

    // 分配緩衝區
    message = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!message) {
        return -ENOMEM;
    }

    // 獲取當前進程資訊
    task = current; // 獲取當前進程
    offset += snprintf(message + offset, BUFFER_SIZE - offset, "Current Task:\n");
    offset += snprintf(message + offset, BUFFER_SIZE - offset, "PID: %d\n", task->pid);
    offset += snprintf(message + offset, BUFFER_SIZE - offset, "Comm: %s\n", task->comm);
    offset += snprintf(message + offset, BUFFER_SIZE - offset, "State: %ld\n", task->stats);

    // 遍歷進程
    offset += snprintf(message + offset, BUFFER_SIZE - offset, "\nAll Tasks:\n");
    for_each_process(task_iter) {
        offset += snprintf(message + offset, BUFFER_SIZE - offset, "PID: %d Comm: %s\n", task_iter->pid, task_iter->comm);
        if (offset >= BUFFER_SIZE - 64) {
            break; // 防止超出緩衝區
        }
    }

    printk(KERN_INFO "task_info: Module loaded.\n");
    return 0;
}

// 清理模組
static void __exit task_info_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    kfree(message);
    printk(KERN_INFO "task_info: Module removed.\n");
}

module_init(task_info_init);
module_exit(task_info_exit);
