//  CITS2002 Project 1 2021
//  Name(s):             Ciaran Engelbrecht
//  Student number(s):   23169641 

//  compile with:  cc -std=c11 -Wall -Werror -o runcool runcool.c

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THE STACK-BASED MACHINE HAS 2^16 (= 65,536) WORDS OF MAIN MEMORY
#define N_MAIN_MEMORY_WORDS (1<<16)

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
AWORD                       main_memory[N_MAIN_MEMORY_WORDS];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32


//  see:  https://teaching.csse.uwa.edu.au/units/CITS2002/projects/coolinstructions.php
enum INSTRUCTION {
    I_HALT       = 0,
    I_NOP,
    I_ADD,
    I_SUB,
    I_MULT,
    I_DIV,
    I_CALL,
    I_RETURN,
    I_JMP,
    I_JEQ,
    I_PRINTI,
    I_PRINTS,
    I_PUSHC,
    I_PUSHA,
    I_PUSHR,
    I_POPA,
    I_POPR
};

//  USE VALUES OF enum INSTRUCTION TO INDEX THE INSTRUCTION_name[] ARRAY
const char *INSTRUCTION_name[] = {
    "halt",
    "nop",
    "add",
    "sub",
    "mult",
    "div",
    "call",
    "return",
    "jmp",
    "jeq",
    "printi",
    "prints",
    "pushc",
    "pusha",
    "pushr",
    "popa",
    "popr"
};

//  ----  IT IS SAFE TO MODIFY ANYTHING BELOW THIS LINE  --------------


//  THE STATISTICS TO BE ACCUMULATED AND REPORTED
int n_main_memory_reads     = 0;
int n_main_memory_writes    = 0;
int n_cache_memory_hits     = 0;
int n_cache_memory_misses   = 0;

