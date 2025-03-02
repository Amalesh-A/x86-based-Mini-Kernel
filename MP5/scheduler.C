/*
 File: scheduler.C
 
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

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "machine.H"
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
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
//  assert(false);
  top_q = NULL;
  bot_q = NULL;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() 
{
  //check if interrupts are enabled
  if(Machine::interrupts_enabled())
  //disable the interrupts
  	Machine::disable_interrupts();
  	
  if(top_q != NULL)
  {
  	ready_queue *t = top_q;
  	Thread * nt = top_q->tid;
  	top_q = top_q->m;
  	if(top_q == NULL)
  		bot_q = NULL;
  	delete t;
  	Thread::dispatch_to(nt);
  }
  //check if the interrupts are enabled
  if(!Machine::interrupts_enabled())
  //enable interrupts
  	Machine::enable_interrupts();
}

void Scheduler::resume(Thread * _thread) {
  //assert(false);
  //check if interrupts are enabled
  if(Machine::interrupts_enabled())
  //disable the interrupts
  	Machine::disable_interrupts();
  if(top_q == NULL|| bot_q == NULL)
  {
  	ready_queue *t = new ready_queue;
  	t->tid = _thread;
  	top_q = bot_q = t;
  	t->m = NULL;
  }
  else{
  	ready_queue *nq = new ready_queue;
  	nq->tid = _thread;
  	bot_q->m = nq;
  	bot_q = nq;
  	nq->m = NULL;
  }
  //check if the interrupts are enabled
  if(!Machine::interrupts_enabled())
  //enable interrupts
	Machine::enable_interrupts();
}

void Scheduler::add(Thread * _thread) {
  //assert(false);
  resume(_thread);
}

void Scheduler::terminate(Thread * _thread) {
  //assert(false);
  delete _thread;
  yield();
}

RRS::RRS(unsigned e) {
//  assert(false);
  top_q = NULL;
  bot_q = NULL;
  SimpleTimer *tr = new SimpleTimer(1000/e);
  InterruptHandler::register_handler(0, tr);
  Console::puts("Constructed Scheduler.\n");
}

void RRS::yield() 
{
  //check if interrupts are enabled
  if(Machine::interrupts_enabled())
  //disable the interrupts
  	Machine::disable_interrupts();
  	
  if(top_q != NULL)
  {
  	ready_queue *t = top_q;
  	Thread * nt = top_q->tid;
  	if(top_q == NULL)
  		bot_q = NULL;
  	top_q = top_q->m
  	delete t;
  	Thread::dispatch_to(nt);
  }
  //check if the interrupts are enabled
  if(!Machine::interrupts_enabled())
  //enable interrupts
  	Machine::enable_interrupts();
}

void RRS::resume(Thread * _thread) {
  //assert(false);
  //check if interrupts are enabled
  if(Machine::interrupts_enabled())
  //disable the interrupts
  	Machine::disable_interrupts();
  ready_queue *nt = new ready_queue;
  nt->tid = _thread;
  nt->m = NULL;
  if(top_q == NULL)
  {
  	top_q = nt;
  	bot_q =nt;
  }
  else{
  	bot_q->m = nt;
  	bot_q = bot_q->m;
  }
  //check if the interrupts are enabled
  if(!Machine::interrupts_enabled())
  //enable interrupts
	Machine::enable_interrupts();
}

void RRS::add(Thread * _thread) {
  //assert(false);
  //resume the thread
  resume(_thread);
}

void RRS::terminate(Thread * _thread) {
  //assert(false);
  //termiante the thread
  delete _thread;
  yield();
}


