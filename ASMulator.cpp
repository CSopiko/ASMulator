#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map> //improved performance than std::map
#include <string.h>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <thread>
#include <time.h>

using namespace std;

//ASSEMU
#define STACK_SIZE_ 32768 //some power of 2
#define MAX_N_REGISTERS 8180
#define AMULATOR_COMMAND_LINE ">_< #: "           // for lineNum
#define AMULATOR_LIVE_COMMAND_LINE ">_< live#%u: " // for livelineNum
#define COMMAND_ENDS ';'
#define WORD_BYTES 4 
#define CHAR_CAST ".1"
#define CHAR_BYTES 1
#define SHORT_CAST ".2"
#define SHORT_BYTES 2
#define PRINT_UNSIGNED_INT "$U"


//Flags and Commands
#define READ_FROM_FILE "-f"
#define READ_FROM_COMMAND_LINE "-l"
#define DISPLAY_MEMORY "-rg"
#define COMMAND_EXIT "exit"
#define COMMAND_GUIDE "guide"
#define WELCOME "Privet" //file name

//special registers
#define SP "SP"
#define PC "PC"
#define RV "RV"
#define REGISTER 'R'
//
#define MEMO 'M'
#define RETURN "RET"
#define JUMP_CALL "JMP"
#define RESET_CALL "RESET"
#define PRINT_CALL "PRINT"
#define FUNCTION_CALL "CALL"
#define DEFINE_CALL "DEFINE"
#define END_DEF_CALL "END_DEF"

//Instruction prefixes 
#define DEFINE_PREFIX_LENGTH 6 //DEFINE
#define BRANCH_PREFIX_LENGTH 3 //B _ _
#define SIGNED_PREFIX_LENGTH 2 //$D
#define PRINT_PREFIX_LENGTH 5 //PRINT
#define CALL_PREFIX_LENGTH 4   //CALL
#define JUMP_PREFIX_LENGTH 3   //JMP
#define CAST_PREFIX_LENGTH 2   // .1 || .2
#define MEMO_PREFIX_LENGTH 1   //M


#define MEMO_PARENTHESES_SIZE 2 //[ ]
#define PRINT_PARENTHESES_SIZE 2 // < >
#define DEFINE_PARENTHESES_SIZE 2 // < >
#define FUNCTION_PARENTHESES_SIZE 2 // < >

//branches
#define BLT "BLT"
#define BLE "BLE"
#define BGT "BGT"
#define BGE "BGE"
#define BEQ "BEQ"
#define BNE "BNE"

//Instuction types
#define ALU 0
#define LOAD 1
#define STORE 2
#define DEFINE 3
#define BRANCH 4
#define JUMP 5
#define CALL 6
#define RET 7
#define END_DEF 8
#define FUNCTION 9
#define RESET 10
#define PRINT 11
#define ASSIGNMENT 12
#define OTHER -1 

//exceptions
#define TOO_MUCH "CSopiko::MULTIPLE_ASSEMBLY_COMMANDS\n"
#define ALU_FAILED  "CSopiko::FAILED_ALU_INSTRUCTION\n"
#define CAST_FAILED "CSopiko::FAILED_TYPE_CONVERSION\n"
#define END_OF_LINE "CSopiko::ASSEMBLY_COMMANT_NOT_PROPERLY_ENDED (line:%u)\n"
#define NO_FUNCTION "CSopiko::NO_FUNCTION_WITH_THAT_NAME\n"
#define PRINT_FAILED "CSopiko::FAILED_TO_PRINT_GIVEN_CONTENT\n"
#define TYPE_NOT_FOUND "CSopiko::TYPE_NOT_FOUND\n"
#define PATH_NOT_FOUND "CSopiko::INVALID_PATH_TO_FILE\n"
#define INCORRECT_COMMAND "CSopiko::INCORRECT_TERMINAL_COMMAND\n"
#define LOAD_STORE_FAILED "CSopiko::FAILED_LOAD/STORE_INSTRUCTION\n"
#define FILE_PROCESSING_FAILED "CSopiko::FILE_PROCESSING_FAILED_ON_%u_INSTRUCTION/LINE\n"

//
#define BRANCH_PASS "BRANCH_PASSED_SUCCESSFULLY\n"
#define BRANCH_NOT_PASS "BRANCH_NOT_PASSED\n"
#define FILE_PARSED "FILE_PARSED_SUCCESSFULLY \n"
#define FUNCTION_DEFINED "FUNCTION_DEFINED_SUCCESSFULLY\n"
#define CAST_PERFORMED "TYPE_CONVERSION_DONE: %u BYTE(S)\n "
#define PROGRAM_ENDS "PROGRAM_ENDS\n"



//thread sleep:
#define TIME_DETECT_TYPE 150
#define TIME_WELCOME 250
#define TIME_TIPS 400

//COLORS
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//functions
void welcomeText();
void amulator_cl();     //command line
void amulator_live_cl();//live command line

void emulate();
bool user_input();
bool live_processing();
void execute_memory();
void execute_guide();
bool file_processing();
string get_path();
bool parse_file(ifstream &file);
void reset_registers();
//line modification functions
bool check_eol(string &line);
bool execute_line(string &str);
void unspace_line(string &str);
void uppercase_line(string &str);

