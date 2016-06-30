#include "physical.h"


struct MUTEX mm_kmallocLock;

int mm_init( multiboot_info_t * m )
{	
	// setup the physical memory manager, after this we can use physical_pageAlloc()
	physical_init( m );	
	
	// init the lock that the kernel mm_kmalloc/mm_kfree will use
	// we cant use mutex_create() as it need mm_kmalloc()
	mutex_init( &mm_kmallocLock );
	
	// from here on in we can use mm_kmalloc() & mm_kfree()
	return SUCCESS;
}
