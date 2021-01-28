### **Load**
Load instructions read bytes into a register. The source may be a constant value, another register, or a location in memory. memory location is expressed **M[*address*]** where *address* may be a constant number, a register, or a register plus a constant offset. Load and Store normally move a whole word at a time starting at the given address. To move less than a whole word at a time, use the variants “**=.1**” (1 byte) and “**=.2**” (2 bytes). 

*Load the constant 23 into register 4* 
- R4 = 23; 

*Copy the contents of register 2 into register 3* 
- R3 = R2; 

*Load char (one byte) starting at memory address 244 into register 6* 
- R6 =.1 M[244]; 

*Load R5 with the word whose memory address is in R1* 
- R5 = M[R1]; 

*Load the word that begins 8 bytes after the address in R1.
This is known as "constant offset" mode and is about the fanciest addressing mode a RISC processor will support.* 
- R4 = M[R1+8];

### **Store**

Store instructions are basically the reverse of load instructions— they move values from registers back out to memory. There is no path in a RISC architecture to move bytes directly from one place in memory to somewhere else in memory. Instead, you need to use loads to get bytes into registers, and then stores to move them back to memory.

*Store the constant number 37 into the word beginning at 400*
- M[400] = 37;

*Store the value in R6 into the word whose address is in R1*

- M[R1] = R6;

*Store lower half-word from R2 into 2 bytes starting at address 1024* 

- M[1024] =.2 R2;

*Store R7 into the word whose address is 12 more than the address in R1*
- M[R1+12] = R7;

***Note***: If you want to store negative integers in the memory, store them into registers first and then store Register at desirable memory address.
### **ALU**
Arithmetic Logical Unit (ALU) operations only work with the registers or constants.

*Add 6 to R3 and store the result in R1*

- R1 = 6 + R3;

*Subtract R3 from R2 and store the result in R1*

- R1 = R2 - R3;

The processor usually has two different versions of the arithmetic operations, one for integers and one for floating point numbers, invoked by two different instructions.

***Note***: Floating point numbers are not supported yet.

### **Branching**
By default, the CPU fetches and executes instructions from memory in order, working from low memory to high. Branch instructions alter this default order. Branch instructions test a condition and possibly change which instruction should be executed next by changing the value of the **PC** register. The operands in the test of a branch statement must be in registers or constant values.\
The full set of branch variants:\
**BLT** branch if first argument is less than second\
**BLE** less than or equal\
**BGT** greater than\
**BGE** greater than or equal\
**BEQ** equal\
**BNE** not equal

*Begin executing at address 344 if R1 equals 0*
- BEQ R1, 0, 344; 

*Begin executing at addr 8 past current instruction if R2 less than R3*
- BLT R2, R3, PC+8;

There is an unconditional **jump** that has no test, but just immediately diverts execution to a new address. Like branch, the address can be specified absolute or PC-relative.

*Begin executing at address 2000 unconditionally- like a goto*
- JMP 2000;

*Begin executing at address 12 before current instruction*
- JMP PC-12;