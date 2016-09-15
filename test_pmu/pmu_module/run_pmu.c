#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");

///MSR_REGISTERS
#define IA32_PMC0 0xC1
#define IA32_PERFEVTSEL0 0x186
#define IA32_PERF_GLOBAL_STATUS 0x38E
#define IA32_PERF_GLOBAL_CTRL 0x38F
#define IA32_PERF_GLOBAL_OVF_CTRL 0x390
#define IA32_PEBS 0x3F1
#define IA32_FIXED_CTR_CTRL 0x38D
#define IA32_FIXED_CTR 0x309
#define IA32_PEBS_ENABLE 0x3f1
#define MSR_PEBS_LD_LAT 0x3f6
#define IA32_DS_AREA 0x600
#define IA32_DEBUG_CTRL 0x1D9



//MSR_PERFEVTSEL_SHIFT
#define CMASK 24
#define Invert_counter_mask 23
#define Enable_counters 22
#define Any_Thread 21
#define APIC_interrupt_enable 20
#define Pin_control 19
#define Edge_detect 18
#define OS 17
#define USR 16

#define PMC_N 4
#define PMC_N_MASK ((1 << PMC_N) - 1)
#define FIXED_N 3
#define FIXED_N_MASK ((((1UL) << FIXED_N) - 1) << 32)
#define FIXED_CONTROL (7)

struct event {
	char *name;
	u32 event, umask, tmask;
};

struct event events[][PMC_N] =
{
{
	//{"DTLB_LOAD_MISS_COMPLETE", 0x8, 0xe, 0},
	//{"DTLB_LOAD_MISS_DURATION", 0x8, 0x10, 0},
	{"DTLB_LOAD_MISS_COMPLETE", 0x8, 0x2, 0},
//     	{"DTLB_LOAD_MISS_DURATION", 0x8, 0x4, 0},
//     	{"L1_HIT", 0xd1, 0x1, 0},
//     	{"L3.MISSES", 0x2e, 0x41, 0},
	{"MEM_UOP_RETIRED_ALL1", 0xd0, 0x81, 0},
	{"DTLB_STORE_MISS_COMPLETE", 0x49, 0x2, 0},
//     	{"DTLB_STORE_MISS_DURATION", 0x49, 0x4, 0},
	{"MEM_UOP_RETIRED_ALL2", 0xd0, 0x82, 0},
},
{
	{"L1_HIT", 0xd1, 0x1, 0},
	{"L2_HIT", 0xd1, 0x2, 0},
	{"LLC_HIT", 0xd1, 0x4, 0},
	{"LLC_MISS", 0xd1, 0x20, 0},
},
{
	{"L3.REFERENCE", 0x2e, 0x4f, 0},
	{"L3.MISSES", 0x2e, 0x41, 0},
	{"L1D.REPLACEMENT", 0x51, 0x1, 0},
	{"L2_LINES_IN.ALL", 0xf1, 0x7, 0},
},
{
	{"MEM_LATENCY", 0xcd, 0x1, 0},
	{"L2_PENDING", 0xa3, 0x5, 0x5000000},
	{"L1D_PENDING", 0xa3, 0xc, 0x6000000},
	{"NO_DISPATCH", 0xa3, 0x4, 0x4000000},
},
{
	{"DTLB_LOAD_MISS_COMPLETE", 0x8, 0xe, 0},
	{"DTLB_STORE_MISS_COMPLETE", 0x49, 0xe, 0},
	{"TLB_FLUSH.DTLB_THREAD", 0xbd, 0x1, 0},
	{"TLB_FLUSH.STLB_ANY", 0xbd, 0x20, 0},
},
{
	{"DTLB_LOAD_MISS_DURATION", 0x8, 0x10, 0},
	{"DTLB_STORE_MISS_DURATION", 0x49, 0x10, 0},
	{"L1D_PENDING", 0xa3, 0xc, 0x6000000},
	{"L2_PENDING", 0xa3, 0x5, 0x5000000},
},
};

char *fixed_name[FIXED_N] = {
	"INST_RETIRED.ANY",
	"CPU_CLK_UNHALTED.CORE",
	"CPU_CLK_UNHALTED.REF",
};


int events_selector;

static inline void trans64to32(u64 src, u32 *dsth, u32 *dstl) {
	*dsth = (u32)(src >> 32);
	*dstl = (u32)(src);
}

static inline void trans32to64(u32 srch, u32 srcl, u64 *dst) {
	*dst = (((u64)srch) << 32) + srcl;
}

typedef struct {
	u32 msr;
	u64 val;
} msr_on_cpu;

void __rdmsr_on_cpu(void *info) {
	msr_on_cpu *msr = (msr_on_cpu *)info;
	u32 valh, vall;
	asm volatile(
		"rdmsr" : "=a"(vall), "=d"(valh) : "c"(msr->msr) :
	);
	trans32to64(valh, vall, &msr->val);
}

