#include<iostream>
#include<vector>
#include"BigInteger.h"

using namespace std;

// 1111222233334444 integer[1]=11112222 integer[0]=33334444
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

    BigInteger operator=(const string &num) { // 重载赋值运算符，从字符串类型转大整数存储
        integer.clear();
        int length = num.size();
        segments = (length - 1) / width + 1; // 这里计算段数的方法十分巧妙
        for (int i = 0; i < segments; i++) {
            int end = length - i * width; // 计算每段结束的地方，从最后一段开始
            int start = max(0, end - width); // 计算每段开始的地方
            integer.emplace_back(stoi(num.substr(start, end - start))); // 字符串转整数
        }
        return *this;
    }

    explicit BigInteger(long long num = 0) {
        *this = num;
    }

    ostream &operator<<(ostream &out) { // 重载输出运算符，vector倒着输出
        for (auto it = integer.rbegin(); it != integer.rend(); it++) {
            cout << *it;
        }
        return out;
    }

    istream &operator>>(istream &in) { // 重载输入运算符，当成字符串输入，用重载的赋值运算符直接赋值
        string num;
        in >> num;
        *this = num;
        return in;
    }

    bool operator<(const BigInteger &bigInteger) const {
        if (segments != bigInteger.segments)
            return segments < bigInteger.segments;
        for (int i = segments - 1; i >= 0; i--) { // 倒着比较，因为低位在vector的前面，先比较高位
            if (integer[i] != bigInteger.integer[i])
                return integer[i] < bigInteger.integer[i];
        }
        return false; // 相等
    }

    bool operator>(const BigInteger &bigInteger) const {
        return bigInteger < *this; // a大于b等价于b小于a
    }

    bool operator<=(const BigInteger &bigInteger) const {
        return !(bigInteger < *this); // a<=b等价于b不小于a
    }

    bool operator>=(const BigInteger &bigInteger) const {
        return !(*this < bigInteger); // a>=b等价于a不小于b
    }

    bool operator!=(const BigInteger &bigInteger) const {
        return *this < bigInteger || bigInteger < *this; // a不等于b等价于a大于b或者小于b
    }

    bool operator==(const BigInteger &bigInteger) const {
        return !(*this < bigInteger) && !(bigInteger < *this); // a等于b等价于a既不大于b也不小于b
    }

    BigInteger operator+(const BigInteger &bigInteger) const {
        BigInteger result;
        for (int i = 0, carry = 0; carry || i < segments || i < bigInteger.segments; i++) {
            if (i < segments)
                carry += integer[i];
            if (i < bigInteger.segments)
                carry += bigInteger.integer[i];
            result.integer.emplace_back(carry % base);
            result.segments++;
            carry = carry / base;
        }
        return result;
    }
};
