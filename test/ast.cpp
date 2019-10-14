#include "../include/mssm/expression/token.hpp"
#include "../include/mssm/expression/expression_eval.hpp"
#include "../include/mssm/expression/expr_parse_tree.hpp"

using namespace MathExpr;

void testA()
{
    MathExpr::Tokenizer<double> TA = MathExpr::Tokenizer<double>("5 * 100 / 20 - XMMM * pow(MMMM)");
    for(auto a = TA.begin(); a != TA.end(); a++){
        std::cout << *a << std::endl;
    }
}

void testB()
{
    MathExpr::MathExprEval<double> EA = MathExpr::MathExprEval<double>("");
    MathExpr::APT<MathExpr::Token>* T = EA.GetParseTree();
    std::cout << std::hex << "0x" << (void*)T << std::endl;
    std::cout << *T << std::endl;
    delete T;
}

int main()
{
//    testB();
    typename FunctionMap<double>::unary_func _sin = FunctionMap<double>::FMap.get_unary("sin");
    double x = sin(1);
    std::cout << std::hex << "0x" << (long)_sin << "  sin(1) = " << x << std::endl;
}
