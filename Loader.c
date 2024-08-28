#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  if(ehdr){
    free(ehdr);
  }
  
  if(phdr){
    free(phdr);
  }
  
  close(fd);
  
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);
  // 1. Load entire binary content into the memory from the ELF file.
  // 2. Iterate through the PHDR table and find the section of PT_LOAD
  int size1=sizeof(ehdr);
  //check --------------?????????
  ehdr=(ehdr*)malloc(size1);
  if(ehdr==NULL){
    printf("Allocation not done\n");
    exit(1);
  }
  int read1=read(fd,&ehdr,size1);
  //    type that contains the address of the entrypoint method in fib.c
  if(read1!=size1){
    printf("Cannnot read properly\n");
    exit(1);
  }
  
  int l=lseek(fd,ehdr.phoff,SEEK_SET);
  if(l==-1){
     printf("Error\n");
     exit(1);
  }
  
  //read Pheader;
  phdr=(phdr*)malloc(ehdr.phnum*e_phentsize);
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  int size2=ehdr.phnum*e_phentsize;
  int read2=read(fd,&phdr,size2);
  if(read2!=size2){
    printf("Error\n");
    exit(1);
  }
  
  
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
