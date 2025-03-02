#include "machine.H"
#ifndef SCHEDULER_H
#define MAXV 4
#define SCHEDULER_H
#include "thread.H"

class Scheduler{
private:
   Thread* rq[MAXV];
   unsigned long hq;
   unsigned long tq;
   unsigned long tn;
   
public:
   Scheduler();
   virtual void yield();
   virtual void resume(Thread* _thread);
   virtual unsigned long Thn();
   virtual void add(Thread * _thread);
   virtual void terminate(Thread * _thread);
   };