void report_statistics(void)
{
    printf("@number-of-main-memory-reads\t%i\n",    n_main_memory_reads);
    printf("@number-of-main-memory-writes\t%i\n",   n_main_memory_writes);
    printf("@number-of-cache-memory-hits\t%i\n",    n_cache_memory_hits);
    printf("@number-of-cache-memory-misses\t%i\n",  n_cache_memory_misses);
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

struct {
    int dirty;
     AWORD value;
     int tag; //first 11 bits ...(index is the final 5bits)
} cache[N_CACHE_WORDS];




AWORD read_memory(int address)
{   int mem_index = address&0x1F; //last 5 bits
    int mem_tag = (address>>5)&0x7FF; //first 11 bits
    
    //cache hit, marks cache as dirty and returns data
    if (cache[mem_index].tag == mem_tag) {++n_cache_memory_hits; return cache[mem_index].value;}
     
    //cache miss
    //save previously stored cache data
    int old_tag = cache[mem_index].tag;
    int old_address = (old_tag <<5) | mem_index;
    
    //checks if dirty, writes previous data in cache back into memory
    if (cache[mem_index].dirty == 1) {++n_main_memory_writes; main_memory[old_address] = cache[mem_index].value;}
    
    //reads data from memory into the cache block and marks as not dirty
        cache[mem_index].value = main_memory[address]; 
        cache[mem_index].tag = mem_tag;
        cache[mem_index].dirty = 0; 
     ++n_cache_memory_misses;     
     ++n_main_memory_reads;
    return main_memory[address];
}

void write_memory(AWORD address, AWORD value)
{   int mem_index = address&0x1F;
    int mem_tag = (address>>5)&0x7FF;
    
    //cache hit, writes new data into cache block, marks cache as dirty and returns
    if (cache[mem_index].tag == mem_tag) {cache[mem_index].value = value; cache[mem_index].dirty = 1; return;}
    
    //cache miss
    //save previously stored cache data   
    int old_tag = cache[mem_index].tag;
    int old_address = (old_tag <<5) | mem_index;

    //checks if dirty, writes previous data in cache back into memory
    if (cache[mem_index].dirty == 1){++n_main_memory_writes; main_memory[old_address] = cache[mem_index].value;}

    //writes new data into the cache block and marks as dirty
    cache[mem_index].value = value; 
    cache[mem_index].tag =mem_tag;
    cache[mem_index].dirty = 1;

    main_memory[address] = value;
}

//  -------------------------------------------------------------------

//  EXECUTE THE INSTRUCTIONS IN main_memory[]
int execute_stackmachine(void)
{
//  THE 3 ON-CPU CONTROL REGISTERS:
    int PC      = 0;                    // 1st instruction is at address=0
    int SP      = N_MAIN_MEMORY_WORDS;  // initialised to top-of-stack
    int FP      = 0;                    // frame pointer

//set all dirty bits to 1 and initialise tags
    for (int i=0; i< N_CACHE_WORDS; i++)
    {cache[i].dirty = 1;
    cache[i].tag = ((SP-1)>>5)&0x7FF;}



    while(true) {
        IWORD value1; 
        IWORD value2; 
        IWORD return_addr;
        IWORD relative_FP;
        IWORD result;
        IWORD ifzero;
        IWORD new_address;
        char front_half;
        char back_half;
        int new_counter;

//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED
        IWORD instruction   = read_memory(PC);
       /*printf("Instruction is: %s\n", INSTRUCTION_name[instruction]);
        printf("  PC is: %i,  SP is: %i,   FP is: %i ,  TOS is: (%i)\n", PC, SP, FP, read_memory(SP));*/
     
        ++PC;

        if(instruction == I_HALT) {
            break;
        }
        switch(instruction) {

        case I_NOP :        
                            break;
        case I_ADD :        
                            //read and pop value at TOS
                            value1 = read_memory(SP);
                            ++SP;
                            //read next value on TOS
                            value2 = read_memory(SP);
                            //add both values and overwrite as new TOS
                            write_memory(SP, value1 + value2);
                            break;
        case I_SUB :        
                            value1 = read_memory(SP);
                            ++SP;
                            value2 = read_memory(SP);
                            //the first value is taken from the second and overwrites the TOS
                            write_memory(SP, value2 - value1);
                            break;
        case I_MULT : 
                            value1 = read_memory(SP);
                            ++SP;
                            value2 = read_memory(SP);
                            //the two values are multiplied and overwrites the TOS
                            result = value1 * value2;
                            write_memory(SP, result);
                            break;
        case  I_DIV :
                            value1 = read_memory(SP);
                            ++SP;
                            value2 = read_memory(SP);
                            //the second value is divided by the first and overwrites the TOS
                            write_memory(SP, (value2 / value1));
                            break;
        case I_CALL :       
                            //stores the current value at PC   
                            new_counter = read_memory(PC);
                            //stores the address to return to and saves to TOS
                            return_addr = PC+1;
                            --SP;         
                            write_memory(SP, return_addr);
                            //saves the FP to TOS
                            --SP;
                            write_memory(SP, FP);
                            //sets the FP as the TOS and sets PC as the value at PC
                            FP = SP; 
                            PC = new_counter;
                            break;
        case I_RETURN :     
                            //stores the FP offset for the return value   
                            relative_FP = read_memory(PC);
                            //sets the PC correctly 
                            PC = read_memory(FP+1);
                            //push the return value to the callers TOS
                            write_memory(FP+relative_FP, read_memory(SP));
                            //restore the SP to the caller's TOS
                            SP = FP + relative_FP;
                            //restores the FP 
                            FP = read_memory(FP);
                            break;
        case I_JMP :        
                            //sets the PC to current value at PC
                            PC = read_memory(PC);
                            break;
        case I_JEQ :        
                            //stores value at TOS and pops that value
                            ifzero = read_memory(SP);
                            SP++;
                            //if the stored value is 0, sets PC to value at PC, otherwise increments PC
                            if (ifzero == 0)
                            PC = read_memory(PC);
                            else PC++;
                            break;
        case I_PRINTI :     
                            //prints the value at TOS and pops it   
                            printf( "%i", read_memory(SP));
                            SP++;
                            break;
        case I_PRINTS :     
                            //stores value at PC++    
                            new_address = read_memory(PC++);
                            int is_running = true;
                            //prints out the first 8bits and last 8bits of AWORD to print their characters until nulbyte is reached
                            while (is_running) {
                                value1 = read_memory(new_address++);
                                back_half = value1&0xff;
                                if (back_half=='\0'){is_running=false;}
                                printf("%c", back_half);
                                front_half = (value1>>8)&0xff;
                                if (front_half=='\0'){is_running=false;}
                                printf("%c", front_half);}
                            break;
        case I_PUSHC :      
                            //stores value at PC and increments PC   
                            value1 = read_memory(PC);
                            ++PC;
                            //pushes stack and writes the stored value to TOS
                            --SP;
                            write_memory(SP, value1);
                            break;
        case I_PUSHA :      
                            //stores address of PC and then stores the value at that address before incrementing PC    
                            value1 = read_memory(PC);
                            value2 = read_memory(value1);
                            ++PC;
                            //pushes stack and writes the stored value to TOS
                            --SP;
                            write_memory(SP, value2);
                            break;
        case I_PUSHR :      
                            //stores the FP offset     
                            relative_FP = read_memory(PC);
                            //stores the value at the FP offset and increments PC
                            value1 = read_memory(FP+relative_FP);
                            ++PC;
                            //pushes stack and saves the value to the TOS
                            --SP;
                            write_memory(SP, value1);
                            break;
        case I_POPA :       
                            //stores the value at PC and the value at TOS    
                            value1 = read_memory(PC);
                            value2 = read_memory(SP);
                            //pops the TOS and stores the previously stored SP into the stored address
                            SP++;
                            write_memory(value1, value2); 
                            ++PC;
                            break;
        case I_POPR :       
                            //stores the FP offset
                            relative_FP = read_memory(PC);
                            //stores the value at TOS
                            value1 = read_memory(SP);
                            //stores the address where the TOS value should be saved to
                            value2 = FP + relative_FP;
                            //pops the TOS and writes the stored value into the address
                            SP++;
                            write_memory(value2, value1); 
                            ++PC;
                            break;
        }
}

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
    return read_memory(SP); 
}

//  -------------------------------------------------------------------

//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
    memset(main_memory, 0, sizeof main_memory);   //  clear all memory


//  READ CONTENTS OF coolexe FILE

FILE  *fp_in = fopen(filename, "rb");
if(fp_in != NULL){
    size_t  got;
    int j=0;
    while((got = fread(&main_memory[j], 1, 2, fp_in)) >0) 
	{j++;}  //printf("%i\n", main_memory[j]); 
// terminate
    fclose (fp_in); 
    }
}


//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  CHECK THE NUMBER OF ARGUMENTS
    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
    read_coolexe_file(argv[1]);

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();
        printf("exit%i\n", result);


    return result;          // or  exit(result);
}
