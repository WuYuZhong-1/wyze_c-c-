#include <iostream>
#include <memory>


void test_unique_ptr()
{
    std::unique_ptr<std::string>  uptr(new std::string("hello word"));
    std::unique_ptr<std::string> uptr1;
    uptr1 = std::move(uptr);

    std::cout << *uptr1 << std::endl;
    if(uptr == nullptr)
        std::cout << "move uptr, it is nullptr" << std::endl;
}

void test_shared_ptr()
{
    std::shared_ptr<std::string> sptr(new std::string("hello world"));
    std::shared_ptr<std::string> sptr1, sptr2;
    sptr1 = sptr;
    sptr2 = std::move(sptr);

    std::cout << "use_count " << sptr2.use_count() << std::endl;
}

void test_weak_ptr()
{
    std::shared_ptr<std::string> sptr(new std::string("hello world"));
    std::weak_ptr<std::string> wptr(sptr);
    std::weak_ptr<std::string> wptr1(wptr);

    std::cout << sptr.use_count() << std::endl;
    sptr.reset();
    sptr = wptr1.lock();
    std::cout << sptr.use_count() << std::endl;
}

class B;
class A {
public:
    // std::shared_ptr<B> m_b;
    std::weak_ptr<B> m_b;
    ~A() {
        std::cout << "A is deleted" << std::endl;
    }
};

class B {
public:
    // std::shared_ptr<A> m_a;
    std::weak_ptr<A> m_a;
    ~B() {
        std::cout << "B is deleted" << std::endl;
    }
};

void test2_weak_ptr()
{
    std::shared_ptr<A> a(new A);
    std::shared_ptr<B> b(new B);
    a->m_b = b;
    b->m_a = a;
}


int main(int argc, char** argv)
{
    // test_unique_ptr();
    // test_shared_ptr();
    // test_weak_ptr();
    test2_weak_ptr();

    std::cout << "main endl\n";
    return 0;
}