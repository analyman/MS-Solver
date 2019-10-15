#include "../include/mssm/expression/token.hpp"
#include "../include/mssm/expression/expression_eval.hpp"
#include "../include/mssm/expression/expr_parse_tree.hpp"

#include <complex>

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
    MathExpr::MathExprEvalS<double> TC = MathExpr::MathExprEvalS<double>("x = fmax(sqrt(100), log(10000000))");
    double x = TC.Eval();
    TC.continue_with("y = sqrt(200) * 50", true);
    double y = TC.Eval();
    TC.continue_with("x * y", true);
    double z = TC.Eval();
    TC.clean_current_ast();
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;
    std::cout << "z = x * y = " << z << std::endl;
}

void testE()
{
    using namespace std;
    using tttt = complex<int>;
    MathExpr::MathExprEvalS<tttt> TC = MathExpr::MathExprEvalS<tttt>("x = 100 + 1000");
    tttt x = TC.Eval();
    TC.continue_with("y = 2 * 50 * 400", true);
    tttt y = TC.Eval();
    TC.continue_with("x * y", true);
    tttt z = TC.Eval();
    TC.clean_current_ast();
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;
    std::cout << "z = x * y = " << z << std::endl;
}

int main()
{
    testE();
    return 0;
}
