#include "mysignal.h"
#include <iostream>
#include <string.h>
#include "pairing_graph.h"

using namespace std ;
using namespace findsplit ;

int exit_by_signal = 0 ;

void default_log(int c){
  cerr << "catch a signal " << to_string(c) << "." << endl ;
}

void default_pause(int c){
  default_log(c);
  cerr << "paused." << endl ;
  sigpause(0);
  cerr << "pause done." << endl ;
}

void default_exit(int c){
  default_log(c);
  cerr << "try to exit." << endl ;
  exit_by_signal = 1 ;
}

void default_core(int c){
  default_log(c);
  cerr << "exit (core)." << endl ;
  exit(2);
}

void default_discard(int c){
  default_log(c);
  cerr << "signal discarded." << endl ;
}

// The following descriptions that maybe come from old FreeBSD or Linux
// manual is not appropriate at the moment.

void (*default_handler_table[32])(int c) = {
  default_discard, // SIG ?     0    internal (cannot be caught)
  default_exit,    // SIGHUP    1    hangup
  default_exit,    // SIGINT    2    interrupt
  default_core,    // SIGQUIT   3*   quit
  default_core,    // SIGILL    4*   illegal instruction
  default_core,    // SIGTRAP   5*   trace trap
  default_core,    // SIGABRT   6*   abort (abort(3) routine)
  default_core,    // SIGEMT    7*   emulator trap
  default_core,    // SIGFPE    8*   arithmetic exception
  default_discard, // SIGKILL   9    kill (cannot be caught)
  default_core,    // SIGBUS    10*  bus error
  default_core,    // SIGSEGV   11*  segmentation violation
  default_core,    // SIGSYS    12*  bad argument to system call
  default_exit,    // SIGPIPE   13   write on a pipe or other socket
                   //                with no one to read it
  default_exit,    // SIGALRM   14   alarm clock
  default_exit,    // SIGTERM   15   software termination signal
  default_discard, // SIGURG    16@  urgent condition present on socket
  default_discard, // SIGSTOP   17+  stop (cannot be caught)
  default_pause,   // SIGTSTP   18+  stop signal generated from keyboard
  default_discard, // SIGCONT   19@  continue after stop
  default_discard, // SIGCHLD   20@  child status has changed
  default_pause,   // SIGTTIN   21+  background read attempted from 
                   //                control terminal
  default_pause,   // SIGTTOU   22+  background write attempted to 
                   //                control terminal
  default_discard, // SIGIO     23@  I/O is possible on a descriptor
  default_exit,    // SIGXCPU   24   cpu time limit exceeded 
  default_exit,    // SIGXFSZ   25   file size limit exceeded
  default_exit,    // SIGVTALRM 26   virtual time alarm (getitimer(2))
  default_exit,    // SIGPROF   27   profiling timer alarm 
  default_discard, // SIGWINCH  28@  window changed
  default_core,    // SIGLOST   29*  resource lost (see lockd(8C))
  default_exit,    // SIGUSR1   30   user-defined signal 1
  default_exit,    // SIGUSR2   31   user-defined signal 2
};

const char * sig_name[] = {
  "SIG ?     internal (cannot be caught)",
  "SIGHUP    hangup",
  "SIGINT    interrupt",
  "SIGQUIT   quit",
  "SIGILL    illegal instruction",
  "SIGTRAP   trace trap",
  "SIGABRT   abort (abort(3) routine)",
  "SIGEMT    emulator trap",
  "SIGFPE    arithmetic exception",
  "SIGKILL   kill (cannot be caught)",
  "SIGBUS    bus error",
  "SIGSEGV   segmentation violation",
  "SIGSYS    bad argument to system call",
  "SIGPIPE   write on a pipe or other socket with no one to read it",
  "SIGALRM   alarm clock",
  "SIGTERM   software termination signal",
  "SIGURG    urgent condition present on socket",
  "SIGSTOP   stop (cannot be caught)",
  "SIGTSTP   stop signal generated from keyboard",
  "SIGCONT   continue after stop",
  "SIGCHLD   child status has changed",
  "SIGTTIN   background read attempted from control terminal",
  "SIGTTOU   background write attempted to control terminal",
  "SIGIO     I/O is possible on a descriptor",
  "SIGXCPU   cpu time limit exceeded ",
  "SIGXFSZ   file size limit exceeded",
  "SIGVTALRM virtual time alarm (getitimer(2))",
  "SIGPROF   profiling timer alarm ",
  "SIGWINCH  window changed",
  "SIGLOST   resource lost (see lockd(8C))",
  "SIGUSR1   user-defined signal 1",
  "SIGUSR2   user-defined signal 2",
};

void (*signal_handler_table[32])(int c) ;

void default_handler(int c){
  cerr << "signal = " << c << ", "
       << (((c<0)||(c>31))?"unknown":sig_name[c]) << endl ;
  signal_handler_table[c](c);
}

void signal_handler_init(){
  int i ;
  memcpy(
    signal_handler_table,
    default_handler_table,
    sizeof(default_handler_table)
  );
  signal(SIGINT,default_handler);
/*
  for(i=1;i<32;i++){ signal(i, default_handler); }
*/
}

void (*log_signal(int signum, void (*handler)(int)))(int){
  void (*old_handler)(int) = SIG_ERR ;

  if(signum >= 0  &&  signum < 32){
    old_handler = signal_handler_table[signum] ;

    if(handler==SIG_DFL){ handler = default_handler_table[signum]; }
    if(handler==SIG_IGN){ handler = default_discard ; }

    signal_handler_table[signum] = handler ;
  }
  return old_handler ;
}
