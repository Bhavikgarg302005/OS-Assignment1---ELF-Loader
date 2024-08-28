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
  if(fd<0){
   perror("Error in opening the executable file");
   exit(1);
  }
  int size1=sizeof(Elf32_Ehdr);
  ehdr=(Elf32_Ehdr *)malloc(size1);
  if(ehdr==NULL){
    perror("Space Allocation not done for elf header");
    exit(1);
  }
  int read1=read(fd,ehdr,size1);
  if(read1!=size1){
    perror("Cannnot read properly elf header");
    exit(1);
  }
  
  int l=lseek(fd,ehdr->phoff,SEEK_SET);
  if(l==-1){
     perror("Error");
     exit(1);
  }
  // 2. Iterate through the PHDR table and find the section of PT_LOAD type that contains the address of the entrypoint method in fib.c
  //read Pheader;
  phdr=(Elf32_Phdr *)malloc(ehdr->phnum*ehdr->e_phentsize);
  if(phdr==NULL){
   perror("Space Allocation not done for program header table");
  }
  int size2=ehdr.phnum*e_phentsize;
  int read2=read(fd,phdr,size2);
  if(read2!=size2){
    perror("Cannnot read properly program header table");
    exit(1);
  }
  // 3. Allocate memory of the size "p_memsz" using mmap function and then copy the segment content
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"
  int add_start=ehdr->e_entry;
  bool flag=false;
  for(int i=0;i<ehdr->phnum;i++){
    if(phdr[i]->p_type==PT_LOAD){
     int start=phdr[i]->p_vaddr;
     int end=start+phdr[i]->p_memsz;
     if(start<=add_start && end>=add_start){
      flag=true;
     }
    }
  }
  
  if(flag){
     int (*_start)(void) = (int (*)(void))(ehdr->e_entry);
     int result = _start();
     printf("User _start return value = %d\n",result);
  }
  else{
   perror("Address Not found\n");
   exit(1);
  }
  
  loader_cleanup();
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
