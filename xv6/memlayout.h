// Memory layout

#define EXTMEM  0x100000            // Start of extended memory
#define PHYSTOP 0x20000000         // Top physical memory
#define DEVSPACE 0xFE000000         // Other devices are at high addresses
#define BOOTPARAM 0x50000

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000         // First kernel virtual address
//#define KERNBASE 0x0000000         // First kernel virtual address
#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#define PCI_BAR_BASE 0x80000000
#define PCI_VP_OFFSET 0x40000000
#define PCI_P2V(a) (((uint)(a)) + PCI_VP_OFFSET)
#define V2P(a) (((uint) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts
