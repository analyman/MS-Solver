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

double max_3(double a, double b, double c){
    if(a >= b)
        return a>=c ? a : c;
    else 
        return b>=c ? b : c;
}

void testD()
{
    MathExpr::FunctionMap<double>::FMap.new_ternary("fmax", max_3);
    MathExpr::MathExprEvalS<double> TC = MathExpr::MathExprEvalS<double>("x = fmax(sqrt(100), 20.2 ,log(10000000)) * fmax(100, 200) / 20.0 + 100 - 200");
    double x = TC.Eval();
    TC.continue_with("y = sqrt(200) * 50", true);
    double y = TC.Eval();
    TC.continue_with("x^3 / 6 + x^4 + x^5 / 20 / 30)", true);
    APT<Token>* ddd = TC.derivative_of("x");
    APT<Token>* zzz = TC.derivative_of("x", ddd);
    std::string sss = TC.toString(ddd);
    std::string yyy = TC.toString(zzz);
    double k = TC.Eval(ddd);
    delete ddd;
    delete zzz;
    double z = TC.Eval();
    TC.clean_current_ast();
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "z = x * y = " << z << std::endl;
    std::cout << sss << std::endl;
    std::cout << yyy << std::endl;
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

void testF()
{
    MathExpr::FunctionMap<double>::FMap.new_ternary("fmax", max_3);
    MathExpr::MathExprEvalS<double> TC = MathExpr::MathExprEvalS<double>("x = fmax(sqrt(100), 20.2 ,log(10000000)) * fmax(100, 200) / 20.0 + 100 - 200");
    double x = TC.Eval();
    TC.continue_with("y = sqrt(200) * 50", true);
    double y = TC.Eval();
    TC.continue_with("5*x^3 + x^4 + x^5 / (20 * 30)", true);
    APT<Token>* ddd = TC.derivative_of("x");
    std::string sss = TC.toString(ddd);
    double k = TC.Eval(ddd);
    delete ddd;
    double z = TC.Eval();
    TC.clean_current_ast();
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "z = x * y = " << z << std::endl;
    std::cout << sss << std::endl;
}



int main()
{
    testD();
    return 0;
}
