// by Xie Tianyang 

//��C���Դʷ���������,�����ɺ� INPUT_FILE���壬����ļ���OUTPUT_FILE����.

//��������Ľ�������OUTPUT_FILE�У���������Ϣ��ͳ����Ϣ���������̨�С�

#include<iostream>
#include<fstream>
#include<array>
#include<string>
#include<map>
using namespace std;

#define INPUT_FILE "test.txt"
#define OUTPUT_FILE "out.txt"

array<char, 1024> buffer;// ������ǰ���Ļ�����
int beginPtr, froPtr = 0;
ifstream readFileStream;
ofstream outputFileStream;
array<string, 33> reserved;//����c���ԵĹؼ���
array<char, 52>letters;//����52����Сд��ĸ
array<char, 62>letterNs; //�����Сд��ĸ��ʮ�����֡�
array<bool, 300>allowTraceBack; // ��Ϊ�棬����������ַ�������ˣ�����������ִ�����Ϊ�٣���ת������������
array<char, 8>punctuations;
int rowCnt, charCnt;
int rowCharCnt;
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
    doubleQuoteStat0 = 13,

    plusStat0 = 14,
    minusStat0 = 15,
    multiStat0 = 16,
    divisionStat0 = 17,
    modStat0 = 18,//%
    greaterStat0 = 19,//>
    lessStat0 = 20,//<
    andStat0 = 21,//&
    orStat0 = 22,// |
    notStat0 = 23,//��
    equalStat0 = 24,//�Ⱥ�
    questionStat0 = 25,//�ʺ�
    colonStat0 = 26,//ð��
    classAccessStat0 = 27,//.
    xorStat0 = 28,
    //���������¶�Ӧ�󲿷ֵ�Ŀ���������ϵȺŵĸ��ϲ�����
    plusStat1 = 29,
    minusStat1 = 30,
    multiStat1 = 31,
    divisionStat1 = 32,
    modStat1 = 33,//%
    greaterStat1 = 34,//>
    lessStat1 = 35,//<
    andStat1 = 36,//&
    orStat1 = 37,// |
    notStat1 = 38,//��
    equalStat1 = 39,//�Ⱥ�
    xorStat1 = 40,
    leftShiftStat0 = 41,
    rightShiftStat0 = 42,
    incrementStat0 = 43,
    decrementStat0 = 44,
    andStat2 = 45,
    orStat2 = 46,
    p2mStat0 = 47 // ->��������pointer-2-member.

};
map<pair<State, char>, State> dfaTable;
map<State, string> tokenTable; // ��Ӧ�ǺŴ�ӡ�����
map<string, int>tokenCnt; // ͳ�Ƹ����Ǻŵ�����������ӡ��


void fillLeftPart();
void fillRightPart();
void DFAInitialize();  //��ʼ��DFA״̬����
void continualTextIdentifier(State stat); // ���ڼ�������ı����磺�ַ�������ע�ͺ�ע�Ϳ�ȵȡ� 


