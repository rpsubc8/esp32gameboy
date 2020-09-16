#ifndef MEM_H
 #define MEM_H

//JJ #ifdef __cplusplus

//JJ extern "C" {
//JJ 
//JJ #endif

#include "rom.h"

//static unsigned char *mem; //memoria al principio
static unsigned char *mem; //memoria al principio
//static unsigned char mem[0x10000];

//void MemPrintROMWIFI(void);
//void MemInitPendingWIFI(void);
//unsigned char * MEMGetMEM();
//void MEMResetROM(void);
//void MEMUpload(unsigned char *auxBuf, int auxLen);
//void mem_initWIFI(void);
void MemAssignPtrMem(unsigned char *);
void MemPreparaBankSwitchPtr(unsigned char **ptr);
unsigned char * MemGetAddressMem(void);
void MemAssignROMPtrMemory(void);
void mem_init(void);
unsigned char mem_get_byte(unsigned short);
unsigned short mem_get_word(unsigned short);
void mem_write_byte(unsigned short, unsigned char);
void mem_write_word(unsigned short, unsigned short);
void mem_bank_switch(unsigned short int);
unsigned char mem_get_raw(unsigned short);
//JJ #ifdef __cplusplus

//JJ   }

//JJ #endif /* end of __cplusplus */
#endif
