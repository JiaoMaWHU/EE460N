/*
        Name: Xinyuan Pan
        UTEID: xp572
*/
/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    LD_PRIV,
    LD_SAVED_SSP,
    LD_SAVED_USP,
    LD_EXCV,
    LD_VECTOR,
    GATE_PCMIN2,
    GATE_PSR,
    GATE_SP,
    GATE_VECTOR,
    SPMUX1, SPMUX0,
    CCMUX,
    PRIVMUX,
    VECTORMUX,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_PRIV(int *x)       { return(x[LD_PRIV]); }
int GetLD_SAVED_SSP(int *x)  { return(x[LD_SAVED_SSP]); }
int GetLD_SAVED_USP(int *x)  { return(x[LD_SAVED_USP]); }
int GetLD_EXCV(int *x)       { return(x[LD_EXCV]); }
int GetLD_VECTOR(int *x)     { return(x[LD_VECTOR]); }
int GetGATE_PCMIN2(int *x)   { return(x[GATE_PCMIN2]); }
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGATE_SP(int *x)       { return(x[GATE_SP]); }
int GetGATE_VECTOR(int *x)   { return(x[GATE_VECTOR]); }
int GetSPMUX(int *x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetCCMUX(int *x)         { return(x[CCMUX]); }
int GetPRIVMUX(int *x)       { return(x[PRIVMUX]); }
int GetVECTORMUX(int *x)     { return(x[VECTORMUX]); }


/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM    0x08000
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */

/* For lab 4 */
int INT;  /* interrupt request bit */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SAVED_SSP; /* supervisor stack pointer */
int SAVED_USP; /* user stack pointer */
int PSR; /* Processor Status Register */
int VECTOR;

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

  eval_micro_sequencer();
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CYCLE_COUNT++;

  if (CYCLE_COUNT == 299) {
     NEXT_LATCHES.INT = 1; /* timer interrupt at cycle 300 */
     NEXT_LATCHES.INTV = 0x01;
  }

  CURRENT_LATCHES = NEXT_LATCHES;


}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename);
	while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SAVED_SSP = 0x3000; /* Initial value of supervisor stack pointer */
    CURRENT_LATCHES.INT = 0;
    CURRENT_LATCHES.PSR = 0x8002; /* initially user mode and Z=1 */
    CURRENT_LATCHES.REGS[6] = 0xFE00; /* it says USP is inited in the user program so this step may not be unnecessary */
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int signextend(int num, int from, int to){
   if (num>>(from-1) == 1) {
      int b = 1<<from;
      int n = to - from;
      while (n-- > 0) {
         num += b;
         b = b << 1;
      }
   }
   return num;
}

int memoryOut;
int ADDERres;
int PCMUXres, MARMUXres, ADDR1MUXres, ADDR2MUXres, drNum, sr1, sr2, VECTORMUXres, SPMUXres, PRIVMUXres;

/*
 * Evaluate the address of the next state according to the
 * micro sequencer logic. Latch the next microinstruction.
 */
void eval_micro_sequencer() {
   int ir = CURRENT_LATCHES.IR;
   int *microcode = CURRENT_LATCHES.MICROINSTRUCTION;
   /* the exception logic */
   int MAR0 = CURRENT_LATCHES.MAR - (CURRENT_LATCHES.MAR>>1<<1);
   int E1 = (CURRENT_LATCHES.MAR<0x2000?1:0) & (CURRENT_LATCHES.PSR>>15);
   int E0 = (MAR0 & GetDATA_SIZE(microcode)) | E1;

   if (GetIRD(microcode))
      NEXT_LATCHES.STATE_NUMBER = ir >> 12;
   else {
      if (GetCOND(microcode) == 0)
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode);
      else if (GetCOND(microcode) == 1)
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (CURRENT_LATCHES.READY<<1);
      else if (GetCOND(microcode) == 2)
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (CURRENT_LATCHES.BEN<<2);
      else if (GetCOND(microcode) == 3)
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | ((ir>>11) - (ir>>12<<1));
      else if (GetCOND(microcode) == 4) {
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (CURRENT_LATCHES.INT<<3);
         NEXT_LATCHES.INT = 0;
         /* Since only timer interrupt is possible and INTV is x01, this is ok. */
      }
      else if (GetCOND(microcode) == 5)
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (CURRENT_LATCHES.PSR>>15<<4);
      else if (GetCOND(microcode) == 6) /* used for state 51 */
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (E0<<4);
      else  /* used for state 26, 45, 53, 58 */
         NEXT_LATCHES.STATE_NUMBER = GetJ(microcode) | (E0<<5);
   }
   memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
}