/// @brief 
/// @return 
int main(){
    tokenCnt["reserved"] = 0;
    tokenCnt["identifier"] = 0;
    tokenCnt["number"] = 0;
    tokenCnt["punctuation"] = 0;
    tokenCnt["operator"] = 0;
    tokenCnt["string"] = 0;
    punctuations = {'(', ')', ',', '[', ']', '{', '}', ';'};
    reserved = {"auto", "else", "long", "switch", "break", "enum", "register", "typedef", "case", "extern", "return", "union", "char", "float", "short", "unsigned", "const", "for", "signed", "void", "continue", "goto", "sizeof", "volatile", "default", "if", "static", "while", "do", "int", "struct", "_Packed", "double"};
    buffer[511] = 26;
    buffer[1023] = 26;
    DFAInitialize();
    readFileStream.open(INPUT_FILE, ifstream::in);
    outputFileStream.open(OUTPUT_FILE, ofstream::out);
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
        charCnt++;
        rowCharCnt++;
        if(curCh == 26 ){ // 26 ��EOF��ֹ��
            if(froPtr == 511){
                fillRightPart();
                froPtr++;
            }
            else if(froPtr == 1023){
                fillLeftPart();
                froPtr = 0;
            }
            else{
                cout << "\nԴ����ʷ�������ϣ�" << endl;
                //������������ͳ����Ϣ��
                for(auto i = tokenCnt.begin(); i != tokenCnt.end(); i++){
                    cout << i->first << ": " << i->second << endl;
                }
                cout << "total chars: " << charCnt << endl; 
                return 0;
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
                        tokenCnt["reserved"]++;
                        break;
                    }
                }
                if(!isReserve){
                    outputFileStream << "identifier##";
                    tokenCnt["identifier"]++;
                }
                str = "";
                beginPtr = froPtr;
                st = start;
            }
            else{
                tokenCnt[tokenTable[st]]++;
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
                if(curCh == '\n'){
                    rowCnt++;
                    rowCharCnt = 0;
                }
            }
            else{
                outputFileStream << "undefined##"; // �����һ����������ơ�
                printf("error at row: %d, line: %d\n", rowCnt, rowCharCnt);
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
    tokenTable[digitStat2] = "number";
    tokenTable[digitStat5] = "number";

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
    tokenTable[blockCommentStat0] = "comments";
    allowTraceBack[slashStat0] = true;
    allowTraceBack[slashEqualStat0] = true;
    
    dfaTable[pair<State, char>(start, '"')] = doubleQuoteStat0;
    dfaTable[pair<State, char>(start, '\'')] = doubleQuoteStat0;

    dfaTable[pair<State, char>(start, '+')] = plusStat0;
    dfaTable[pair<State, char>(plusStat0, '+')] = incrementStat0;
    dfaTable[pair<State, char>(plusStat0, '=')] = plusStat1;
    allowTraceBack[plusStat0] = true;
    allowTraceBack[plusStat1] = true;
    allowTraceBack[incrementStat0] = true;
    tokenTable[plusStat0] = "operator";
    tokenTable[plusStat1] = "operator";
    tokenTable[incrementStat0] = "operator";

    dfaTable[pair<State, char>(start, '-')] = minusStat0;
    dfaTable[pair<State, char>(minusStat0, '-')] = decrementStat0;
    dfaTable[pair<State, char>(minusStat0, '=')] = minusStat1;
    dfaTable[pair<State, char>(minusStat0, '>')] = p2mStat0;
    allowTraceBack[minusStat0] = true;
    allowTraceBack[minusStat1] = true;
    allowTraceBack[p2mStat0] = true;
    allowTraceBack[decrementStat0] = true;
    tokenTable[minusStat0] = "operator";
    tokenTable[minusStat1] = "operator";
    tokenTable[decrementStat0] = "operator";
    tokenTable[p2mStat0] = "operator";

    dfaTable[pair<State, char>(start, '*')] = multiStat0;
    dfaTable[pair<State, char>(multiStat0, '=')] = multiStat1;
    allowTraceBack[multiStat0] = true;
    allowTraceBack[multiStat1] = true;
    tokenTable[multiStat0] = "operator";
    tokenTable[multiStat1] = "operator";

    dfaTable[pair<State, char>(start, '>')] = greaterStat0;
    dfaTable[pair<State, char>(greaterStat0, '=')] = greaterStat1;
    dfaTable[pair<State, char>(greaterStat0, '>')] = rightShiftStat0;
    allowTraceBack[greaterStat0] = true;
    allowTraceBack[greaterStat1] = true;
    allowTraceBack[rightShiftStat0] = true;
    tokenTable[greaterStat0] = "operator";
    tokenTable[greaterStat1] = "operator";
    tokenTable[rightShiftStat0] = "operator";

    dfaTable[pair<State, char>(start, '<')] = lessStat0;
    dfaTable[pair<State, char>(lessStat0, '=')] = lessStat1;
    dfaTable[pair<State, char>(lessStat0, '<')] = leftShiftStat0;
    allowTraceBack[lessStat0] = true;
    allowTraceBack[lessStat1] = true;
    allowTraceBack[leftShiftStat0] = true;
    tokenTable[lessStat0] = "operator";
    tokenTable[lessStat1] = "operator";
    tokenTable[leftShiftStat0] = "operator";

    dfaTable[pair<State, char>(start, '&')] = andStat0;
    dfaTable[pair<State, char>(andStat0, '=')] = andStat1;
    dfaTable[pair<State, char>(andStat0, '&')] = andStat2;
    allowTraceBack[andStat0] = true;
    allowTraceBack[andStat1] = true;
    allowTraceBack[andStat2] = true;
    tokenTable[andStat0] = "operator";
    tokenTable[andStat1] = "operator";
    tokenTable[andStat2] = "operator";

    dfaTable[pair<State, char>(start, '|')] = orStat0;
    dfaTable[pair<State, char>(orStat0, '=')] = orStat1;
    dfaTable[pair<State, char>(orStat0, '|')] = orStat2;
    allowTraceBack[orStat0] = true;
    allowTraceBack[orStat1] = true;
    allowTraceBack[orStat2] = true;
    tokenTable[orStat0] = "operator";
    tokenTable[orStat1] = "operator";
    tokenTable[orStat2] = "operator";

    dfaTable[pair<State, char>(start, '^')] = xorStat0;
    dfaTable[pair<State, char>(xorStat0, '=')] = xorStat1;
    allowTraceBack[xorStat0] = true;
    allowTraceBack[xorStat1] = true;
    tokenTable[xorStat0] = "operator";
    tokenTable[xorStat1] = "operator";

    dfaTable[pair<State, char>(start, '!')] = notStat0;
    dfaTable[pair<State, char>(notStat0, '=')] = notStat1;
    allowTraceBack[notStat0] = true;
    allowTraceBack[notStat1] = true;
    tokenTable[notStat0] = "operator";
    tokenTable[notStat1] = "operator";

    dfaTable[pair<State, char>(start, '?')] = questionStat0;
    dfaTable[pair<State, char>(start, ':')] = colonStat0;
    dfaTable[pair<State, char>(start, '.')] = classAccessStat0;
    allowTraceBack[questionStat0] = true;
    allowTraceBack[colonStat0] = true;
    allowTraceBack[classAccessStat0] = true;
    tokenTable[questionStat0] = "operator";
    tokenTable[colonStat0] = "operator";
    tokenTable[classAccessStat0] = "operator";

    dfaTable[pair<State, char>(start, '%')] = modStat0;
    dfaTable[pair<State, char>(modStat0, '=')] = modStat1;
    allowTraceBack[modStat0] = true;
    allowTraceBack[modStat1] = true;
    tokenTable[modStat0] = "operator";
    tokenTable[modStat1] = "operator";

    dfaTable[pair<State, char>(start, '=')] = equalStat0;
    dfaTable[pair<State, char>(equalStat0, '=')] = equalStat1;
    allowTraceBack[equalStat0] = true;
    allowTraceBack[equalStat1] = true;
    tokenTable[equalStat0] = "operator";
    tokenTable[equalStat1] = "operator";
}

void continualTextIdentifier(State stat){
    if(stat == doubleQuoteStat0){
        tokenCnt["string"]++;
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
                    cout << "\nԴ����ʷ�������ϣ�" << endl;
                    return;
                }
                curCh = buffer[froPtr];
            }
            froPtr++;
            if(stat == 0){
                if(curCh == '"' || curCh == '\''){
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
                rowCharCnt = 0;
                rowCnt++;
                outputFileStream << "rowComments##" << endl;
                break;
            }
        }
    
    }
    else{
        int stat = 0; // �����ע�ͣ�Ĭ��Ϊ0״̬�����ա�*����ת��Ϊ1״̬��1״̬�½���'/'������ע�ͣ����������ַ�ת��Ϊ0״̬��
        while(1){
            char curCh = buffer[froPtr];
            if(curCh == '\n'){rowCnt++; rowCharCnt = 0;}
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