#ifndef THREAD_H
#define	THREAD_H

#include "timer.h"
#include "isr.h"
#include "paging.h"

typedef struct process_struct {
    registers_t regs;
    page_directory_t* page_dir;
    struct process_struct* next;
    uint32 pid;
} process_t;

//process/thread function typedef
typedef void (*thread_func_t)();

void init_scheduler();

uint32 start_thread(thread_func_t);

void exit_thread();

void wait_for_thread(uint32);

#endif	/* THREAD_H */

