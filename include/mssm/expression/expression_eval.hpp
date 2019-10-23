#ifndef _EXPRESSIOIN_EVAL_HPP
#define _EXPRESSIOIN_EVAL_HPP

#include "token.hpp"
#include "expr_parse_tree.hpp"
#include "../utils/exception.hpp"
#include <stack>
#include <type_traits>
#include <stdexcept>
#include <sstream>

#include <cstdlib>

#define safedelete(ptr) if(ptr != nullptr){delete ptr; ptr = nullptr;}

namespace MathExpr {

template<typename T>
struct _TernaryHolder //{
{
    T* m_cons1;
    T* m_cons2;
    T* m_var1;
    bool m_1;
    bool m_2;
    bool m_3;
}; //}

template<typename T>
class CircularList //{
{
    public:
        typedef T      value_type;
        typedef size_t size_type;
    private:
        T* m_data;
        size_type m_head;
        size_type m_size;
        size_type m_capacity;

    public:
        CircularList() = delete;
        CircularList(const size_type& max): m_head(0), m_size(0), m_capacity(max){
            m_data = (T*)malloc(sizeof(T) * m_capacity);
        }
        ~CircularList(){free(m_data);}
        CircularList& operator=(const value_type& oth){ // TODO: copy assignment
            if(&oth == this) return *this;
        }

        void clear_all(){
            if(m_size == 0){m_head = 0; return;}
            for(;m_size>=1;m_size--){
                (m_data + (m_head + m_size - 1) % m_capacity)->~value_type();
            }
            m_head = 0; return;
        }

        bool push_front(const value_type& v){
            if(m_size == m_capacity) return false;
            ++m_size;
            if(m_head == 0)
                m_head = m_capacity - 1;
            else 
                --m_head;
            new(m_data + m_head) value_type(v);
            return true;
        }
        bool push_back(const value_type& v){
            if(m_size == m_capacity) return false;
            ++m_size;
            new(m_data + (m_size + m_head - 1) % m_capacity) value_type(v);
            return true;
        }

        inline bool      empty(){return this->m_size == 0;}
        inline bool      full(){return this->m_size == this->m_capacity;}
        inline size_type size(){return this->m_size;}

        value_type pop_front()
        {
            if(empty()) throw *new std::out_of_range("Pop element in empty list");
            value_type x = std::move(*(this->m_data + this->m_head));
            (this->m_data + this->m_head)->~value_type();
            if(m_head == m_capacity - 1) m_head = 0; else --m_head;
            m_size--;
            return x;
        }

        value_type pop_back()
        {
            if(empty()) throw *new std::out_of_range("Pop element in empty list");
            value_type x = std::move(*(this->m_data + (m_head + m_size - 1) % m_capacity));
            (m_data + (m_head + m_size - 1) % m_capacity)->~value_type();
            m_size--;
            return x;
        }

        value_type& operator[](size_type i){
            if(i >= m_size) throw *new std::out_of_range("[] operator out of range.");
            return *(m_data + (m_head + i) % m_capacity);
        }
}; //}

enum accept_token_type //{
{
    id_func_lp_im,  // BEGIN, AFTER_LP, AFTER_OP-ABCD, AFTER_DELIMITER
    op_abc_rp_del,  // AFTER_RP, AFTER_IM
    op_abcd_rp_del, // AFTER_ID
    lp              // AFTER_FUNC
}; //}

bool accept_match(accept_token_type at, token_enum t) //{
{
    switch(at){
        case id_func_lp_im:
            if(t == token_enum::Id ||
                    t == token_enum::Function ||
                    t == token_enum::LP ||
                    t == token_enum::ImmediateValue)
                return true; else return false;
        case op_abc_rp_del:
            if(t == token_enum::OperatorA ||
                    t == token_enum::OperatorB ||
                    t == token_enum::RP ||
                    t == token_enum::Delimiter ||
                    t == token_enum::OperatorC )
                return true; else return false;
        case op_abcd_rp_del:
            if(t == token_enum::OperatorA ||
                    t == token_enum::OperatorB ||
                    t == token_enum::RP ||
                    t == token_enum::Delimiter ||
                    t == token_enum::OperatorC ||
                    t == token_enum::OperatorD )
                return true; else return false;
        case lp:
            if(t == token_enum::LP) return true; else return false;
        default: return false;
    }
} //}
accept_token_type nextAcceptance(token_enum t) //{
{
    switch(t){
        case token_enum::LP:
        case token_enum::OperatorA:
        case token_enum::OperatorB:
        case token_enum::OperatorC:
        case token_enum::OperatorD:
        case token_enum::Delimiter:
            return accept_token_type::id_func_lp_im;
        case token_enum::RP:
        case token_enum::ImmediateValue:
            return accept_token_type::op_abc_rp_del;
        case token_enum::Id:
            return accept_token_type::op_abcd_rp_del;
        case token_enum::Function:
            return accept_token_type::lp;
    }
} //}

class ParseXException: public SMSolver::SMSolverException {
    public:
    ParseXException(const char*        s): SMSolver::SMSolverException(s){}
    ParseXException(const std::string& s): SMSolver::SMSolverException(s){}
    const char* what() const noexcept{std::cout << this->msg << std::endl; return this->msg.c_str();}
};

template<typename T> class MathExprEvalS;
#define TOKEN_BUF_SIZE 10
template<typename T>
class MathExprEval //{
{
    typedef T                                     ValType;
    typedef typename Token::LevelType           LevelType;
    typedef typename Tokenizer<double>::SizeType SizeType;

