#ifndef __HTMLAYOUT_QUEUE_H__
#define __HTMLAYOUT_QUEUE_H__

/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * Asynchronous GUI Task Queue.
 * Use these primitives when you need to run code in GUI thread.
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * 
 * (C) 2003-2006, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/*!\file
\brief Asynchronous GUI Task Queue
*/
#include <windows.h>
#include <stdio.h>
#include <assert.h>

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

namespace htmlayout 
{
  class mutex 
  { 
    CRITICAL_SECTION cs;
 public:
    void lock()     { EnterCriticalSection(&cs); } 
    void unlock()   { LeaveCriticalSection(&cs); } 
    mutex()         { InitializeCriticalSection(&cs); }   
    ~mutex()        { DeleteCriticalSection(&cs); }
  };
  
  class critical_section { 
    mutex& m;
  public:
    critical_section(mutex& guard) : m(guard) { m.lock(); }
    ~critical_section() { m.unlock(); }
  };

  // derive your own tasks from this and implement your own exec()
  class gui_task 
  {
    friend class queue;
    gui_task* next;
  public:
    gui_task(): next(0) {}
    virtual ~gui_task() {}
    virtual void exec() = 0; // override it 
  };
  
  // this one needs to be created as singleton - one instance per GUI thread(s)
  class queue
  { 
    gui_task* head;
    gui_task* tail;
    mutex     guard;
  
  public:
    queue():head(0),tail(0) {}
    
    void push( gui_task* new_task )
    {
      assert(new_task);
      {
        critical_section cs(guard);
        if( tail )
          tail->next = new_task;
        else
          head = new_task;
        tail = new_task;
      }
      PostMessage(NULL, WM_NULL, 0,0);
    }
    
    
    // Place this call after GetMessage()/PeekMessage() in main loop
    void execute()
    {
      gui_task* next;
      while(next = pop())
      {
        next->exec(); // do it
        delete next;
      }
    }
    
  private:
    gui_task* pop()
    {
      critical_section cs(guard);
      if( !head ) return 0;
      
      gui_task* t = head; 
      head = head->next;
      if( !head ) tail = 0;
      return t;
    }
  
  };
 
}

// for one GUI thread per application cases:
extern htmlayout::queue gui_queue;

/* 
   gui_queue shall be instantiated somewhere in the main file of your application and
   gui_queue.execute() call shall be made in message pump loop as:
   
// Main message loop:
	while (GetMessageW(&msg, NULL, 0, 0)) 
	{
    // execute asynchronous tasks in GUI thread.
    gui_queue.execute(); // <-- here

		if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}   

 */

/* Example of asynchronous operation wrapper:

     struct append_and_update: public gui_task
     {
       dom::element   what;
       dom::element   where;
              
       append_and_update( const dom::element& to, const dom::element& el ): where(to), what(el) {}
       
       // will be executed in GUI thread
       inline void exec() { where.append(what); where.update(true); }
     };

     when you need this oparation simply do: 
     
     void SomeThreadProc()
     {
       ...
       gui_queue.push( new append_and_update( parent, child ) );
     }

     so next queue::execute() invocation will execute that append_and_update::exec().

 */



#endif // __cplusplus

#endif // __HTMLAYOUT_QUEUE_H__
