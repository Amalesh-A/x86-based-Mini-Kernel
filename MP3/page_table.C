#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;


void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //Assign kernel_mem_pool to _kernel_mem_pool
   PageTable::kernel_mem_pool = _kernel_mem_pool;
   //Assign process_mem_pool to _process_mem_pool
   PageTable::process_mem_pool = _process_mem_pool;
   //Assign shared_size to _shared_size
   PageTable::shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   //assert(false);
   unsigned long *_page_directory = (unsigned long *) ((kernel_mem_pool->get_frames(1))*4096);
   unsigned long *page_table;
   
   unsigned long address = 0;
   unsigned int i;
   
   for(i=0; i<1024; i++)
   {
   	page_table[i] = address | 3;
   	address = address + 4096;
   }
   page_directory[0] = (unsigned long)page_table | 2 | 1;
   page_directory[0] = page_directory[0] | 3;
   
   for(i=0; i< 1024; i++)
   {
   	page_directory[i] = 0 | 2;
   }
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   //assert(false);
   current_page_table = this;
   write_cr3((unsigned long)page_directory);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   //assert(false);
   paging_enabled = 1;
   write_cr0(read_cr0() | 0x80000000);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  //assert(false);
  unsigned long * currentpagedirectory = (unsigned long *) read_cr3();
  unsigned long maskaddress = 0;
  unsigned long * pagetable = NULL;
  unsigned long pageaddress = read_cr2();
  unsigned long pdaddress = pageaddress >> 22;
  unsigned long ptaddress = pageaddress >> 12;
  unsigned long er = _r->err_code;
  
  if((er & 1)== 0)
  {
  	if((currentpagedirectory[pdaddress] & 1) == 1)
  	{
  		pagetable = (unsigned long *) (currentpagedirectory[pdaddress] & 0xFFFFF000);
  		pagetable[ptaddress & 0x3FF] = (PageTable::process_mem_pool->get_frames (1)*PAGE_SIZE) | 2 | 1;
  	}
  	else
  	{
  		currentpagedirectory[pdaddress] = (unsigned long) ((kernel_mem_pool -> get_frames (1)*PAGE_SIZE) | 2 | 1);
  		pagetable = (unsigned long *)(currentpagedirectory[pdaddress] & 0x3FF);
  		for(int j = 0; j < 1024; j++)
  		{
  			pagetable[j] = maskaddress | 4;
  		}
  		pagetable[ptaddress & 0x3FF] = (PageTable::process_mem_pool->get_frames(1)*PAGE_SIZE) | 2 | 1;
  	}
  }
  Console::puts("handled page fault\n");
}








































