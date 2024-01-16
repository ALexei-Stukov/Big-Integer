#include<iostream>
#include<vector>
#include"BigInteger.h"
using namespace std;

class BigInteger {
    const int width = 8; // 切分段的长度
    const int base = 1e8; // 切分段的数量级
    vector<int> integer; // 存储各个段
    int segments = 0; // 切分的段数
    BigInteger operator=(long long num) { // 重载赋值运算符，从基本数据类型转换大整数存储
        integer.clear();
        do {
            integer.emplace_back(num % base);
            num /= base;
            segments++;
        } while (num > 0);
        return *this;
    }

    BigInteger operator=(const string &num) {
        integer.clear();
        int length=num.size();
        segments = (length- 1) / width + 1; // 这里计算段数的方法十分巧妙
        for(int i=0;i<segments;i++){
            int end=length-i*width; // 计算每段结束的地方，从最后一段开始
            int start=max(0,end-width); // 计算每段开始的地方
            integer.emplace_back(stoi(num.substr(start,end -start))); // 字符串转整数
        }
        return *this;
    }

    explicit BigInteger(long long num = 0) {
        *this = num;
    }
};
