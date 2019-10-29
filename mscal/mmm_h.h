#ifndef _MMM_H_H_
#define _MMM_H_H_
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef int loc_t;
typedef int tid_t;

typedef enum {nnone, nfunc, nidt, noper, nnum} NodeType;

typedef struct _ASTNode {
    struct _ASTNode* m_first_child;
    struct _ASTNode* m_next;
    struct _ASTNode* m_parent;
    NodeType         m_node_type;
    union {
        double m_val;          // number
        void*  m_ptr;          // identify
        struct func_info {
            int   m_arity;
            char* m_fname;
        }      m_func;         // function
        char   m_operator[4];  // operator
    }                m_value;
} ASTNode;

// Functions
ASTNode* new_astnode(ASTNode* first_child);
ASTNode* new_astnode_v();
int      delete_astnode(ASTNode* node);
double*  get_identify_ptr(char* id);
void     show_context();
char*    get_context();

// variables
extern ASTNode* parser_astnode;

typedef struct _TokenCom {
    loc_t    m_start_pos;
    loc_t    m_end_pos;
    loc_t    m_start_line;
    loc_t    m_end_line;
    tid_t    m_token_id;
    ASTNode* m_node;
} TokenCom;

typedef struct _Opeartor {
// 0     1      2          3
// long  char_1 char_2(op) char_3(op)
    TokenCom m_token_info;
    char     m_op[4];
} _Operator;

typedef struct _Identify {
    TokenCom m_token_info;
} _Identify;

typedef struct _Num {
    TokenCom m_token_info;
} _Num;

typedef struct _Func {
    TokenCom m_token_info;
    char     m_arity;
} _Func;

typedef struct _Common {
    TokenCom m_token_info;
} _Common;

typedef union {
    _Identify m_id;
    _Num      m_num;
    _Func     m_func;
    _Operator m_opr;
    _Common   m_com;
} DUMMYSTYPE;

#endif // _MMM_H_H_