    private:
        friend class MathExprEvalS<T>;
        Tokenizer<ValType>      m_tokenizer;
        std::stack<Token>       m_operator_stack;
        std::stack<APT<Token>*> m_val_stack;
        LevelType               m_current_level;
        accept_token_type       m_accept_type;

        // save the previous token
        decltype(m_tokenizer.begin()) m_iter;
        decltype(m_tokenizer.end())   m_iter_end;
        SizeType                      m_prev_counter;
        CircularList<Token>           m_token_buf;

        bool getNext(Token& _out) //{
        {
            if(m_prev_counter == 0){
                if(m_iter_end == m_iter) return false;
                if(m_token_buf.size() == TOKEN_BUF_SIZE)
                    m_token_buf.pop_back();
                m_token_buf.push_front(*m_iter);
                _out = *m_iter;
                ++m_iter;
                return true;
            } else {
                _out = m_token_buf[--m_prev_counter];
                return true;
            }
        } //}
        bool setPrev() //{
        {
            if(m_prev_counter==TOKEN_BUF_SIZE || m_prev_counter == m_token_buf.size())
                return false;
            m_prev_counter++; return true;
        } //}

#define EVAL_BINARY_OPERATOR  \
    first  = m_val_stack.top(); \
    m_val_stack.pop(); \
    second = m_val_stack.top(); \
    m_val_stack.pop(); \
    if(!(first->GetToken().GetLevel() == op.GetLevel() && second->GetToken().GetLevel() == op.GetLevel())){ \
        m_val_stack.push(second); m_val_stack.push(first);return;} \
    \
    eval__              = new APT<Token>(op, Operand_type::Binary, second, first); \
    first->GetParent()  = eval__; \
    second->GetParent() = eval__; \
    m_operator_stack.pop(); \
    m_val_stack.push(eval__);

