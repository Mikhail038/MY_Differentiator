//DIFFERENTIATOR FUNCTIONS
//--14.11.22--started
//=================================================================================================================================================================================================================

#include "MYassert.h"

//=================================================================================================================================================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <wchar.h>
#include <locale.h>

//=================================================================================================================================================================================================================

#include "diff.h"

//=================================================================================================================================================================================================================

void test_main (void)
{

    SNode* Root = read_eq (Root);

//     if (read_eq (Root) != 0)
//     {
//         printf (KRED "Error during reading occured!\n" KNRM);
//
//         delete_tree (Root);
//
//         return;
//     }

    printf ("|\n");

    print_inorder (Root);

    printf ("^\n");

    delete_tree (Root);

    return;
}

//=================================================================================================================================================================================================================

void seek (SBuffer* Buffer)
{
    for (; Buffer->ip < Buffer->size; Buffer->ip++)
    {
        if ((Buffer->Array[Buffer->ip] != '\n') && ((Buffer->Array[Buffer->ip] != ' ')))
        {
            return;
        }
    }

    return;
}

void seek_out (SBuffer* Buffer)
{
    for (; Buffer->ip < Buffer->size; Buffer->ip++)
    {
        if ((Buffer->Array[Buffer->ip] == '\n'))
        {
            Buffer->ip++;
            return;
        }
    }

    return;
}

//=================================================================================================================================================================================================================

SNode* read_eq (SNode* Root)
{
    int ErrCounter = 0;

    SBuffer Buffer = {};

    fscanf (stdin, "%m[^\n.]", &(Buffer.Array));

    Buffer.size = strlen (Buffer.Array);

    Root = read_node (&Buffer, Root, Left);

//     printf ("root %p\n", Root);
//     print_node (Root);
//     printf ("\n");
//
//
//     printf ("root l %p\n", Root->left);
//     printf ("root l %p\n", Root->left);
//     print_node (Root->left);
//     printf ("\n");
//
//     printf ("root r %p\n", Root->right);
//     printf ("root l %p\n", Root->left);
//     print_node (Root->right);
//     printf ("\n");


    // for (; Buffer.ip < Buffer.size; Buffer.ip++)
    // {
    //     ErrCounter += read_node (&Buffer, Root);
    // }

    free (Buffer.Array);

    return Root;
}

// void add_brackets_to_eq (SBuffer* Buffer)
// {
//     Buffer->size += 2;
//     Buffer->Array = (char*) realloc (Buffer->Array, Buffer->size * sizeof (char));
//
//
//     return;
// }

SNode* read_node (SBuffer* Buffer, SNode* Node, EBranch Branch)
{
    Node = (SNode*) calloc (1, sizeof (SNode));
    //Node->left = (SNode*) calloc (1, sizeof (SNode));
    //Node->right = (SNode*) calloc (1, sizeof (SNode));

    seek (Buffer);

    if (Buffer->Array[Buffer->ip] == '(')
    {
        if (Branch == Left)
        {
            printf ("l %p\n", Node);
        }
        else
        {
            printf ("r %p\n", Node);
        }

        Buffer->ip++;

        seek (Buffer);

        if (read_node (Buffer, Node->left, Left) == NULL)
        {
            printf ("^\n");

            Node->type = Node->left->type;
            Node->data = Node->left->data;

            //print_node (Node);

            free (Node->left);
            Node->left = NULL;

            return Node;
        }

        seek (Buffer);

        char* Word = NULL;

        sscanf (&(Buffer->Array[Buffer->ip]), "%m[^ ]", &Word);

        printf ("mid '%s'\n", Word);

        Buffer->ip += strlen (Word);

        seek (Buffer);

        UData Data = {};

        if (0) {}
        #define DEF_OP(e_num, num, line) \
        else if (strcasecmp (Word, line) == 0) \
        { \
            Data.op = e_num; \
            fill_node (Node, Data, Operation); \
        }

        #include "DEF_Operations.h"

        #undef DEF_OP
        else
        {
            printf ("'%s'" KRED " wrong operation\n" KNRM , Word);
        }

        read_node (Buffer, Node->right, Right);

        return Node;
    }
    else
    {
        UData Data = {};

        char* Word = NULL;

        sscanf (&(Buffer->Array[Buffer->ip]), "%m[^)]", &Word);

        if (Branch == Left)
        {
            printf ("lft '%s'\n", Word);
        }
        else
        {
            printf ("rgt '%s'\n", Word);
        }

        if (strtod (Buffer->Array, NULL) != 0)
        {
            Data.val = strtod (Buffer->Array, NULL);
            fill_node (Node, Data, Value);
        }
        else
        {
            Data.var = Word;
            fill_node (Node, Data, Variable);
        }

        Buffer->ip += strlen (Word);

        if (Buffer->Array[Buffer->ip] != ')')
        {
            printf ("'%c' " KRED "wrong symbol (in \"...%.5s...\") expected " KGRN "')'\n" KNRM  , Buffer->Array[Buffer->ip], &(Buffer->Array[Buffer->ip]));

            return NULL;
        }

        Buffer->ip++;

        //printf ("i have memory! %p\n", Node);

        free (Node->left);
        free (Node->right);

        return NULL;
    }
//     else
//     {
//         printf ( "'%c'" KRED " wrong symbol (in \"...%.5s...\") expected " KGRN "'('\n" KNRM , Buffer->Array[Buffer->ip], &(Buffer->Array[Buffer->ip]));
//
//         return 1;
//     }

    return 0;
}

