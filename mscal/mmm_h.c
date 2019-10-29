#include "./mmm_h.h"
#include <stdlib.h>
#include <assert.h>

ASTNode* new_astnode(ASTNode* first_child)
{
    ASTNode* result       = (ASTNode*)malloc(sizeof(ASTNode));
    result->m_first_child = first_child;
    result->m_next        = NULL;
    result->m_parent      = NULL;
    result->m_node_type   = nnone;
    return result;
}

ASTNode* new_astnode_v()
{
    ASTNode* result       = (ASTNode*)malloc(sizeof(ASTNode));
    result->m_first_child = NULL;
    result->m_next        = NULL;
    result->m_parent      = NULL;
    result->m_node_type   = nnone;
    return result;
}

int __delete_astnode(ASTNode* node)
{
    int result = 0;
    if(node == NULL) return result;
    if(node->m_node_type == nfunc)
        free(node->m_value.m_func.m_fname); // function name
    while(1){
        ASTNode* cur = node->m_first_child;
        if(cur == NULL) break;
        node->m_first_child = cur->m_next;
        result += __delete_astnode(cur);
    }
    free(node);
    result += 1;
    return result;
}

int delete_astnode(ASTNode* node)
{
    assert(node->m_next == NULL);
    return __delete_astnode(node);
}
