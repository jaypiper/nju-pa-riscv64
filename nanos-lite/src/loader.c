#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
#define PGSIZE 4096
#define PG_OFFSET 0xfff
#define PG_BEGIN(addr) ((void*)((uintptr_t)addr & (~PG_OFFSET)))
#define PG_END(addr) ((void*)(PG_BEGIN(addr) + PGSIZE))
#define PADDR_FROM_VADDR(paddr,vaddr) ((void*)((uintptr_t)paddr | ((uintptr_t)(vaddr) & PG_OFFSET)))
#define min(a,b) ((uintptr_t)(a) < (uintptr_t)(b)? (a):(b))

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {

  Elf64_Ehdr _Eheader;
  int fd = fs_open(filename, 0, 0);

  fs_read(fd, &_Eheader, sizeof(_Eheader));
  //检查一下是不是elf, 讲义上的这种方法更简洁: assert(*(uint32_t *)elf->e_ident == 0xBadC0de); 
  assert(_Eheader.e_ident[0] == 0x7f && _Eheader.e_ident[1] == 'E' && _Eheader.e_ident[2] == 'L' && _Eheader.e_ident[3] == 'F');

  for(int i = 0; i < _Eheader.e_phnum; i++){
    Elf64_Phdr _Pheader;
    fs_lseek(fd,  _Eheader.e_phoff + i * _Eheader.e_phentsize, SEEK_SET);
    fs_read(fd, &_Pheader, sizeof(_Pheader));
    if(_Pheader.p_type == PT_LOAD){
#ifdef HAS_VME
      // printf("offset: %x p_vaddr: %x p_filesz: %x p_memsz: %x\n", _Pheader.p_offset, _Pheader.p_vaddr, _Pheader.p_filesz, _Pheader.p_memsz);
      uintptr_t _offset = 0;
      void* _paddr = NULL;
      int read_sz;
      for(; _offset < _Pheader.p_filesz; _offset += read_sz){

        _paddr = new_page(1);
        void* _vaddr = (void*)(_Pheader.p_vaddr + _offset);
        map(&(pcb->as), PG_BEGIN(_vaddr), _paddr, 0);
        read_sz = min(PG_END(_vaddr) - _vaddr, _Pheader.p_filesz - _offset);
        fs_lseek(fd, _Pheader.p_offset + _offset, SEEK_SET);
        fs_read(fd, PADDR_FROM_VADDR(_paddr, _vaddr), read_sz);
      } 
      // printf("offset: %lx\n", _offset);
      for(; _offset < _Pheader.p_memsz; _offset += read_sz){

        void* _vaddr = (void*)(_Pheader.p_vaddr + _offset);
        if(((uintptr_t)_vaddr & PG_OFFSET) == 0) _paddr = new_page(1);
        map(&(pcb->as), PG_BEGIN(_vaddr), _paddr, 0);
        read_sz = min(PG_END(_vaddr) - _vaddr, _Pheader.p_memsz - _offset);
        memset(PADDR_FROM_VADDR(_paddr, _vaddr), 0, read_sz);
        // printf("%x %x %x\n", _Pheader.p_vaddr, _Pheader.p_filesz, _offset);
      }
      pcb->max_brk = (uintptr_t)PG_BEGIN((_Pheader.p_vaddr + _Pheader.p_memsz + 0xfff));
#else
      for(int _offset = 0; _offset < _Pheader.p_filesz; _offset ++){
        uint8_t _data;
        fs_lseek(fd, _Pheader.p_offset + _offset, SEEK_SET);
        fs_read(fd, &_data, 1);
        *((uint8_t*)(_Pheader.p_vaddr + _offset)) = _data;
      }
      for(int _offset = _Pheader.p_filesz; _offset < _Pheader.p_memsz; _offset ++){
        *((uint8_t*)(_Pheader.p_vaddr + _offset)) = 0;
      }
#endif
    } 
  }
  
  return _Eheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void*), void* arg){
  // printf("in context_kload, %lx %lx %lx\n", pcb->as.area.start, pcb->as.area.end, STACK_SIZE);
  pcb->as.area.start = (void*)(pcb->stack);
  pcb->as.area.end = (void*)((uint8_t*)pcb->stack + STACK_SIZE);
  // printf("in context_kload, %lx %lx %lx\n", pcb->as.area.start, pcb->as.area.end, STACK_SIZE);
  pcb->cp = kcontext(pcb->as.area, entry, arg);
  
  return;
}

