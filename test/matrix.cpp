//#define LU__DEBUG__ 1

#include "../include/mssm/matrix/matrix.hpp"
#include "../include/mssm/model//model.hpp"

#include<cstdlib>

#define MAX_TEST_NUM 55
#define TEST_SIZE 55

using namespace SMSolver;

class A
{
    private:
        char hello[1000];
        std::string* x;
    public:
        A(A& x){(*this) = static_cast<A&&>(x);}
        A(){x = new std::string("Hello world!"); hello[0]=100;}
        ~A(){delete x; std::cout << "destructor of A is called." << std::endl;}

        A& operator=(A&& x)
        {
            std::cout << "rvalue reference ... assign." << std::endl;
            return x;
        }
        A& operator=(A& x)
        {
            std::cout << "lvalue reference ... assign." << std::endl;
            return x;
        }

        A& operator+(A& x)
        {
            return x;
        }
};

class B
{
    private:
        A s;
        int i;
    public:
        B(){i = 100; s = *(new A());}
        ~B(){std::cout << "destructor of B is called." << std::endl;}
};

void TESTA()
{
    smatrix<double> S(5);
    S.set([](size_t i, size_t j){
            return i + j * 1.0;
            });
    std::cout << S.toString() << std::endl;
    S.comAdd(1, 3);
    std::cout << S.toString();
    return;
}

void TESTB()
{
    smatrix<basic_unit<double>> S(10);
    S.set([](size_t i, size_t j){
            basic_length<double> r(1.0 + (i - i + 1 + j - j - 1));
            return static_cast<basic_unit<double>>(r);
            });
    std::cout << S.toString();
    auto X = S * S;
    std::cout << X.toString();
    return;
}

void matrixTest()
{
    smatrix<double> x1(TEST_SIZE);
    smatrix<double> x2(1);
    cvector<double> v1(1);

    v1.set([](size_t i){return i * 1.0;});
    x2.set([](size_t i, size_t j){return i + j * 1.0;});

    std::cout << v1.toString();
    std::cout << x2.toString();

    std::cout << (x2 * v1).toString();

    return;
}

void Xtest()
{
    B xx;
    std::cout << "Xtest() is running." << std::endl; 
    // if the outpur is both of destructor of A and B IS CALL, then ok
    return;
}

void solverTest()
{
    smatrix<double> A(TEST_SIZE);
    std::srand(time(nullptr));
    A.set([](size_t i, size_t j){
            if(i == j) return 1.0; else 
                if(std::rand() % 100 < 99)
                    return 0.1 * (std::rand() % MAX_TEST_NUM - MAX_TEST_NUM / 2);
                return 0.0;
            });
    /*
    for(size_t i = 1;i<A.get_size(); i++){
        for(size_t j = i;j<A.get_size(); j++){
            if(j == i) continue;
            auto x = A.get(i, j) + A.get(j, i);
            A.set(i, j, x);
            A.set(j, i, x);
        }
    };
    */

    cvector<double> b(TEST_SIZE);
    b.set([](size_t i){return 1.0 * i;});

//    auto x = A.LU_Decomposition_solver(b);
    auto x = A.Guass_Elimination_solver(b);
    auto t = A * x - b;
    std::cout << A.toString() << std::endl;
    std::cout << x.toString() << std::endl;
    std::cout << b.toString() << std::endl;
    std::cout << t.toString() << std::endl;
}

void XsolverTest()
{
    smatrix<basic_unit<double>> A(TEST_SIZE);
    std::srand(time(nullptr));
    A.set([](size_t i, size_t j) -> basic_unit<double>{
            if(i == j) return basic_length<double>(1.0); else 
                if(std::rand() % 100 < 90){
                    return length_mm<double>(0.1 * (std::rand() % MAX_TEST_NUM - MAX_TEST_NUM / 2));
                    }
                return length_mm<double>(0.0);
            });

    cvector<basic_unit<double>> b(TEST_SIZE);
    b.set([](size_t i) -> decltype(b)::dataType {return basic_mass<double>(1.0 * i);});

    smatrix<basic_unit<double>> B(TEST_SIZE);
    std::srand(time(nullptr));
    B.set([](size_t i, size_t j) -> basic_unit<double>{
            if(i == j) return basic_second<double>(1.0); else 
                if(std::rand() % 100 < 90){
                    return basic_second<double>(0.1 * (std::rand() % MAX_TEST_NUM - MAX_TEST_NUM / 2));
                    }
                return basic_second<double>(0.0);
            });

    auto n = A * B;

    auto x = A.Guass_Elimination_solver(b);
    auto t = A * x - b;

    std::cout << A.toString() << std::endl;
    std::cout << x.toString() << std::endl;
    std::cout << b.toString() << std::endl;
    std::cout << t.toString() << std::endl;
}

int main()
{
    //    XsolverTest();
    //    solverTest();
    //    matrixTest();
    //    Xtest();
    TESTA();
    //    TESTB();
    return 0;
}