//check instructions
int  check_type(string left, string right, string line);
bool is_alu(string half);
bool is_constant(string constNum);
bool is_constants_alu(string const1, string const2);
bool is_register_constant_alu(string reg1, string const2);
bool is_registers_alu(string reg1, string reg2);
bool is_load_store(string half);
bool is_branch(string br);
bool is_branch_prefix(string prefix);
bool is_pc_relative(string destAddr);
bool is_register(string reg);
bool is_function_call(string funcal);
bool is_function_legal(string fun);
bool is_return(string ret);
bool is_jump(string jmp);
bool is_define(string def);
bool is_enddef(string enddef);
bool is_reset(string reset_str);
bool is_print(string pr);
bool passed_branch(string br, int lvalue, int rvalue);

//executing instructions
void execute_alu(string alu_str);
void execute_load(string load_str);
void execute_store(string store_str);
void execute_assignment(string ass_str);
void execute_branch(string br_str);
void execute_jump(string jmp_str);
void execute_func_call(string func_call);
void execute_func_def(string func_def);
void execute_print(string print_str);
void execute_return();

//find indexes
int contains_assignment(string s);
int contains_alu_oper(string s);
unsigned int bytes_to_copy(string expr);
unsigned int store_load_address(string destAddr);

/* GLOBAL VARIABLES */
//Virtual memory M
char STACK[STACK_SIZE_];

unordered_map<string, int> registers;
unordered_map<string, unsigned int> functions;
vector<string> instructions;

//command line numbers
unsigned int lineNum;
unsigned int livelineNum;
bool is_live; //general state


int main(){    
    welcomeText();    
    emulate();
    return 0;
}

int  check_type(string left, string right, string line){
    if(left.length() == 0 || right.length() == 0){ // check for CALL, RET, JUMP, BRANCH type;
        if(is_branch(line)) return BRANCH;
        if(is_jump(line))return JUMP;
        if(is_function_call(line))return CALL;
        if(is_return(line))return RET;
        if(is_define(line))return DEFINE;
        //if(is_enddef(line))return END_DEF;
        if(is_reset(line))return RESET;
        if(is_print(line))return PRINT;
    }else
    {// check for store load alu 
        if(is_register(left)){ //left register : load or alu
            unsigned int bytesnum = bytes_to_copy(line);
            if(bytesnum != WORD_BYTES) right = right.substr(CAST_PREFIX_LENGTH, right.length()-CAST_PREFIX_LENGTH);
            if(is_load_store(right))return LOAD;
            if(is_alu(right))return ALU;
            if(right[0] == '-')right = right.substr(1, right.length()-1);
            if(is_constant(right) || is_register(right)) return ASSIGNMENT;
        }else if(is_load_store(left)){ // store
            //right: const, register .2 .1 
            if(is_constant(right) || is_register(right))return STORE;
        }        
    }
    return OTHER;
}
void amulator_cl(){
    printf(BOLDCYAN AMULATOR_COMMAND_LINE ANSI_COLOR_RESET);
    lineNum++;
}
void amulator_live_cl(){
    printf(BOLDMAGENTA AMULATOR_LIVE_COMMAND_LINE ANSI_COLOR_RESET, livelineNum);
    livelineNum++;
}
void execute_guide(){
  ifstream file;
  file.open("guide");
  if (file.is_open())
      cout << file.rdbuf();
  file.close();
}

