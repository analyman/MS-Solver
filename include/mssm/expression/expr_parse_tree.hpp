#ifndef _EXPR_PARSE_TREE_HPP
#define _EXPR_PARSE_TREE_HPP

#include "token.hpp"
#include "../utils/exception.hpp"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

namespace MathExpr {

class parseTreeException: public SMSolver::SMSolverException {
    public:
        parseTreeException(const char*        s): SMSolverException(s){}
        parseTreeException(const std::string& s): SMSolverException(s){}
};

template<typename T> class APT;
template<typename T>
std::ostream& operator_out_aux(std::ostream& o_s, const APT<T>& t);
template<typename T> class MathExprEvalS;

template<typename T> T t_pow(const T&, const T&){return T(0);}
template<> double t_pow(const double& a, const double& b){return ::pow(a, b);}
template<> float  t_pow(const float & a, const float & b){return ::pow(a, b);}
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
        template<typename U> 
        friend class MathExprEvalS;
        APT*           m_child[3];
        token_type     m_token;
        Operand_type   m_operand_type;
        APT*           m_parent;
        bool           m_isvalid;
        result_id_type m_result_id;

        void init_result(){m_isvalid = false; m_result_id = 0;}

        APT(): m_operand_type(Operand_type::Leaf){}

    public:
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
                if(m_child[0] != nullptr) delete m_child[0];
                break;
            case Operand_type::Binary:
                if(m_child[0] != nullptr) delete m_child[0];
                if(m_child[1] != nullptr) delete m_child[1];
                break;
            case Operand_type::Ternary:
                if(m_child[0] != nullptr) delete m_child[0];
                if(m_child[1] != nullptr) delete m_child[1];
                if(m_child[2] != nullptr) delete m_child[2];
                break;
        }
    } //}
    APT(const APT& _o): m_operand_type(Operand_type::Leaf){*this = _o;}
    APT(APT&& _o): m_operand_type(Operand_type::Leaf)     {*this = std::move(_o);}

    APT& operator=(const APT& _o) //{
    {
        if(this == &_o) return *this;
        switch(this->m_operand_type){
            case Operand_type::Leaf   :  break;
            case Operand_type::Unary  :  delete m_child[0]; break;
            case Operand_type::Binary :  delete m_child[0]; delete m_child[1]; break;
            case Operand_type::Ternary:  delete m_child[0]; delete m_child[1]; delete m_child[2]; break;
        }
        m_child[0] = nullptr;
        m_child[1] = nullptr;
        m_child[2] = nullptr;
        this->m_token        = _o.m_token;
        this->m_parent       = _o.m_parent;
        this->m_operand_type = _o.m_operand_type;
        this->m_isvalid      = _o.m_isvalid;
        this->m_result_id    = _o.m_result_id;
        switch(this->m_operand_type){
            case Operand_type::Leaf   :  break;
            case Operand_type::Unary  :  m_child[0] = new APT(); *m_child[0] = *_o.m_child[0]; break;
            case Operand_type::Binary :  m_child[0] = new APT(); *m_child[0] = *_o.m_child[0];
                                         m_child[1] = new APT(); *m_child[1] = *_o.m_child[1]; break;
            case Operand_type::Ternary:  m_child[0] = new APT(); *m_child[0] = *_o.m_child[0];
                                         m_child[1] = new APT(); *m_child[1] = *_o.m_child[1];
                                         m_child[2] = new APT(); *m_child[2] = *_o.m_child[2]; break;
        }
        return *this;
    } //}
    APT& operator=(APT&& _o) //{
    {
        if(this == &_o) return *this;
        switch(this->m_operand_type){
            case Operand_type::Leaf   :  break;
            case Operand_type::Unary  :  delete m_child[0]; break;
            case Operand_type::Binary :  delete m_child[0]; delete m_child[1]; break;
            case Operand_type::Ternary:  delete m_child[0]; delete m_child[1]; delete m_child[2]; break;
        }
        m_child[0] = nullptr;
        m_child[1] = nullptr;
        m_child[2] = nullptr;
        this->m_token        = _o.m_token;
        this->m_parent       = _o.m_parent;
        this->m_operand_type = _o.m_operand_type;
        this->m_isvalid      = _o.m_isvalid;
        this->m_result_id    = _o.m_result_id;
        switch(this->m_operand_type){
            case Operand_type::Leaf   :  break;
            case Operand_type::Unary  :  m_child[0] = _o.m_child[0]; break;
            case Operand_type::Binary :  m_child[0] = _o.m_child[0];
                                         m_child[1] = _o.m_child[1]; break;
            case Operand_type::Ternary:  m_child[0] = _o.m_child[0];
                                         m_child[1] = _o.m_child[1];
                                         m_child[2] = _o.m_child[2]; break;
        }
        _o.m_operand_type = Operand_type::Leaf;
        return *this;
    } //}

        APT*& GetParent(){return this->m_parent;}
        Token& GetToken(){return this->m_token;}

        bool decsendants_has_id(Token::IdType id) //{
        {
            if(this->m_token.GetId() == id){
                if(this->m_token.GetType() == token_enum::Id)
                    return true;
                else 
                    return false;
            }
            switch(this->m_token.GetType()){
                case token_enum::OperatorA:
                case token_enum::OperatorB:
                case token_enum::OperatorC:
                    return this->m_child[0]->decsendants_has_id(id) || 
                           this->m_child[1]->decsendants_has_id(id);
                case token_enum::OperatorD:
                    return this->m_child[1]->decsendants_has_id(id);
                case token_enum::Function:
                    switch(this->m_operand_type){
                        case Operand_type::Leaf:
                            return false;
                        case Operand_type::Unary:
                            return this->m_child[0]->decsendants_has_id(id); 
                        case Operand_type::Binary:
                            return this->m_child[0]->decsendants_has_id(id) || 
                                   this->m_child[1]->decsendants_has_id(id);
                        case Operand_type::Ternary:
                            return this->m_child[0]->decsendants_has_id(id) || 
                                   this->m_child[1]->decsendants_has_id(id) ||
                                   this->m_child[2]->decsendants_has_id(id);
                    }
                default:
                    return false;
            }
            return false;
        } //}
        template<typename U>
        bool decsendants_has_id(const std::string& id, U& idmap) //{
        {
            if(idmap[this->m_token.GetId()] == id && id != ""){
                if(this->m_token.GetType() == token_enum::Id)
                    return true;
                else 
                    return false;
            }
            switch(this->m_token.GetType()){
                case token_enum::OperatorA:
                case token_enum::OperatorB:
                case token_enum::OperatorC:
                    return this->m_child[0]->decsendants_has_id(id, idmap) || 
                           this->m_child[1]->decsendants_has_id(id, idmap);
                case token_enum::OperatorD:
                    return this->m_child[1]->decsendants_has_id(id, idmap);
                case token_enum::Function:
                    switch(this->m_operand_type){
                        case Operand_type::Leaf:
                            return false;
                        case Operand_type::Unary:
                            return this->m_child[0]->decsendants_has_id(id, idmap); 
                        case Operand_type::Binary:
                            return this->m_child[0]->decsendants_has_id(id, idmap) || 
                                   this->m_child[1]->decsendants_has_id(id, idmap);
                        case Operand_type::Ternary:
                            return this->m_child[0]->decsendants_has_id(id, idmap) || 
                                   this->m_child[1]->decsendants_has_id(id, idmap) ||
                                   this->m_child[2]->decsendants_has_id(id, idmap);
                    }
                default:
                    return false;
            }
            return false;
        } //}

    bool is_constant() //{
    {
        switch(this->m_token.GetType()){
            case token_enum::OperatorA:
            case token_enum::OperatorB:
            case token_enum::OperatorC:
            case token_enum::OperatorD:
                return this->m_child[0]->is_constant() && 
                    this->m_child[1]->is_constant();
            case token_enum::Function:
                switch(this->m_operand_type){
                    case Operand_type::Leaf:
                        return true;
                    case Operand_type::Unary:
                        return this->m_child[0]->is_constant(); 
                    case Operand_type::Binary:
                        return this->m_child[0]->is_constant() && 
                            this->m_child[1]->is_constant();
                    case Operand_type::Ternary:
                        return this->m_child[0]->is_constant() &&
                            this->m_child[1]->is_constant() &&
                            this->m_child[2]->is_constant();
                }
            case token_enum::Id: return false;
            default:
                return true;
        }
    } //}

    void clear_children() //{
    {
        switch(this->m_operand_type){
            case Operand_type::Leaf:
                break;
            case Operand_type::Unary:
                delete this->m_child[0]; break;
            case Operand_type::Binary:
                delete this->m_child[0];
                delete this->m_child[1]; break;
            case Operand_type::Ternary:
                delete this->m_child[0];
                delete this->m_child[1];
                delete this->m_child[2]; break;
        }
        this->m_child[0]     = nullptr;
        this->m_child[1]     = nullptr;
        this->m_child[2]     = nullptr;
        this->m_operand_type = Operand_type::Leaf;
    } //}

    void fix_parent(APT<token_type>* p_ptr = nullptr) //{
    {
        this->m_parent = p_ptr;
        switch(this->m_operand_type){
            case Operand_type::Leaf:
                break;
            case Operand_type::Unary:
                this->m_child[0]->fix_parent(this);
                break;
            case Operand_type::Binary:
                this->m_child[0]->fix_parent(this);
                this->m_child[1]->fix_parent(this);
                break;
            case Operand_type::Ternary:
                this->m_child[0]->fix_parent(this);
                this->m_child[1]->fix_parent(this);
                this->m_child[2]->fix_parent(this);
                break;
        }
    } //}

    template<typename V>
        V EvalSyntaxTree(typename MathExpr::Tokenizer<V>::ContextType&      _context,
                typename MathExpr::Tokenizer<V>::IdMapType&        _idMap,
                typename MathExpr::Tokenizer<V>::FuncMapType&      _funcMap,
                typename MathExpr::Tokenizer<V>::ImmediateMapType& _immMap) //{
        {
            FunctionMap<V>& FGET = FunctionMap<V>::FMap;
            const Token& t = this->m_token;
            V arg1, arg2, arg3;
            APT* ch_1 = this->m_child[0], *ch_2 = this->m_child[1], *ch_3 = this->m_child[2];
            switch(t.GetType()){
                case token_enum::Delimiter:
                case token_enum::LP:
                case token_enum::RP:
                    throw *new std::runtime_error("unexcepted token.");
                case token_enum::Function:
                    {// HANGLE Function //{
                        std::string& fname = _funcMap[t.GetId()];
                        typename FunctionMap<V>::nonary_func  n_f;
                        typename FunctionMap<V>::unary_func   u_f;
                        typename FunctionMap<V>::binary_func  b_f;
                        typename FunctionMap<V>::ternary_func t_f;
                        switch(this->m_operand_type){
                            case Operand_type::Leaf:
                                n_f = FGET.get_nonary(fname);
                                if(!n_f) throw *new std::runtime_error("unknow function: " + fname + "()");
                                return n_f();
                            case Operand_type::Unary:
                                u_f = FGET.get_unary(fname);
                                if(!u_f) throw *new std::runtime_error("unknow function: " + fname + "(x)");
                                arg1 = ch_1->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                return u_f(arg1);
                            case Operand_type::Binary:
                                b_f = FGET.get_binary(fname);
                                if(!b_f) throw *new std::runtime_error("unknow function: " + fname + "(x, y)");
                                arg1 = ch_1->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                arg2 = ch_2->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                return b_f(arg1, arg2);
                            case Operand_type::Ternary:
                                t_f = FGET.get_ternary(fname);
                                if(!t_f) throw *new std::runtime_error("unknow function: " + fname + "(x, y, z)");
                                arg1 = ch_1->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                arg2 = ch_2->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                arg3 = ch_3->EvalSyntaxTree<V>(_context, _idMap, _funcMap ,_immMap);
                                return t_f(arg1, arg2, arg3);
                            default:
                                throw *new std::runtime_error("unknow function: " + fname);
                        }
                    } //}
                case token_enum::OperatorA:
                case token_enum::OperatorB:
                case token_enum::OperatorC:
                    arg1 = ch_1->EvalSyntaxTree<V>(_context, _idMap, _funcMap, _immMap);
                    arg2 = ch_2->EvalSyntaxTree<V>(_context, _idMap, _funcMap, _immMap);
                    switch(t.GetOpType()){
                        case operator_type::OP_pow:
                            return t_pow(arg1, arg2);
                        case operator_type::OP_time:
                            return arg1 * arg2;
                        case operator_type::OP_div:
                            return arg1 / arg2;
                        case operator_type::OP_plus:
                            return arg1 + arg2;
                        case operator_type::OP_minus:
                            return arg1 - arg2;
                        default:
                            throw *new std::runtime_error("unexcepted token.");
                    }

                case token_enum::OperatorD: 
                    {
                        assert(ch_1->GetToken().GetType() == token_enum::Id);
                        arg2 = ch_2->EvalSyntaxTree<V>(_context, _idMap, _funcMap, _immMap);
                        const std::string& id_str1 = _idMap[ch_1->GetToken().GetId()];
                        if(id_str1 == "") throw *new std::runtime_error("error ID or something else wrong.");
                        _context[id_str1] = arg2;
                        return arg2;
                    }
                case token_enum::Id:
                    {
                        const std::string& id_str2 = _idMap[t.GetId()];
                        if(id_str2 == "") throw *new std::runtime_error("error ID or something else wrong.");
                        return _context[id_str2];
                    }
                case token_enum::ImmediateValue:
                    {
                        return _immMap[t.GetId()];
                    }
                default:
                    throw *new std::runtime_error("unexcepted token.");
            }
        } //}
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

template class APT<Token>;
template double APT<Token>::EvalSyntaxTree<double>(
        typename MathExpr::Tokenizer<double>::ContextType&      _context,
        typename MathExpr::Tokenizer<double>::IdMapType&        _idMap,
        typename MathExpr::Tokenizer<double>::FuncMapType&      _funcMap,
        typename MathExpr::Tokenizer<double>::ImmediateMapType& _immMap
        );

}
#endif // _EXPR_PARSE_TREE_HPP