    void op_eval_stack() //{
    {
        assert(!m_val_stack.empty());
        if(m_operator_stack.empty()) return;
        Token& op = m_operator_stack.top();
        Token test;
        APT<Token>* first  = nullptr;
        APT<Token>* second = nullptr;
        APT<Token>* eval__ = nullptr;
        switch(op.GetType()){
            case token_enum::OperatorA:
                EVAL_BINARY_OPERATOR;
                op_eval_stack();
                return;
            case token_enum::OperatorB:
                if(!getNext(test)){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                this->setPrev();
                if(test.GetType() !=  token_enum::OperatorA){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                return;
            case token_enum::OperatorC:
                if(!getNext(test)){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                this->setPrev();
                if(test.GetType() != token_enum::OperatorA && 
                   test.GetType() != token_enum::OperatorB){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                return;
            case token_enum::OperatorD:
                if(!getNext(test)){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                this->setPrev();
                if(test.GetType() != token_enum::OperatorA && 
                   test.GetType() != token_enum::OperatorB && 
                   test.GetType() != token_enum::OperatorC){
                    EVAL_BINARY_OPERATOR;
                    op_eval_stack();
                    return;
                }
                return;
            default: return;
        }
    } //}

void func_eval_stack() //{
{
    assert(!m_val_stack.empty());
    if(m_operator_stack.empty()){m_val_stack.top()->GetToken().GetLevel()--; return;}
    Token& op = m_operator_stack.top();
    APT<Token>* tp     = nullptr;
    APT<Token>* second = nullptr;
    APT<Token>* third  = nullptr;
    tp = m_val_stack.top();
    Operand_type func_arity = Operand_type::Leaf;
    if(op.GetType() != token_enum::Function) { // operator
        tp->GetToken().GetLevel()--;
        if(tp->GetToken().GetLevel() == op.GetLevel()){
            op_eval_stack();
        }
        return;
    }

    if(tp->GetToken().GetLevel() == op.GetLevel() + 1)
        func_arity = Operand_type::Unary;
    else goto handle_func;
    m_val_stack.pop();

    if(m_val_stack.empty()) goto handle_func;
    second = m_val_stack.top();
    if(second->GetToken().GetLevel() == op.GetLevel() + 1)
        func_arity = Operand_type::Binary;
    else goto handle_func;
    m_val_stack.pop();

    if(m_val_stack.empty()) goto handle_func;
    third = m_val_stack.top();
    if(third->GetToken().GetLevel() == op.GetLevel() + 1)
        func_arity = Operand_type::Ternary;
    else goto handle_func;
    m_val_stack.pop();

    if(!m_val_stack.empty() && m_val_stack.top()->GetToken().GetLevel() == op.GetLevel() + 1)
        throw *new ParseXException("function call with number of arguments above 3 is unsupported.");

handle_func:
    APT<Token>* new_node = nullptr;
    m_operator_stack.pop();
    switch(func_arity){
        case Operand_type::Leaf:
            new_node = new APT<Token>(op, func_arity);
            break;
        case Operand_type::Unary:
            new_node        = new APT<Token>(op, func_arity, tp);
            tp->GetParent() = new_node;
            break;
        case Operand_type::Binary:
            new_node            = new APT<Token>(op, func_arity, second, tp);
            tp->GetParent()     = new_node;
            second->GetParent() = new_node;
            break;
        case Operand_type::Ternary:
            new_node            = new APT<Token>(op, func_arity, third, second, tp);
            tp->GetParent()     = new_node;
            second->GetParent() = new_node;
            third->GetParent()  = new_node;
            break;
    }
    m_val_stack.push(new_node);
    op_eval_stack();
    return;
} //}

void initialization() //{
{
    this->m_iter     = m_tokenizer.begin();
    this->m_iter_end = m_tokenizer.end();
    if(m_iter != m_iter_end){
        m_token_buf.push_front(*m_iter);
        ++m_iter;
    }
    else
        m_prev_counter = 0;
} //}
void clean_stack() //{
{
    while(!m_val_stack.empty())      m_val_stack.pop();
    while(!m_operator_stack.empty()) m_operator_stack.pop();
} //}

    public:
        MathExprEval() = delete;
        inline Tokenizer<ValType>& GetTokenizer(){return this->m_tokenizer;}
        MathExprEval(const std::string& str): //{
            m_tokenizer(str), 
            m_current_level(0), 
            m_accept_type(id_func_lp_im),
            m_prev_counter(1),
            m_token_buf(TOKEN_BUF_SIZE){initialization();}  //}
        APT<Token>* GetParseTree() //{
        {
            MathExpr::APT<Token>* push_apt = nullptr;
            for(Token t_t;getNext(t_t);){
                t_t.GetLevel() = this->m_current_level;
                if(!accept_match(this->m_accept_type, t_t.GetType())){
                    const std::string& p_str = m_tokenizer.GetParseStr();
                    throw *new ParseXException("Invalid token at: " + p_str.substr(t_t.GetPos()));
                } else {
                    this->m_accept_type = nextAcceptance(t_t.GetType());
                }
                switch(t_t.GetType()){
                    case token_enum::OperatorA:
                    case token_enum::OperatorB:
                    case token_enum::OperatorC:
                    case token_enum::OperatorD:
                    case token_enum::Function :
                        m_operator_stack.push(t_t);
                        continue;
                    case token_enum::LP:
                        ++m_current_level;
                        continue;
                    case token_enum::RP:
                        --m_current_level;
                        this->func_eval_stack();
                        continue;
                    case token_enum::Delimiter:
                        continue;
                    case token_enum::Id:
                    case token_enum::ImmediateValue:
                        push_apt = new MathExpr::APT<Token>(t_t, Operand_type::Leaf);
                        m_val_stack.push(push_apt);
                        this->op_eval_stack();
                        continue;
                }
            }
            if(m_operator_stack.empty() && this->m_current_level == 0 && m_val_stack.size() <= 1){
                if(m_val_stack.size() == 1)
                    return m_val_stack.top();
                else return nullptr; // empty syntax tree
            }
            else
                throw *new ParseXException("Parse Error.");
        } //}
}; //}

template<typename T>
class MathExprEvalS: public MathExprEval<T> //{
{
    public:
    typedef typename MathExprEval<T>::SizeType            SizeType;
    typedef typename MathExprEval<T>::LevelType           LevelType;
    typedef typename MathExprEval<T>::ValType             ValType;
    typedef typename Tokenizer<ValType>::ContextType      ContextType;
    typedef typename Tokenizer<ValType>::FuncMapType      FuncMapType;
    typedef typename Tokenizer<ValType>::IdMapType        IdMapType;
    typedef typename Tokenizer<ValType>::ImmediateMapType ImmediateMapType;
    typedef typename Token::IdType                        IdType;

    private:
    APT<Token>* m_current_ast;
    ContextType m_context;

    template<typename TT>
    std::ostringstream& toStringAux(std::ostringstream& o_s, const APT<TT>& t, const token_enum& t_type = token_enum::LP) //{
    {
        const TT& c = t.m_token;
        FuncMapType&      fmap  = this->m_tokenizer.GetFuncMap();
        ImmediateMapType& imap  = this->m_tokenizer.GetImmediateMap();
        IdMapType&        idmap = this->m_tokenizer.GetIdMap();
        std::string f_name;
        switch(c.m_token_type){
            case token_enum::Id:
                o_s << idmap[c.m_id];
                break;
            case token_enum::ImmediateValue:
                o_s << imap[c.m_id];
                break;
            case token_enum::OperatorA:
                toStringAux(o_s, *t.m_child[0], token_enum::OperatorA);
                o_s << "^";
                toStringAux(o_s, *t.m_child[1], token_enum::OperatorA);
                break;
            case token_enum::OperatorB:
                if(t_type == token_enum::OperatorA)
                    o_s << "(";
                toStringAux(o_s, *t.m_child[0], token_enum::OperatorB);
                if(c.m_op_type == operator_type::OP_time)
                    o_s << " * ";
                else
                    o_s << " / ";
                toStringAux(o_s, *t.m_child[1], token_enum::OperatorB);
                if(t_type == token_enum::OperatorA)
                    o_s << ")";
                break;
            case token_enum::OperatorC:
                if(t_type == token_enum::OperatorA || t_type == token_enum::OperatorB)
                    o_s << "(";
                toStringAux(o_s, *t.m_child[0], token_enum::OperatorC);
                if(c.m_op_type == operator_type::OP_plus)
                    o_s << " + ";
                else
                    o_s << " - ";
                toStringAux(o_s, *t.m_child[1], token_enum::OperatorC);
                if(t_type == token_enum::OperatorA || t_type == token_enum::OperatorB)
                    o_s << ")";
                break;
            case token_enum::OperatorD:
                if(t_type == token_enum::OperatorA || t_type == token_enum::OperatorB || t_type == token_enum::OperatorC)
                    o_s << "(";
                toStringAux(o_s, *t.m_child[0]);
                o_s << " = ";
                toStringAux(o_s, *t.m_child[1]);
                if(t_type == token_enum::OperatorA || t_type == token_enum::OperatorB || t_type == token_enum::OperatorC)
                    o_s << ")";
                break;
            case token_enum::Function:
                switch(t.m_operand_type){
                    case Operand_type::Leaf:
                        o_s << fmap[c.m_id] << "()";
                        break;
                    case Operand_type::Unary:
                        o_s << fmap[c.m_id] << "(";
                        toStringAux(o_s, *t.m_child[0]); 
                        o_s << ")";
                        break;
                    case Operand_type::Binary:
                        o_s << fmap[c.m_id] << "(";
                        toStringAux(o_s, *t.m_child[0]); 
                        o_s << ", ";
                        toStringAux(o_s, *t.m_child[1]); 
                        o_s << ")";
                        break;
                    case Operand_type::Ternary:
                        o_s << fmap[c.m_id] << "(";
                        toStringAux(o_s, *t.m_child[0]); 
                        o_s << ", ";
                        toStringAux(o_s, *t.m_child[1]); 
                        o_s << ", ";
                        toStringAux(o_s, *t.m_child[2]); 
                        o_s << ")";
                        break;
                }
            default:
                break;
        }
        return o_s;
    } //}
    APT<Token>* helper_derivative_of(const std::string& _id, APT<Token>* t = nullptr) //{
    {
        if(t == nullptr) t = this->m_current_ast;
        APT<Token>* ret_ast = nullptr;
        APT<Token> *aux1 = nullptr, *aux2 = nullptr, *aux3 = nullptr, *aux4 = nullptr, *aux5 = nullptr;
        if(!t->decsendants_has_id(_id, this->m_tokenizer.GetIdMap())){
            ret_ast = new APT<Token>(Token(this->m_tokenizer.new_imm(0), token_enum::ImmediateValue), Operand_type::Leaf);
            return ret_ast;
        }
        const Token& c = t->m_token;
        FuncMapType&      fmap  = this->m_tokenizer.GetFuncMap();
        ImmediateMapType& imap  = this->m_tokenizer.GetImmediateMap();
        IdMapType&        idmap = this->m_tokenizer.GetIdMap();
        switch(c.m_token_type){
            case token_enum::Id:
                return new APT<Token>(Token(this->m_tokenizer.new_imm(1), token_enum::ImmediateValue), Operand_type::Leaf);
            case token_enum::OperatorA:
                if(t->m_child[0]->decsendants_has_id(_id, this->m_tokenizer.GetIdMap()) && 
                   t->m_child[1]->decsendants_has_id(_id, this->m_tokenizer.GetIdMap()))
                    throw *new ParseXException("Unimplemented");
                if(t->m_child[0]->decsendants_has_id(_id, this->m_tokenizer.GetIdMap())){ // d(f(x)^k)/dx = k * f(x) ^ (k - 1)
                    aux1 = new APT<Token>();
                    *aux1 = *t->m_child[1];
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_imm(1), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux3 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus),
                            Operand_type::Binary, aux1, aux2);
                    aux4 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorA, operator_type::OP_pow),
                            Operand_type::Binary, new APT<Token>(*t->m_child[0]), aux3);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, new APT<Token>(*t->m_child[1]), aux4);
                    return ret_ast;
                } else { // d(k^f(x))/dx = d(f(x))/dx * ln(k) * k^f(x)
                    aux1  = new APT<Token>();
                    *aux1 = *t->m_child[0];
                    aux2  = new APT<Token>();
                    *aux2 = *t->m_child[1];
                    aux2  = this->helper_derivative_of(_id, aux2);
                    aux3  = new APT<Token>(
                            Token(this->m_tokenizer.new_func("log"), token_enum::Function, operator_type::OP_NONE),
                            Operand_type::Unary, aux1);
                    aux4  = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux2, aux3);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, t, aux4);
                    return ret_ast;
                }
            case token_enum::OperatorB:
                if(t->m_token.GetOpType() == operator_type::OP_time){ // f(x) * g(x)
                    aux1 = this->helper_derivative_of(_id, t->m_child[0]);
                    aux2 = this->helper_derivative_of(_id, t->m_child[1]);
                    aux3 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, new APT<Token>(*t->m_child[1]));
                    aux4 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux2, new APT<Token>(*t->m_child[0]));
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_plus),
                            Operand_type::Binary, aux3, aux4);
                    return ret_ast;
                } else { // f(x) / g(x)
                    aux5  = new APT<Token>();
                    *aux5 = *t->m_child[1];
                    aux1  = this->helper_derivative_of(_id, t->m_child[0]);
                    aux2  = this->helper_derivative_of(_id, t->m_child[1]);
                    aux3  = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, new APT<Token>(*t->m_child[1]));
                    aux4  = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux2, new APT<Token>(*t->m_child[0]));
                    aux3  = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus),
                            Operand_type::Binary, aux3, aux4);
                    aux4  = new APT<Token>(
                            Token(this->m_tokenizer.new_imm(2), token_enum::ImmediateValue, operator_type::OP_NONE),
                            Operand_type::Leaf);
                    aux4  = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorA, operator_type::OP_pow),
                            Operand_type::Binary, aux5, aux4);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div),
                            Operand_type::Binary, aux3, aux4);
                    return ret_ast;
                }
            case token_enum::OperatorC:
                aux1 = this->helper_derivative_of(_id, t->m_child[0]);
                aux2 = this->helper_derivative_of(_id, t->m_child[1]);
                if(t->m_token.GetOpType() == operator_type::OP_plus)
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_plus),
                            Operand_type::Binary, aux1, aux2);
                else
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus),
                            Operand_type::Binary, aux1, aux2);
                return ret_ast;
            case token_enum::OperatorD:
                return helper_derivative_of(_id, t->m_child[1]);
            case token_enum::Function: // only unary function
                if(t->m_operand_type == Operand_type::Leaf){
                    ret_ast = new APT<Token>(Token(this->m_tokenizer.new_imm(0), token_enum::ImmediateValue), Operand_type::Leaf);
                    return ret_ast;
                }
                if(t->m_operand_type != Operand_type::Unary)
                    throw *new ParseXException("unexcepted function");
                aux1 = this->helper_derivative_of(_id, t->m_child[0]);
                if(fmap[t->m_token.m_id] == "log"){
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_imm(1), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux3 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div),
                            Operand_type::Binary, aux2, new APT<Token>(*t->m_child[0]));
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux3);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "log2") {
#define E_CONS 2.718281828459045235360287471352662
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_imm(E_CONS), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_func("log2"), token_enum::Function), Operand_type::Unary, aux2);
                    aux4 = new APT<Token>(Token(this->m_tokenizer.new_func("log"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    aux2 = this->helper_derivative_of(_id, aux4);
                    delete aux1; delete aux4;
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux3, aux2);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "log10") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_imm(E_CONS), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_func("log10"), token_enum::Function), Operand_type::Unary, aux2);
                    aux4 = new APT<Token>(Token(this->m_tokenizer.new_func("log"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    aux2 = this->helper_derivative_of(_id, aux4);
                    delete aux1; delete aux4;
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux3, aux2);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "sin") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("cos"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux2);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "cos") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("sin"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_imm(0), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux4 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus),
                            Operand_type::Binary, aux3, aux2);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux4);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "tan") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("cos"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_imm(2), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux4 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorA, operator_type::OP_pow),
                            Operand_type::Binary, aux2, aux3);
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_imm(1), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux5 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div),
                            Operand_type::Binary, aux3, aux4);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux5);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "sinh") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("cosh"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux2);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "cosh") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("sinh"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux2);
                    return ret_ast;
                } else if (fmap[t->m_token.m_id] == "tanh") {
                    aux2 = new APT<Token>(Token(this->m_tokenizer.new_func("cosh"), token_enum::Function), Operand_type::Unary, new APT<Token>(*t->m_child[0]));
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_imm(2), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux4 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorA, operator_type::OP_pow),
                            Operand_type::Binary, aux2, aux3);
                    aux3 = new APT<Token>(Token(this->m_tokenizer.new_imm(1), token_enum::ImmediateValue), Operand_type::Leaf);
                    aux5 = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div),
                            Operand_type::Binary, aux3, aux4);
                    ret_ast = new APT<Token>(
                            Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time),
                            Operand_type::Binary, aux1, aux5);
                    return ret_ast;
                } else {
                    throw *new ParseXException("Derivative of unknown function: " + fmap[t->m_token.GetId()]);
                }
            case token_enum::ImmediateValue:
            case token_enum::Delimiter:
            case token_enum::LP:
            case token_enum::RP:
                throw *new ParseXException("helper_derivative_of(): unexcepted exception.");
        }
    } //}

    public:
    MathExprEvalS(const std::string& str): MathExprEval<ValType>(str), m_current_ast(nullptr), m_context(){this->m_current_ast = this->GetParseTree();}
    MathExprEvalS(): MathExprEval<ValType>(""), m_current_ast(nullptr), m_context(){}

    inline APT<Token>*  GetCurrentAST(){return this->m_current_ast;}
    inline ContextType& GetContext   (){return this->m_context;}

    ValType Eval(APT<Token>* ast) //{
    {
        if(ast == nullptr) throw *new std::invalid_argument("invalid argument - nullptr.");
        return ast->EvalSyntaxTree<ValType>(this->m_context, 
                this->m_tokenizer.GetIdMap(), 
                this->m_tokenizer.GetFuncMap(), 
                this->m_tokenizer.GetImmediateMap());
    } //}
    ValType Eval(){return this->Eval(this->m_current_ast);}

    std::string toString(APT<Token>* ast) //{
    {
        std::ostringstream str;
        this->toStringAux(str, *ast);
        return str.str();
    } //}
    std::string toString() //{
    {
        return toString(this->m_current_ast);
    } //}

    APT<Token>* derivative_of(const std::string& _id, APT<Token>* t = nullptr) //{
    {
        APT<Token>* xxx = this->simplified_ast(this->helper_derivative_of(_id, t));
        xxx->fix_parent();
        return xxx;
    } //}
    APT<Token>* derivative_of(IdType num_id, APT<Token>* t = nullptr) //{
    {
        if(t == nullptr) t = this->m_current_ast;
        const std::string& _id = this->m_tokenizer.GetIdMap()[num_id];
        if(_id == "") throw *new ParseXException("Can't found identify <"   "> in this environment.");
        return this->derivative_of(_id, t);
    } //}

    APT<Token>* simplified_ast(APT<Token>* t) //{
    {
        ValType imm_result;
        APT<Token> *aux1 = nullptr, *aux2 = nullptr, *aux3 = nullptr;
        FuncMapType&      fmap  = this->m_tokenizer.GetFuncMap();
        ImmediateMapType& imap  = this->m_tokenizer.GetImmediateMap();
        IdMapType&        idmap = this->m_tokenizer.GetIdMap();
        if(t->is_constant()) {
            if(t->m_token.GetType() == token_enum::ImmediateValue)
                return t;
            ValType result = this->Eval(t);
            t->clear_children();
            aux1 = new APT<Token>(Token(this->m_tokenizer.new_imm(result), token_enum::ImmediateValue), Operand_type::Leaf);
            *t = std::move(*aux1);
            delete aux1;
            aux1 = nullptr;
            return t;
        }
        switch(t->m_token.m_op_type) //{
        {
            case OP_time:
            case OP_div:
            case OP_plus:
            case OP_minus:
            case OP_pow:
                this->simplified_ast(t->m_child[0]);
                this->simplified_ast(t->m_child[1]);
                if(!(t->m_child[0]->is_constant() || t->m_child[1]->is_constant()))
                    return t;
                break;
            case OP_assign:
                this->simplified_ast(t->m_child[1]);
                return t;
            case OP_NONE:
                switch(t->m_operand_type){
                    case Unary:
                        this->simplified_ast(t->m_child[0]);
                        return t;
                    case Binary:
                        this->simplified_ast(t->m_child[0]);
                        this->simplified_ast(t->m_child[1]);
                        return t;
                    case Ternary:
                        this->simplified_ast(t->m_child[0]);
                        this->simplified_ast(t->m_child[1]);
                        this->simplified_ast(t->m_child[2]);
                        return t;
                    default:
                        return t;
                }
        } //}

        // x + 0, x - 0, x * 0, 0 / x, x^0 etc.
        // x*1, x/1, x^1 etc.
#define __IMM__(x) new APT<Token>(Token(this->m_tokenizer.new_imm(x), token_enum::ImmediateValue), Operand_type::Leaf)
#define _IMM_(x)  (aux2 = __IMM__(x)); delete aux2; aux2 = nullptr;
#define _IMM_ZERO _IMM_(0)
#define _IMM_ONE  _IMM_(1)
        switch(t->m_token.m_op_type) //{
        {
            case OP_time: //{
                if(t->m_child[0]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[0]->m_token.GetId()] == 0){
                        t->clear_children();
                        *t = *_IMM_ZERO;
                        break;
                    } else if(imap[t->m_child[0]->m_token.GetId()] == 1){
                        aux1 = t->m_child[1];
                        t->m_child[1] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                }
                if(t->m_child[1]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[1]->m_token.GetId()] == 0){
                        t->clear_children();
                        *t = *_IMM_ZERO;
                        break;
                    } else if(imap[t->m_child[1]->m_token.GetId()] == 1){
                        aux1 = t->m_child[0];
                        t->m_child[0] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                } break; //}
            case OP_div: //{
                if(t->m_child[0]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[0]->m_token.GetId()] == 0){
                        t->clear_children();
                        *t = *_IMM_ZERO;
                        break;
                    }
                }
                if(t->m_child[1]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[1]->m_token.GetId()] == 1){
                        aux1 = t->m_child[0];
                        t->m_child[0] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                } break; //}
            case OP_plus: //{
                if(t->m_child[0]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[0]->m_token.GetId()] == 0){
                        aux1 = t->m_child[1];
                        t->m_child[1] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                }
                if(t->m_child[1]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[1]->m_token.GetId()] == 0){
                        aux1 = t->m_child[0];
                        t->m_child[0] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                } break; //}
            case OP_minus: //{
                if(t->m_child[1]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[1]->m_token.GetId()] == 0){
                        aux1 = t->m_child[0];
                        t->m_child[0] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                } break; //}
            case OP_pow: //{
                if(t->m_child[1]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[1]->m_token.GetId()] == 0){
                        t->clear_children();
                        *t = *_IMM_ONE;
                        break;
                    } else if(imap[t->m_child[1]->m_token.GetId()] == 1){
                        aux1 = t->m_child[0];
                        t->m_child[0] = nullptr;
                        t->clear_children();
                        *t = std::move(*aux1);
                        delete aux1;
                        break;
                    }
                }
                if(t->m_child[0]->m_token.GetType() == token_enum::ImmediateValue){
                    if(imap[t->m_child[0]->m_token.GetId()] == 0){
                        t->clear_children();
                        *t = *_IMM_ZERO;
                        break;
                    } else if(imap[t->m_child[0]->m_token.GetId()] == 1){
                        t->clear_children();
                        *t = *_IMM_ONE;
                        break;
                    }
                } break; //}
            case OP_NONE:
            case OP_assign:
                throw *new ParseXException("IMPOSSIBLE");
        } //}

        // <cons> * <var> * <cons>
        _TernaryHolder<APT<Token>> th;
        switch(t->m_token.GetType()) //{
        {
            case token_enum::OperatorA:
            case token_enum::OperatorD:
            case token_enum::Delimiter:
            case token_enum::LP:
            case token_enum::RP:
            case token_enum::ImmediateValue:
            case token_enum::Id:
            case token_enum::Function:
                break;
            case token_enum::OperatorB: //{
                if(t->m_child[0]->is_constant()) {
                    th.m_cons1 = t->m_child[0];
                    th.m_1 = true;
                    if(t->m_child[1]->m_token.GetType() != token_enum::OperatorB)
                        break;
                    if(t->m_child[1]->m_child[0]->is_constant()){
                        th.m_cons2 = t->m_child[1]->m_child[0];
                        th.m_var1  = t->m_child[1]->m_child[1];
                        th.m_2 = t->m_token.GetOpType() == operator_type::OP_time;
                        th.m_3 = t->m_token.GetOpType() == t->m_child[1]->m_token.GetOpType();
                        t->m_child[1]->m_child[0] = nullptr;
                        t->m_child[1]->m_child[1] = nullptr;
                        delete t->m_child[1];
                    } else if (t->m_child[1]->m_child[1]->is_constant()){
                        th.m_cons2 = t->m_child[1]->m_child[1];
                        th.m_var1  = t->m_child[1]->m_child[0];
                        th.m_3 = t->m_token.GetOpType() == operator_type::OP_time;
                        th.m_2 = t->m_token.GetOpType() == t->m_child[1]->m_token.GetOpType();
                        t->m_child[1]->m_child[0] = nullptr;
                        t->m_child[1]->m_child[1] = nullptr;
                        delete t->m_child[1];
                    } else break;
                } else if (t->m_child[1]->is_constant()) {
                    th.m_cons1 = t->m_child[1];
                    th.m_1 = t->m_token.GetOpType() == operator_type::OP_time;
                    if(t->m_child[0]->m_token.GetType() != token_enum::OperatorB)
                        break;
                    if(t->m_child[0]->m_child[0]->is_constant()){
                        th.m_cons2 = t->m_child[0]->m_child[0];
                        th.m_var1  = t->m_child[0]->m_child[1];
                        th.m_2 = true;
                        th.m_3 = t->m_child[0]->m_token.GetOpType() == operator_type::OP_time;
                        t->m_child[0]->m_child[0] = nullptr;
                        t->m_child[0]->m_child[1] = nullptr;
                        delete t->m_child[0];
                    } else if (t->m_child[0]->m_child[1]->is_constant()){
                        th.m_cons2 = t->m_child[0]->m_child[1];
                        th.m_var1  = t->m_child[0]->m_child[0];
                        th.m_3 = true;
                        th.m_2 = t->m_child[0]->m_token.GetOpType() == operator_type::OP_time;
                        t->m_child[0]->m_child[0] = nullptr;
                        t->m_child[0]->m_child[1] = nullptr;
                        delete t->m_child[0];
                    } else break;
                } else break;
                if(th.m_1 == th.m_2)
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_time), Operand_type::Binary, 
                            th.m_cons1, th.m_cons2);
                else if (th.m_1 == true)
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div), Operand_type::Binary, 
                            th.m_cons1, th.m_cons2);
                else
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorB, operator_type::OP_div), Operand_type::Binary, 
                            th.m_cons2, th.m_cons1);
                if(th.m_3 && (th.m_1 || th.m_2)) t->m_token.m_op_type = operator_type::OP_time;
                else t->m_token.m_op_type = operator_type::OP_div;
                imm_result = this->Eval(aux1);
                aux3 = __IMM__(imm_result);
                delete aux1; aux1 = nullptr;
                if(th.m_3 == true){
                    t->m_child[0] = th.m_var1;
                    t->m_child[1] = aux3;
                    aux1 = nullptr;
                } else {
                    t->m_child[1] = th.m_var1;
                    t->m_child[0] = aux3;
                    aux1 = nullptr;
                }
                this->simplified_ast(t);
                break; //}
            case token_enum::OperatorC: //{
                if(t->m_child[0]->is_constant()) {
                    th.m_cons1 = t->m_child[0];
                    th.m_1 = true;
                    if(t->m_child[1]->m_token.GetType() != token_enum::OperatorC)
                        break;
                    if(t->m_child[1]->m_child[0]->is_constant()){
                        th.m_cons2 = t->m_child[1]->m_child[0];
                        th.m_var1  = t->m_child[1]->m_child[1];
                        th.m_2 = t->m_token.GetOpType() == operator_type::OP_plus;
                        th.m_3 = t->m_token.GetOpType() == t->m_child[1]->m_token.GetOpType();
                        t->m_child[1]->m_child[0] = nullptr;
                        t->m_child[1]->m_child[1] = nullptr;
                        delete t->m_child[1];
                    } else if (t->m_child[1]->m_child[1]->is_constant()){
                        th.m_cons2 = t->m_child[1]->m_child[1];
                        th.m_var1  = t->m_child[1]->m_child[0];
                        th.m_3 = t->m_token.GetOpType() == operator_type::OP_plus;
                        th.m_2 = t->m_token.GetOpType() == t->m_child[1]->m_token.GetOpType();
                        t->m_child[1]->m_child[0] = nullptr;
                        t->m_child[1]->m_child[1] = nullptr;
                        delete t->m_child[1];
                    } else break;
                } else if (t->m_child[1]->is_constant()) {
                    th.m_cons1 = t->m_child[1];
                    th.m_1 = t->m_token.GetOpType() == operator_type::OP_plus;
                    if(t->m_child[0]->m_token.GetType() != token_enum::OperatorC)
                        break;
                    if(t->m_child[0]->m_child[0]->is_constant()){
                        th.m_cons2 = t->m_child[0]->m_child[0];
                        th.m_var1  = t->m_child[0]->m_child[1];
                        th.m_2 = true;
                        th.m_3 = t->m_child[0]->m_token.GetOpType() == operator_type::OP_plus;
                        t->m_child[0]->m_child[0] = nullptr;
                        t->m_child[0]->m_child[1] = nullptr;
                        delete t->m_child[0];
                    } else if (t->m_child[0]->m_child[1]->is_constant()){
                        th.m_cons2 = t->m_child[0]->m_child[1];
                        th.m_var1  = t->m_child[0]->m_child[0];
                        th.m_3 = true;
                        th.m_2 = t->m_child[0]->m_token.GetOpType() == operator_type::OP_plus;
                        t->m_child[0]->m_child[0] = nullptr;
                        t->m_child[0]->m_child[1] = nullptr;
                        delete t->m_child[0];
                    } else break;
                } else break;
                if(th.m_1 == th.m_2)
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_plus), Operand_type::Binary, 
                            th.m_cons1, th.m_cons2);
                else if (th.m_1 == true)
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus), Operand_type::Binary, 
                            th.m_cons1, th.m_cons2);
                else
                    aux1 = new APT<Token>(Token(this->m_tokenizer.inc_allocated_id(), token_enum::OperatorC, operator_type::OP_minus), Operand_type::Binary, 
                            th.m_cons2, th.m_cons1);
                if(th.m_3 && (th.m_1 || th.m_2)) t->m_token.m_op_type = operator_type::OP_plus;
                else t->m_token.m_op_type = operator_type::OP_minus;
                aux3 = __IMM__(this->Eval(aux1));
                delete aux1;
                if(th.m_3 == true){
                    t->m_child[0] = th.m_var1;
                    t->m_child[1] = aux3;
                    aux1 = nullptr;
                } else {
                    t->m_child[1] = th.m_var1;
                    t->m_child[0] = aux3;
                    aux1 = nullptr;
                }
                this->simplified_ast(t);
                break; //}
        } //}
        return t;
    } //}

    void clean_current_ast() {
        if(nullptr != this->m_current_ast) delete this->m_current_ast;
        this->m_current_ast = nullptr;
    }

    void continue_with(const std::string& str, bool clean = false) //{
    {
        if(clean) this->clean_current_ast();
        this->m_current_ast = nullptr;
        this->m_tokenizer.continue_with(str);
        this->m_token_buf.clear_all();
        this->clean_stack();
        this->m_accept_type = id_func_lp_im,
        this->m_current_level = 0;
        this->m_prev_counter = 1;
        this->initialization();
        this->m_current_ast = this->GetParseTree();
    } //}
}; //}

extern template class MathExprEval<double>;
extern template class MathExprEvalS<double>;

}

#endif // _EXPRESSION_EVAL_HPP