void execute_alu(string alu_str){
    size_t eqIndex = contains_assignment(alu_str);
    string reg = alu_str.substr(0, eqIndex);
    string right = alu_str.substr(eqIndex+1, alu_str.length()-eqIndex-1 );
    if(is_constant(right)){
        registers[reg] = std::__cxx11::stoi(right); //converts string to int;
    }else{
        size_t aluIndex = contains_alu_oper(right);
        string loperand = right.substr(0, aluIndex);
        string roperand = right.substr(aluIndex+1, right.length()-aluIndex-1);
        int val1, val2;
        if(is_constants_alu(loperand, roperand)){
            val1 = std::__cxx11::stoi(loperand);
            val2 = std::__cxx11::stoi(roperand);
        }else if(is_registers_alu(loperand, roperand)){
            val1 = registers[loperand];
            val2 = registers[roperand];
        }else if(is_register_constant_alu(loperand, roperand)){
            val1 = registers[loperand];
            val2 = std::__cxx11::stoi(roperand);
        }else if(is_register_constant_alu(roperand, loperand)){
            val1 = std::__cxx11::stoi(loperand);
            val2 = registers[roperand];
        }
        switch (right[aluIndex])
        {
            case '-':
                registers[reg] = val1-val2;
                break;
            case '+':
                registers[reg] = val1+val2;            
                break;
            case '*':
                registers[reg] = val1*val2;
                break;
            case '/':
                registers[reg] = val1/val2;
                break;                    
            default:
                printf(ANSI_COLOR_RED ALU_FAILED ANSI_COLOR_RESET);
                return;
                break;
        }
    }
}
unsigned int store_load_address(string destAddr){
    unsigned int memoAddr;                                

    if(is_constant(destAddr)){
        memoAddr = std::__cxx11::stoi(destAddr);       
    }else if(is_register(destAddr)){
        memoAddr = registers[destAddr];
    }else{
        size_t signIndex = contains_alu_oper(destAddr);
        string loperand = destAddr.substr(0, signIndex);
        string roperand = destAddr.substr(signIndex+1, destAddr.length()-signIndex-1);
        int val1, val2;
        if(is_register_constant_alu(loperand, roperand)){
            val1 = registers[loperand];
            val2 = std::__cxx11::stoi(roperand);
        }else if(is_register_constant_alu(roperand, loperand)){
            val1 = std::__cxx11::stoi(loperand);
            val2 = registers[roperand];
        }        
        switch (destAddr[signIndex])
        {
        case '+': 
            memoAddr = val1+val2;
            break;
        case '-': 
            memoAddr = val1-val2;
            break;
        default:
            printf(ANSI_COLOR_RED LOAD_STORE_FAILED ANSI_COLOR_RESET);
            break;
        }                
    }
    return memoAddr;    

}
void execute_load(string load_str){
    size_t eqIndex = contains_assignment(load_str);
    string reg = load_str.substr(0, eqIndex);
    string destAddr = load_str.substr(eqIndex+1+MEMO_PREFIX_LENGTH + MEMO_PARENTHESES_SIZE/2, 
                                load_str.length()- eqIndex-1 - MEMO_PREFIX_LENGTH - MEMO_PARENTHESES_SIZE);
    //cout << "dest addr: " <<destAddr << endl;                                
    //how many bytes to load????
    unsigned int bytesnum = bytes_to_copy(load_str);
    if(bytesnum != WORD_BYTES)destAddr = destAddr.substr(CAST_PREFIX_LENGTH, destAddr.length()-CAST_PREFIX_LENGTH);
    unsigned int memoAddr = store_load_address(destAddr);
    //cout << memoAddr << endl;   
    unsigned int val;                       
    switch (bytesnum)
    {
    case CHAR_BYTES: 
        val =*((unsigned char*)(&STACK[memoAddr]));
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, CHAR_BYTES);
        break;
    case SHORT_BYTES:
        val =*((unsigned short*)(&STACK[memoAddr]));
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, SHORT_BYTES);
        break;
    case WORD_BYTES:
        val =*((unsigned int*)(&STACK[memoAddr]));
        break;       
    default:
        printf(ANSI_COLOR_RED CAST_FAILED ANSI_COLOR_RESET);
        return;
        break;
    }  
    registers[reg] = val;    
}

