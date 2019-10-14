#ifndef _EXPRESSIOIN_EVAL_HPP
#define _EXPRESSIOIN_EVAL_HPP

#include "token.hpp"
#include "expr_parse_tree.hpp"
#include <stack>
#include <type_traits>
#include <stdexcept>

#include <cstdlib>

namespace MathExpr {

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

class ParseXException //{
{
    private:
        std::string msg;
    public:
        ParseXException(const std::string& _s): msg(_s){}
        ParseXException(std::string&& _s): msg(std::move(_s)){}
        ParseXException(const char* _c_str): msg(_c_str){}

        virtual const char * what() const noexcept {return msg.c_str();}
}; //}

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
        Tokenizer<double>       m_tokenizer; // FIXME:
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

    public:
        MathExprEval() = delete;
        MathExprEval(const std::string& str): //{
            m_tokenizer(str), 
            m_current_level(0), 
            m_accept_type(id_func_lp_im),
            m_prev_counter(1),
            m_token_buf(TOKEN_BUF_SIZE) {
                this->m_iter     = m_tokenizer.begin();
                this->m_iter_end = m_tokenizer.end();
                if(m_iter != m_iter_end){
                    m_token_buf.push_front(*m_iter);
                    ++m_iter;
                }
                else
                    m_prev_counter = 0;
            } //}
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
    typedef typename MathExprEval<T>::SizeType  SizeType;
    typedef typename MathExprEval<T>::LevelType LevelType;
    typedef typename MathExprEval<T>::ValType   ValType;

    private:
    APT<Token>* m_current_ast;

    public:
    MathExprEvalS(const std::string& str): MathExprEval<ValType>(str){}
}; //}

extern template class MathExprEval<double>;

}

#endif // _EXPRESSION_EVAL_HPP
