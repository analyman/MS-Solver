/* calculator program */
%{
#include "./mmm_h.h"
#include "./y.tab.h"
#include <ctype.h>
#include <stdlib.h>

#define add_just_column() \
{ \
    _line_num += yyleng; \
    _char_num += yyleng; \
}
#define ADDJUST add_just_column()

#define set_position_info() \
{ \
    yylval.m_com.m_token_info.m_start_line = _line_num; \
    yylval.m_com.m_token_info.m_end_line   = _line_num; \
    yylval.m_com.m_token_info.m_start_pos  = _column_num - yyleng; \
    yylval.m_com.m_token_info.m_end_pos    = _column_num; \
}
#define set_common_field() \
{ \
    set_position_info(); \
    yylval.m_com.m_token_info.m_node                 = new_astnode_v(); \
    yylval.m_func.m_token_info.m_node->m_first_child = NULL; \
    yylval.m_func.m_token_info.m_node->m_next        = NULL; \
    yylval.m_func.m_token_info.m_node->m_parent      = NULL; \
}
#define SETCOMF set_common_field()

extern YYSTYPE yylval;
%}

%option noyywrap
%x comment

_NUM    [1-9][[:digit:]]*(\.[[:digit:]]+)?
_IDE    [_A-z]+[_A-z0-9]*
_OPR    [-+*/^=]
_LPAR   [(]
_RPAR   [)]
_DELIM  [,]
_COM_BG \/\*
_COM_ED \*\/

%%

%{
int _line_num   = 0;
int _column_num = 0;
int _char_num   = 0;
%}

<*>\n ++_line_num; _column_num = 0; {return EOF;} // interactive

{_COM_BG}          BEGIN(comment);
<comment>{_COM_ED} BEGIN(INITIAL);
<comment>.* // IGNORE

{_NUM}               {
                        // NUMBER
                        ADDJUST; SETCOMF;
                        double result = atof(yytext);
                        yylval.m_num.m_token_info.m_token_id            = NUMBER;
                        yylval.m_num.m_token_info.m_node->m_value.m_val = result;
                        yylval.m_num.m_token_info.m_node->m_node_type   = nnum;
                        return NUMBER;
                     }
{_IDE}               {
                        // IDENTIFY
                        ADDJUST; SETCOMF;
                        yylval.m_id.m_token_info.m_token_id            = ID;
                        yylval.m_id.m_token_info.m_node->m_value.m_ptr = get_identify_ptr(yytext);
                        yylval.m_num.m_token_info.m_node->m_node_type  = nidt;
                        return ID;
                     }
{_IDE}\(             {
                        // FUNCTION
                        unput('(');
                        ADDJUST; SETCOMF;
                        char* funcname = (char*)malloc(yyleng + 1);
                        funcname[yyleng] = '\0';
                        memcpy(funcname, yytext, yyleng - 1);
                        yylval.m_func.m_token_info.m_token_id                     = FUNC;
                        yylval.m_func.m_token_info.m_node->m_value.m_func.m_arity = 0;
                        yylval.m_func.m_token_info.m_node->m_value.m_func.m_fname = funcname;
                        yylval.m_num.m_token_info.m_node->m_node_type             = nfunc;
                        return FUNC;
                     }
{_OPR}               {
                        // OPERATOR
                        ADDJUST; SETCOMF;
                        switch(yytext[0]){
                            case '^': yylval.m_opr.m_token_info.m_token_id = OPA;
                                      break;
                            case '+': yylval.m_opr.m_token_info.m_token_id = PLUS;
                                      break;
                            case '-': yylval.m_opr.m_token_info.m_token_id = MINUS;
                                      break;
                            case '*': yylval.m_opr.m_token_info.m_token_id = OPC;
                                      break;
                            case '/': yylval.m_opr.m_token_info.m_token_id = OPC;
                                      break;
                            case '=': yylval.m_opr.m_token_info.m_token_id = OPD;
                                      break;
                            default: return 0;
                        }
                        yylval.m_opr.m_token_info.m_node->m_value.m_operator[0] = 1;
                        yylval.m_opr.m_token_info.m_node->m_value.m_operator[1] = yytext[0];
                        yylval.m_num.m_token_info.m_node->m_node_type           = noper;
                        return yylval.m_opr.m_token_info.m_token_id;
                     }
{_LPAR}              {
                        // LPARA
                        ADDJUST; set_position_info();
                        return LPARA;
                     }
{_RPAR}              {
                        // RPARA
                        ADDJUST; set_position_info();
                        return RPARA;
                     }
{_DELIM}             {
                        // DELIM
                        printf("find delimiter.\n");
                        ADDJUST; set_position_info();
                        return DELIM;
                     }
[[:blank:]]*         {ADDJUST;}
<<EOF>>              {return EOF;}

%%

/*
int main()
{
    yyin = stdin;
    while(1){printf("yylex() is called.\n");yylex();}
    return 0;
}
*/