void execute_store(string store_str){
    size_t eqIndex = contains_assignment(store_str);
    string right = store_str.substr(eqIndex+1, store_str.length() - eqIndex - 1);
    string destAddr = store_str.substr(MEMO_PREFIX_LENGTH + MEMO_PARENTHESES_SIZE/2, 
                            store_str.length()- right.length() - MEMO_PARENTHESES_SIZE - MEMO_PREFIX_LENGTH - 1);
    //cout << "dest addr: " <<destAddr << endl;                                
    unsigned int bytesnum = bytes_to_copy(store_str);
    if(bytesnum != WORD_BYTES)destAddr = right.substr(CAST_PREFIX_LENGTH, right.length()-CAST_PREFIX_LENGTH);
    unsigned int memoAddr = store_load_address(destAddr); 
    //cout << memoAddr << endl;                                

    int buffer;
    if(is_constant(right)) {
        buffer = std::__cxx11::stoi(right);
    }else if(is_register(right)){
        buffer = registers[right];
    }
    //cout << "buffer: "  <<buffer << endl;                                

    //how many bytes to load????
    switch (bytesnum)
    {
    case CHAR_BYTES: 
        *((unsigned char*)(&STACK[memoAddr])) = (char)buffer;        
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, CHAR_BYTES);
        break;
    case SHORT_BYTES:
        *((unsigned short*)(&STACK[memoAddr])) = (short)buffer;        
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, SHORT_BYTES);
        break;
    case WORD_BYTES:
        *((unsigned int*)(&STACK[memoAddr])) = (int)buffer;        
        break;       
    default:
        printf(ANSI_COLOR_RED CAST_FAILED ANSI_COLOR_RESET);
        return;
        break;
    }
}
void execute_assignment(string ass_str){
    size_t eqIndex = contains_assignment(ass_str);
    string reg = ass_str.substr(0, eqIndex);
    string value = ass_str.substr(eqIndex+1, ass_str.length()- eqIndex-1);

    unsigned int bytesnum = bytes_to_copy(ass_str);
    if(bytesnum != WORD_BYTES)value = value.substr(CAST_PREFIX_LENGTH, value.length()-CAST_PREFIX_LENGTH);
    bool negative = false;
    if(value[0] == '-'){
        value = value.substr(1, value.length()-1);        
        negative = true;
    }
    int buffer;
    if(is_register(value)){
        buffer = registers[value];
    }else if(is_constant(value)){
        buffer = std::__cxx11::stoi(value);
    }

    switch (bytesnum)
    {
    case CHAR_BYTES: 
        *((unsigned char*)(&registers[reg])) = (unsigned char)buffer;        
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, CHAR_BYTES);
        break;
    case SHORT_BYTES:
        *((unsigned short*)(&registers[reg])) = (unsigned short)buffer;  
        printf(ANSI_COLOR_GREEN CAST_PERFORMED ANSI_COLOR_RESET, SHORT_BYTES);
        break;
    case WORD_BYTES:
        if(negative) buffer *=-1;
        *((unsigned int*)(&registers[reg])) = (unsigned int)buffer;        
        break;       
    default:
        printf(ANSI_COLOR_RED CAST_FAILED ANSI_COLOR_RESET);
        return;
        break;
    }
}
bool passed_branch(string br, int lvalue, int rvalue){
    return ((br == BLE && lvalue <= rvalue) || (br == BLT && lvalue < rvalue) || (br == BEQ && lvalue == rvalue) || 
            (br == BNE && lvalue != rvalue) || (br == BGT && lvalue > rvalue) || (br == BGE && lvalue >= rvalue));
}
void execute_branch(string br_str){
    string branchInstr = br_str.substr(0, BRANCH_PREFIX_LENGTH);
    size_t firstComma = br_str.find(',');
    size_t secondComma = br_str.rfind(',');

    string firstArg = br_str.substr(BRANCH_PREFIX_LENGTH, firstComma- BRANCH_PREFIX_LENGTH);
    string secondArg = br_str.substr(firstComma +1, secondComma - firstComma -1);
    string jumpCommand = JUMP_CALL + br_str.substr(secondComma+1, br_str.length()- secondComma -1);

    
    int firstOper, secondOper;
    if(is_constant(firstArg))firstOper = std::__cxx11::stoi(firstArg);
    else if(is_register(firstArg))firstOper = registers[firstArg];

    if(is_constant(secondArg))secondOper = std::__cxx11::stoi(secondArg);
    else if(is_register(secondArg))secondOper = registers[secondArg];

    if(is_live){
        string jmpSuffix = jumpCommand.substr(JUMP_PREFIX_LENGTH, jumpCommand.length()-JUMP_PREFIX_LENGTH);
        size_t plusIndex = jmpSuffix.find('+');
        if(plusIndex != string::npos){
            int numlines = std::__cxx11::stoi(jmpSuffix.substr(plusIndex+1, jmpSuffix.length()-plusIndex - 1));
            string currline;
            for(int i = 0; i < numlines; i++){
                amulator_live_cl();
                getline(cin, currline);
                if(!check_eol(currline)){
                    printf(ANSI_COLOR_RED END_OF_LINE ANSI_COLOR_RESET, livelineNum-1);
                    i--;
                    continue;
                }
                unspace_line(currline);
                uppercase_line(currline);
                instructions.push_back(currline);
            }
        }
    }

    if(passed_branch(branchInstr, firstOper, secondOper)){
        printf(ANSI_COLOR_BLUE BRANCH_PASS ANSI_COLOR_RESET);
        execute_jump(jumpCommand);
    }else{
        printf(ANSI_COLOR_MAGENTA BRANCH_NOT_PASS ANSI_COLOR_RESET);
        if(is_live){
            for(; registers[PC]/WORD_BYTES < instructions.size(); registers[PC] += WORD_BYTES){
                execute_line(instructions[registers[PC]/WORD_BYTES]);
            }
        }
    }

}
void execute_func_call(string func_call){
        //saved pc
    registers[SP] -= WORD_BYTES;
    *((unsigned int*)(&STACK[registers[SP]])) = registers[PC]; //all 4 bytes
    string func_name = func_call.substr(CALL_PREFIX_LENGTH + FUNCTION_PARENTHESES_SIZE/2, 
                                        func_call.length()- CALL_PREFIX_LENGTH - FUNCTION_PARENTHESES_SIZE);
    registers[PC] = functions[func_name]; //address of the definition 
    if(is_live){
        string currline;
        //execute from instructions set
        while(currline != RETURN){
            registers[PC] += WORD_BYTES;
            currline = instructions[registers[PC]/WORD_BYTES];
            execute_line(currline);
        }
    }
}
void execute_func_def(string func_def){
    //where the function definition starts
    unsigned int defStart = registers[PC];
    string func_name = func_def.substr(DEFINE_PREFIX_LENGTH + DEFINE_PARENTHESES_SIZE/2, 
                                        func_def.length() - DEFINE_PREFIX_LENGTH-DEFINE_PARENTHESES_SIZE);
    //cout << func_name << " -location- " << defStart<< endl;                                       
    functions[func_name] = defStart;
    //cout << "saved function " << endl;
    if(is_live){
        string command;
        while(command != END_DEF_CALL){
            amulator_live_cl();
            getline(cin, command);        
            if(!check_eol(command)){
                printf(ANSI_COLOR_RED END_OF_LINE ANSI_COLOR_RESET, livelineNum-1);
            }else{
                unspace_line(command);
                uppercase_line(command);
                instructions.push_back(command);
                registers[PC] += WORD_BYTES;
            }            
        }
    }else{
        //for(;instructions[registers[PC]] != END_DEF_CALL; registers[PC] += WORD_BYTES); 
        //or while loop
        while(instructions[registers[PC]/WORD_BYTES] != END_DEF_CALL){
            //cout << "PC: " << registers[PC]<< " ";
            //cout << instructions[registers[PC]/WORD_BYTES] << endl;
            registers[PC] += WORD_BYTES;
        }
    }                                        
    printf(ANSI_COLOR_GREEN FUNCTION_DEFINED ANSI_COLOR_RESET);

}
void execute_return(){
    //programm ends
    if(registers[PC] / WORD_BYTES >= instructions.size()-1){ //last instruction
        printf(ANSI_COLOR_GREEN PROGRAM_ENDS ANSI_COLOR_RESET);
        return;
    }
    //function ends
    unsigned int savedPC = *((unsigned int*)(&STACK[registers[SP]]));
    registers[PC] = savedPC;
    registers[SP]+= WORD_BYTES; //returning SP
    //print where am i

    if(is_live){
        for(;registers[PC]/WORD_BYTES <instructions.size(); registers[PC]+=WORD_BYTES){
            execute_line(instructions[registers[PC]]);
        }
    }
}
void execute_print(string print_str){
    string content = print_str.substr(PRINT_PREFIX_LENGTH + PRINT_PARENTHESES_SIZE/2, 
                                    print_str.length()- PRINT_PREFIX_LENGTH - PRINT_PARENTHESES_SIZE);
    //printf("Content of: "); cout << content << endl; 
    bool unsign = false;
    size_t signedPrint = content.find(PRINT_UNSIGNED_INT);
    if(signedPrint != string::npos){
        content = content.substr(SIGNED_PREFIX_LENGTH, content.length()-SIGNED_PREFIX_LENGTH);
        unsign = true;
    }
    unsigned int bytesNum = bytes_to_copy(content);
    if(bytesNum != WORD_BYTES)content = content.substr(CAST_PREFIX_LENGTH, content.length()-CAST_PREFIX_LENGTH);
    int buffer;
    if(is_constant(content)){
        buffer = std::__cxx11::stoi(content);
        //cout << "--- " <<content << " ---" << endl;
    }else if(is_register(content)){
        buffer = registers[content];
        //cout << "--- " <<registers[content] << " ---" << endl;
    }else if(is_load_store(content)){
        string addr_str = content.substr(MEMO_PREFIX_LENGTH+MEMO_PARENTHESES_SIZE/2, 
                                        content.length()-MEMO_PREFIX_LENGTH - MEMO_PARENTHESES_SIZE);
        unsigned int destAddr = store_load_address(addr_str);
        buffer = *((unsigned int*)(&STACK[destAddr]));
        //cout << "--- " <<*((unsigned int*)(&STACK[destAddr])) << " ---" << endl;
    }else{
        //cout << "Can't print " << endl;
        printf(ANSI_COLOR_RED PRINT_FAILED ANSI_COLOR_RESET);
        return;
    }   
    switch (bytesNum)
    {
    case CHAR_BYTES: 
        if(!unsign)cout << ".... " << *((char*)(&buffer)) << " .... " << endl;
        else cout << ".... " << *((unsigned char*)(&buffer)) << " .... " << endl;
        break;
    case SHORT_BYTES:
        if(!unsign) cout << ".... " << *((short*)(&buffer)) << " .... " << endl;
        else cout << ".... " << *((unsigned short*)(&buffer)) << " .... " << endl;
        break;
    case WORD_BYTES:
        if(!unsign) cout << ".... " << *((int*)(&buffer)) << " .... " << endl;
        else cout << ".... " << *((unsigned int*)(&buffer)) << " .... " << endl;
        break;
    default:
        break;
    }                        

}
void execute_jump(string jmp_str){
    string hop = jmp_str.substr(JUMP_PREFIX_LENGTH, jmp_str.length() - JUMP_PREFIX_LENGTH);
    if(is_pc_relative(hop)){
        int jmpnum = std::__cxx11::stoi(hop.substr(3, hop.length()-3));
        if(hop[2] == '-') jmpnum *=-1;
        registers[PC] += (jmpnum - WORD_BYTES);
    }else if(is_constant(hop)){
        registers[PC] = (unsigned int)std::__cxx11::stoi(hop);
    }
    if(is_live){
        for(; registers[PC]/WORD_BYTES < instructions.size(); registers[PC]+=WORD_BYTES){
            execute_line(instructions[registers[PC]/WORD_BYTES]);
        }
    }
}
unsigned int bytes_to_copy(string expr){
    size_t found = expr.find(CHAR_CAST);
    if(found != string::npos)return CHAR_BYTES;
    found = expr.find(SHORT_CAST);
    if(found != string::npos)return SHORT_BYTES;
    return WORD_BYTES;
}
string get_path(){
    string result;
    amulator_cl();
    printf("Enter path to the file: ");
    getline(cin, result) ;    
    return result;
}
bool parse_file(ifstream &file){
    string s;
    for(string s; getline(file, s);){
        unspace_line(s);
        uppercase_line(s);
        instructions.push_back(s);
        if(!check_eol(instructions[instructions.size()-1])){
            printf(ANSI_COLOR_RED END_OF_LINE ANSI_COLOR_RESET, (unsigned int)(instructions.size()-1));
            return false;
        }
    }
    return true;
}

