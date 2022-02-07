#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#include <linux/version.h>
#include <generated/utsrelease.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/smp.h>
#include <linux/timex.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/quicklist.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <linux/spinlock.h>
#include <linux/linkage.h>
#include <linux/mmzone.h>
#include <linux/list.h>
#include <linux/memcontrol.h>
#include <linux/sched.h>
#include <linux/node.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/page-flags.h>
#include <asm/page.h>
#include <asm/processor.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "my_info"
#define si_swapinfo(val) \
	do { (val)->freeswap = (val)->totalswap = 0; } while (0)
extern unsigned long total_swapcache_pages(void);

/* This function is called at the beginning of a sequence.
 * ie, when:
 *   - the /proc file is read (first time)
 *   - after the function stop (end of sequence)
 */
static void *my_seq_start(struct seq_file *m, loff_t *pos)
{
    *pos = cpumask_next(*pos - 1, cpu_online_mask);
    if ((*pos) < nr_cpu_ids)
        return &cpu_data(*pos);
    return NULL;
}


/* This function is called after the beginning of a sequence.
 * It is called untill the return is WARNING: "total_swapcache_pages" [/home/os2021/os/hw1-matt89919-master-20211108T050255Z-001/hw1-matt89919-master/module/my_info.ko] undefined!
WARNING: "si_swapinfo" [/home/os2021/os/hw1-matt89919-master-20211108T050255Z-001/hw1-matt89919-master/module/my_info.ko] undefined!
NULL (this ends the sequence).
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    unsigned long *tmp_v = (unsigned long *)v;
    (*tmp_v)++;
    (*pos)++;
    return NULL;
}

/* This function is called at the end of a sequence. */
static void my_seq_stop(struct seq_file *s, void *v)
{
    /* nothing to do, we use a static value in start() */
}

static void show_val_kb(struct seq_file *m, const char *s, unsigned long num)
{
    char v[32];
//	static const char blanks[7] = {' ', ' ', ' ', ' ',' ', ' ', ' '};
    int len;

    len = snprintf(v, sizeof(v),"%lu", num << (PAGE_SHIFT - 10));

    seq_printf(m, s);

    if (len > 0)
    {
        seq_write(m, v, len);
    }
    seq_printf(m, " kB\n");
}