void* new_page(size_t nr_page);
void switch_boot_pcb();

void context_uload(PCB* pcb, const char* filename, char *const argv[], char *const envp[]){
  assert(filename);
  printf("uload %s...\n", filename);
#ifdef HAS_VME
  protect(&pcb->as);
  void* _start = new_page(8);
  void* _end = (void*)((uint8_t*)_start + STACK_SIZE);
  for(int i = 1; i <= 8; i++){
    map(&pcb->as, pcb->as.area.end - i * PGSIZE, _end - i * PGSIZE, 0x7);
  }
#else
  pcb->as.area.start = new_page(8);
  pcb->as.area.end = (void*)((uint8_t*)pcb->as.area.start + STACK_SIZE);
#endif
  // Area _as = {_start, _end};

  uintptr_t entry = loader(pcb, filename);
  /*
  void* cur = _end;
  
  int argc = 0, envc = 0;
  // int arg_len = 0, env_len = 0;
  while(argv && argv[argc]) argc ++;
  while(envp && envp[envc]) envc ++;
  // printf("argc: %d, envc: %d\n", argc, envc);
  uintptr_t arg_p[argc];
  uintptr_t env_p[envc];
  
  int offset = sizeof(uintptr_t);
  for(int i = 0; i < argc; i++){
    int len = strlen(argv[i]);
    strcpy(cur - offset - len - 1, argv[i]);
    arg_p[i] = (uintptr_t)(cur - offset - len - 1);
    // printf("%d %s %s %lx\n", argc, argv[i], (char*)(cur - offset - len - 1), cur - offset - len - 1);
    offset += len + 1;
  }
  
  for(int j = 0; j < envc; j++){
    int len = strlen(envp[j]);
    strcpy(cur - offset - len - 1, envp[j]);
    env_p[j] = (uintptr_t)(cur - offset - len - 1);
    offset += len + 1;
  }
  
  *(uintptr_t*)(cur - offset - sizeof(uintptr_t)) = 0;
  *(uintptr_t*)(cur - offset - sizeof(uintptr_t)*2) = 0;
  offset += sizeof(uintptr_t) * 2;
  
  for(int j = envc-1; j >= 0; j--){
    *(uintptr_t*)(cur - offset - sizeof(uintptr_t)) = env_p[j];
    offset += sizeof(uintptr_t);
  }
  *(uintptr_t*)(cur - offset - sizeof(uintptr_t)) = 0;
  offset += sizeof(uintptr_t);

  for(int i = argc-1; i >= 0; i--){
    *(uintptr_t*)(cur - offset - sizeof(uintptr_t)) = arg_p[i];
    offset += sizeof(uintptr_t);
  }
  
  *(uintptr_t*)(cur - offset - sizeof(uintptr_t)) = argc; 
  offset += sizeof(uintptr_t);

  // printf("offset: %d %d\n", offset, sizeof(uintptr_t));
  Area _area = {.start = pcb->as.area.start, .end = pcb->as.area.end - offset};
  */
  Area _stack = {pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = ucontext(&pcb->as, _stack, (void*)entry);
  pcb->cp->GPRx = (uintptr_t)pcb->as.area.end;
  pcb->cp->gpr[2] = (uintptr_t)pcb->as.area.end;
  // printf("GPRx: %lx %lx\n", (uintptr_t)_area.end, cur - offset - sizeof(uintptr_t), *(uintptr_t*)(cur - offset - sizeof(uintptr_t)));
  printf("uload finished\n");
  // switch_boot_pcb();
  // yield();
  return;
}
