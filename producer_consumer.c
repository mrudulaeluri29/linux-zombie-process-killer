#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mrudula Eluri");

static int prod = 1;
static int cons = 0;
static int size = 0;
static int uid = 0;

module_param(prod, int, 0);
module_param(cons, int, 0);
module_param(size, int, 0);
module_param(uid, int, 0);

static struct semaphore empty;
static struct semaphore full;
static struct task_struct **buffer;
static int in = 0;
static int out = 0;

static struct task_struct *producer_thread;
static struct task_struct **consumer_threads;
static DEFINE_MUTEX(buffer_lock);  // Added mutex

static int producer_fn(void *data) {
    while (!kthread_should_stop()) {
        struct task_struct *p;
        for_each_process(p) {
            if (p->cred->uid.val != uid)
                continue;
            if (p->exit_state & EXIT_ZOMBIE) {
                if (down_interruptible(&empty))
                    continue;

                mutex_lock(&buffer_lock);  // Lock before adding to buffer
                buffer[in] = p;
                in = (in + 1) % size;
                mutex_unlock(&buffer_lock);  // Unlock after adding

                printk(KERN_INFO "[Producer-1] has produced a zombie process with pid %d and parent pid %d\n", p->pid, p->parent->pid);
                up(&full);
            }
        }
        msleep(250);
    }
    return 0;
}

static int consumer_fn(void *data) {
    int id = *(int *)data;
    while (!kthread_should_stop()) {
        if (down_interruptible(&full))
            continue;

        struct task_struct *zombie;

        mutex_lock(&buffer_lock);  // Lock before accessing buffer
        zombie = buffer[out];
        out = (out + 1) % size;
        mutex_unlock(&buffer_lock);  // Unlock after access

        if (zombie->exit_state & EXIT_ZOMBIE) {  // Double-check it's still a zombie
            kill_pid(zombie->parent->thread_pid, SIGKILL, 0);
            printk(KERN_INFO "[Consumer-%d] has consumed a zombie process with pid %d and parent pid %d\n", id, zombie->pid, zombie->parent->pid);
        }

        up(&empty);
    }
    kfree(data);
    return 0;
}

static int __init zombie_killer_init(void) {
    int i;
    sema_init(&empty, size);
    sema_init(&full, 0);
    buffer = kmalloc_array(size, sizeof(struct task_struct *), GFP_KERNEL);
    consumer_threads = kmalloc_array(cons, sizeof(struct task_struct *), GFP_KERNEL);

    producer_thread = kthread_run(producer_fn, NULL, "Producer-1");

    for (i = 0; i < cons; i++) {
        int *id = kmalloc(sizeof(int), GFP_KERNEL);
        *id = i + 1;
        consumer_threads[i] = kthread_run(consumer_fn, id, "Consumer-%d", *id);
    }
    return 0;
}

static void __exit zombie_killer_exit(void) {
    int i;
    kthread_stop(producer_thread);
    for (i = 0; i < cons; i++)
        kthread_stop(consumer_threads[i]);

    kfree(buffer);
    kfree(consumer_threads);
}

module_init(zombie_killer_init);
module_exit(zombie_killer_exit);
