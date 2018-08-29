#ifndef MYSIGNAL_H
#define MYSIGNAL_H

#include <signal.h>
#include <sys/signal.h>

extern int  exit_by_signal ;
extern const char * sig_name [] ;
extern void (*default_handler_table[32])(int c) ;
extern void (*signal_handler_table [32])(int c) ;

extern void default_log    (int c) ;
extern void default_pause  (int c) ;
extern void default_exit   (int c) ;
extern void default_core   (int c) ;
extern void default_discard(int c) ;

extern void default_handler(int c) ;


extern void signal_handler_init() ;
extern void (*log_signal(int signum, void (*handler)(int)))(int) ;

#endif /* MYSIGNAL_H */
