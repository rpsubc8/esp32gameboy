#include "gbConfig.h"
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "rom.h"
#include "lcd.h"
#include "mbc.h"
#include "interrupt.h"
#include "timer.h"
#include "sdl.h"
#include "cpu.h"


//static unsigned char *mem;
static int DMA_pending = 0;
static int joypad_select_buttons, joypad_select_directions;

static unsigned short int gb_mem_bank_switch_prev=0;
const unsigned char *ptr_rom_getbytes=NULL; //Puntero a la ROM para switch

#if defined(use_max_ram)|| defined (use_half_ram)
 static unsigned char cont_bank_switch_cur = 0;
 static unsigned short int cache_bank_switch_number[maxBankSwitch];
 static unsigned char * cache_bank_switch_data[maxBankSwitch];
#endif 

//static unsigned int gbContROM = 0;

/*void MemPrintROMWIFI()
{
 byte contLinea=0;
 printf("ROM DUMP\n");
 char cad[200]="";
 for (unsigned int i=0;i<0x10000;i++)
 {
  printf("%02x",mem[i]);
  contLinea++;
  if (contLinea>60)
  {
   contLinea=0;
   printf("\n");
  }
 }  
}
*/

//**********************************
/*void MemInitPendingWIFI()
{//Resetea
 DMA_pending = 0;
 printf ("DMA %d CPU %d\n",DMA_pending,cpu_get_cycles());
 printf ("MEM %x\n",mem);
}
*/

//**********************************
/*unsigned char * MEMGetMEM()
{
 return mem;  
}
*/

//**********************************
/*void MEMResetROM()
{//Resetea contador memoria ROM para cargar desde WIFI
 gbContROM=0;
 memset(mem,1,0x10000); //La reinicio
}*/

//**********************************
/*void MEMUpload(unsigned char *auxBuf, int auxLen)
{
 if (auxLen<=0)
  return;

 for (unsigned int i=0;i<auxLen;i++)
 {
  if (gbContROM<0x10000)
   mem[gbContROM++] = auxBuf[i];
  else
   break;
 } 
}
*/

void ResetDMAPending()
{
 DMA_pending = 0;
 gb_mem_bank_switch_prev = 0;
}

void MemAssignPtrMem(unsigned char *ptr)
{
 mem = ptr;
}

#if defined(use_max_ram) || defined(use_half_ram)
 //*****************************************
 void MEMResetBankSwithBuffers()
 {
  cont_bank_switch_cur =0;
  for (unsigned char i=0;i<maxBankSwitch;i++)
  {
   cache_bank_switch_number[i]=0;  
   //memset(cache_bank_switch_data[i],0,0x4000);
  }
 } 

 //********************************************************
 void MemPreparaBankSwitchPtr(unsigned char **ptr)
 {
  for (unsigned char i=0;i<maxBankSwitch;i++)
   cache_bank_switch_data[i] = ptr[i];
 }

 void MemBankSwitchClear()
 {
  for (unsigned char i=0;i<maxBankSwitch;i++)
   memset(cache_bank_switch_data[i],1,0x4000);
  MEMResetBankSwithBuffers();
  cont_bank_switch_cur=0;
 }
#endif

//********************************************************
unsigned char * MemGetAddressMem()
{//Retorna direccion de mem
 return mem;
}

//************************************************
void MemAssignROMPtrMemory()
{//Asigno punteros que necesito para no llamar luego
 ptr_rom_getbytes= rom_getbytes();
}

#if defined(use_max_ram)|| defined (use_half_ram)
 //********************************************************
 char SearchBank(unsigned short int aBank)
 {
  unsigned char aReturn;
  for (unsigned char i=0;i<maxBankSwitch;i++)
  {
   if (cache_bank_switch_number[i] == aBank)
    return i;
  }
 
  //No se ha encontrado,lo metemos
  aReturn = cont_bank_switch_cur;
  cache_bank_switch_number[cont_bank_switch_cur++]= aBank;
  //memcpy (cache_bank_switch_data[aReturn],&ptr_rom_getbytes[aBank * 0x4000],0x4000); 
  memcpy (cache_bank_switch_data[aReturn],&ptr_rom_getbytes[(aBank<<14)],0x4000);
  #ifdef use_max_ram
   cont_bank_switch_cur = cont_bank_switch_cur & 0x07;
  #else 
   #ifdef use_half_ram
    cont_bank_switch_cur = cont_bank_switch_cur & 0x03;
   #endif  
  #endif
  return aReturn;
 }