u64 myrdmsr_on_cpu(int cpu, u32 msrReg) {
	msr_on_cpu msr;
	cpumask_t c;

	cpumask_clear(&c);
	cpumask_set_cpu(cpu, &c);
	msr.msr = msrReg;

	//on_selected_cpus(&c, __rdmsr_on_cpu, (void *)(&msr), -1);
	smp_call_function_single(cpu, __rdmsr_on_cpu, (void *)(&msr), 1);
	return msr.val;
}

void __wrmsr_on_cpu(void *info) {
	msr_on_cpu *msr = (msr_on_cpu *)info;
	u32 valh, vall;
	trans64to32(msr->val, &valh, &vall);
	asm volatile(
		"wrmsr" : : "a"(vall), "d"(valh), "c"(msr->msr) :
	);
}

void mywrmsr_on_cpu(unsigned long cpu, u32 msrReg, u64 val) {
	msr_on_cpu msr;
	cpumask_t c;

	cpumask_clear(&c);
	cpumask_set_cpu(cpu, &c);
	msr.msr = msrReg;
	msr.val = val;

	//on_selected_cpus(&c, __wrmsr_on_cpu, (void *)(&msr), -1);
	smp_call_function_single(cpu, __wrmsr_on_cpu, (void *)(&msr), 1);
}

u64 base = 0;

void init_evtsel(int cpu, u32 counter, u32 event, u32 umask, u32 tmask) {
	u32 evtsel, pmc;
	volatile u64 val;
	evtsel = IA32_PERFEVTSEL0 + counter;
	pmc = IA32_PMC0 + counter;
	val = 0;
	val |= (u64) (event | (umask << 8));
	//val |= (1L << USR) | (1 << OS);
	val |= (1L << USR);
	val ^= tmask;
	mywrmsr_on_cpu(cpu, evtsel, val);
	mywrmsr_on_cpu(cpu, pmc, base);
}


static void start_pmu(int cpu, int arg2) {
	u32 i;
	volatile u64 val;

	// global control
	val = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_CTRL);
	val |= PMC_N_MASK | FIXED_N_MASK;
	mywrmsr_on_cpu(cpu, IA32_PERF_GLOBAL_CTRL, val);
	val = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_OVF_CTRL);
	val |= PMC_N_MASK | FIXED_N_MASK;
	mywrmsr_on_cpu(cpu, IA32_PERF_GLOBAL_OVF_CTRL, val);

	// pmc
	events_selector = arg2;
	for (i = 0; i < PMC_N; i++)
		init_evtsel(cpu, i, events[arg2][i].event, events[arg2][i].umask, events[arg2][i].tmask);
	for (i = 0 ; i < PMC_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i);
		val |= (1L << Enable_counters);
		mywrmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i, val);
	}

	// fixed pmc
	val = 0;
	for (i = 0; i < FIXED_N; i++) {
		u32 fixed = IA32_FIXED_CTR + i;
		val = (val << 4) | FIXED_CONTROL;
		mywrmsr_on_cpu(cpu, fixed, 0);
	}
	mywrmsr_on_cpu(cpu, IA32_FIXED_CTR_CTRL, val);
}

static void stop_pmu(int cpu, int arg2) {
	u32 i;
	u64 pmc_val[PMC_N], fixed_val[FIXED_N];
	volatile u64 val, ovf;

	ovf = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_STATUS);

	for (i = 0; i < PMC_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_PMC0 + i);
		val -= base;
		if (val + base < base) val += 0xffffffffffff;
		pmc_val[i] = val;
		printk("%s val : %llu overflow : %d\n",
			events[events_selector][i].name, val, (ovf & (1 << i)) > 0);
	}
	for (i = 0; i < PMC_N; i++)
		printk("%llu ", pmc_val[i]);
	printk("\n\n");

	for (i = 0; i < FIXED_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_FIXED_CTR + i);
		fixed_val[i] = val;
		printk("%s val : %llu overflow : %llX\n",
			fixed_name[i], val, (ovf & ((1UL) << (i + 32))));
	}
	for (i = 0; i < FIXED_N; i++)
		printk("%llu ", fixed_val[i]);
	printk("\n\n");

	if (arg2) {
		for (i = 0 ; i < PMC_N; i++) {
			val = myrdmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i);
			val &= ~(1L << Enable_counters);
			mywrmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i, val);
			mywrmsr_on_cpu(cpu, IA32_PMC0 + i, 0);
		}

		val = 0;
		mywrmsr_on_cpu(cpu, IA32_FIXED_CTR_CTRL, val);
		for (i = 0; i < FIXED_N; i++)
			mywrmsr_on_cpu(cpu, IA32_FIXED_CTR + i, 0);
	}
}
extern u64 stop_pmu_return(int cpu, int arg2) {
	u32 i;
	u64 pmc_val[PMC_N], fixed_val[FIXED_N];
	volatile u64 val, ovf;
	base = 0;
	ovf = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_STATUS);

	for (i = 0; i < PMC_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_PMC0 + i);
		val -= base;
		if (val + base < base) { val += 0xffffffffffff; printk("[WARNING] val:%llx\n", val); }
		pmc_val[i] = val;
	}

	//printk("[INFO] val:%lu\n", pmc_val[0] + pmc_val[2]);

	if (arg2) {
		for (i = 0 ; i < PMC_N; i++) {
			val = myrdmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i);
			val &= ~(1L << Enable_counters);
			mywrmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i, val);
			mywrmsr_on_cpu(cpu, IA32_PMC0 + i, 0);
		}

		val = 0;
		mywrmsr_on_cpu(cpu, IA32_FIXED_CTR_CTRL, val);
		for (i = 0; i < FIXED_N; i++)
			mywrmsr_on_cpu(cpu, IA32_FIXED_CTR + i, 0);
	}
	return (pmc_val[0]+pmc_val[2]);
	//return (pmc_val[2]);
}

