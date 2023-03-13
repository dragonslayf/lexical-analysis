//��дC���Դʷ�����Դ����Ҫ��

//����ʶ�����c���Ա�д��Դ�����е�ÿ�����ʷ��ţ����ԼǺŵ���ʽ���ÿ�����ʷ��š�

//����ʶ������Դ�����е�ע��

//����ͳ�Ƶ��ʡ��ַ������ĸ����������ͳ�ƽ����

//���Լ���Դ�����г��ֵ��﷨���󣬲����������ֵ�λ�á�

//��Դ�����г��ֵĴ�����лָ���ʹ�ôʷ��������Լ������У���ֻ��һ��ɨ�輴��������пɱ����ֵĴ���

#include<iostream>
#include<fstream>
#include<array>
#include<string>
#include<map>
using namespace std;

array<char, 1024> buffer;// ������ǰ���Ļ�����
int beginPtr, froPtr = 0;
ifstream readFileStream;
ofstream outputFileStream;
array<string, 33> reserved;//����c���ԵĹؼ���
array<char, 52>letters;//����52����Сд��ĸ
array<char, 62>letterNs; //�����Сд��ĸ��ʮ�����֡�
array<bool, 300>allowTraceBack; // ��Ϊ�棬����������ַ�������ˣ�����������ִ�����Ϊ�٣���ת������������
array<char, 8>punctuations;
enum State{ // �Զ�״̬����״̬��
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
map<State, string> tokenTable; // ��Ӧ�ǺŴ�ӡ�����



void fillLeftPart();
void fillRightPart();
int wordAnalysis(int begin, int end); // �ʷ���������
void DFAInitialize();  //��ʼ��DFA״̬����
void continualTextIdentifier(State stat); // ���ڼ�������ı����磺�ַ�������ע�ͺ�ע�Ϳ�ȵȡ� 


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
    //��ʼ����.
    State st = start;
    string str;
    
    while(1){
        if(st == doubleQuoteStat0 || st == lineCommentStat0 || st == blockCommentStat0){
            continualTextIdentifier(st);
            str = "";
            st = start;
        }
        char curCh = buffer[froPtr];
        if(curCh == 26){ // 26 ��EOF��ֹ��
            if(froPtr == 511){
                fillRightPart();
                froPtr++;
            }
            else if(froPtr == 1023){
                fillLeftPart();
                froPtr = 0;
            }
            else{
                outputFileStream << "\nԴ����ʷ�������ϣ�" << endl;
                return 0;
                //������������ͳ����Ϣ��
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
                beginPtr = froPtr; // ���ﲻ��froPtr���൱�ڻ��˲�����
                st = start;
            }
        }
        else{
            if(curCh == ' ' || curCh == '\n'){//�������еȣ�ֱ�������
                outputFileStream << curCh;
                froPtr++;
                beginPtr = froPtr;
            }
            else{
                outputFileStream << "undefined##"; // �����һ����������ơ�
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
        buffer[i] = 26; // ���������������ֹ��.
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
    //�ʷ����������buffer������в�����ȡ�ַ�ʱ���±��1024ȡ�ࡣ

    //�����ո�����ո���������������У�����EOF��ascii��Ϊ26�����ԡ�

    //ʶ��ɹ����ӡ��Ӧ�ļǺű�ʶ������1��

    //ʶ��ʧ�ܷ���0

    //�������״̬��Ҫ���о���ͻָ���
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

    //��ʼ����ĸ�����ĸ���ֱ�
    for(ch = 'a'; ch <= 'z'; ch++){
       letters[i] = letterNs[i++] = ch;
    }
    for(ch = 'A'; ch <= 'Z'; ch++){
        letters[i] = letterNs[i++] = ch;   
    }
    for(ch = '0'; ch <= '9'; ch++){
        letterNs[i++] = ch;
    }

    //ʶ���ʶ��
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
        int stat = 0; // ��0״̬�£����յ�����һ��˫���Ž���ֱ�ӽ������ַ�����0״̬�½��յ�ת���ַ���/�������1״̬��1״̬����ת��һ��˫���ţ����ص�0״̬��
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == 26){ // 26 ��EOF��ֹ��
                if(froPtr == 511){
                    fillRightPart();
                    froPtr++;
                }
                else if(froPtr == 1023){
                    fillLeftPart();
                    froPtr = 0;
                }
                else{ // ��ô����һ������ֱ�ӵ�����β�������
                    outputFileStream << "\nԴ����ʷ�������ϣ�" << endl;
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
            if(curCh == 26){ // 26 ��EOF��ֹ��
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
        int stat = 0; // �����ע�ͣ�Ĭ��Ϊ0״̬�����ա�*����ת��Ϊ1״̬��1״̬�½���'/'������ע�ͣ����������ַ�ת��Ϊ0״̬��
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == 26){ // 26 ��EOF��ֹ��
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