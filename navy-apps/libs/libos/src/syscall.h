#ifndef __SYSCALL_H__
#define __SYSCALL_H__

// enum {
//   SYS_exit,
//   SYS_yield,
//   SYS_open,
//   SYS_read,
//   SYS_write,
//   SYS_kill,
//   SYS_getpid,
//   SYS_close,
//   SYS_lseek,
//   SYS_brk,
//   SYS_fstat,
//   SYS_time,
//   SYS_signal,
//   SYS_execve,
//   SYS_fork,
//   SYS_link,
//   SYS_unlink,
//   SYS_wait,
//   SYS_times,
//   SYS_gettimeofday
// };

#define SYS_read      0x0
#define SYS_write     0x1
#define SYS_open      0x2
#define SYS_close     0x3
#define SYS_fstat     0x5
#define SYS_lseek     0x8
#define SYS_mmap      0x9
#define SYS_brk       0xc
#define SYS_getpid    0x14
#define SYS_yield     0x18
#define SYS_dup       0x20
#define SYS_fork      0x39
#define SYS_execve    0x3b
#define SYS_exit      0x3c
#define SYS_kill      0x3e
#define SYS_chdir     0x50
#define SYS_mkdir     0x53
#define SYS_link      0x56
#define SYS_unlink    0x57
#define SYS_gettimeofday 0x60

#endif