/*
 * This function emulates memory and the WE logic.
 * Keep track of which cycle of MEM_EN we are dealing with.
 * If fourth, we need to latch Ready bit at the end of
 * cycle to prepare microsequencer for the fifth cycle.
 */
void cycle_memory() {
   static int memenCycle = 1;
   NEXT_LATCHES.READY = 0;
   /* I think current latch ready bit should be set to 0 at initialization,
      but since we know the first state has no memory access, we can do
      this here, first setting READY to 0 at the beginning of every cycle,
      then evaluate and see if we need to assert R */

   int MAR0 = CURRENT_LATCHES.MAR - (CURRENT_LATCHES.MAR>>1<<1);
   int *microcode = CURRENT_LATCHES.MICROINSTRUCTION;


   /* no memory mapped I/O */
   if (GetMIO_EN(microcode)) {
      if (memenCycle == MEM_CYCLES-1)
         NEXT_LATCHES.READY = 1; /* didnt even use READY in my implementation */
      if (memenCycle < MEM_CYCLES)
         memenCycle++;
      else { /* operate memory */
         memenCycle = 1;
         int WE1 = 0, WE0 = 0;

         if (GetR_W(microcode) == 0) {  /* read */

            int addr = CURRENT_LATCHES.MAR>>1<<1; /* unaligned access should be delt in the previous state, so this is ok */
            memoryOut = MEMORY[addr/2][0] + (MEMORY[addr/2][1] << 8);
         }
         else {   /* write */
            if (GetDATA_SIZE(microcode) == 0 && MAR0 == 0)
               WE0 = 1;
            else if (GetDATA_SIZE(microcode) == 0 && MAR0 == 1)
               WE1 = 1;
            else if (GetDATA_SIZE(microcode) == 1 && MAR0 == 0) {
               WE1 = 1;
               WE0 = 1;
            }
            /* Won't be 11 because unaligned store is delt in the previous state */
            if (WE0)
               MEMORY[CURRENT_LATCHES.MAR/2][0] = CURRENT_LATCHES.MDR - (CURRENT_LATCHES.MDR >> 8 << 8);
            if (WE1)
               MEMORY[CURRENT_LATCHES.MAR/2][1] = CURRENT_LATCHES.MDR >> 8;
         }
      }
   }
}



/*
 * Datapath routine emulating operations before driving the bus.
 * Evaluate the input of tristate drivers
 *             Gate_MARMUX, Gate_PC, Gate_ALU, Gate_SHF, Gate_MDR.
 */
