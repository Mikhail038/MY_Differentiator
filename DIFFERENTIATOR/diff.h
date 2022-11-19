#ifndef DIFF_H
#define DIFF_H

#include <stddef.h>
#include <stdio.h>


//=================================================================================================================================================================================================================

enum ENodeType
{
    Operation = 1,
    Value     = 2,
    Variable  = 3
};

#define DEF_OP(e_num, num, line) \
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
    const char* var;
} UData;

typedef struct SNode
{
    SNode*      left   = NULL;
    SNode*      right  = NULL;
    ENodeType   type   = Operation;
    UData       data   = {};
    SNode*      parent = NULL;
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
    size_t   size   = 0;
    EBranch  branch = Left;
    char*    Array  = NULL;
} SBuffer;

//=================================================================================================================================================================================================================

void test_main (void);

//=================================================================================================================================================================================================================

void seek (SBuffer* Buffer);

void seek_out (SBuffer* Buffer);

//=================================================================================================================================================================================================================

SNode* read_eq (SNode* Root);

SNode* read_node (SBuffer* Buffer, SNode* Parent, EBranch Branch);

//=================================================================================================================================================================================================================

void correct_node_df (SNode* Node, UData Data);

void fill_node (SNode* Node, UData Data, ENodeType Type);

SNode* add_left_branch (SNode* Node, SNode* Parent);

SNode* add_right_branch (SNode* Node, SNode* Parent);

int delete_tree (SNode* Node);

//=================================================================================================================================================================================================================

void print_preorder (SNode* Node);

void print_inorder (SNode* Node);

void print_postorder (SNode* Node);

void print_node (SNode* Node);

//=================================================================================================================================================================================================================


#endif



