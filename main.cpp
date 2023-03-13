//编写C语言词法分析源程序，要求：

//可以识别出用c语言编写的源程序中的每个单词符号，并以记号的形式输出每个单词符号。

//可以识别并跳过源程序中的注释

//可以统计单词、字符、语句的个数，并输出统计结果。

//可以检查出源程序中出现的语法错误，并报告错误出现的位置。

//对源程序中出现的错误进行恢复，使得词法分析可以继续进行，且只需一遍扫描即可输出所有可悲发现的错误。

#include<iostream>
#include<fstream>
#include<array>
#include<string>
#include<map>
using namespace std;

array<char, 1024> buffer;// 用于提前读的缓冲区
int beginPtr, froPtr = 0;
ifstream readFileStream;
ofstream outputFileStream;
array<string, 33> reserved;//保存c语言的关键字
array<char, 52>letters;//保存52个大小写字母
array<char, 62>letterNs; //保存大小写字母和十个数字。
array<bool, 300>allowTraceBack; // 若为真，则出现其他字符允许回退，并不代表出现错误。若为假，则转到错误处理函数。
array<char, 8>punctuations;
enum State{ // 自动状态机的状态。
    start = 0,
    kwdStat0 = 1,
    digitStat0 = 2,
    digitStat1 = 3,
    digitStat2 = 4,
    digitStat3 = 5,
    digitStat4 = 6,
    digitStat5 = 7,
    punctuationStat0 = 8,
    slashStat0 = 9,
    slashEqualStat0 = 10,
    lineCommentStat0 = 11,
    blockCommentStat0 = 12,
    doubleQuoteStat0 = 13
};
map<pair<State, char>, State> dfaTable;
map<State, string> tokenTable; // 对应记号打印结果。



void fillLeftPart();
void fillRightPart();
int wordAnalysis(int begin, int end); // 词法分析程序。
void DFAInitialize();  //初始化DFA状态机。
void continualTextIdentifier(State stat); // 用于检测连续文本，如：字符串，行注释和注释块等等。 


/// @brief 
/// @return 
int main(){
    punctuations = {'(', ')', ',', '[', ']', '{', '}', ';'};
    reserved = {"auto", "else", "long", "switch", "break", "enum", "register", "typedef", "case", "extern", "return", "union", "char", "float", "short", "unsigned", "const", "for", "signed", "void", "continue", "goto", "sizeof", "volatile", "default", "if", "static", "while", "do", "int", "struct", "_Packed", "double"};
    buffer[511] = 26;
    buffer[1023] = 26;
    DFAInitialize();
    readFileStream.open("test.txt", ifstream::in);
    outputFileStream.open("out.txt", ofstream::out);
    fillLeftPart();
    beginPtr = froPtr = 0;
    //开始遍历.
    State st = start;
    string str;
    
    while(1){
        if(st == doubleQuoteStat0 || st == lineCommentStat0 || st == blockCommentStat0){
            continualTextIdentifier(st);
            str = "";
            st = start;
        }
        char curCh = buffer[froPtr];
        if(curCh == 26){ // 26 是EOF终止符
            if(froPtr == 511){
                fillRightPart();
                froPtr++;
            }
            else if(froPtr == 1023){
                fillLeftPart();
                froPtr = 0;
            }
            else{
                outputFileStream << "\n源程序词法分析完毕！" << endl;
                return 0;
                //在这里可以添加统计信息。
            }
            curCh = buffer[froPtr];
        }
        auto iter = dfaTable.find(pair<State, char>(st, curCh));
        if(iter != dfaTable.end()){
            st = iter->second;
            str += curCh;
            froPtr++;
        }
        else if(allowTraceBack[st]){
            if(st == kwdStat0){
                bool isReserve = false;
                for(int i = 0; i < 33; i++){
                    if(str == reserved[i]){
                        isReserve = true;
                        outputFileStream << "reserved##";
                        break;
                    }
                }
                if(!isReserve){
                    outputFileStream << "identifier##";
                }
                str = "";
                beginPtr = froPtr;
                st = start;
            }
            else{
                outputFileStream << tokenTable[st] << "##";
                str = "";
                beginPtr = froPtr; // 这里不加froPtr，相当于回退操作。
                st = start;
            }
        }
        else{
            if(curCh == ' ' || curCh == '\n'){//遇到换行等，直接输出。
                outputFileStream << curCh;
                froPtr++;
                beginPtr = froPtr;
            }
            else{
                outputFileStream << "undefined##"; // 这里加一个错误处理机制。
                str = "";
                froPtr++;
                beginPtr = froPtr;
            }
        }
    }
    readFileStream.close();
    outputFileStream.close();
}

