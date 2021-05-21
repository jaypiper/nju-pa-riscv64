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
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf64_Ehdr _Eheader;
  int fd = fs_open(filename, 0, 0);
  // if(fd == -1) return fd;
  fs_read(fd, &_Eheader, sizeof(_Eheader));
  //检查一下是不是elf, 讲义上的这种方法更简洁: assert(*(uint32_t *)elf->e_ident == 0xBadC0de); 
  assert(_Eheader.e_ident[0] == 0x7f && _Eheader.e_ident[1] == 'E' && _Eheader.e_ident[2] == 'L' && _Eheader.e_ident[3] == 'F');
  //跳转在naive_uload中，这里返回entry就OK

  for(int i = 0; i < _Eheader.e_phnum; i++){
    Elf64_Phdr _Pheader;
    fs_lseek(fd,  _Eheader.e_phoff + i * _Eheader.e_phentsize, SEEK_SET);
    fs_read(fd, &_Pheader, sizeof(_Pheader));
    if(_Pheader.p_type == PT_LOAD){
      for(int _offset = 0; _offset < _Pheader.p_filesz; _offset ++){
        uint8_t _data;
        fs_lseek(fd, _Pheader.p_offset + _offset, SEEK_SET);
        fs_read(fd, &_data, 1);
        *((uint8_t*)(_Pheader.p_vaddr + _offset)) = _data;
      }
      for(int _offset = _Pheader.p_filesz; _offset < _Pheader.p_memsz; _offset ++){
        *((uint8_t*)(_Pheader.p_vaddr + _offset)) = 0;
      }
    } 
  }
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

