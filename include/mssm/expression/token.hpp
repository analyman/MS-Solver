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
#include<cmath>

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

extern const char* alpha_num;
extern const char* __num_dot;

enum token_enum //{
{
    OperatorA,     // ^
    OperatorB,     // *, /
    OperatorC,     // +, -
    OperatorD,     // =
    LP,            // left parentheses
    RP,            // right parentheses
    Delimiter,     // Delimiter ,
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

template<typename T> class APT;
class Token //{
{
    public:
        typedef unsigned int      IdType;
        typedef unsigned short LevelType;
        typedef size_t           PosType;
    private:
        template<typename T>
        friend std::ostream& operator_out_aux(std::ostream&, const APT<T>&);
        IdType        m_id;
        token_enum    m_token_type;
        operator_type m_op_type;
        LevelType     m_level;
        PosType       m_pos;
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
        inline PosType&      GetPos()          {return this->m_pos;}
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

        case token_enum::Delimiter:
            o_s << "Delimiter - ','}";
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
class FunctionMap //{
{
    public:
    typedef T value_type;
    typedef value_type (*nonary_func )();
    typedef value_type (*unary_func  )(value_type);
    typedef value_type (*binary_func )(value_type, value_type);
    typedef value_type (*ternary_func)(value_type, value_type, value_type);

    private:
    std::map<std::string, nonary_func>  m_nonary_func;
    std::map<std::string, unary_func>   m_unary_func;
    std::map<std::string, binary_func>  m_binary_func;
    std::map<std::string, ternary_func> m_ternary_func;

    public:
    static FunctionMap FMap;

    private:
    struct static_constructor {
        static_constructor();
    };
    friend struct static_constructor;
    static static_constructor __static_constructor; // SHOULD BE LAST STATIC MEMBER

    public:
    constexpr FunctionMap(): m_nonary_func(), m_unary_func(), m_binary_func(), m_ternary_func(){}
    constexpr void new_nonary (const std::string& str, const nonary_func&  ff){this->m_nonary_func[str]  = ff;}
    constexpr void new_unary  (const std::string& str, const unary_func&   ff){this->m_unary_func[str]   = ff;};
    constexpr void new_binary (const std::string& str, const binary_func&  ff){this->m_binary_func[str]  = ff;};
    constexpr void new_ternary(const std::string& str, const ternary_func& ff){this->m_ternary_func[str] = ff;};
#define GETFUNC(name) name##_func get_##name(std::string& str) {return this->m_##name##_func[str];}
    GETFUNC(nonary) GETFUNC(unary) GETFUNC(binary) GETFUNC(ternary);
}; //}
#define UNARY_FUNC(name) FunctionMap<double>::FMap.new_unary(#name, ::name);
template<typename T>
inline FunctionMap<T>::static_constructor::static_constructor(){} // static constructor nothing
template<>
inline FunctionMap<double>::static_constructor::static_constructor(){ // static constructor - specification of <double> //{
    UNARY_FUNC(sin); UNARY_FUNC(cos); UNARY_FUNC(tan); UNARY_FUNC(log);
} //}

template<typename T>
FunctionMap<T> FunctionMap<T>::FMap;
template<typename T>
typename FunctionMap<T>::static_constructor FunctionMap<T>::__static_constructor;

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
                TokenIterator(): m_tokenizer(nullptr), m_current_token(){};
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
                TokenIterator  operator ++(int){ TokenIterator ret(*this); ++(*this); return ret;}
                bool operator ==(const TokenIterator& x)   {return this->m_tokenizer == x.m_tokenizer;}
                bool operator !=(const TokenIterator& x) { return !this->operator==(x);}
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

        inline IdMapType&        GetIdMap()        { return this->m_id_map;}
        inline FuncMapType&      GetFuncMap()      { return this->m_func_map;}
        inline ImmediateMapType& GetImmediateMap() { return this->m_immediate_map;}

        void continue_with(const std::string& str){m_parse_str = str; m_pointer = 0;}

        inline IdType GetCurrentId(){return this->m_allocated_id;}

        Tokenizer& operator=(const Tokenizer&) = delete;
        Tokenizer& operator=(Tokenizer&& _oth){ // TODO: context pointer
            this->m_parse_str = _oth.m_parse_str;
            return *this;
        }

        ~Tokenizer() = default;

        bool next(Token& _out);

        inline const std::string& GetParseStr(){return this->m_parse_str;}

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
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case ')':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::RP);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case ',':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::Delimiter);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '^':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorA, operator_type::OP_pow);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '*':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorB, operator_type::OP_time);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '/':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorB, operator_type::OP_div);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '+':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorC, operator_type::OP_plus);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '-':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorC, operator_type::OP_minus);
                    _out.GetPos() = m_pointer;
                    ++m_pointer;
                    return true;
                case '=':
                    _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::OperatorD, operator_type::OP_assign);
                    _out.GetPos() = m_pointer;
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
                _out.GetPos() = m_pointer;
                m_func_map.get(m_allocated_id) = m_parse_str.substr(m_pointer, p - m_pointer);
                m_pointer = p;
                return true;
            } else {
                _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::Id);
                _out.GetPos() = m_pointer;
                m_id_map.get(m_allocated_id) = m_parse_str.substr(m_pointer, p - m_pointer);
                m_pointer = p;
                return true;
            }
        } else {
            SizeType p = m_parse_str.find_first_not_of(__num_dot, m_pointer);
            if(p == std::string::npos) p = m_parse_str.length();
            SizeType s = m_parse_str.find_first_of(".", m_pointer);
            SizeType e = m_parse_str.find_first_of(".", s + 1);
            if(e != std::string::npos && e <= p) 
                throw *new TokenizeException("uncorrect number: " + m_parse_str.substr(m_pointer, p - m_pointer));
            _out = MathExpr::Token(++m_allocated_id, MathExpr::token_enum::ImmediateValue);
            _out.GetPos() = m_pointer;
            double IM = std::stod(m_parse_str.substr(m_pointer, p - m_pointer));
            m_immediate_map.get(m_allocated_id) = IM;
            m_pointer = p;
            return true;
        }
    }
    return true;
} //}

extern template class FunctionMap<double>;
extern template class Tokenizer<double>;
}

#endif // _TOKEN_HPP