extern u64 pmu_l3_return(int cpu, int arg2) {
	u32 i;
	u64 pmc_val[PMC_N], fixed_val[FIXED_N];
	volatile u64 val, ovf;
	base = 0;
	ovf = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_STATUS);

	for (i = 0; i < PMC_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_PMC0 + i);
		val -= base;
		if (val + base < base) { val += 0xffffffffffff; printk("[WARNING] val:%llx\n", val); }
		pmc_val[i] = val;
	}

	//printk("[INFO] val:%lu\n", pmc_val[0] + pmc_val[2]);

	if (arg2) {
		for (i = 0 ; i < PMC_N; i++) {
			val = myrdmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i);
			val &= ~(1L << Enable_counters);
			mywrmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i, val);
			mywrmsr_on_cpu(cpu, IA32_PMC0 + i, 0);
		}

		val = 0;
		mywrmsr_on_cpu(cpu, IA32_FIXED_CTR_CTRL, val);
		for (i = 0; i < FIXED_N; i++)
			mywrmsr_on_cpu(cpu, IA32_FIXED_CTR + i, 0);
	}
	return (pmc_val[1]);
}

extern u64 pmu_mem_return(int cpu, int arg2) {
	u32 i;
	u64 pmc_val[PMC_N], fixed_val[FIXED_N];
	volatile u64 val, ovf;
	base = 0;
	ovf = myrdmsr_on_cpu(cpu, IA32_PERF_GLOBAL_STATUS);

	for (i = 0; i < PMC_N; i++) {
		val = myrdmsr_on_cpu(cpu, IA32_PMC0 + i);
		val -= base;
		if (val + base < base) { val += 0xffffffffffff; printk("[WARNING] val:%llx\n", val); }
		pmc_val[i] = val;
	}

	//printk("[INFO] val:%lu\n", pmc_val[0] + pmc_val[2]);

	if (arg2) {
		for (i = 0 ; i < PMC_N; i++) {
			val = myrdmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i);
			val &= ~(1L << Enable_counters);
			mywrmsr_on_cpu(cpu, IA32_PERFEVTSEL0 + i, val);
			mywrmsr_on_cpu(cpu, IA32_PMC0 + i, 0);
		}

		val = 0;
		mywrmsr_on_cpu(cpu, IA32_FIXED_CTR_CTRL, val);
		for (i = 0; i < FIXED_N; i++)
			mywrmsr_on_cpu(cpu, IA32_FIXED_CTR + i, 0);
	}
	return (pmc_val[1] + pmc_val[3]);
}

static int readCPUID(unsigned int arg) {
	unsigned int a = arg, b = 0, c = 0, d = 0;
	asm volatile(
		"cpuid":
		"=a"(a), "=b"(b), "=c"(c), "=d"(d):
		"a"(a), "b"(b), "c"(c), "d"(d):
	);
	printk("cpuid eax %X ebx %X ecx %X edx %X\n", a, b, c, d);
	return 0;
}

unsigned long do_xc_reserved_op(unsigned long cpu, unsigned long arg1, unsigned long arg2) {
	printk("do_xc_reserved_op, cpu = %lu, arg1 = %lu, arg2 = %lu\nFrom cpu: %X\n", cpu, arg1, arg2, smp_processor_id());
	switch (arg1) {
		case 0 : readCPUID(arg2); break;
		case 1 : start_pmu(cpu, arg2); break;
		case 2 : stop_pmu(cpu, arg2); break;
	}
	return 0;
}

static int hello_init(void)
{
	printk("hello,I am houfang\n");
	start_pmu(0, 0);
	return 0;
}

static void hello_exit(void)
{
	stop_pmu(0, 0);
	printk("goodbye,kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);
