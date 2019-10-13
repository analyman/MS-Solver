#include "../src/expression/token.hpp"
#include "../src/expression/expression_eval.hpp"
#include "../src/expression/expr_parse_tree.hpp"

void testA()
{
    MathExpr::Tokenizer<double> TA = MathExpr::Tokenizer<double>("5 * 100 / 20 - XMMM * pow(MMMM)");
    for(auto a = TA.begin(); a != TA.end(); a++){
        std::cout << *a << std::endl;
    }
}

void testB()
{
    MathExpr::MathExprEval<double> EA = MathExpr::MathExprEval<double>("5 * 100 / 20 - xyzk * pow(33)");
    MathExpr::APT<MathExpr::Token>* T = EA.GetParseTree();
    std::cout << std::hex << "0x" << (void*)T << std::endl;
}

int main()
{
    testB();
}
