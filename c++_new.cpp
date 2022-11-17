#include <iostream>

void test_auto() 
{
    int a = 11;
    auto& b = a;
    auto c = b;

    const int aa = 10;
    auto& bb = aa;
    auto cc = bb;

    int aaa = 10;
    static auto bbb = aaa;

    char* ccc = "welcome";
    auto ddd = ccc;
}

void ret_void(int i) { (void)i; }

void test_decltype()
{
    // decltype(re_void()) aa;
    int a = 10;
    int c = 10;
    int d ;
    decltype((a)) b = a;
}


int main(int argc, char** argv)
{
    return 0;
}
