#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
void naive_uload(PCB *pcb, const char *filename);
void switch_boot_pcb();
void context_uload(PCB* pcb, const char* filename, char *const argv[], char *const envp[]);

static int _sys_brk(int new_brk){
  // Log("in brk %lx\n", new_brk);
  return 0;
}

static int _sys_time(){
  AM_TIMER_UPTIME_T _timer = io_read(AM_TIMER_UPTIME);
  // printf("sys time %ld\n", _timer.us);
  return _timer.us;
}
static int _sys_execve(char* filename, char* argv[], char* envp[]){
  // naive_uload(NULL, filename);
  context_uload(current, filename, argv, envp); //这里是不是还需要yield一下
  // assert(0);
  switch_boot_pcb();
  yield();
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("syscall: %ld %lx %lx %lx\n", a[0], a[1], a[2], a[3]);
  switch (a[0]) {
    case SYS_exit: _sys_execve("/bin/nterm", NULL, NULL); break;
    case SYS_yield: yield(); break; 
    case SYS_open: c->GPRx = fs_open((char*)a[1], a[2], a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void*)a[2], a[3]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (char*)a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_brk: c->GPRx = _sys_brk(a[1]); break;
    case SYS_execve: c->GPRx = _sys_execve((char*)a[1], (char**)a[2], (char**)a[3]); break;
    case SYS_gettimeofday: c->GPRx = _sys_time(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
