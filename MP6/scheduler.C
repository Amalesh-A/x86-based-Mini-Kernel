#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "simple_timer.H"
#include "blocking_disk.H"
#include "scheduler.H"

extern BlockingDisk *SYSTEM_DISK;
Scheduler::Scheduler()
{
	hq = 0;
	tq = 0;
	tn = 0;
	Console::puts("Constructor has been scheduled");
};

void Scheduler::yield()
{
	if(Machine::interrupts_enabled())
		Machine::disable_interrupts();
		
	if((BlockingDisk::btn >	0) && (SYSTEM_DISK -> is_ready()))
        {
        unsigned long t = BlockingDisk::bh;
        BlockingDisk::bh = (BlockingDisk::bh +1) % MAXV;
        BlockingDisk::btn--;
        resume(BlockingDisk::bq[t]);
    	}
	if(tn>0)
	{
        	Thread* n = rq[hq];
        	rq[hq] = NULL;
        	hq = (hq + 1) % MAXV;
        	tn--;
        	Thread::dispatch_to(next_thread);
    	}
    	else{
	        if(tn ==0)
        	{
        	    hq = 0;
        	    tq = 0;
        	    Console::puts("Threads have been dequeued");
        	}
    	}
}

void Scheduler::resume()
{
	if(Machine::interrupts_enabled())
		Machine::disable_interrupts();
	rw[tq] = _thread;
	tq = (tq + 1) % MAXV;
	tn++;
}

void Scheduler::add()
{
	if(Machine::interrupts_enabled())
		Machine::disable_interrupts();
	rq[tq] = _thread;
	tq = (tq + 1) % MAXV;
	tn++;
}

void Scheduler::terminate()
{
	if(_thread == (Thread::CurrentThread()))
		yield();
}
unsigned long Scheduler::thn()
{
	return tn;
}
