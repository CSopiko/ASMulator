# ASMulator
## Overview
Assembly Virtual Machine Emulator written in C++ for a Programming Paradigms course (CS107) bonus assignment. \
Emulates a subset of simplified assembly instruction set of a typical Reduced Instruction Set Computer (RISC) processor. \
The syntax of this assembly language is designed to make it easier to read and write, but the basic functionality is quite similar to any current RISC instruction set.
## Usage
#### Clone repository
```
git clone https://github.com/CSopiko/ASMulator.git
```

#### Run program
```
./asm
```
#### Compile and Run program
```
g++ ASMulator.cpp
./a.out
```
## View
<img src="https://github.com/CSopiko/ASMulator/blob/main/asmulator.png" width="362" height="234" >

## Supported Assembly Instructions
- Load/Store \
    Moves bytes back and forth between registers and memory 
- ALU \
    Operates on the registers
- Branch/Jump \
    Alters which instruction is executed next 


## License
[MIT](https://choosealicense.com/licenses/mit/)
