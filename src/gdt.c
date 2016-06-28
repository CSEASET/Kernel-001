#define GDT_SIZE 3
#define GDT_MEM_LOW 0
#define GDT_MEM_LEN 0xFFFFFFFF

#define GDT_EXE 0x8
#define GDT_READ 0x2
#define GDT_WRITE 0x2

/* Kernel code always runs in ring 0 */
#define DPL_KERNEL 0

/* GDT entry numbers */
enum {
	_GDT_NULL,
	_KERNEL_CS,
	_KERNEL_DS
};

/* GDT entry offsets */
#define GDT_NULL (_GDT_NULL << 3)
#define KERNEL_CS (_KERNEL_CS << 3)
#define KERNEL_DS (_KERNEL_DS << 3)

#define ENTER_KEY_CODE 0x1C



struct GDT_entry {
	/* Low 8 bits of the "limit", or length of memory this descriptor refers to. */
	unsigned short limit_low;
	unsigned short base_low; /* 'Low' 16-bits of the base */
	unsigned char base_middle; /* 'middle' 8 bits of the base */

	unsigned char type :4; /* Flags for type of memory this descriptor describes */
	unsigned char one :1;
	unsigned char dpl :2; /* Descriptor privilege level - Ring level */
	unsigned char present :1; /* 1 for any valid GDT entry */

	unsigned char limit :4; /* Top 4 bytes of 'limit' */
	unsigned char avilable :1;
	unsigned char zero :1;
	unsigned char op_size :1; /* Selects between 16-bit and 32-bit */
	unsigned char gran :1; /* If this bit is set, then 'limit' is a count of 4K blocks, not bytes */

	unsigned char base_high; /* High 8 bits of the base */
} __attribute__((packed));

struct gdt_ptr {
	unsigned short limit;
	unsigned long base;
} __attribute__((packed));

extern void load_gdt(struct gdt_ptr *gdt_ptr);

#define GDT_ENTRY(gdt_type, gdt_base, gdt_limit, gdt_dpl) \
	{                                                     \
		.limit_low   = (((gdt_limit) >> 12) & 0xFFFF),    \
		.base_low    = ((gdt_base) & 0xFFFF),             \
		.base_middle = (((gdt_base) >> 16) & 0xFF),       \
		.type = gdt_type,                                 \
		.one = 1,                                         \
		.dpl = gdt_dpl,                                   \
		.present = 1,                                     \
		.limit = ((gdt_limit) >> 28),                     \
		.avilable = 0,                                    \
		.zero = 0,                                        \
		.op_size = 1,                                     \
		.gran = 1,                                        \
		.base_high = (((gdt_base) >> 24) & 0xFF),         \
	}

/* Define our GDT that we'll use - We know everything upfront, so we just
 * initalize it with the correct settings.
 *
 * This sets up the NULL, entry, and then the kernel's CS and DS segments,
 * which just span all 4GB of memory. */
struct GDT_entry GDT[GDT_SIZE] = {
	[_GDT_NULL] = { 0 /* NULL GDT entry - Required */ },
	[_KERNEL_CS] = GDT_ENTRY(GDT_EXE | GDT_READ, 0, 0xFFFFFFFF, DPL_KERNEL),
	[_KERNEL_DS] = GDT_ENTRY(GDT_WRITE,          0, 0xFFFFFFFF, DPL_KERNEL)
};

void gdt_init(void)
{
	struct gdt_ptr gdt_ptr;

	gdt_ptr.base = (unsigned long)GDT;
	gdt_ptr.limit = sizeof(GDT);
	load_gdt(&gdt_ptr);
}
