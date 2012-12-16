#include <memory>
#include <iostream>
#include <string>

struct B;

struct A
{
    B& b_ref_;

    A( B& b_ref ) : b_ref_(b_ref) {}

    void print_me() const
    {
        std::cout << "A: " << i_ << "\n";
    }

    int i_ = 1;
};

struct B
{
    A* a_ptr_;

    void set_a( A& a_ref )
    {
        a_ptr_  = std::addressof(a_ref);
    }


    void print_me() const
    {
        std::cout << "B: " << i_ << "\n";
    }

    int i_ = 2;
};



void f()
{
    B b;
    A a(b);
    b.set_a(a);


    // b.print_me()
    // a.print_me()
    // a.do_something_with_b()
    // b.do_something_with_a()     //Run-time Error

    // b.set_A(A_ref)

    // b.do_something_with_a()
}
//----------------------------------------------------------------------------




int main()
{
    f();
}
