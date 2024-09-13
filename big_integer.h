#include<iostream>
#include<vector>
#include<iomanip>

using namespace std;

//应该以这种方式来看待这个大整数类：
//  [3][2][1][0]: vector
//  越高的下标，对应着越大的数值
//  左移: [3][2][1][0][新元素]: vector
//  右移: [3][2][1]: vector     [0]被移除
// 1111222233334444 integer[1]=11112222 integer[0]=33334444
// 默认构造函数会产生+0
class BigInteger {
    static const int width = 4; // 切分段的长度
    static const int base = 1e4; // 切分段的数量级
                                /*
                                    最初这两个变量是 width = 8，base = 1e8,
                                    这样有个小问题，在做乘法时，，最大可能会产生 (1e8-1)*(1e8-1) 的新值，这是一个约为42bit的整数。
                                    在部分系统内，int指的是32bit整数。
                                    这导致在做乘法时直接进行 int * int 可能导致溢出。在一些条件下，这会引发一些玄妙的BUG，一般集中在跨平台方面。
                                    为了程序的健壮性，我选择使用 width = 4，base = 1e4，这样最多能产生(1e4-1)*(1e4-1)的数，在绝大多数情况下，可以直接用int型处理。
                                    按照这个思路，base最大就是1e4，甚至不能是1e5 。
                                */
private:
    vector<int> integer; // 存储各个段
                            /*
                                在任何情况下，integer内的数都是正数或0。
                                负大整数只有符号位是-1,integer内同样是正数或0。
                            */
    int segments = 0; // 切分的段数
                        /*
                            绝大多数情况下，segments = integer.size();
                            只有少部分操作会暂时性地打破这个规定。
                        */
    int flag = 1; //符号位
                    /*
                        默认为1(正数)，若为-1则为负数。0的符号位应为1。
                    */
    // 检查最高有效位,并重新计算segments
    // 比如说，正数+负数可能会等于让部分向量元素等于0，导致segments的数值产生偏差。此时需要尝试更新segments
    void update_real_segments(BigInteger &input) const {
        for(auto it = input.integer.rbegin();it < input.integer.rend();it++)
        {
            if( *it == 0 ) {
                input.integer.pop_back();
            }
            else {
                break;
            }
        } 

        //更新segments的值。如果相加后为0，则设置为初始状态，即 integer = 0,segments = 1。
        if( input.integer.size() == 0) 
        {
            input.integer.push_back(0);
            input.flag = 1;
        }
        input.segments = input.integer.size();
    }
public:
    int get_segments(){
        return segments;
    }
    

/*
    return A/B 的百分比形式
    这个其实稍微有点取巧了。
    要想实现精确的百分比，那么必须要引入小数，可能要添加幂。然而在讨论“百分比”的时候，我们其实不需要很精确的数值。
    所以可以只计算部分数值。
    一个segment的大小是1e4，取大数的前2个segment已经足够了。
    当然，两个数不相等则不能返回100，需要用99.99999的形式做一下区分。
*/
    double get_persent_with(BigInteger B)
    {
        BigInteger A = *this;

        double result = 0;
        if(A >= B) {
            result = 1;
            return result*100;
        }

        if(A.segments + 2 < B.segments)
        {
            return 0;
        }

        int value_A = 0;
        int value_B = 0;
        //取前两个segments的数据
        for(int i = B.segments-1;i > B.segments-1-2;i--)
        {
            if( i < A.segments ){
                value_A *= base;
                value_A += A.integer[i];
            }

            if( i < B.segments ){
                value_B *= base;
                value_B += B.integer[i];
            }
        }
        //结果的数值
        result = (double)value_A/(double)value_B;
        //排除掉 A B 整体不相等，但是前两个byte相等的情况
        if( value_A == value_B && A != B)
        {
            result -= 0.1;
        }

        //结果的符号
        int ret_flag = 1;
        if(A.flag == B.flag){
            ret_flag = 1;
        }
        else{
            ret_flag = -1;
        }

        return result * 100 * ret_flag;
    }

    //默认构造函数，构造出的大整数是+0
    explicit BigInteger() {
        flag = 1;   //初始化为正数0
        segments = 1;
        integer.push_back(0);
    }
    //整数构造函数
    BigInteger(long long num) { 
        if ( num < 0 ) {
            flag = -1;
        }
        else {
            flag = 1;
        }

        integer.clear();
        segments=0;
        do {
            integer.emplace_back( (num % base) * flag );  //这里要把符号提取到符号位，integer内只保留正数
            num /= base;
            segments++;
        } while (num > 0);
    }