int busValue; /* idk why eval_bus_drivers and drive_bus are separated. This is the workaround */
void eval_bus_drivers() {
   int *microcode = CURRENT_LATCHES.MICROINSTRUCTION;
   int ir = CURRENT_LATCHES.IR;



   /* MUX */
   if (GetVECTORMUX(microcode))
      VECTORMUXres = CURRENT_LATCHES.INTV;
   else
      VECTORMUXres = CURRENT_LATCHES.EXCV;




   if (GetDRMUX(microcode) == 1)
      drNum = 7;
   else if (GetDRMUX(microcode) == 0)
      drNum = (ir>>9) - (ir>>12<<3);
   else
      drNum = 6;

   if (GetSR1MUX(microcode) == 1)
      sr1 = CURRENT_LATCHES.REGS[(ir>>6) - (ir>>9<<3)];
   else if (GetSR1MUX(microcode) == 0)
      sr1 = CURRENT_LATCHES.REGS[(ir>>9) - (ir>>12<<3)];
   else
      sr1 = CURRENT_LATCHES.REGS[6];

   if ((ir>>5)-(ir>>6<<1)) /* sr2 MUX */
      sr2 = signextend(ir - (ir>>5<<5), 5, 16);
   else
      sr2 = CURRENT_LATCHES.REGS[ir - (ir>>3<<3)];

   if (GetSPMUX(microcode) == 0)
      SPMUXres = CURRENT_LATCHES.SAVED_USP;
   else if (GetSPMUX(microcode) == 1)
      SPMUXres = CURRENT_LATCHES.SAVED_SSP;
   else if (GetSPMUX(microcode) == 2)
      SPMUXres = sr1 + 2;
   else
      SPMUXres = sr1 - 2;

   if (GetADDR1MUX(microcode))
      ADDR1MUXres = sr1;
   else
      ADDR1MUXres = CURRENT_LATCHES.PC;

   if (GetADDR2MUX(microcode) == 0)
      ADDR2MUXres = 0;
   else if (GetADDR2MUX(microcode) == 1)
      ADDR2MUXres = signextend(ir - (ir>>6<<6), 6, 16);
   else if (GetADDR2MUX(microcode) == 2)
      ADDR2MUXres = signextend(ir - (ir>>9<<9), 9, 16);
   else if (GetADDR2MUX(microcode) == 3)
      ADDR2MUXres = signextend(ir - (ir>>11<<11), 11, 16);

   if (GetLSHF1(microcode))
      ADDERres = Low16bits((ADDR2MUXres<<1) + ADDR1MUXres);
   else
      ADDERres = Low16bits(ADDR2MUXres + ADDR1MUXres);

   if (GetMARMUX(microcode))
      MARMUXres = ADDERres;
   else
      MARMUXres = (ir - (ir>>8<<8))<<1;

   /* Gate    only 1 is gated in one cycle */
   if (GetGATE_PC(microcode))
      busValue = CURRENT_LATCHES.PC;

   else if (GetGATE_MDR(microcode)) {
      if (GetDATA_SIZE(microcode)) { /* word */
         busValue = CURRENT_LATCHES.MDR;
      } else { /* byte */
         int MAR0 = CURRENT_LATCHES.MAR - (CURRENT_LATCHES.MAR>>1<<1);
         if (MAR0)
            busValue = signextend(CURRENT_LATCHES.MDR >> 8, 8, 16);
         else
            busValue = signextend(CURRENT_LATCHES.MDR - (CURRENT_LATCHES.MDR >> 8 << 8), 8, 16);
      }
   }

   else if (GetGATE_MARMUX(microcode))
      busValue = MARMUXres;

   else if (GetGATE_ALU(microcode)) {
      if (GetALUK(microcode) == 0)
         busValue = Low16bits(sr1 + sr2);
      else if (GetALUK(microcode) == 1)
         busValue = sr1 & sr2;
      else if (GetALUK(microcode) == 2)
         busValue = sr1 ^ sr2;
      else if (GetALUK(microcode) == 3)
         busValue = sr1;
   }

   else if (GetGATE_SHF(microcode)) {
      int bit5 = (ir>>5) - (ir>>6<<1);
      int bit4 = (ir>>4) - (ir>>5<<1);
      int amount4 = ir - (ir>>4<<4);
      if (bit4 == 0) {
         busValue = Low16bits(sr1 << amount4);
      } else if (bit5 == 1) { /* arithmetic */
         if (sr1>>15 == 0) {
            busValue = sr1 >> amount4;
         } else {
            int temp = sr1;
            while (amount4-->0) {
               temp = (temp >> 1) + 0x8000;
            }
            busValue = temp;
         }
      } else if (bit5 == 0) { /* logic */
         busValue = sr1 >> amount4;
      }
   }

   else if (GetGATE_PCMIN2(microcode))
      busValue = CURRENT_LATCHES.PC - 2;

   else if (GetGATE_PSR(microcode))
      busValue = CURRENT_LATCHES.PSR;

   else if (GetGATE_SP(microcode))
      busValue = SPMUXres;

   else if (GetGATE_VECTOR(microcode))
      busValue = 0x0200 + (CURRENT_LATCHES.VECTOR<<1);

   else { /* no gate is asserted */
      busValue = 0;
   }
}

/*
 * Datapath routine for driving the bus from one of the 5 possible
 * tristate drivers.
 */
void drive_bus() {
   BUS = busValue;
}

/*
 * Datapath routine for computing all functions that need to latch
 * values in the data path at the end of this cycle.  Some values
 * require sourcing the bus; therefore, this routine has to come
 * after drive_bus.
 */
