#include <iostream>

struct A 
{
	void print_me() const
	{
		std::cout << i_ << std::endl;
	}

	int i_ = 999;
};


#define IF(_local_, _ptr_, _if_code_, _else_code_) { \
auto _local_ptr_ = _ptr_; \
if (_local_ptr_ != nullptr) { \
	auto& _local_ = *_local_ptr_; \
	_if_code_ \
} \
else { \
	_else_code_ \
} \
}

#define SAFE(_type_, _variable_) _type_* _variable_ = nullptr


void f( A* ptr )
{
	// if ( local : ptr )
	// {
	//     local.print_me(); 
	// }
	// else
	// {
	// }


	IF( local, ptr, 
	{
		local.print_me(); 
	}, 
	{ 
		std::cout << "is nullptr\n"; 
	})
}

int main()
{



	// keyword or identifier with special meaning (like override and final) ????
	//A* ref = nullptr;
	//A safe& ref;		?????			
	//A friend& ref;	?????
	//A protected& ref;	?????

	SAFE(A, ref);

	//ref.print_me(); 	//Invalid, not protected statement!

	f(ref);

	A a;
	ref = &a;

	f(ref);

}