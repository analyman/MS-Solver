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

void testC()
{
    MathExpr::MathExprEval<double> TC = MathExpr::MathExprEval<double>("");
    APT<Token>* ps =  TC.GetParseTree();
    if(ps == nullptr) return;
    typename Tokenizer<double>::ContextType context;
    double r = ps->EvalSyntaxTree<double>(context, TC.GetTokenizer().GetIdMap(), TC.GetTokenizer().GetFuncMap(), TC.GetTokenizer().GetImmediateMap());
    std::cout << r << std::endl;
}

void testD()
{
    MathExpr::MathExprEvalS<double> TC = MathExpr::MathExprEvalS<double>("x = 100");
    double x = TC.Eval();
    TC.clean_current_ast();
    TC.continue_with("x * x");
    double y = TC.Eval();
    TC.clean_current_ast();
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;
}

int main()
{
    testD();
    return 0;
}