    //字符串构造函数
    BigInteger(const string &num) { // 重载赋值运算符，从字符串类型转大整数存储
        if( *(num.begin()) == '-') {
            flag = -1;
        }
        else {
            flag = 1;
        }

        integer.clear();
        segments=0;
        int length = num.size();

        int offset = 0; //若string带有符号位，则offset能帮助程序规避掉符号位
        if( *(num.begin()) == '-' || *(num.begin()) == '+' ){
            length -= 1;
            offset = 1;
        }

        segments = (length - 1) / width + 1; /*
                                                这里计算段数的方法十分巧妙,可以避开 length % width = 0 的情况
                                                example:
                                                length = 16, width = 4
                                                (length - 1) / width = 3,
                                                segments = 3 + 1 = 4;
                                                即使 length % width !=0，该算式同样等得到正确结果。
                                            */

        for (int i = 0; i < segments; i++) {
            int end = num.size() - i * width; // 计算每段结束的地方，从最后一段开始
            int start = max(offset, end - width); // 计算每段开始的地方
            integer.emplace_back(stoi(num.substr(start, end - start))); // 字符串转整数
        }
    }
    
    //const char* 构造函数
    BigInteger(const char* input) { // 重载赋值运算符，从字符串类型转大整数存储
        string num(input);
        BigInteger result(num);
        *this = result;   
    }

    //拷贝构造函数
    BigInteger(const BigInteger &input) {  //拷贝构造还是不添加explicit为好
        integer.clear();
        for(int i=0;i < input.segments;i++) {
            integer.push_back(input.integer[i]);
        }
        segments = input.segments;
        flag = input.flag;
    }

    //拷贝运算-不借助拷贝构造函数，而是和拷贝构造函数做类似的操作。
    BigInteger operator=(const BigInteger &input) {  
        integer.clear();
        for(int i=0;i < input.segments;i++) {
            integer.push_back(input.integer[i]);
        }
        segments = input.segments;
        flag = input.flag;
     
        return *this;
    }

    //赋值运算-借助整数构造函数+拷贝运算来完成
    BigInteger operator=(long long num) { 
        BigInteger result(num);
        *this = result;
        return *this;
    }

    //重载赋值运算符-借助字符串构造函数+拷贝运算来完成
    BigInteger operator=(const string &num) { 
        BigInteger result(num);
        *this = result;
        return *this;
    }

    //重载赋值运算符-借助字符串构造函数+拷贝运算来完成
    BigInteger operator=(const char* input) {
        string num(input); 
        BigInteger result(num);
        *this = result;
        return *this;
    }
    
    bool operator<(const BigInteger &bigInteger) const {
        //加入符号位的对比,符号不同仅对比符号
        if ( flag != bigInteger.flag ) { 
            return flag < bigInteger.flag;
        }

        //符号相同则对比数值
        
        //先对比位数
        if (segments != bigInteger.segments) {
            return (segments * flag) < (bigInteger.segments * bigInteger.flag);
        }

        //再逐个对比
        for (int i = segments - 1; i >= 0; i--) { // 倒着比较，因为低位在vector的前面，先比较高位
            if (integer[i] != bigInteger.integer[i])
                return (integer[i] * flag) < (bigInteger.integer[i] * bigInteger.flag);
        }

        return false;
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
        //为了方便，做加法运算时需要清空integer的内容，并把segments设置为0
        BigInteger result;
        result.integer.clear();
        result.segments=0;
        
        for (int i = 0, carry = 0; carry || i < segments || i < bigInteger.segments; i++) {
            if (i < segments) {
                carry += integer[i] * flag; //带上符号参与计算
            }
            if (i < bigInteger.segments) {
                carry += bigInteger.integer[i] * bigInteger.flag;   //带上符号参与计算
            }

            result.integer.emplace_back(carry % base);
            result.segments++;
            carry = carry / base;
        }

        update_real_segments(result);

        //最高位的正负决定了最终符号。
        if( *(result.integer.end()-1) < 0 ) {
            result.flag = -1;
        }
        else {
            result.flag = 1;
        }

        //把符号位转移到result.flag上。确保integer内均为整数。
        for(int i=0;i<result.integer.size();i++)
        {
            result.integer[i] *= result.flag;
        }
        
        //统一处理进退位，由此得到最终长度。符号将决定进退位的逻辑
        //进位过程中,segments可能会动态变化
        for(int i=0;i<segments;i++)
        {
            //超出base，需要进位
            if( result.integer[i] > result.base)
            {
                //可能出现最高位需要进位的情况
                if(i == segments-1)
                {
                    result.integer.push_back(0);
                    result.segments += 1;
                }
                int highest = result.integer[i];
                result.integer[i] = highest % base;
                result.integer[i+1] += highest / base;
            }
            else if( result.integer[i] < 0) // 当integer内出现负数，需要通过"退位"来确保integer内是正数。
            {
                /*经过之前的符号提取后，在这一步，不存在最高的vector元素是负数的情况*/
                result.integer[i] += base;    

                if(result.flag >= 1) {
                    //符号位flag为正，此时的退位是"借1当10"
                    result.integer[i+1] -= 1;
                }
                else {
                    //符号位flag为负，此时integer内的负数反而代表正值，integer内的正数代表的才是负值。
                    //此时的退位是"借-1当-10"。由于被借走了-1，integer内的数值会+1。
                    result.integer[i+1] += 1;
                }
            }
        }

        return result;
    }

