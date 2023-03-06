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
using namespace std;
void fillLeftPart();
void fillRightPart();
int wordAnalysis(int begin, int end); // 词法分析程序。
array<char, 1024> buffer;
int beginPtr, froPtr = 0;
ifstream readFileStream;
ofstream outputFileStream;
/// @brief 
/// @return 
int main(){
    buffer[511] = 26;
    buffer[1023] = 26;
    readFileStream.open("test.txt", ifstream::in);
    outputFileStream.open("out.txt", ofstream::out);
    fillLeftPart();
    beginPtr = froPtr = 0;
    //开始遍历.
    int st = 0;
    string str;
    while(1){
        char curCh = buffer[froPtr];
        st = 0;
        if(curCh == 26){
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

        if(curCh == ' ' || curCh == '\n'){
            outputFileStream << str;
            beginPtr = froPtr + 1;
            str = "";
            outputFileStream << curCh;
            froPtr++;
            beginPtr = froPtr;
            
        }
        else{
            str += curCh;
            froPtr++;
        }
        // st = wordAnalysis(beginPtr, froPtr);
        // if(st == 1){
        //     beginPtr = froPtr + 1;
        // }
        // else if(st == 2){ // 回退操作,可能不正确，待进一步商榷。
        //     beginPtr = froPtr;
        //     froPtr--;
        // }
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