#endif

//************************************************
void mem_bank_switch(unsigned short int n)
{//Tarda entre 432 y 656 micros paso a 44 microsegundos con cache bancos
 unsigned char aSearch;
 //unsigned long time_prev;
#ifdef usb_lib_optimice_checkchange_bankswitch
 if (n != gb_mem_bank_switch_prev)
#endif
 {  
  gb_mem_bank_switch_prev= n; 
  //time_prev = micros();
  #if defined(use_max_ram)|| defined (use_half_ram)
   aSearch = SearchBank(n);
   memcpy (&mem[0x4000],cache_bank_switch_data[aSearch],0x4000);    
  #else
   //#ifdef use_min_ram
   //memcpy(&mem[0x4000], &ptr_rom_getbytes[n * 0x4000], 0x4000);   
   memcpy(&mem[0x4000], &ptr_rom_getbytes[(n<<14)], 0x4000);   
   //#endif
  #endif
  //memcpy (&mem[0x4000],&ptr_rom_getbytes[n * 0x4000],0x4000);
  //time_prev = micros()-time_prev;
  //printf("Tiempo %d\n",time_prev);    
 }
   
 //const unsigned char *b = rom_getbytes();
 //memcpy(&mem[0x4000], &b[n * 0x4000], 0x4000);
}

/* LCD's access to VRAM */
unsigned char mem_get_raw(unsigned short p)
{
	return mem[p];
}

unsigned char mem_get_byte(unsigned short i)
{
	unsigned long elapsed;
	unsigned char mask = 0;

	if(DMA_pending && i < 0xFF80)
	{
		elapsed = cpu_get_cycles() - DMA_pending;
		if(elapsed >= 160)
			DMA_pending = 0;
		else
		{
			return mem[0xFE00+elapsed];
		}
	}

	if(i < 0xFF00)
		return mem[i];

	switch(i)
	{
		case 0xFF00:	/* Joypad */
			if(!joypad_select_buttons)
				mask = sdl_get_buttons();
			if(!joypad_select_directions)
				mask = sdl_get_directions();
			return 0xC0 | (0xF^mask) | (joypad_select_buttons | joypad_select_directions);
		break;
		case 0xFF04:
			return timer_get_div();
		break;
		case 0xFF05:
			return timer_get_counter();
		break;
		case 0xFF06:
			return timer_get_modulo();
		break;
		case 0xFF07:
			return timer_get_tac();
		break;
		case 0xFF0F:
			return interrupt_get_IF();
		break;
		case 0xFF41:
			return lcd_get_stat();
		break;
		case 0xFF44:
			return lcd_get_line();
		break;
		case 0xFF4D:	/* GBC speed switch */
			return 0xFF;
		break;
		case 0xFFFF:
			return interrupt_get_mask();
		break;
	}

	return mem[i];
}

unsigned short mem_get_word(unsigned short i)
{
	unsigned long elapsed;

	if(DMA_pending && i < 0xFF80)
	{
		elapsed = cpu_get_cycles() - DMA_pending;
		if(elapsed >= 160)
			DMA_pending = 0;
		else
		{
			return mem[0xFE00+elapsed];
		}
	}
	return mem[i] | (mem[i+1]<<8);
}