    BigInteger operator-(const BigInteger &bigInteger) const {
        BigInteger temp = bigInteger;
        temp.flag = bigInteger.flag * -1;
        BigInteger result = *this + temp;
        return result;
    }

    //按segment截取内容产生新BigInteger,注意低位存储在低segment
    //截取范围是[a,b)
    BigInteger sub_by_segment(int a,int b) const  { //记录一下const标记的相关事项
        BigInteger result;
        BigInteger temp;    //用加法消除掉可能的截断为全零

        if( a >= 0 && b <= segments && a < b)
        {
            temp.integer.clear();
            for(int i = a ; i < b ; i++)
            {
                temp.integer.push_back(integer[i]);
            }
            temp.segments = b-a;
        }
        return result + temp;
    }

    //右移n个segment
    BigInteger operator>>(int n) const {
        BigInteger zero;   //初始值为0
        if( *this == zero ){    //左右移对0都不起作用。
            return zero;
        }
        BigInteger result = *this;

        for( int i=0;i<n;i++)
        {
            result.integer.erase(result.integer.begin());
        }
        result.segments -= n;
        return result;
    }
    //左移n个segment
    BigInteger operator<<(int n) const {
        BigInteger zero;   //初始值为0
        if( *this == zero ){    //左右移对0都不起作用。
            return zero;
        }
        BigInteger result = *this;

        for( int i=0;i<n;i++)
        {
            result.integer.insert(result.integer.begin(),0);
        }
        result.segments += n;
        return result;
    }
    //先写一个最简单的版本，后续可以使用分治递归的方法计算乘法
    //这个版本能用，只适合长度较短的时候。可以作为递归的终点调用
    BigInteger mul(const BigInteger &bigInteger) const {
        BigInteger result;  //初始值为0
        BigInteger zero;   //初始值为0
        if( bigInteger == zero )
        {
            return zero;
        }
        for( int i=0;i<segments;i++)
        {
            for( int j=0;j<bigInteger.segments;j++)
            {
                BigInteger temp;
                temp = integer[i] * bigInteger.integer[j];
                result += (temp << (i+j));
            } 
        }
        
        result.flag = flag * bigInteger.flag; //同号为正，异号为负
        return result;
    }
    //递归版本
    BigInteger operator*(const BigInteger &bigInteger) const {
        //递归终止条件之一  帮助剪支
        BigInteger zero;   //初始值为0
        if( *this == zero || bigInteger == zero )
        {
            return zero;
        }

        //开始计算
        if( this->segments + bigInteger.segments <= 8)  //长度太短，可以使用直接相乘的形式
        {
            return mul(bigInteger);
        }

        //使用乘法分配律进行递归
        //this = B<<(segments - segments/2) + A
        BigInteger A = sub_by_segment(0,segments/2);
        BigInteger B = sub_by_segment(segments/2,segments);

        //bigInteger = D<<(segments - segments/2) + C
        BigInteger C = bigInteger.sub_by_segment(0,bigInteger.segments/2);
        BigInteger D = bigInteger.sub_by_segment(bigInteger.segments/2,bigInteger.segments);
        //cout<<"A"<<A<<" "<<"B"<<B<<" "<<"C"<<C<<" "<<"D"<<D<<" "<<endl;
        int offset = segments/2;
        int bigInteger_offset =  bigInteger.segments/2;
        BigInteger result = ((B * D)<<(offset)<<(bigInteger_offset)) + ((B * C)<<offset) + ((A * D)<<bigInteger_offset) + (A * C);
        
        result.flag = flag * bigInteger.flag; //同号为正，异号为负
        return result;
    }