bool file_processing(){
    ifstream file;
    while(true){
        string pathToFile = get_path();
        file.open(pathToFile.c_str());
        if(file) break;
        else if(pathToFile == COMMAND_EXIT) return true;
        else printf(ANSI_COLOR_RED PATH_NOT_FOUND ANSI_COLOR_RESET);
    }
    bool res = parse_file(file);
    file.close();
    if(!res){
        printf(ANSI_COLOR_RED FILE_PROCESSING_FAILED ANSI_COLOR_RESET, registers[PC]/WORD_BYTES);
        return res;

    }
    printf(FILE_PARSED);
    for( ;registers[PC]/WORD_BYTES < instructions.size(); registers[PC] += WORD_BYTES){
        if(!execute_line(instructions[registers[PC]/WORD_BYTES])){
            printf(ANSI_COLOR_RED FILE_PROCESSING_FAILED ANSI_COLOR_RESET, registers[PC]/WORD_BYTES);
            return false;
        }
    }
    return true;
}
bool check_eol(string &line){
    if(line[line.length()-1] != COMMAND_ENDS)
    return false;
    line = line.substr(0, line.length()-1);
    //cout << line << " ---- line" << endl;
    if(!is_live)instructions[instructions.size()-1] = line;
    return true;
}
bool live_processing(){    
    string command;
    amulator_live_cl();
    getline(cin, command);
    if(command == COMMAND_EXIT) return false;
    if(command == DISPLAY_MEMORY){
        execute_memory();
        return true;
    }
    if(!check_eol(command)){
        printf(ANSI_COLOR_RED END_OF_LINE ANSI_COLOR_RESET, livelineNum-1);
        return true;
    }
    unspace_line(command);
    uppercase_line(command);
    instructions.push_back(command);
    if(!execute_line(instructions[registers[PC]/WORD_BYTES])){
        instructions.pop_back();
        return true;
    }
    if(!is_reset(command))registers[PC] += WORD_BYTES;

    return true;
}
void execute_memory(){
   // printf();
    printf(ANSI_COLOR_CYAN "-------------displaying memory-------------\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_GREEN"-------------DEFINED FUNCTIONS-------------\n" ANSI_COLOR_RESET);

    for(auto i = functions.begin(); i != functions.end(); i++){
        cout << "Name: " << i->first << " :: " << " Address: " <<(i->second) <<endl;
    }
    printf(ANSI_COLOR_GREEN"-----------------REGISTERS-----------------\n" ANSI_COLOR_RESET);

    for(auto i = registers.begin(); i != registers.end(); i++){
        cout << i->first << " :: " << bitset<sizeof(int)*8> (i->second) <<endl;
    }
    printf(ANSI_COLOR_GREEN "-------------------STACK-------------------\n" ANSI_COLOR_RESET);
    unsigned int used = STACK_SIZE_ - registers[SP];
    int sp = registers[SP];
    printf(ANSI_COLOR_GREEN "SIZE OF STACK:" ANSI_COLOR_RESET " %u BYTES\n", STACK_SIZE_);
    printf(ANSI_COLOR_GREEN "USED MEMORY:" ANSI_COLOR_RESET"   %u BYTES\n" , used);
    for(unsigned int i = STACK_SIZE_ - WORD_BYTES; i >= STACK_SIZE_ - used; i -= WORD_BYTES){
        unsigned int currword = *((unsigned int *)(&STACK[i]));
        printf("M[%X]  :: ", i);
        cout  << bitset<sizeof(int)*8> (currword) <<endl;
    }
}
bool user_input(){
    amulator_cl();
    string s;
    getline(cin, s);
    if(s == COMMAND_GUIDE){
        is_live = false;
        execute_guide();
        return true;
    }else if(s == READ_FROM_FILE){
        is_live = false;
        reset_registers();
        file_processing();
        return true;
    }else if(s == READ_FROM_COMMAND_LINE){
//        printf("-----Live processing-----\n");
        reset_registers();
        livelineNum = 1;
        is_live = true;
        while(live_processing());
        is_live = false;
        return true;
    }else if(s == DISPLAY_MEMORY){
        execute_memory();
        return true;
    }else if(s == COMMAND_EXIT){
        reset_registers();
        return false;
    }else if(s == RESET_CALL){ 
        reset_registers();  
        return true;      
    }else{
        printf(ANSI_COLOR_RED INCORRECT_COMMAND ANSI_COLOR_RESET);
        return true;
    }
}
int contains_alu_oper(string s){
    int index;
    index = s.find('+');
    if(index != string::npos)return index;
    index = s.find('-');
    if(index != string::npos)return index;
    index = s.find('*');
    if(index != string::npos)return index;
    index = s.find('/');
    if(index != string::npos)return index;
    return -1;
}

bool is_alu(string half){
    //return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
    int operPos = contains_alu_oper(half);
    if(operPos == -1 || operPos == 0)return false;

    string lOperand = half.substr(0, operPos);
    string rOperand = half.substr(operPos+1, half.length()-operPos-1);

    return is_constants_alu(lOperand, rOperand) || is_register_constant_alu(lOperand, rOperand) ||
            is_register_constant_alu(rOperand, lOperand) || is_registers_alu(lOperand, rOperand);
}
bool is_jump(string jmp){
    if(jmp.substr(0, JUMP_PREFIX_LENGTH) == JUMP_CALL){
        string suffix = jmp.substr(JUMP_PREFIX_LENGTH, jmp.length()-JUMP_PREFIX_LENGTH);
        return (is_constant(suffix) || is_pc_relative(suffix));
    }
    return false;
}
bool is_branch_prefix(string prefix){
    return (prefix == BLT || prefix == BLE || prefix == BGE || 
            prefix == BGT || prefix == BEQ || prefix == BNE);
}
bool is_pc_relative(string destAddr){
    if(destAddr == PC)return true;
    if(destAddr.substr(0, 2) == PC && (destAddr[2] == '+'|| destAddr[2] == '-')){
        return is_constant(destAddr.substr(3, destAddr.length()-3));
        //return is_alu(destAddr);
    }
    return false;
}
bool is_branch(string br){
    if(br.length() < BRANCH_PREFIX_LENGTH)return false;
    string pefix = br.substr(0, BRANCH_PREFIX_LENGTH);
    if(!is_branch_prefix(pefix))return false;
    else{
        string suffix = br.substr(BRANCH_PREFIX_LENGTH, br.length()-BRANCH_PREFIX_LENGTH);
        size_t firstComma = suffix.find(',');        
        size_t secondComma = suffix.rfind(',');
        if(firstComma == string::npos || secondComma == string::npos || firstComma == secondComma)return false;
        string br1 = suffix.substr(0, firstComma);
        string br2 = suffix.substr(firstComma+1, secondComma - firstComma - 1);
        if(!is_constants_alu(br1, br2) && !is_register_constant_alu(br1, br2) &&
            !is_register_constant_alu(br2, br1) && !is_registers_alu(br1, br2))return false;
        //destination address: absolute constant or pc realtive
        string destinationAddress = suffix.substr(secondComma+1, suffix.length()-secondComma-1);
        return (is_constant(destinationAddress) || is_pc_relative(destinationAddress));
    }
    return false;
}
bool is_constant(string constNum){
    for(int i = 0; i < constNum.length(); i++ ){
        if(!isdigit(constNum[i]))return false;
    }
    return true;
}
bool is_constants_alu(string const1, string const2){
    return is_constant(const1)&& is_constant(const2);
}
bool is_register_constant_alu(string reg1, string const2){
    return (is_register(reg1) && is_constant(const2));
}
bool is_registers_alu(string reg1, string reg2){
    return is_register(reg1) && is_register(reg2);
}
bool is_load_store(string half){
    if(half.length() == 0 || half.length() == 1)return false;
    if(half[0] != MEMO)return false;
    size_t leftpr = half.find('['); //[
    size_t rightpr = half.find(']'); //]
    if(leftpr == string::npos || leftpr!=1 || rightpr != half.length()-1 || rightpr == string::npos)return false;
    string middlepr = half.substr(leftpr+1, rightpr - leftpr-1);
    size_t pluspos = middlepr.find('+');
    if(pluspos != string::npos){
        string reg1 = middlepr.substr(0, pluspos);
        string const2 = middlepr.substr(pluspos+1, middlepr.length()-pluspos-1);
        return (is_register_constant_alu(reg1, const2) || is_register_constant_alu(const2, reg1));
    }else{
        return (is_register(middlepr) || is_constant(middlepr));
    }
}
bool is_register(string reg){ 
    if(reg.length() == 0 || reg.length() == 1)return false;
    if(reg == SP || reg == PC || reg == RV)return true;
    if(reg[0] == REGISTER){
        string checkConst = reg.substr(1, reg.length()-1);
        if(checkConst.length() == 0)return false;
        return is_constant(checkConst); 
    }
    return false; 
}
void welcomeText(){
    printf(BOLDGREEN "                    1    0000000  0     0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));   
    printf(BOLDGREEN "                   1 1   0        01   10                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));  
    printf(BOLDGREEN "                  1   1  0        0 1 1 0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDGREEN "                 1111111 0000000  0  1  0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));  
    printf(BOLDGREEN "                 1     1       0  0     0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDGREEN "                 1     1       0  0     0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDGREEN "                 1     1 0000000  0     0                  \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));

    printf(BOLDBLUE "                         WELCOME!                           \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDBLACK "                   I'm Assembly Emulator                   \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDBLACK "                 Friends call me Asmulator                 \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_WELCOME));
    printf(BOLDBLACK "         There are few tips how to interact with me:        \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));
    printf(BOLDBLACK "If you aren't familiar with assembly language syntax- type \n" ANSI_COLOR_RESET);
    printf(BOLDBLUE "                          guide                             \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));
    printf(BOLDBLACK "If you want to execute file with assembly instructions- type\n" ANSI_COLOR_RESET);
    printf(BOLDBLUE "                            -f                               \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));
    printf(BOLDBLACK "If you want to execute assembly instructions live- type  \n" ANSI_COLOR_RESET);
    printf(BOLDBLUE "                            -l                               \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));
    printf(BOLDBLACK "If you want to display memory you used for program- type  \n" ANSI_COLOR_RESET);
    printf(BOLDBLUE "                            -rg                              \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));
    printf(BOLDBLACK "If you want to exit program or live mode- type       \n" ANSI_COLOR_RESET);
    printf(BOLDBLUE "                            exit                             \n" ANSI_COLOR_RESET);
    this_thread::sleep_for(chrono::milliseconds(TIME_TIPS));

}
void emulate(){
    reset_registers();
    lineNum = 1;
    while(user_input());    
}
void unspace_line(string &str){
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());    
}
void uppercase_line(string &str){
    unsigned long size = str.size();
    for(int i = 0 ; i < size; i++){
        str[i] = toupper(str[i]);
    }
}

bool execute_line(string &str){
    printf(ANSI_COLOR_GREEN "--------------------------------\n" ANSI_COLOR_RESET);
    
    if(!is_live)cout << "   EXECUTING LINE " << (registers[PC]/WORD_BYTES) + 1 <<": " << str << endl;
    else cout << "   EXECUTING LINE " << livelineNum - 1 <<": " << str << endl;

    int assignmentIndex = contains_assignment(str);
    int type = OTHER;
    cout << "...DETECTING TYPE..."<<endl;
    this_thread::sleep_for(chrono::milliseconds(TIME_DETECT_TYPE));

    if(assignmentIndex != -1){
        string lvalue = str.substr(0, assignmentIndex);
        string rvalue = str.substr(assignmentIndex+1, str.size()-1 - assignmentIndex);
        type = check_type(lvalue, rvalue, str);      
    }else{
        type = check_type("", "", str);
    }    
    if(type == OTHER){
        printf(ANSI_COLOR_RED TYPE_NOT_FOUND ANSI_COLOR_RESET);
        return false;
    }
    //printf("Type: %d\n", type);
    //cout << str << " :";
    if(type == ALU){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "ALU\n" );
        execute_alu(str);
    }else if(type == LOAD){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "LOAD\n" );
        execute_load(str);
    }else if(type == STORE){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "STORE\n" );
        execute_store(str);
    }else if(type == ASSIGNMENT){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "ALU\n" );
        execute_assignment(str);
    }
    else if(type == BRANCH){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "BRANCH\n" );
        execute_branch(str);
    }else if(type == JUMP){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "JUMP\n" );
        execute_jump(str);
    }else if(type == CALL){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "FUNCTION CALL\n" );
        execute_func_call(str);
    }else if(type == RET){//
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "RETURN\n" );
        execute_return();
    }else if(type == DEFINE){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "FUNCTION DEFINITION\n" );
        execute_func_def(str);
    }else if(type == RESET){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "RESETTING PROGRAM\n" );
        reset_registers();
    }else if(type == PRINT){
        printf(ANSI_COLOR_GREEN "TYPE DETECTED: " ANSI_COLOR_RESET "PRINT\n" );
        execute_print(str);
    }
    return true;
}

