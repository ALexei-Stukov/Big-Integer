#include "big_integer.h"

int main() {
    BigInteger a, b ,c ,d;
    a = "-187654321";
    b = "123456789";
    c = "123456789";
    d = a;
    cout<<d<<endl;

    BigInteger p1 = "-4444111122223333";
    BigInteger p2 = "+10000000000000000";
    cout <<p1<< " * "<<p2<<" = "<< p1 * p2 <<endl;

    cout <<b<< " - "<<c<<" = "<< b - c <<endl;
    cout <<"cut segment [0,1) is  "<< b.sub_by_segment(0,1)<<endl;

    cout <<a<< " <= "<<b<<" = "<< (a <= b )<<endl;
    cout <<a<< " > "<<b<<" = "<< (a > b )<<endl;
    cout <<a<< " + "<<b<<" = "<< a + b <<endl;
    cout <<a<< " - "<<b<<" = "<< a - b <<endl;
    cout <<b<< " + "<<c<<" = "<< b + c <<endl;

    cout <<b<< " >> "<<1<<" = "<< (b>>1) <<endl;
    cout <<b<< " >> "<<0<<" = "<< (b>>0) <<endl;
    cout <<b<< " << "<<1<<" = "<< (b<<1) <<endl;
    cout <<b<< " * "<<c<<" = "<< b * c <<endl;  //15,241,578,750,190,521

    BigInteger L1,L2,L3;
    L1 = "-4444111122223333";
    L2 = "-4444111122223333";
    L3 = "18901234567901234579023479012384719";

    cout <<L1<< " mul "<<L2<<" = "<< L1.mul(L2) <<endl;
    cout <<L1<< " * "<<L2<<" = "<< L1 * L2 <<endl;  //19750123666669132222469129628889
    cout <<a<< " / "<<5<<" = "<< a / 5 <<endl;
    cout <<L1<< " / "<<L2<<" = "<< L1 / L2 <<endl;
    cout <<L2<< " / "<<L3<<" = "<< L2 / L3 <<endl;
    cout <<L3<< " / "<<L2<<" = "<< L3 / L2 <<endl;

    cout <<L3<< " % "<<L2<<" = "<< L3 % L2 <<endl;
    cout <<L3<< " / "<<2<<" = "<< L3 / BigInteger(2) <<endl;
    cout <<L3<< " % "<<2<<" = "<< L3 % BigInteger(2) <<endl;

    cout <<p1<< " get_the_persent_with "<<p2<<" = "<<p1.get_persent_with(p2)<<"%"<<endl;
    
    BigInteger t1,t2;
    t1 = "908123458907123490";
    t2 = "958971234895734567";
    cout <<t1<< " get_the_persent_with "<<t2<<" = "<<t1.get_persent_with(t2)<<"%"<<endl;
    
    return 0;
}