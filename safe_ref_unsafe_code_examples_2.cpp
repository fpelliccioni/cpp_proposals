#include <iostream>

struct B;
struct A
{
    A(B& b);

    B* b_ptr_;

    void print() const
    {
    	std::cout << i_ << std::endl;
    }

    int i_ = 1;
};

struct B
{
    B(A& a) : a_ptr_(&a) {}
    A* a_ptr_;

    void print() const
    {
    	std::cout << i_ << std::endl;
    }

    int i_ = 2;
};

A::A(B& b)
	: b_ptr_(&b) 
{
	b.print();
}

struct C 
{
    A a;
    B b;

    C() : a(b), b(a) {}

    void print_a() const
    {
    	a.print();
    }

    void print_b() const
    {
    	b.print();
    }
};


int main()
{
	C c;

	c.print_a();
	c.print_b();
}