void reset_registers(){
    registers.clear();
    instructions.clear();
    functions.clear();
    registers.insert(make_pair<string,  int>(SP, STACK_SIZE_));
    registers.insert(make_pair<string,  int>(PC, 0));
}
int contains_assignment(string s){
    unsigned int size = s.size(); 
    for(int i = 0; i < size; i++){
        if(s[i] == '=')return i;        
    }
    return -1;
}
bool is_return(string ret){
    return (ret == RETURN);
}
bool is_define(string def){
    return (def.substr(0, DEFINE_PREFIX_LENGTH) == DEFINE_CALL);
            //&& def[DEFINE_PREFIX_LENGTH] == '<' && def[def.length()-1] == '<');
    
}
bool is_enddef(string enddef){
    return (enddef == END_DEF_CALL);
}
bool is_function_legal(string fun){
    if(functions.find(fun.substr(FUNCTION_PARENTHESES_SIZE/2, fun.length()-FUNCTION_PARENTHESES_SIZE)) 
            == functions.end()){
        printf(ANSI_COLOR_RED NO_FUNCTION ANSI_COLOR_RESET);
        return false;
    }
    return (fun[0] == '<' && fun[fun.length()-1] == '>'); 
}
bool is_function_call(string funcal){
    return (funcal.substr(0, CALL_PREFIX_LENGTH) == FUNCTION_CALL && 
            (is_function_legal(funcal.substr(CALL_PREFIX_LENGTH, funcal.length()-CALL_PREFIX_LENGTH)))); 
         // ||   is_register() || is_constant() ???????
}
bool is_reset(string reset_str){
    return (reset_str == RESET_CALL);
}
bool is_print(string pr){
    return (pr.substr(0, PRINT_PREFIX_LENGTH) == PRINT_CALL );
           // && pr[PRINT_PREFIX_LENGTH] == '<' && pr[pr.length()-1] == '<');
}
