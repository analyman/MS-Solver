#ifndef _TOKEN_HPP
#define _TOKEN_HPP
/*
 * tokenize a string stream, the result is a stream of token.
 */

#include<map>
#include<vector>
#include<iterator>
#include<string>
#include<cctype>
#include<iostream>
#include<iomanip>

#include<exception>

class TokenizeException //{
{
    private:
        std::string msg;
    public:
        TokenizeException(const std::string& _s): msg(_s){}
        TokenizeException(std::string&& _s): msg(std::move(_s)){}
        TokenizeException(const char* _c_str): msg(_c_str){}

        virtual const char * what() const noexcept {return msg.c_str();}
}; //}

namespace MathExpr {

const char* alpha_num = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char* __num_dot = "0123456789ABCDEFabcdef.";

enum token_enum //{
{
    LP,            // left parentheses
    RP,            // right parentheses
    OperatorA,     // ^
    OperatorB,     // *, /
    OperatorC,     // +, -
    OperatorD,     // =
    Id,            // identify
    Function,      // function name, function name must follow a left parentheses
    ImmediateValue // Immediate Value
}; //}
enum operator_type //{
{
    OP_NONE,  // not operator
    OP_time,  // *
    OP_div,   // /
    OP_plus,  // +
    OP_minus, // -
    OP_pow,   // ^
    OP_assign // =
}; //}
enum func_enum  //{
{
    f_sin,
    f_cos,
    f_tan,
    f_ln,
    f_log,
    f_pow,
    f_sqrt,
    f_max,
    f_min
}; //}

class Token //{
{
    public:
        typedef unsigned int IdType;
        typedef unsigned short LevelType;
    private:
        IdType        m_id;
        token_enum    m_token_type;
        operator_type m_op_type;
        LevelType     m_level;
    public:
        Token() = default;
        Token(const IdType& id, const token_enum& type, const operator_type& op_type = operator_type::OP_NONE): 
            m_id(id), 
            m_token_type(type), 
            m_op_type(op_type){}

        inline IdType        GetId()     const {return this->m_id;}
        inline token_enum    GetType()   const {return this->m_token_type;}
        inline operator_type GetOpType() const {return this->m_op_type;}
        inline LevelType&    GetLevel()        {return this->m_level;}
}; //}

std::ostream& operator<<(std::ostream& o_s, const Token& t) //{
{
    o_s << "{ Id: ";
    o_s << std::setw(3) << std::right <<t.GetId() << ", TokenType: ";
    switch(t.GetType()){
        case token_enum::LP:
            o_s << "Left Parentheses }";
            break;
        case token_enum::RP:
            o_s << "Right parenthese }";
            break;
        case token_enum::Function:
            o_s << "Function }";
            break;
        case token_enum::OperatorA:
            o_s << "^(pow) }";
            break;

        case token_enum::OperatorB:
            if(t.GetOpType() == OP_time)
                o_s << "* }";
            else 
                o_s << "/ }";
            break;
        case token_enum::OperatorC:
            if(t.GetOpType() == OP_plus)
                o_s << "+ }";
            else 
                o_s << "- }";
            break;
        case token_enum::OperatorD:
            o_s << "=(assignment) }";
            break;
        case token_enum::ImmediateValue:
            o_s << "Immediate Value }";
            break;

        case token_enum::Id:
            o_s << "Identify }";
            break;
    }
    return o_s;
} //}

template<typename T1, typename T2>
class ValContext //{
{
    public:
        typedef Token::IdType IdType;
        typedef T1            KeyType;
        typedef T2            ValType;

    private:
        std::map<KeyType, ValType> m_context_map;

    public:
        ValContext() = default;
        ValType& operator[](const KeyType& str){return m_context_map[str];}
        ValType& get       (const KeyType& str){return m_context_map[str];}
}; //}

template<typename T>
class Tokenizer  //{
{
    public:
        typedef T             ValType;
        typedef size_t        SizeType;
        typedef Token::IdType IdType;

        typedef ValContext<std::string, ValType>  ContextType;
        typedef ValContext<IdType, std::string>   IdMapType;
        typedef ValContext<IdType, std::string>   FuncMapType;
        typedef ValContext<IdType, ValType>       ImmediateMapType;

