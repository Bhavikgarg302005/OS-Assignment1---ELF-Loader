#Create lib_simpleloader.so from loader.c
all:
	gcc -m32 -c -fpic loader.c
	gcc -m32 -shared -o lib_simpleloader.so loader.o && mv lib_simpleloader.so ../bin/ 
#Provide the command for cleanup
