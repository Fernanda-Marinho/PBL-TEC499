#include "address_map_arm.h"
#include "physical.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h> 
#include <fcntl.h>


void* map_physical(int fd, unsigned int base, unsigned int span)
{
	void *virtual_base;

	// Obter um mapeamento dos endereços físicos para endereços virtuais
	virtual_base = mmap (NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
	if (virtual_base == MAP_FAILED)
	{
		printf ("ERRO: mmap() falhou...\n");
		close (fd);
		return (NULL);
	}
	return virtual_base;
}

int main(){

    static unsigned int * sysmgr_base_ptr;
    static void * sysmgrbase_virtual;
    static int fd_i2c0base = -1, fd_sysmgr = -1;
    int fd; 
    int * reg; 

    if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1){
        printf ("ERRO: não foi possível abrir \"/dev/mem\"...\n");
    }

    if ((sysmgrbase_virtual = map_physical(fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL) {
        return(-1);
    }

    sysmgr_base_ptr = (unsigned int *) sysmgrbase_virtual;
    reg = sysmgr_base_ptr + 0x50;
    *sysmgr_base_ptr += 1;
    while(1){
     
    printf("%d\n",*reg);
    }
    return 0; 
}


