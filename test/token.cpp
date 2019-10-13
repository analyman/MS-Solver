#include "../src/expression/token.hpp"

void testA()
{
    MathExpr::Tokenizer<double> TA = MathExpr::Tokenizer<double>("5 * 100 / 20 - XMMM * pow(MMMM)");
    for(auto a = TA.begin(); a != TA.end(); a++){
        std::cout << *a << std::endl;
    }
}

int main()
{
    testA();
}
