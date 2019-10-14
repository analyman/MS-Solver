#ifndef _EXPR_PARSE_TREE_HPP
#define _EXPR_PARSE_TREE_HPP

#include "token.hpp"
#include "exception"
#include <cassert>
#include <cstring>
#include <cstdlib>

namespace MathExpr {

class parseTreeException //{
{
    private:
        std::string msg;
    public:
        parseTreeException(std::string& _s): msg(_s){}
        parseTreeException(std::string&& _s): msg(std::move(_s)){}
        parseTreeException(char* _c_str): msg(_c_str){}

        virtual const char * what() const noexcept {return msg.c_str();}
}; //}

template<typename T> class APT;
template<typename T>
std::ostream& operator_out_aux(std::ostream& o_s, const APT<T>& t);

enum Operand_type //{
{
    Unary,
    Binary,
    Ternary,
    Leaf
}; //}
// Abstract Parse Tree of Expression
template<typename T>
class APT //{
{
    public:
        typedef T                      token_type;
        typedef typename T::IdType result_id_type;

    private:
        friend std::ostream& operator_out_aux<T>(std::ostream&, const APT<T>&);
        APT*           m_child[3];
        token_type     m_token;
        Operand_type   m_operand_type;
        APT*           m_parent;
        bool           m_isvalid;
        result_id_type m_result_id;

        void init_result(){m_isvalid = false; m_result_id = 0;}

    public:
        APT() = delete;
        APT(const token_type& token, const Operand_type& t): //{
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Leaf);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
        } //}
        APT(const token_type& token, const Operand_type& t, APT* op1): //{
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Unary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
        } //}
        APT(const token_type& token, const Operand_type& t, APT* op1, APT* op2): //{
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Binary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
                m_child[1] = op2;
        } //}
        APT(const token_type& token, const Operand_type& t, APT* op1, APT* op2, APT* op3): //{
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Ternary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
                m_child[1] = op2;
                m_child[2] = op3;
        } //}
        ~APT() //{
        {
            switch(this->m_operand_type){
                case Operand_type::Leaf:
                    break;
                case Operand_type::Unary:
                    delete m_child[0];
                    break;
                case Operand_type::Binary:
                    delete m_child[0];
                    delete m_child[1];
                    break;
                case Operand_type::Ternary:
                    delete m_child[0];
                    delete m_child[1];
                    delete m_child[2];
                    break;
            }
        } //}

        APT*& GetParent(){return this->m_parent;}
        Token& GetToken(){return this->m_token;}

        template<typename V>
        V EvalSyntaxTree(const typename MathExpr::Tokenizer<V>::ContextType&      _context,
                         const typename MathExpr::Tokenizer<V>::IdMapType&        _idMap,
                         const typename MathExpr::Tokenizer<V>::FuncMapType&      _funcMap,
                         const typename MathExpr::Tokenizer<V>::ImmediateMapType& _immMap)
        { // TODO
            const Token& t = this->m_token;
            switch(t.GetType()){
                case token_enum::Delimiter:
                case token_enum::LP:
                case token_enum::RP:
                    throw *new parseTreeException("unexcepted token.");
                case token_enum::Function:
                    return V(0);
            }
        }
}; //}

template<typename T>
std::ostream& operator_out_aux(std::ostream& o_s, const APT<T>& t) //{
{
    const T& c = t.m_token;
    switch(c.m_token_type){
        case token_enum::Id:
            o_s << "<Id-" << c.m_id << ">";
            break;
        case token_enum::ImmediateValue:
            o_s << "<Val-" << c.m_id << ">";
            break;
        case token_enum::OperatorA:
            operator_out_aux(o_s, *t.m_child[0]);
            o_s << "^";
            operator_out_aux(o_s, *t.m_child[1]);
            break;
        case token_enum::OperatorB:
            o_s << "(";
            operator_out_aux(o_s, *t.m_child[0]);
            if(c.m_op_type == operator_type::OP_time)
                o_s << " * ";
            else
                o_s << " / ";
            operator_out_aux(o_s, *t.m_child[1]);
            o_s << ")";
            break;
        case token_enum::OperatorC:
            o_s << "(";
            operator_out_aux(o_s, *t.m_child[0]);
            if(c.m_op_type == operator_type::OP_plus)
                o_s << " + ";
            else
                o_s << " - ";
            operator_out_aux(o_s, *t.m_child[1]);
            o_s << ")";
            break;
        case token_enum::OperatorD:
            o_s << "(";
            operator_out_aux(o_s, *t.m_child[0]);
            o_s << " = ";
            operator_out_aux(o_s, *t.m_child[1]);
            o_s << ")";
            break;
        case token_enum::Function:
            switch(t.m_operand_type){
                case Operand_type::Leaf:
                    o_s << "<Func-" << c.m_id << ">()";
                    break;
                case Operand_type::Unary:
                    o_s << "<Func-" << c.m_id << ">(";
                    o_s << *t.m_child[0] << ")";
                    break;
                case Operand_type::Binary:
                    o_s << "<Func-" << c.m_id << ">(";
                    o_s << *t.m_child[0] << ", ";
                    o_s << *t.m_child[1] << ")";
                    break;
                case Operand_type::Ternary:
                    o_s << "<Func-" << c.m_id << ">(";
                    o_s << *t.m_child[0] << ", ";
                    o_s << *t.m_child[1] << ", ";
                    o_s << *t.m_child[2] << ")";
                    break;
            }
        default:
            break;
    }
    return o_s;
} //}
template<typename T>
std::ostream& operator<<(std::ostream& o_s, const APT<T>& t){ //{
    std::ios_base::fmtflags f(o_s.flags());
    o_s << std::dec;
    operator_out_aux(o_s, t);
    o_s.flags(f);
    return o_s;
} //}

}
#endif // _EXPR_PARSE_TREE_HPP
