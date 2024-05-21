#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/irq.h>
#define PS2_KEY_PRESSED_MASK 0x80
#define PS2_DATA_PORT 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Kalmykov");
MODULE_DESCRIPTION("Counts PS/2 keyboard keys per minute");
MODULE_VERSION("1.0");

static const unsigned int PS2_IRQ = 1;
static const unsigned long DURATION = 60000;

static atomic64_t counter;
static struct timer_list timer;

void timer_callback(struct timer_list* timer) {
    pr_info("%lld symbols per minute\n", atomic64_xchg(&counter, 0));
    mod_timer(timer, jiffies + msecs_to_jiffies(DURATION));
}

irqreturn_t interrupt_callback(int irq, void *dev_id) {
    if ((inb(PS2_DATA_PORT) & PS2_KEY_PRESSED_MASK) == 0) atomic64_add(1, &counter);
    return IRQ_HANDLED;
}

static int __init key_counter_init(void)
{
    pr_info("Starting KeyCounter\n");

    atomic64_set(&counter, 0);
    timer_setup(&timer, (void*)timer_callback, 0);
    mod_timer(&timer, jiffies + msecs_to_jiffies(DURATION));

    int err = request_irq(PS2_IRQ,
                          (void*)interrupt_callback,
                          IRQF_SHARED,
                          "KeyCounter",
                          (void*)interrupt_callback);
    if (err) return 1;

    pr_info("KeyCounter is on\n");
    return 0;
}
static void __exit key_counter_exit(void)
{
    free_irq(PS2_IRQ, (void*)interrupt_callback);
    timer_shutdown_sync(&timer);

    pr_info("KeyCounter is off\n");
}

module_init(key_counter_init);
module_exit(key_counter_exit);
