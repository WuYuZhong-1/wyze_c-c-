#include <iostream>


class A {
public:
    void func() {
        std::cout << "func void\n";
    }

    void func(int a) {
        std::cout << "func int\n";
    }
};

class B : public A {
public:
    using A::func;  //导入父类的方法
    void func(int a, int b) {
        std::cout << "func int int\n";
    }
};

int main(int argc, char** argv) {
    B b;
    b.func();

    return 0;
}