void mem_write_byte(unsigned short d, unsigned char i)
{
	unsigned int filtered = 0;

	switch(rom_get_mapper())
	{
		case NROM:
			if(d < 0x8000)
				filtered = 1;
		break;
		case MBC2:
		case MBC3:
			filtered = MBC3_write_byte(d, i);
		break;
		case MBC1:
			filtered = MBC1_write_byte(d, i);
		break;
	}

	if(filtered)
		return;

	switch(d)
	{
		case 0xFF00:	/* Joypad */
			joypad_select_buttons = i&0x20;
			joypad_select_directions = i&0x10;
		break;
		case 0xFF01: /* Link port data */
//			fprintf(stderr, "%c", i);
		break;
		case 0xFF04:
			timer_set_div(i);
		break;
		case 0xFF05:
			timer_set_counter(i);
		break;
		case 0xFF06:
			timer_set_modulo(i);
		break;
		case 0xFF07:
			timer_set_tac(i);
		break;
		case 0xFF0F:
			interrupt_set_IF(i);
		break;
		case 0xFF40:
			lcd_write_control(i);
		break;
		case 0xFF41:
			lcd_write_stat(i);
		break;
		case 0xFF42:
			lcd_write_scroll_y(i);
		break;
		case 0xFF43:
			lcd_write_scroll_x(i);
		break;
		case 0xFF45:
			lcd_set_ly_compare(i);
		break;
		case 0xFF46: /* OAM DMA */
			/* Copy bytes from i*0x100 to OAM */
			memcpy(&mem[0xFE00], &mem[i*0x100], 0xA0);
			DMA_pending = cpu_get_cycles();
		break;
		case 0xFF47:
			lcd_write_bg_palette(i);
		break;
		case 0xFF48:
			lcd_write_spr_palette1(i);
		break;
		case 0xFF49:
			lcd_write_spr_palette2(i);
		break;
		case 0xFF4A:
			lcd_set_window_y(i); break;
		case 0xFF4B:
			lcd_set_window_x(i); break;
		case 0xFFFF:
			interrupt_set_mask(i);
			return;
		break;
	}
	
	mem[d] = i;
}

void mem_write_word(unsigned short d, unsigned short i)
{
	mem[d] = i&0xFF;
	mem[d+1] = i>>8;
}

void mem_init(void)
{
  const unsigned char *bytes = rom_getbytes();
  #if defined(use_max_ram) || defined (use_half_ram)
   MEMResetBankSwithBuffers();
  #endif

	//mem = (unsigned char *)calloc(1, 0x10000);
  memset(mem,1,0x10000); //La reinicio

	memcpy(&mem[0x0000], &bytes[0x0000], 0x4000);
	memcpy(&mem[0x4000], &bytes[0x4000], 0x4000);

	mem[0xFF10] = 0x80;
	mem[0xFF11] = 0xBF;
	mem[0xFF12] = 0xF3;
	mem[0xFF14] = 0xBF;
	mem[0xFF16] = 0x3F;
	mem[0xFF19] = 0xBF;
	mem[0xFF1A] = 0x7F;
	mem[0xFF1B] = 0xFF;
	mem[0xFF1C] = 0x9F;
	mem[0xFF1E] = 0xBF;
	mem[0xFF20] = 0xFF;
	mem[0xFF23] = 0xBF;
	mem[0xFF24] = 0x77;
	mem[0xFF25] = 0xF3;
	mem[0xFF26] = 0xF1;
	mem[0xFF40] = 0x91;
	mem[0xFF47] = 0xFC;
	mem[0xFF48] = 0xFF;
	mem[0xFF49] = 0xFF;
}



//***********************************
/*void mem_initWIFI(void)
{//Cuandos de carga de WIFI
  const unsigned char *bytes = rom_getbytes();

  //mem = (unsigned char *)calloc(1, 0x10000);
  memset(mem,1,0x10000); //La reinicio

  memcpy(&mem[0x0000], &bytes[0x0000], 0x4000);
  memcpy(&mem[0x4000], &bytes[0x4000], 0x4000);  
  
  mem[0xFF10] = 0x80;
  mem[0xFF11] = 0xBF;
  mem[0xFF12] = 0xF3;
  mem[0xFF14] = 0xBF;
  mem[0xFF16] = 0x3F;
  mem[0xFF19] = 0xBF;
  mem[0xFF1A] = 0x7F;
  mem[0xFF1B] = 0xFF;
  mem[0xFF1C] = 0x9F;
  mem[0xFF1E] = 0xBF;
  mem[0xFF20] = 0xFF;
  mem[0xFF23] = 0xBF;
  mem[0xFF24] = 0x77;
  mem[0xFF25] = 0xF3;
  mem[0xFF26] = 0xF1;
  mem[0xFF40] = 0x91;
  mem[0xFF47] = 0xFC;
  mem[0xFF48] = 0xFF;
  mem[0xFF49] = 0xFF;
}
*/
