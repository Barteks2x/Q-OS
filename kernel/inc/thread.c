#include "thread.h"
#include "paging.h"

extern page_directory_t *kernel_directory;

process_t* queue_start;
process_t* current_process;
uint32 next_pid = 0;

static void preempt(registers_t* regs) {
    if(current_process != NULL)
        memcpy(&(current_process->regs), regs, sizeof(registers_t));
    
    if(current_process != NULL && current_process->next != NULL) 
        current_process = current_process->next;
    else
        current_process = queue_start;
    
    memcpy(regs, &current_process->regs, sizeof(registers_t));
    switch_page_directory(current_process->page_dir);
    //print("test", 0x3F);
}

void init_scheduler() {
    process_t* system_process = (process_t*) kmalloc(sizeof(process_t));
    system_process->next = NULL;
    system_process->page_dir = kernel_directory;
    system_process->pid = next_pid++;
    
    queue_start = current_process = system_process;
    
    //TODO: Dummy timer frequency. Need to find good value
    init_timer(100, &preempt);
}

uint32 start_thread(thread_func_t run_thread) {
   process_t *proc = (process_t*) kmalloc(sizeof(process_t));
   //kernel thread - so kernel page directory
   proc->page_dir = kernel_directory;
 
   /* now we need to set up registers, of course. */
   proc->regs.cs  = 0x08;
   proc->regs.ds  = 0x10;
   proc->regs.ss  = 0x10;
   proc->regs.eip = (uint32) run_thread;
   proc->next = NULL;
   proc->pid = next_pid++;
 
   /* add the new process to the end of the queue. */
   process_t *last = queue_start;
   while (last->next != NULL) last = last->next;
   last->next = proc;
   return proc->pid;
}

void exit_thread() {
    //this shouldn't be interrupted...
    __asm__ __volatile__("cli");
    process_t* prev = queue_start;
    process_t* next = current_process->next;
    while(prev->next != current_process) prev = prev->next;
    prev->next = next;
    kfree(current_process);
    current_process = NULL;
    __asm__ __volatile__("sti");
    //wait for next interrupt. This probably isn't the best way to do it
    //TODO: change it
    __asm__ __volatile__("hlt");
    
}

bool has_process(uint32 pid) {
    __asm__ __volatile__("cli");
    process_t* proc = queue_start;
    bool found = false;
    while(proc != NULL) {
        if(proc->pid == pid) {
            found = true;
            break;
        }
        proc = proc->next;
    }
    __asm__ __volatile__("sti");
    return found;
}
void wait_for_thread(uint32 pid) {
    while(has_process(pid)) {
        int i = 0;
       __asm__ __volatile__("hlt");
    }
}