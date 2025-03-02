/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "thread.H"
#include "scheduler.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
Thread* BlockingDisk::bq[MAXV];
unsigned long BlockingDisk::bh = 0;
unsigned long BlockingDisk::bt = 0;
unsigned long BlockingDisk::btn = 0;
extern Scheduler* SYSTEM_SCHEDULER
BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {

  }

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/
/*void BlockingDisk::wait_r()
{
	if(!is_ready())
	{
		Thread *z = Thread::CurrentThread();
		bq->enqueue(z);
		qs++;
		this->scheduler->yield();
	}
}
void BlockingDisk::popq()
{
	if(qs!=0)
	{
		Thread *z = this->bq->dequeue();
		if(scheduler->IssueDiskOperationFlag == false)
			this->scheduler->enqueueFront(t);
		else
			this->scheduler->resume(t);
		qs--;
	}
}*/
void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  //check for interrupts
  if(Machine::interrupts_enabled())
  	Machine::disable_interrupts();
  //issue read operation
  issue_operation(READ, _block_no);
  //check if ready for read operation
  if(is_ready()) //if yes
  {
  	Console::puts("Ready to read");
  	int x;
  	unsigned short temp;
  	for(x=0;x<256;x++)
  	{
  		//UPDATE temp
  		temp = Machine:importw(0x1f0);
  		//UPDATE _buf[]
  		_buf[x*2] = (unsigned char)temp;
  		_buf[x*2] = (unsigned char)(temp >> 8);
  	}
  }
  else //if no
  {
  	//UPDATE values
  	Console::puts("Not Ready to read");
  	bq[bt] = Thread::CurrentThread();
  	bt = (bt + 1) % MAXV;
  	btn++;
  }
//  SimpleDisk::read(_block_no, _buf);
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  //check for interrupts
  if(Machine::interrupts_enabled())
  	Machine::disable_interrupts();
  //issue write operation
  issue_operation(WRITE, _block_no);
  Console::puts("Not ready to write");
  bq[bt] = Thread::CurrentThread();
  bt = (bt + 1) % MAXV;
  btn++;
  SYSTEM_SCHEDULER->yield();
  //check if ready for write operation
  if(is_ready()) //if yes
  {
  	Console::puts("Ready to write");
  	int x;
  	unsigned short temp;
  	for(x=0;x<256;x++)
  	{
  		temp = _buf[x*2] | (_buf[x*2 + 1] << 8);
  		Machine::outportw(0x1F0, temp);
  	}
  }
  //SimpleDisk::write(_block_no, _buf);
}

bool BlockingDisk::is_ready(){
	//returnS SimpleDisk::is_ready();
	return SimpleDisk::is_ready();
}