void latch_datapath_values() {
   int *microcode = CURRENT_LATCHES.MICROINSTRUCTION;
   int ir = CURRENT_LATCHES.IR;

   /* the following MUXes have to be here cuz they may use bus value*/
   if (GetPCMUX(microcode) == 0)
      PCMUXres = Low16bits(CURRENT_LATCHES.PC + 2);  /*overflow*/
   else if (GetPCMUX(microcode) == 1)
      PCMUXres = BUS;
   else if (GetPCMUX(microcode) == 2)
      PCMUXres = ADDERres;

   if (GetPRIVMUX(microcode))
      PRIVMUXres = 0;
   else PRIVMUXres = BUS>>15;

   /* Load values */
   if (GetLD_MAR(microcode))
      NEXT_LATCHES.MAR = BUS;

   if (GetLD_MDR(microcode)) {
      if (GetMIO_EN(microcode))
         NEXT_LATCHES.MDR = memoryOut; /* no memory mapped I/O */
      else {
         if (GetDATA_SIZE(microcode))  /* word */
            NEXT_LATCHES.MDR = BUS;
         else  /* byte */
            NEXT_LATCHES.MDR = ((BUS & 0x00FF) << 8) + (BUS & 0x00FF);
      }
   }

   if (GetLD_IR(microcode))
      NEXT_LATCHES.IR = BUS;

   if (GetLD_BEN(microcode)) {
      int n = (ir>>11) - (ir>>11<<1);
      int z = (ir>>10) - (ir>>10<<1);
      int p = (ir>>9) - (ir>>9<<1);
      NEXT_LATCHES.BEN = (n & CURRENT_LATCHES.N) | (z & CURRENT_LATCHES.Z) | (p & CURRENT_LATCHES.P);
   }

   if (GetLD_REG(microcode))
      NEXT_LATCHES.REGS[drNum] = BUS;

   if (GetLD_CC(microcode)) {
      if (GetCCMUX(microcode)) { /* put CCMUX here, easier to code */
         NEXT_LATCHES.P = BUS&0x01;
         NEXT_LATCHES.Z = (BUS&0x02) >> 1;
         NEXT_LATCHES.N = (BUS&0x04) >> 2;
      }
      else {
         int sign = BUS>>15;
         if (BUS == 0) {
            NEXT_LATCHES.P = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.N = 0;
         } else if (sign == 1) {
            NEXT_LATCHES.P = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.N = 1;
         } else {
            NEXT_LATCHES.P = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.N = 0;
         }
      }
      /* update the CC in PSR */
      /* use NEXT_LATCHES.PSR here, since current and next are the same, and we are modifying different parts of PSR separately */
      NEXT_LATCHES.PSR = (NEXT_LATCHES.PSR&0xFFF8) + NEXT_LATCHES.P + (NEXT_LATCHES.Z<<1) + (NEXT_LATCHES.N<<2);
   }

   if (GetLD_PC(microcode))
      NEXT_LATCHES.PC = PCMUXres;

   if (GetLD_EXCV(microcode)) {
      int MAR0 = CURRENT_LATCHES.MAR - (CURRENT_LATCHES.MAR>>1<<1);
      int E1 = (CURRENT_LATCHES.MAR<0x2000?1:0) & (CURRENT_LATCHES.PSR>>15);
      int E0 = (MAR0 & GetDATA_SIZE(microcode)) | E1;
      /* check exception */
      if (E0 == 1) {
         if (E1 == 0) /* unaligned access */
            NEXT_LATCHES.EXCV = 0x03;
         else if (E1 == 1) /* protection */
            NEXT_LATCHES.EXCV = 0x02;
      } else /* illegal opcode */
            NEXT_LATCHES.EXCV = 0x04;
   }

   if (GetLD_VECTOR(microcode))
      NEXT_LATCHES.VECTOR = VECTORMUXres;

   if (GetLD_PRIV(microcode)) /* use NEXT_LATCHES.PSR here, since we are modifying different parts of PSR separately */
      NEXT_LATCHES.PSR = (PRIVMUXres << 15) + (NEXT_LATCHES.PSR&0x7FFF);

   if (GetLD_SAVED_USP(microcode))
      NEXT_LATCHES.SAVED_USP = sr1;

   if (GetLD_SAVED_SSP(microcode))
      NEXT_LATCHES.SAVED_SSP = sr1;

}
