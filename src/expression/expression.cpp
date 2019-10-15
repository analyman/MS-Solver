#include "../../include/mssm/expression/expr_parse_tree.hpp"
#include "../../include/mssm/expression/expression_eval.hpp"
#include "../../include/mssm/expression/token.hpp"

namespace MathExpr {
template class MathExpr::FunctionMap<double>;
template class MathExpr::Tokenizer<double>;
template class MathExpr::MathExprEval<double>;
template class MathExpr::MathExprEvalS<double>;

const char* alpha_num = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char* __num_dot = "0123456789ABCDEFabcdef.";
}
