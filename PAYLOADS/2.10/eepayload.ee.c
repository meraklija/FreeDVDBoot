// ElReino & CTurt 2020

int (*SifIopReset)(char *, int) = (void *)0x85360;
void (*SifInitRpc)(int) = (void *)0x84500;
void (*SifExitRpc)(void) = (void *)0x84690;

extern void SifWriteBackDCache(void *ptr, int size);
extern int SifSetReg(unsigned int register_num, unsigned int register_value);
extern int SifGetReg(unsigned int register_num);

static int SifIopSync(void) {
	#define SIF_REG_SMFLAG 4
	#define SIF_STAT_BOOTEND 0x40000
	return((SifGetReg(SIF_REG_SMFLAG) & SIF_STAT_BOOTEND) != 0);
}

static void flush(void) {
	asm volatile("la $v1, 0x64; la $a0, 0; syscall 0x64"); /* FlushCache data writeback */
	asm volatile("la $v1, 0x64; la $a0, 2; syscall 0x64"); /* FlushCache instruction invalidate */
}

int main(void) {
	volatile int *waitAddress = (void *)0x21FFF7F0;
	while(!*waitAddress);

	flush();
	
	SifInitRpc(0);
	SifExitRpc();
	while(!SifIopReset("", 0));
	while(!SifIopSync()){};
	
	//SifInitRpc(0);
	
	volatile void **entry_point_address = (void *)0x01FFF7E0;
	ExecPS2(*entry_point_address, 0, 0, 0);
}
