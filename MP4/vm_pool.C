/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "page_table.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) 
{
    //assert(false);
    //assign base address
    ba = _base_address;
    //assign size
    s = _size;
    //assign frame pool
    fp = _frame_pool;
    //assign pagetable
    pt = _page_table;
    //assign vmpointer
    vp = NULL;
    //assign counter
    c = 0;
    //register pool pt
    pt -> register_pool(this);
    //store base address using the first data entry
    vm_region *pr = (vm_region*)ba;
    //assign base address to the pointer
    pr[0].ba = ba;
    //assign pagetable's size to length
    pr[0].len = PageTable::PAGE_SIZE;
    
    reg = pr;
    //assign rems
    rems -= PageTable::PAGE_SIZE;
    //increment the counter
    c+=1;
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) 
{
    //assert(false);
    if(_size > rems)
    {
        Console::puts("Not enough space.\n");
        assert(false);
    }
    //assign np
    unsigned long np = (_size/PageTable::PAGE_SIZE) + ((_size%PageTable::PAGE_SIZE)>0?1:0);
    //assign base address to reg
    reg[c].ba = reg[c-1].ba + reg[c-1].len;
    //assign length to area
    reg[c].len = np*PageTable::PAGE_SIZE;
    //increment the counter
    c += 1;
    //evaluate rems
    rems -= np*PageTable::PAGE_SIZE;
    Console::puts("Allocated region of memory.\n");
    return reg[c-1].ba;
}

void VMPool::release(unsigned long _start_address) 
{
    //initialize r,x
    int r = -1, x;
    //run the for loop
    for(x = 1; x < c; x++)
    {
    	if(reg[x].ba == _start_address)
    		r = x;
    }
    //initialize np
    unsigned long np = reg[region].len / PageTable::PAGE_SIZE;
    while(np > 0)
    {
       //update pt
    	pt -> free_page(_start_address);
    	//decrement np
    	np--;
    	//update start address
    	_start_address += PageTable::PAGE_SIZE;
    }
    for(x=r;x<c;x++)
    	reg[x] = reg[x+1];
    //update counter
    c -= 1;
    //update rems
    rems += reg[region].len;
    //assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) 
{
    //assert(false);
    if((_address > (ba + s)) || (_address < ba))
    	return false;
    return true;
    Console::puts("Checked whether address is part of an allocated region.\n");
}