    //有时候只需要简单的除法运算
    BigInteger operator/(int input) const {
        BigInteger result = *this;

        for(int i=segments-1;i >= 0;i--)
        {
            int target = result.integer[i] / input;
            int residue = result.integer[i] % input;

            result.integer[i] = target;
            if( i >= 1)
            {
                result.integer[i-1] += residue*base;
            }
        }

        update_real_segments(result);

        return result;
    }

    //先使用左移运算，快速寻找近似解。再进行深度优先搜索。
    //除法结果的符号仅由符号位决定，在计算时需要排除符号位进行计算。
    BigInteger operator/(const BigInteger &bigInteger) const {
        BigInteger result = 1;
        BigInteger A = *this;
        BigInteger B = bigInteger;
        int real_flag;
        //确定符号位,最终会进行赋值
        if(A.flag != B.flag){
            real_flag = -1;
        }
        else {
            real_flag = 1;
        }
        //统一 AB的符号位为正，然后开始计算
        A.flag *= A.flag;
        B.flag *= B.flag;

        //到这一步，AB都是正数，接下来开始计算
        
        BigInteger zero;
        if( A < B )
        {
            return zero;
        }

        //对result做粗略的偏移,偏移可能是0
        int offset = A.segments - B.segments;
        if(offset > 0) {
            offset -= 1;
        }

        result <<= offset;  //初始偏移
        bool run_flag = true;   //运行标记
        
        BigInteger step = "18446744073709551616";    //迭代步长，初始步长为2^64
        BigInteger ret = (result * B);

        BigInteger current = (A - ret);   //当前迭代值。current最初一定是正数或0

        BigInteger next;    //下一个迭代值。next 永远 <= current
        
        //开始迭代
        while( run_flag )
        {   
            //终止条件--找到最接近的那个数
            if( ( current * (current - B)) <= zero) {
                run_flag = false;
                continue;
            }

            next = A - (result+step) * B;  
            if( current * next < zero ) {   //一正一负，说明step太大，已经接近答案
                //cout<<"branch1"<<endl;
                step /= 2;  //使用普通除法对step进行缩小
            }
            else if( current * next > zero ){   //两个都是正数，说明result可以迭代,且可以尝试放大step
                //cout<<"branch2"<<endl;
                result += step;
                current = A - result * B;
                step *= 2;  
            }
            //getchar();    //按下回车手动迭代，可以实时观察变量内容。
        }
        result.flag = real_flag;
        return result;
    }

    //鉴于求余运算的运算量都在除法里面，而除法已经被优化过了，此处不妨直接使用大整数求余
    BigInteger operator%(int num) const {
        BigInteger result;
        BigInteger N = num;
        result = *this % N;
        return result;
    }
    BigInteger operator%(const BigInteger &bigInteger) const {
        BigInteger result = *this - ((*this)/bigInteger)*bigInteger;
        return result;
    }

    BigInteger operator>>=(int n) {
        *this = *this >> n;
        return *this;
    }
    BigInteger operator<<=(int n) {
        *this = *this << n;
        return *this;
    }

    BigInteger operator+=(const BigInteger &bigInteger) {
        *this = *this + bigInteger;
        return *this;
    }
    
    BigInteger operator-=(const BigInteger &bigInteger) {
        *this = *this - bigInteger;
        return *this;
    }
    
    BigInteger operator*=(const BigInteger &bigInteger) {
        *this = *this * bigInteger;
        return *this;
    }

    BigInteger operator/=(int num) {
        *this = *this / num;
        return *this;
    }
    
    BigInteger operator/=(const BigInteger &bigInteger) {
        *this = *this / bigInteger;
        return *this;
    }

    BigInteger operator%=(int num) {
        *this = *this / num;
        return *this;
    }
    BigInteger operator%=(const BigInteger &bigInteger) {
        *this = *this % bigInteger;
        return *this;
    }

    friend ostream &operator<<(ostream &out, const BigInteger &bigInteger);
};

ostream &operator<<(ostream &out, const BigInteger &bigInteger) { // 重载输出运算符，vector倒着输出
    if( bigInteger.flag >= 1) {
        out << "+" ;
    }
    else {
        out << "-" ;
    }
    for (auto it = bigInteger.integer.rbegin(); it != bigInteger.integer.rend(); it++) {
        //除了最高的segment，每一个segment在输出时不足8位要补0
        if( it != bigInteger.integer.rbegin() ) {
            out << setw(bigInteger.width) <<setfill('0')<<(*it);
        }
        else {
            out <<(*it);
        }
    }
    return out;
}

istream &operator>>(istream &in, BigInteger &bigInteger) { // 重载输入运算符，当成字符串输入，用重载的赋值运算符直接赋值
    string num;
    in >> num;
    bigInteger = num;
    return in;
}