void fillLeftPart(){
    char c;
    int i = 0;
    while(readFileStream.good() && i < 511){
        c = readFileStream.get();
        buffer[i++] = c;
    }
    if(i < 511){
        buffer[i] = 26; // 程序结束，加上终止符.
    }
}

void fillRightPart(){
    char c;
    int i = 512;
    while(readFileStream.good() && i < 1023){
        c = readFileStream.get();
        buffer[i++] = c;
    }
    if(i < 1023){
        buffer[i] = 26;
    }
}

int wordAnalysis(int begin, int end){
    //词法分析程序对buffer数组进行操作，取字符时，下标对1024取余。

    //遇到空格输出空格，遇到换行输出换行，遇到EOF（ascii码为26）忽略。

    //识别成功后打印对应的记号标识并返回1。

    //识别失败返回0

    //进入错误状态后要进行警告和恢复。
    int st = 0;
    if(end < begin){end = end + 1024;}
    int index;
    char ch;
    for(int i = begin; i <= end; i++){
        index = i % 1024;
        ch = buffer[index];
        if(ch != 26){
            if(ch == ' '){st = 1; outputFileStream << " ";}
            outputFileStream << ch;
        }
    }
    return st;
}

void DFAInitialize(){
    char ch;
    int i = 0;

    //初始化字母表和字母数字表。
    for(ch = 'a'; ch <= 'z'; ch++){
       letters[i] = letterNs[i++] = ch;
    }
    for(ch = 'A'; ch <= 'Z'; ch++){
        letters[i] = letterNs[i++] = ch;   
    }
    for(ch = '0'; ch <= '9'; ch++){
        letterNs[i++] = ch;
    }

    //识别标识符
    for(int i = 0; i < letters.size(); i++){
        dfaTable[pair<State, char>(start, letters[i])] = kwdStat0;
    }

    dfaTable[pair<State, char>(start, '_')] = kwdStat0;

    for(int i = 0; i < letterNs.size(); i++){
        dfaTable[pair<State, char>(kwdStat0, letterNs[i])] = kwdStat0;
    }
    allowTraceBack[kwdStat0] = true;
    tokenTable[kwdStat0] = "identifier";
    dfaTable[pair<State, char>(kwdStat0, '_')] = kwdStat0;

    for(int i = 52; i < 62; i++){
        dfaTable[pair<State, char>(start, letterNs[i])] = digitStat0;
        dfaTable[pair<State, char>(digitStat0, letterNs[i])] = digitStat0;
        dfaTable[pair<State, char>(digitStat2, letterNs[i])] = digitStat2;
        dfaTable[pair<State, char>(digitStat1, letterNs[i])] = digitStat2;
        dfaTable[pair<State, char>(digitStat3, letterNs[i])] = digitStat5;
        dfaTable[pair<State, char>(digitStat4, letterNs[i])] = digitStat5;
        dfaTable[pair<State, char>(digitStat5, letterNs[i])] = digitStat5;
    }
    dfaTable[pair<State, char>(digitStat0, 'e')] = digitStat3;
    dfaTable[pair<State, char>(digitStat0, 'E')] = digitStat3;
    dfaTable[pair<State, char>(digitStat0, '.')] = digitStat1;
    dfaTable[pair<State, char>(digitStat2, 'e')] = digitStat3;
    dfaTable[pair<State, char>(digitStat2, 'E')] = digitStat3;
    dfaTable[pair<State, char>(digitStat3, '+')] = digitStat4;
    dfaTable[pair<State, char>(digitStat3, '-')] = digitStat4;
    allowTraceBack[digitStat0] = true;
    allowTraceBack[digitStat2] = true;
    allowTraceBack[digitStat5] = true;
    tokenTable[digitStat0] = "number";
    tokenTable[digitStat2] = "fraction number";
    tokenTable[digitStat5] = "exponential number";

    for(int i = 0; i < 8; i++){
        dfaTable[pair<State, char>(start, punctuations[i])] = punctuationStat0;
    }
    tokenTable[punctuationStat0] = "punctuation";
    allowTraceBack[punctuationStat0] = true;

    dfaTable[pair<State, char>(start, '/')] = slashStat0;
    dfaTable[pair<State, char>(slashStat0, '=')] = slashEqualStat0;
    dfaTable[pair<State, char>(slashStat0, '/')] = lineCommentStat0;
    dfaTable[pair<State, char>(slashStat0, '*')] = blockCommentStat0;
    tokenTable[slashStat0] = "operation";
    tokenTable[slashEqualStat0] = "operation";
    tokenTable[lineCommentStat0] = "comments";
    tokenTable[blockCommentStat0] = 'comments';
    allowTraceBack[slashStat0] = true;
    allowTraceBack[slashEqualStat0] = true;
    
    dfaTable[pair<State, char>(start, '"')] = doubleQuoteStat0;

}

