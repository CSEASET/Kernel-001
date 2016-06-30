

struct MUTEX physical_bitmapLock;

char * physical_bitmap;

int physical_bitmapSize;

void * physical_maxAddress;

extern void end;

int physical_init( multiboot_info_t * m )
{
	void * physicalAddress;
	// create the lock
	mutex_init( &physical_bitmapLock );
	// we need the multiboot info to give us the mem size so we can init the bitmap
	if( (m->flags & MULTIBOOT_FLAG_MEM) != MULTIBOOT_FLAG_MEM )
		return FAIL;
	// calculate the size of the bitmap so we have 1bit for every 4KB page in actual physical memory
	physical_bitmapSize = ( ( ( m->mem_upper / SIZE_1KB ) + 1 ) * 256 ) / 8;
	// calculate the maximum physical memory address we can use
	physical_maxAddress = (void *)( (SIZE_1KB * SIZE_1KB) * ( ( m->mem_upper / SIZE_1KB ) + 1 ) );
	// set the address of the bitmap pointer to the end location of the kernel
	physical_bitmap = (char *)&end;
	// initially we clear the bitmap so all pages are marked as free
	memset( (void *)physical_bitmap, 0x00, physical_bitmapSize );
	// if the multiboot info contains a memory map we will use it
	if( (m->flags & MULTIBOOT_FLAG_MMAP) == MULTIBOOT_FLAG_MMAP )
	{
		MULTIBOOT_MEMORY_MAP * mmap = (MULTIBOOT_MEMORY_MAP * ) m->mmap_addr;
		// mark all pages as allocated
		memset( (void *)physical_bitmap, 0xFF, physical_bitmapSize );
		// loop through all entries in the memory map
		while( (DWORD)mmap < ((DWORD)m->mmap_addr + m->mmap_length) )
		{
			// free all pages marked as available in the memory map
			if( mmap->type == MULTIBOOT_MMAP_AVAILABLE )
			{
				for( physicalAddress=(void *)mmap->base_addr_low ; physicalAddress<(void *)(mmap->base_addr_low+mmap->length_low) ; physicalAddress+=SIZE_4KB )
					physical_pageFree( physicalAddress );
			}
			// get the next entry in the memory map
			mmap = ( MULTIBOOT_MEMORY_MAP *)( (DWORD)mmap + mmap->size + sizeof(DWORD) );
		}
	}
	
	// finally, reserve all the physical memory currently being taken up by the kernel and the physical memory
	// bitmap tacked on to the end of the kernel image, this avoids us allocating this memory later on.
	// ...lets hope it works :)
	for( physicalAddress=V2P(&start) ; physicalAddress<V2P(&end)+physical_bitmapSize ; physicalAddress+=SIZE_4KB )
		physical_pageAllocAddress( physicalAddress );
	// we finished successfully
	return SUCCESS;
}