/* This function is called for each "step" of a sequence. */
static int my_seq_show(struct seq_file *s, void *v)
{
    struct cpuinfo_x86 *c = v;
    unsigned int cpu;

    //use by time
    struct timespec uptime;
    struct timespec idle;
    u64 nsec;
    u32 rem;
    int j;
    //use by cpu
//	struct cpuinfo_x86* c= v;
//	unsigned int cpu;
    //use by memory
    struct sysinfo i;
    unsigned long committed;
//	long cached;
    long available;
    unsigned long pages[NR_LRU_LISTS];
    int lru;



    seq_puts(s, "\n=============Version=============\n");
    seq_printf(s, "Linux Version %s\n", UTS_RELEASE);

    seq_puts(s, "\n==============CPU================\n");
//    struct cpuinfo_x86 *c = v;
//    unsigned int cpu;
    cpu = c->cpu_index;
    /*
        seq_printf(s,"processor:\t\t %u\n"
    	"model name:\t\t %s\n"
    	"physical id:\t\t %d\n"
    	"core id:\t\t %d\n"
    	"cache size:\t\t %d KB\n"
    	"clflush size:\t\t %u\n"
    	"cache_alignment:\t %d\n"
    	"address sizes:\t\t %u bits physical, %u bits virtual\n"
    	,cpu,c->x86_model_id[0] ? c->x86_model_id : "unknown",
    	 c->phys_proc_id, c->cpu_core_id, c->x86_cache_size,
    	 c->x86_clflush_size,
    	 c->x86_cache_alignment,
    	 c->x86_phys_bits,
    	 c->x86_virt_bits);
      */
    seq_printf(s, "processor\t\t: %u\n"
               "model name\t\t: %s\n"
               "physical id\t\t: %d\n"
               "core id\t\t\t: %d\n"
               "cpu cores\t\t: %d\n"
               "cache size\t\t: %u KB\n"
               "clflush size\t\t: %u\n"
               "cache_alignment\t\t: %d\n"
               "address sizes\t\t: %u bits physical, %u bits virtual\n\n"
               , c->cpu_index, c->x86_model_id[0] ? c->x86_model_id : "unknown", c->phys_proc_id
               , c->cpu_core_id, c->booted_cores, c->x86_cache_size, c->x86_clflush_size
               , c->x86_cache_alignment, c->x86_phys_bits, c->x86_virt_bits);

    //seq_printf(s, "proccessor: \n");

    seq_puts(s, "\n=============Memory==============\n");
    si_meminfo(&i);
    si_swapinfo(&i);
    committed = percpu_counter_read_positive(&vm_committed_as);

    //cached = global_node_page_state(NR_FILE_PAGES) -
    //		total_swapcache_pages() - i.bufferram;
    //if (cached < 0)
    //	cached = 0;

    for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
        pages[lru] = global_node_page_state(NR_LRU_BASE + lru);



    available = si_mem_available();
    show_val_kb(s, "MemTotal\t\t:", i.totalram);
    show_val_kb(s, "MemFree\t\t\t:", i.freeram);
    show_val_kb(s, "Buffers\t\t\t:", i.bufferram);
    show_val_kb(s, "Active\t\t\t:", pages[LRU_ACTIVE_ANON] +
                pages[LRU_ACTIVE_FILE]);
    show_val_kb(s, "Inactive\t\t:", pages[LRU_INACTIVE_ANON] +
                pages[LRU_INACTIVE_FILE]);
    show_val_kb(s, "Shmem\t\t\t:", i.sharedram);
    show_val_kb(s, "Dirty\t\t\t:",
                global_node_page_state(NR_FILE_DIRTY));
    show_val_kb(s, "Writeback\t\t:",
                global_node_page_state(NR_WRITEBACK));
    seq_printf(s, "KernelStack\t\t:%lu kB\n",
               global_zone_page_state(NR_KERNEL_STACK_KB));
    show_val_kb(s, "PageTables\t\t:",
                global_zone_page_state(NR_PAGETABLE));


    seq_puts(s, "\n==============Time===============\n");
    nsec = 0;
    for_each_possible_cpu(j)
    nsec += (__force u64) kcpustat_cpu(j).cpustat[CPUTIME_IDLE];

    get_monotonic_boottime(&uptime);
    idle.tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
    idle.tv_nsec = rem;
    seq_printf(s, "Uptime\t\t        :%lu.%02lu (s)\n",
               (unsigned long) uptime.tv_sec,
               (uptime.tv_nsec / (NSEC_PER_SEC / 100))
              );

    seq_printf(s, "Idletime\t        :%lu.%02lu (s)\n",
               (unsigned long) idle.tv_sec,
               (idle.tv_nsec / (NSEC_PER_SEC / 100))
              );

    return 0;
}

/* This structure gather "function" to manage the sequence */
static struct seq_operations my_seq_ops =
{
    .start = my_seq_start,
    .next = my_seq_next,
    .stop = my_seq_stop,
    .show = my_seq_show,
};

/* This function is called when the /proc file is open. */
static int my_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &my_seq_ops);
};

/* This structure gather "function" that manage the /proc file */
#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops =
{
    .proc_open = my_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = seq_release,
};
#else
static const struct file_operations my_file_ops =
{
    .open = my_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};
#endif

static int __init procfs4_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
    if (entry == NULL)
    {
        remove_proc_entry(PROC_NAME, NULL);
        pr_debug("Error: Could not initialize /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit procfs4_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_debug("/proc/%s removed\n", PROC_NAME);
}

module_init(procfs4_init);
module_exit(procfs4_exit);

MODULE_LICENSE("GPL");