//=================================================================================================================================================================================================================

void correct_node (SNode* Node, UData Data)
{
    Node->data = Data;

    return;
}

void fill_node (SNode* Node, UData Data, ENodeType Type)
{
    Node->type = Type;

    Node->data = Data;

    return;
}

SNode* add_left_branch (SNode* Node, SNode* Parent)
{
    Parent->left = Node;

    Node->parent = Parent;

    return Node;
}

SNode* add_right_branch (SNode* Node, SNode* Parent)
{
    Parent->right = Node;

    Node->parent = Parent;

    return Node;
}

int delete_tree (SNode* Node)
{
    if (Node == NULL)
    {
        return 0;
    }

    if (Node->left != NULL)
    {
        delete_tree (Node->left);
    }

    if (Node->right != NULL)
    {
        delete_tree (Node->right);
    }

    //free (Node->data);

    free (Node);

    return 0;
}

//=================================================================================================================================================================================================================

void print_preorder (SNode* Node)
{
    if (Node == NULL)
    {
        return;
    }

    printf ("(");

    print_node (Node);

    if (Node->left != NULL)
    {
        print_preorder (Node->left);
    }

    if (Node->right != NULL)
    {
        print_preorder (Node->right);
    }

    printf (")");

    return;
}

void print_inorder (SNode* Node)
{
    if (Node == NULL)
    {
        return;
    }

    printf ("(");

    if (Node->left != NULL)
    {
        print_inorder (Node->left);
    }

    print_node (Node);

    if (Node->right != NULL)
    {
        print_inorder (Node->right);
    }

    printf (")");

    return;
}

void print_postorder (SNode* Node)
{
    if (Node == NULL)
    {
        return;
    }

    printf ("(");

    if (Node->left != NULL)
    {
        print_postorder (Node->left);
    }

    if (Node->right != NULL)
    {
        print_postorder (Node->right);
    }

    print_node (Node);

    printf (")");

    return;
}

void print_node (SNode* Node)
{
    switch (Node->type)
    {
        case Operation:
            switch (Node->data.op)
            {
                #define DEF_OP(e_num, num, line) \
                case e_num: \
                    printf ("%s", line); \
                    break;

                #include "DEF_Operations.h"

                #undef DEF_OP

                default:
                    printf (KRED "Op_type default error! '%d'\n" KNRM, Node->data.op);
            }
            break;

        case Value:
            printf ("%lg", Node->data.val);
            break;

        case Variable:
            printf ("%s", Node->data.var);
            break;

        default:
            printf (KRED "Node_type Default error!" KNRM);
    }

    return;
}

//=================================================================================================================================================================================================================
