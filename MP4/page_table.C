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
    //assert(false);
    Console::puts("Initialized Paging System\n");
    //assign kernelmempool
    kernel_mem_pool = _kernel_mem_pool;
    //assign processmempool
    process_mem_pool = _process_mem_pool;
    //assign sharedsize
    shared_size = _shared_size; 
}

PageTable::PageTable()
{
    //assert(false);
    Console::puts("Constructed Page Table object\n");
    //assign page directory
    pagedir = (unsigned long *)(kernel_mem_pool -> get_frames(1) * PAGE_SIZE);
    //update page directory
    pagedir[1023] = (unsigned long)(pagedir)|3;
    //initialize pt
    unsigned long *pt;
    pt = (unsigned long *)(process_mem_pool->get_frames(1)*PAGE_SIZE);
    //initialize ad
    unsigned long ad;
    ad = 0;
    //initialize for loop
    unsigned int x;
    for(x=0;x<1024;x++)
    {
    	pt[x] = ad|3;
    	ad += PAGE_SIZE;
    }
    //update page directory entries
    pagedir[0] = (unsigned long)pt;
    //update page directory entries
    pagedir[0] = pagedir[0]|3;
    //update entries
    for(x=1;x<1023;x++)
    {
    	pagedir[x]=0|2;
    }
    //set paging_enabled to 0
    paging_enabled = 0;
}


void PageTable::load()
{
    //assert(false);
    Console::puts("Loaded page table\n");
    //update current page table
    current_page_table = this;
    Console::puts((unsigned long)(current_page_table -> pagedir[1]));
    //use write_cr3 to write in register 3
    write_cr3((unsigned long)(current_page_table -> pagedir));
}

void PageTable::enable_paging()
{
    //assert(false);
    write_cr0(read_cr0() | 0x80000000);
    paging_enabled = 1;
    Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
    //assert(false);
    unsigned long ad = read_cr2();
    //print ad
    Console::puts(ad);
    //initialize adp
    unsigned int adp = 0;
    //initialize pointer a
    VMPool *a = PageTable::VMPoolList_HEAD;
    //run for loop
    for(;a != NULL; a = a->vp)
    {
    	{
    		//check if the address is legitimate
    		if(a->is_legitimate(ad) ==true)
    		{
    			//update adp
    			adp = 1; break;
    		}
    	}
    }
    //check conditions
    if(adp == 0 && a != NULL)
    {
    	assert(false);
    	Console::puts("Not Valid!");
    }
    //initialize ppdir
    unsigned long ppdir = current_page_table -> pagedir
    //initialize odir
    unsigned long odir = ad>>22;
    //initialize odti
    unsigned long odti = (ad &(0x03FF << 12))>>12;
    //initialize ptt    
    unsigned long *ptt;
    //initialize pte 
    unsigned long *pte;
    if((ppdir[odir] & 1) == 0)
    {
    	//assign pt
    	pt = (unsigned long *)(process_mem_pool -> get_frames(1) * PAGE_SIZE);
    	//assign de
    	unsigned long *de = (unsigned long *)(0xFFFFF << 12);
    	//update de
    	de[odir] = (unsigned long)(page_table)|3;
    }
    //assign pte
    pte = (unsigned long *)(process_mem_pool -> get_frames(1)*PAGE_SIZE);
    //initialize pen    
    unsigned long *pen = (unsigned long *)((0x3FF << 22)|(odir << 12));
    //assign pen
    pen[odti] = (unsigned long)(pte)|3;
    Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
//    assert(false);
    if(PageTabble::VMPoolList_HEAD == NULL)
    {
    	//initialize v
    	PageTable::VMPoolList_HEAD = v;
    }
    else
    {
    	//initialize t
    	VMPool *t = PageTable::VMPoolList_HEAD;
    	//update v
    	for(; t->vp != NULL; t = t->vp)
    	t->vp = v;
    }
    Console::puts("registered VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) 
{
    //assert(false);
    //initialize odir
    unsigned long odir = (_page_no & 0xFFC) >> 22;
    //initialize odti
    unsigned long odti = (_page_no & 0x003FF) >> 12;
    //initialize pte
    unsigned long pte = (unsigned long *)((0x000003FF << 22) | (odir << 12));
    //initialize fr
    unsigned long fr = (pte[odti] & 0xFFFFF000)/PAGE_SIZE;
    //free frames from the process memory pool
    process_mem_pool -> release_frames(fr);
    //update pte
    pte[odti] |= 2;
    Console::puts("freed page\n");
    load();
}
