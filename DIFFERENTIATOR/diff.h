#ifndef DIFF_H
#define DIFF_H

#include <stddef.h>
#include <stdio.h>


//=================================================================================================================================================================================================================

enum ENodeType
{
    TOperation = 1,
    TValue     = 2,
    TVariable  = 3
};

#define DEF_OP(e_num, num, ...) \
    e_num = num,

enum EOperations
{
    #include "DEF_Operations.h"
};

#undef DEF_OP


typedef union
{
    EOperations  op;
    double      val;
    char        var; //maybe const char* or int ??
} UData;

typedef struct SNode
{
    int         priority = 0;
    SNode*      left     = NULL;
    SNode*      right    = NULL;
    ENodeType   type     = TOperation;
    UData       data     = {};
    SNode*      parent   = NULL;
}
SNode;

//=================================================================================================================================================================================================================

enum EBranch
{
    Left  = -1,
    Right =  1
};

typedef struct
{
    size_t   ip     = 0;
    EBranch  branch = Left;
    char*    Array  = NULL;
} SBuffer;

//=================================================================================================================================================================================================================

enum ETreeMode
{
    MODE_FUN = 1,
    MODE_DER = 2,
    MODE_EQ  = 3
};

//=================================================================================================================================================================================================================
//
//  Main        ::= Add$
//  Add         ::= Mul{[+-]Mul}*
//  Mul         ::= Pow{[*/]Pow}*
//  Pow         ::= Bracket{[^]Bracket}*
//  Bracket     ::= '('Add')' | Number | Function
//  Number      ::= [eE] | [Pp][Ii] | Int{[.]Frac}?{[eE]Int}?
//  Frac        ::= [0-9]+
//  Int         ::= [0-9]+
//  Function    ::= "sin"Number | "cos"Number | "tg"Number | "ctg"Number | "ln"Number
//  $           ::= [\0]
//
//=================================================================================================================================================================================================================

void test_main (void);

//=================================================================================================================================================================================================================

SNode* get_Main (SBuffer* Buffer);

SNode* get_Add (SBuffer* Buffer);

SNode* get_Mul (SBuffer* Buffer);

SNode* get_Pow (SBuffer* Buffer);

SNode* get_Bracket (SBuffer* Buffer);

SNode* get_Variable (SBuffer* Buffer);

SNode* get_Function (SBuffer* Buffer);

SNode* get_Number (SBuffer* Buffer);

double get_Frac (SBuffer* Buffer);

double get_Int (SBuffer* Buffer);

void get_$ (SBuffer* Buffer);

//=================================================================================================================================================================================================================

SNode* diff_tree (SNode* Root);

SNode* diff_node (SNode* Node);

SNode* copy_node (SNode* Original);

void collapse_tree (SNode* Root);

int clean_const (SNode* Node);

int clean_zero (SNode* Node);

int find_vars (SNode* Node);

//=================================================================================================================================================================================================================

void seek (SBuffer* Buffer);

void seek_out (SBuffer* Buffer);

void remove_spaces (SBuffer* Buffer);

//=================================================================================================================================================================================================================

SNode* read_eq (SNode* Root);

SNode* read_node (SBuffer* Buffer, SNode** Node, EBranch Branch);

//=================================================================================================================================================================================================================

SNode* create_OP_node (EOperations OP, SNode* Left, SNode* Right);

SNode* create_VAR_node (char Variable);

SNode* create_VAL_node (double Value);

void refactor_left (SNode* Node);

void refactor_right (SNode* Node);

void correct_node (SNode* Node, UData Data);

void correct_node_df (SNode* Node, UData Data);

void fill_node (SNode* Node, UData Data, ENodeType Type);

SNode* add_left_branch (SNode* Node, SNode* Parent);

SNode* add_right_branch (SNode* Node, SNode* Parent);

int delete_tree (SNode** Node);

//=================================================================================================================================================================================================================

void print_preorder (SNode* Node);

void print_inorder (SNode* Node);

void print_postorder (SNode* Node);

void print_node (SNode* Node);

void find_all_parents (SNode* Root);

void find_parent (SNode* Node, SNode* Parent);

//=================================================================================================================================================================================================================

FILE* tex_head (void);

void tex_tail (FILE*);

void tex_tree (FILE* TEXFile, SNode* Root, ETreeMode Mode);

void tex_node (FILE* TEXFile, SNode* Root);

void tex_print (FILE* TEXFile, SNode* Node);

void do_pdf (const char* TEXName);

//=================================================================================================================================================================================================================

#endif

