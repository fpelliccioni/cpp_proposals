//----------------------------------------------------------------------------
// Proposed (draft)
//----------------------------------------------------------------------------

#include <memory>

#include <cstddef>  //std::nullptr_t;


template <typename T>
class safe_ref
{
public:
    typedef T type;
    typedef T& reference;

    // explicit safe_ref( std::nullptr_t )
    //     : t_( nullptr )
    // {}

    explicit safe_ref()
        : t_( nullptr )
    {}

    explicit safe_ref(T& t)
        : t_( std::addressof(t) )
    {}

    template < typename Y > 
    explicit safe_ref( Y& t )
        : t_( std::addressof(t) )
    {}

    template < typename Y > 
    safe_ref( safe_ref<Y> const& r )
        : t_( r.t_ )
    {}

    template <typename Func>
    void use ( Func f ) const
    {
        T* local_ptr = t_;
        
        // ( is_initialized() )
        if ( local_ptr != nullptr )
        {
            f( *t_ );       //side note: how to know if f(...) is const?
        }
    }

    template <typename Func, typename FuncElse>
    void use ( Func f, FuncElse fe ) const
    {
        T* local_ptr = t_;
        
        // ( is_initialized() )
        if ( local_ptr != nullptr )
        {
            f( *t_ );       //side note: how to know if f(...) is const?
        }
        else
        {
            fe();
        }
    }    

    safe_ref& operator=( std::nullptr_t )
    {
        t_ = nullptr;
    }

    safe_ref& operator=( T& t )
    {
        t_ = std::addressof(t);
    }    

private:
    bool is_initialized() const
    {
        return t_ != nullptr;
    }

private:
    template <typename Y> friend class safe_ref;
    T* t_;
};

template <typename T>
inline safe_ref<T> const ref(T& t)
{
    return safe_ref<T>(t);
}

template <typename T>
inline safe_ref<T const> const cref(T const& t)
{
    return safe_ref<T const>(t);
}


//----------------------------------------------------------------------------
// Usage
//----------------------------------------------------------------------------

#include <iostream>
#include <string>

struct A
{
    void do_something( std::string s ) const
    {
        std::cout << "A::do_something with " << s << " and " << i_ << "\n";
    }

    int i_ = 99;
};

struct B : A
{
    void do_something( std::string s ) const
    {
        std::cout << "B::do_something with " << s << " and " << i2_ << "\n";
    }

    int i2_ = 888;
};



struct PolyA
{
    virtual void doit() const
    {
        std::cout << "PolyA::doit()\n";
    }

    int i_ = 99;
};

struct PolyB : PolyA
{
    virtual void doit() const
    {
        std::cout << "PolyB::doit()\n";
    }

    int i2_ = 888;
};



// Motivation: unsafe traditional code
struct Unsafe
{
    void set_ref( A& a )
    {
        a_ptr_ = std::addressof(a);
    }

    void use_ref( std::string s ) const
    {
        if (a_ptr_ != nullptr)
        {
            a_ptr_->do_something(s);
        }
    }

    void use_ref_2( std::string s ) const
    {
        //forgot to check for nullptr
        a_ptr_->do_something(s);
    }    

    A* a_ptr_ = nullptr;
};

void f()
{
    A a;

    Unsafe unsafe;
    unsafe.use_ref( "hello" );              //null reference, do nothing, hope!
    unsafe.set_ref(a);
    unsafe.use_ref( "hello world!" );       //not-null reference, Ok!

    Unsafe unsafe2;
    unsafe2.use_ref( "hello" );              //null reference, do nothing, hope!
    unsafe2.use_ref_2( "hello" );            //null reference, Ops!    
}
//----------------------------------------------------------------------------





struct Safe
{
    void set_ref( A& a )
    {
        //ref_ = ref(a);
        a_ref_ = a;
    }

    void use_ref( std::string s ) const
    {
        a_ref_.use( [&s] ( A& a ) {
            a.do_something(s);
        });
    }

    void reset()
    {
        a_ref_ = nullptr;
    }

    safe_ref<A> a_ref_;
};




void usage1()
{
    safe_ref<int> si;
    si.use( [] (int& i ) {
        std::cout << i << std::endl;
    });


    int temp = 15;

    safe_ref<int> si2(temp);
    si2.use( [] (int& i ) {
        std::cout << i << std::endl;
    });    


    si = ref(temp);
    si.use( [] (int& i ) {
        std::cout << i << std::endl;
    });
}

void usage2()
{
    A a;

    Safe safe;
    safe.use_ref( "hello" );          //null reference, do nothing
    safe.set_ref(a);
    safe.use_ref( "hello world!" );   //safe reference, Ok!    
}


void usage3()
{
    A a;

    Safe safe;
    safe.use_ref( "hello" );          //null reference, do nothing

    safe.set_ref(a);
    safe.use_ref( "hello world!" );   //safe reference, Ok!    

    safe.reset();
    safe.use_ref( "hello" );          //null reference, do nothing

}

// #define IF(_local_, _ref_, _if_code_, _else_code_) { \
// auto _local_ptr_ = _ptr_; \
// if (_local_ptr_ != nullptr) { \
//     auto& _local_ = *_local_ptr_; \
//     _if_code_ \
// } \
// else { \
//     _else_code_ \
// } \
// }

#define IF(_local_, _ref_, _if_code_) \
    _ref_.use( [&] ( typename decltype(_ref_)::reference _local_ ) { \
        _if_code_  \
});

#define IF_ELSE(_local_, _ref_, _if_code_, _else_code_) \
    _ref_.use( [&] ( typename decltype(_ref_)::reference _local_ ) { \
                _if_code_  \
                }, \
                [&] { \
                    _else_code_ \
                });

void f( safe_ref<A> ref )
{
    // if ( local : ptr )
    // {
    //     local.print_me(); 
    // }
    // else
    // {
    // }

    std::string s =" hello";

    IF( local, ref, 
    {
        local.do_something(s);
    })

    IF_ELSE( local, ref, 
    {
        local.do_something(s);
    }, 
    { 
        std::cout << "is nullptr\n"; 
    })
}


void usage4()
{
    A a;

    safe_ref<A> r;
    f(r);

    r = ref(a);
    f(r);

}

void usage5()
{
    B b;
    safe_ref<A> r(b);
    f(r);
}



void call_poly( PolyA& ref )
{
    ref.doit();
}

void call_poly_safe( safe_ref<PolyA> ref )
{
    IF_ELSE( local, ref, 
    {
        local.doit();
    }, 
    { 
        std::cout << "is nullptr\n"; 
    })
}


void usage6()
{
    PolyA pa;
    PolyB pb;

    call_poly_safe( ref(pa) );
    call_poly_safe( ref(pb) );

    call_poly_safe( safe_ref<PolyA>() );
    call_poly_safe( safe_ref<PolyB>() );
}





int main()
{
    //f();

    // usage1();
    // usage2();
    // usage3();
    // usage4();

    // usage5();
    usage6();


    // PolyA pa;
    // PolyB pb;

    // call_poly(pa);
    // call_poly(pb);
}
