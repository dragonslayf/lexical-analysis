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
using namespace std;
void fillLeftPart();
void fillRightPart();
int wordAnalysis(int begin, int end); // �ʷ���������
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
    //��ʼ����.
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
                outputFileStream << "\nԴ����ʷ�������ϣ�" << endl;
                return 0;
                //������������ͳ����Ϣ��
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
        // else if(st == 2){ // ���˲���,���ܲ���ȷ������һ����ȶ��
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