void continualTextIdentifier(State stat){
    if(stat == doubleQuoteStat0){
        int stat = 0; // 在0状态下，接收到另外一个双引号将会直接结束该字符串。0状态下接收到转义字符’/‘会进入1状态，1状态可以转义一个双引号，随后回到0状态。
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == 26){ // 26 是EOF终止符
                if(froPtr == 511){
                    fillRightPart();
                    froPtr++;
                }
                else if(froPtr == 1023){
                    fillLeftPart();
                    froPtr = 0;
                }
                else{ // 怎么处理一个引号直接到程序尾的情况？
                    outputFileStream << "\n源程序词法分析完毕！" << endl;
                    return;
                }
                curCh = buffer[froPtr];
            }
            froPtr++;
            if(stat == 0){
                if(curCh == '"'){
                    outputFileStream << "string##";
                    break;
                }
                else if(curCh == '/'){
                    stat = 1;
                }
            }
            else{
                stat = 0;
            }
        }
    }
    else if(stat == lineCommentStat0){
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == 26){ // 26 是EOF终止符
                if(froPtr == 511){
                    fillRightPart();
                    froPtr++;
                }
                else if(froPtr == 1023){
                    fillLeftPart();
                    froPtr = 0;
                }
                curCh = buffer[froPtr];
            }
            froPtr++;
            if(curCh == '\n'){
                outputFileStream << "rowComments##" << endl;
                break;
            }
        }
    
    }
    else{
        int stat = 0; // 处理块注释，默认为0状态，接收‘*’后转换为1状态，1状态下接收'/'结束块注释，接收其他字符转换为0状态。
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == 26){ // 26 是EOF终止符
                if(froPtr == 511){
                    fillRightPart();
                    froPtr++;
                }
                else if(froPtr == 1023){
                    fillLeftPart();
                    froPtr = 0;
                }
                curCh = buffer[froPtr];
            }
            froPtr++;
            if(stat == 0){
                if(curCh == '*'){
                    stat = 1;
                }
            }
            else{
                if(curCh == '/'){
                    outputFileStream << "blockComments##";
                    break;
                }
                stat = 0;
            }
        }
    }
}