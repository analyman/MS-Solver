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
        APT*           m_child[3];
        token_type     m_token;
        Operand_type   m_operand_type;
        APT*           m_parent;
        bool           m_isvalid;
        result_id_type m_result_id;

        void init_result(){m_isvalid = false; m_result_id = 0;}

    public:
        APT() = delete;
        APT(const token_type& token, const Operand_type& t): 
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Leaf);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
        }
        APT(const token_type& token, const Operand_type& t, APT* op1): 
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Unary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
        }
        APT(const token_type& token, const Operand_type& t, APT* op1, APT* op2): 
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Binary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
                m_child[1] = op2;
        }
        APT(const token_type& token, const Operand_type& t, APT* op1, APT* op2, APT* op3): 
            m_token(token), m_operand_type(t){
                assert(this->m_operand_type == Operand_type::Binary);
                ::memset(m_child, '\0', 3 * sizeof(APT*));
                m_child[0] = op1;
                m_child[1] = op2;
                m_child[2] = op3;
        }
        ~APT(){
            switch(this->m_operand_type){
                case Operand_type::Leaf:
                    break;
                case Operand_type::Unary:
                    delete m_child[0];
                case Operand_type::Binary:
                    delete m_child[0];
                    delete m_child[1];
                case Operand_type::Ternary:
                    delete m_child[0];
                    delete m_child[1];
                    delete m_child[2];
            }
        }

        APT*& GetParent(){return this->m_parent;}
        Token& GetToken(){return this->m_token;}
}; //}

}
#endif // _EXPR_PARSE_TREE_HPP