    private:
        class TokenIterator //{
        {
            public:
                typedef size_t                  difference_type;
                typedef Token                   value_type;
                typedef Token*                  pointer;
                typedef Token&                  reference;
                typedef std::input_iterator_tag iterator_category;
            private:
                Tokenizer<ValType>* m_tokenizer;
                Token               m_current_token;
            public:
                TokenIterator(): m_tokenizer(nullptr){};
                TokenIterator(Tokenizer<ValType>* t): m_tokenizer(t){
                    if(nullptr != this->m_tokenizer && m_tokenizer->next(this->m_current_token))
                        return;
                    this->m_tokenizer = nullptr;
                    return;
                }
                TokenIterator(const TokenIterator& t){(*this) = t;}
                // InputIterator Required operator
                reference      operator *()                        { return this->m_current_token;}
                TokenIterator& operator ++()                       {
                    if(this->m_tokenizer->next(this->m_current_token))
                        return *this;
                    this->m_tokenizer = nullptr;
                    return *this;
                }
                TokenIterator  operator ++(int)                    { TokenIterator ret(*this); ++(*this); return ret;}
                bool           operator ==(const TokenIterator&)   { return this->m_tokenizer == nullptr;}
                bool           operator !=(const TokenIterator& x) { return !this->operator==(x);}
                // copy assignment
                TokenIterator& operator=(const TokenIterator& x) = default;
                ~TokenIterator() = default;
        }; //}

    private:
        std::string          m_parse_str;
        SizeType             m_pointer = 0;
        IdType               m_allocated_id = 0;

        IdMapType        m_id_map;
        FuncMapType      m_func_map;
        ImmediateMapType m_immediate_map;

    public:
        Tokenizer() = delete;
        Tokenizer(const std::string&  _str): m_parse_str(_str){}
        Tokenizer(const char*  p_str): m_parse_str(p_str){}
        Tokenizer(std::string&& _str): m_parse_str(std::move(_str)){}
        Tokenizer(const Tokenizer&) = delete;
        Tokenizer(Tokenizer&& _oth) {(*this) = std::move(_oth);};

        IdMapType&        GetIdMap()        { return this->m_id_map;}
        FuncMapType&      GetFuncMap()      { return this->m_func_map;}
        ImmediateMapType& GetImmediateMap() { return this->m_immediate_map;}

        Tokenizer& operator=(const Tokenizer&) = delete;
        Tokenizer& operator=(Tokenizer&& _oth){ // TODO: context pointer
            this->m_parse_str = _oth.m_parse_str;
            return *this;
        }

        ~Tokenizer() = default; // TODO: delete context ?

        bool next(Token& _out); // TODO

        TokenIterator begin(){return TokenIterator(this);};
        TokenIterator end(){return TokenIterator();};
}; //}

template<typename T>
inline bool Tokenizer<T>::next(Token& _out) //{
{
    for(;;){
        if(m_parse_str.length() > this->m_pointer){
            switch (m_parse_str[m_pointer]) {
                case '(':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::LP);
                    ++m_pointer;
                    return true;
                case ')':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::RP);
                    ++m_pointer;
                    return true;
                case '^':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorA, operator_type::OP_pow);
                    ++m_pointer;
                    return true;
                case '*':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorB, operator_type::OP_time);
                    ++m_pointer;
                    return true;
                case '/':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorB, operator_type::OP_div);
                    ++m_pointer;
                    return true;
                case '+':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorC, operator_type::OP_plus);
                    ++m_pointer;
                    return true;
                case '-':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorC, operator_type::OP_minus);
                    ++m_pointer;
                    return true;
                case '=':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorD, operator_type::OP_assign);
                    ++m_pointer;
                    return true;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    ++m_pointer;
                    continue;
                default: break;
            }
        } else return false;

        // Handling when the token is identify or function call
        char c_ = m_parse_str[m_pointer];
        if(!::isalnum(c_)){
            throw *new TokenizeException("Expression lexical Error: " + m_parse_str.substr(m_pointer, 8));
        }
        if(::isalpha(c_)){
            SizeType p = m_parse_str.find_first_not_of(alpha_num, m_pointer);
            if(m_parse_str[p] == '('){
                _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::Function);
                m_func_map.get(m_allocated_id) = m_parse_str.substr(m_pointer, p - m_pointer);
                m_pointer = p;
                return true;
            } else {
                _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::Id);
                m_id_map.get(m_allocated_id) = m_parse_str.substr(m_pointer, p - m_pointer);
                m_pointer = p;
                return true;
            }
        } else {
            SizeType p = m_parse_str.find_first_not_of(__num_dot, m_pointer);
            SizeType s = m_parse_str.find_first_of(".", m_pointer);
            SizeType e = m_parse_str.find_first_of(".", s);
            if(e <= p) throw *new TokenizeException("uncorrect number: " + m_parse_str.substr(m_pointer, p - m_pointer));
            _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::ImmediateValue);
            double IM = std::stod(m_parse_str.substr(m_pointer, p - m_pointer));
            m_immediate_map.get(m_allocated_id) = IM;
            m_pointer = p;
            return true;
        }
    }
    return true;
} //}

template class Tokenizer<double>;
}

#endif // _TOKEN_HPP
