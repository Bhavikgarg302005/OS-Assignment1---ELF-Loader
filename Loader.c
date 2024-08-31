#include "loader.h"

Elf32_Ehdr *ehdr;/the pointer to an instance of struct Elf32_Ehdr
Elf32_Phdr *phdr;/the pointer to an instance of struct Elf32_Phdr which will be used for storing the program headers
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  if(ehdr){
    free(ehdr);
    ehdr=NULL;
  }
  if(phdr){
    free(phdr);
    phdr=NULL;
  }
  close(fd);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);
  //1.Load entire binary content into the memory from the ELF file.
  //2.checking error in opening executable file:
  if(fd<0){
   perror("Error in opening the executable file");
   loader_cleanup();
   exit(1);
  }
  //3.Allocating size to elf header:
  int size1=sizeof(Elf32_Ehdr);
  ehdr=(Elf32_Ehdr *)malloc(size1);
  //4.Checking error that allocation of space has been done properly or not:
  if(ehdr==NULL){
    perror("Space Allocation not done for elf header");
    loader_cleanup();
    exit(1);
  }
  //5.reading Elf header to get offset of Phdr as well as e_entry:
  int read1=read(fd,ehdr,size1);
  //6.checking wheather reading is done properly or not:
  if(read1!=size1){
    perror("Cannnot read properly elf header");
    loader_cleanup();
    exit(1);
  }
  //7.Moving fd to phdr offset so that we can read phdr table properly:
  int l=lseek(fd,ehdr->e_phoff,SEEK_SET);
  //8.Cheking wheather movement of fd is done or not properly:
  if(l<0){
     perror("Error in moving to offset");
     loader_cleanup();
     exit(1);
  }
  //9.Allocating space to Phdr table:
  phdr=(Elf32_Phdr *)malloc(ehdr->e_phnum*ehdr->e_phentsize);
  int size2=ehdr->e_phnum*ehdr->e_phentsize;
  //10.Checking error that allocation of space has been done properly or not:
  if(phdr==NULL){
   perror("Space Allocation not done for program header table");
   loader_cleanup();
   exit(1);
  }
  //11.reading phdr table:
  int read2=read(fd,phdr,size2);
  //12.has reading done properly or not:
  if(read2!=size2){
    perror("Cannnot read properly program header table");
    loader_cleanup();
    exit(1);
  }
  //13.Iterate through the PHDR table and find the section of PT_LOAD type that contains the address of the entrypoint method in fib.c
  for(int i=0;i<ehdr->e_phnum;i++){
    if(phdr[i].p_type==PT_LOAD && (phdr[i].p_vaddr)<=ehdr->e_entry && (ehdr->e_entry<=phdr[i].p_vaddr+phdr[i].p_memsz)){
     //14.Allocate memory of the size "p_memsz" using mmap function and then copy the segment content
     void *virtual_mem=mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS ,0 ,0);
     //15.Error checking is space allocated correctly:
     if(virtual_mem == MAP_FAILED){
      perror("Cannot load data in memory");
      exit(1);
     }
     ehdr->e_entry=virtual_mem+(ehdr->e_entry-phdr[i].p_vaddr);
     //16.Moving fd to each phdr[i].offset so as to copy the segment:
     int xyz=lseek(fd,phdr[i].p_offset,SEEK_SET);
     //17.Error in moving fd to phdr[i].offset
     if(xyz==-1){
      perror("Error in reading");
      loader_cleanup();
      exit(1);
     }
     //18.coping the segment 
     int size3=phdr[i].p_memsz;
     int read3=read(fd,virtual_mem,size3);
     //19.Error checking 
     if(read3<0){
       perror("Segment cannot be loaded properly");
       exit(1);
     }
    }
  }
  //20.Typecasting the address to that of function pointer matching "_start" method in fib.c.
  int (*_start)(void) = (int (*)(void))(ehdr->e_entry);
  //21.Calling the "_start" method and printing the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
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
