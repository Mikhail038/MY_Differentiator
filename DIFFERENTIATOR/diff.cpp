//DIFFERENTIATOR FUNCTIONS
//--14.11.22--started
//=================================================================================================================================================================================================================
//DO NOT NAME VARS NOT X!!!
//=================================================================================================================================================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <locale.h>
#include <wchar.h>

//=================================================================================================================================================================================================================

#include "MYassert.h"

//=================================================================================================================================================================================================================

#include "diff.h"

//=================================================================================================================================================================================================================

#define MAX_VARS 10

#define ZERO 0
#define ONE  1

#define VAR_PRIOR -2
#define VAL_PRIOR -2

#define ADD_PRIOR 1
#define SUB_PRIOR 1
#define MUL_PRIOR 3
#define DIV_PRIOR 4
#define POW_PRIOR 5


#define FUNC_PRIOR 100

//=================================================================================================================================================================================================================

#define UNITE(OP, Left, Right) create_OP_node (OP, Left, Right)
#define CR_VAL(value) create_VAL_node (value)

#define CL copy_node (Node->left)
#define CR copy_node (Node->right)

#define DL diff_node (Node->left)
#define DR diff_node (Node->right)

//=================================================================================================================================================================================================================

//#define POISON_VAR -1

// typedef struct
// {
//     char*   name   = NULL;
//     int     label  = POISON_VAR;
// } SVar;
//
// static SVar ArrVars[MAX_VARS]= {};

//=================================================================================================================================================================================================================
//main//
//=================================================================================================================================================================================================================

void test_main (void)
{
    setlocale(LC_CTYPE, "");

    SBuffer Buffer = {};

    fscanf (stdin, "%m[^\n]", &(Buffer.Array));

    SNode* Root = get_Main (&Buffer);
    find_all_parents (Root);

    free (Buffer.Array);

    FILE* TEXFile = tex_head ();

    printf ("function:             ");
    print_inorder (Root);
    tex_tree (TEXFile, Root, MODE_FUN);
    printf ("\n");

    make_gv_tree (Root, "GRAPH_VIZ/gvDiff_func.dot");

    collapse_tree (Root);
    find_all_parents (Root);

    fwprintf (TEXFile, L"\\\\ Упростим по возможности: \\\\");
    printf ("collapsed function:   ");
    print_inorder (Root);
    tex_tree (TEXFile, Root, MODE_FUN);
    printf ("\n");

    SNode* DifRoot = diff_tree (Root);
    find_all_parents (DifRoot);

    fwprintf (TEXFile, L"\\\\ Возьмём производную: \\\\");
    printf ("derivative:           ");
    print_inorder (DifRoot);
    tex_tree (TEXFile, DifRoot, MODE_DER);
    printf ("\n");

    collapse_tree (DifRoot);
    find_all_parents (DifRoot);

    fwprintf (TEXFile, L"\\\\ Упростим по возможности: \\\\");
    printf ("collapsed derivative: ");
    print_inorder (DifRoot);
    tex_tree (TEXFile, DifRoot, MODE_DER);
    printf ("\n");

    tex_tail (TEXFile);

    make_gv_tree (DifRoot, "GRAPH_VIZ/gvDiff_der.dot");

    delete_tree (&Root);

    delete_tree (&DifRoot);

    //do_pdf ("diff.tex");

    return;
}

//=================================================================================================================================================================================================================
//some buffer seekers and removers//
//=================================================================================================================================================================================================================

void seek (SBuffer* Buffer)
{
    while (Buffer->Array[Buffer->ip] != '\0')
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
    while (Buffer->Array[Buffer->ip] != '\0')
    {
        if ((Buffer->Array[Buffer->ip] == '\n'))
        {
            Buffer->ip++;
            return;
        }
    }

    return;
}

void remove_spaces (SBuffer* Buffer)
{
    int counter = 0;
    int inline_counter = 0;


    while (Buffer->Array[counter] != '\0')
    {
        if (Buffer->Array[counter] == ' ')
        {
            inline_counter = counter;

            while (Buffer->Array[inline_counter] != '\0')
            {
                Buffer->Array[inline_counter] = Buffer->Array[inline_counter + 1];  //printf ("%d '%c' <- '%c'\n", inline_counter, Buffer[inline_counter], Buffer[inline_counter + 1]);

                inline_counter++; //printf ("'%c'\n", Buffer[inline_counter]);
            }
        }
        else
        {
            ++counter;
        }
    }

    return;
}

//=================================================================================================================================================================================================================
//reader//
//=================================================================================================================================================================================================================

SNode* get_Main (SBuffer* Buffer)
{
    remove_spaces (Buffer);

    SNode* Son = get_Add (Buffer);

    get_$ (Buffer);

    return Son;
}

SNode* get_Add (SBuffer* Buffer)
{
    SNode* LeftSon = get_Mul (Buffer);

    if ((Buffer->Array[Buffer->ip] == '+') || (Buffer->Array[Buffer->ip] == '-'))
    {
        SNode* Node = (SNode*) calloc (1, sizeof (SNode));

        Node->left = LeftSon;

        char Operation = Buffer->Array[Buffer->ip];
        Buffer->ip++;

        Node->type = TOperation;

        Node->right = get_Add (Buffer);

        if (Operation == '+')
        {
            Node->data.op = ADD;
            Node->priority = ADD_PRIOR;
        }
        else
        {
            Node->data.op = SUB;
            Node->priority = SUB_PRIOR;
        }

        return Node;
    }

    return LeftSon;
}

SNode* get_Mul (SBuffer* Buffer)
{
    SNode* LeftSon = get_Pow (Buffer);

    while ((Buffer->Array[Buffer->ip] == '*') || (Buffer->Array[Buffer->ip] == '/'))
    {
        SNode* Node = (SNode*) calloc (1, sizeof (SNode));

        Node->left = LeftSon;

        char Operation = Buffer->Array[Buffer->ip];
        Buffer->ip++;

        Node->type = TOperation;

        Node->right = get_Mul (Buffer);

        if (Operation == '*')
        {
            Node->data.op  = MUL;
            Node->priority = MUL_PRIOR;
        }
        else
        {
            Node->data.op  = DIV;
            Node->priority = DIV_PRIOR;
        }

        return Node;
    }

    return LeftSon;
}

SNode* get_Pow (SBuffer* Buffer)
{
    SNode* LeftSon = get_Bracket (Buffer);

    while (Buffer->Array[Buffer->ip] == '^')
    {
        SNode* Node = (SNode*) calloc (1, sizeof (SNode));

        Node->left = LeftSon;

        Buffer->ip++;

        Node->type = TOperation;
        Node->priority = POW_PRIOR;

        Node->right = get_Pow (Buffer);

        Node->data.op = POW;

        return Node;
    }

    return LeftSon;
}

SNode* get_Bracket (SBuffer* Buffer)
{
    SNode* Node = NULL;//(SNode*) calloc (1, sizeof (SNode));

    if (Buffer->Array[Buffer->ip] == '(')
    {
        Buffer->ip++;

        Node = get_Add (Buffer);

        MLA (Buffer->Array[Buffer->ip] == ')');

        Buffer->ip++;
    }

    #define DEF_OP(e_num, num, line, size, ...) \
    else if (strncasecmp (&(Buffer->Array[Buffer->ip]), line, size) == 0) \
    {\
        if (size > 1)\
            Node = get_Function (Buffer);\
    }

    #include "DEF_Operations.h"

    #undef DEF_OP

    else if ((Buffer->Array[Buffer->ip] >= '0' && Buffer->Array[Buffer->ip] <= '9') ||
            ((strncasecmp (&(Buffer->Array[Buffer->ip]), "pi", 2) == 0) || (strncasecmp (&(Buffer->Array[Buffer->ip]), "e", 1)) == 0))
    {
        Node = get_Number (Buffer);
    }
    else
    {
        Node = get_Variable (Buffer);
    }

    return Node;
}

SNode* get_Variable (SBuffer* Buffer)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = TVariable;
    Node->priority = VAR_PRIOR;

    Node->data.var = Buffer->Array[Buffer->ip];

    //sscanf (&(Buffer->Array[Buffer->ip]), "%m[^+-*/()\n]", &VarName); //REMAKE IF NEEDED!!
    // MLA (VarName != NULL);

    //size_t Length = strlen (VarName);

    Buffer->ip++;//= Length;

//     for (int counter = 0; counter < MAX_VARS; ++counter)
//     {
//         if (ArrVars[counter].name != NULL)
//         {
//             if (strcmp (VarName, ArrVars[counter].name) == 0)
//             {
//                 free (VarName);
//
//                 Node->data.var = ArrVars[counter].label;
//
//                 return Node;
//             }
//         }
//     }
//
//     for (int counter = 0; counter < MAX_VARS; ++counter)
//     {
//         if (ArrVars[counter].label == POISON_VAR)
//         {
//             ArrVars[counter].name = VarName;
//
//             ArrVars[counter].label = counter;
//
//             Node->data.var = ArrVars[counter].label;
//
//             return Node;
//         }
//     }
//
//     printf ("TOO MUCH VARIABLES!\n");
//
//     for (int counter = 0; counter < MAX_VARS; ++counter)
//     {
//         printf ("%.2d| '%s' [%.2d]\n", counter, ArrVars[counter].name, ArrVars[counter].label);
//     }

    return Node;
}

SNode* get_Number (SBuffer* Buffer)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    double Value = 0;

    if (Buffer->Array[Buffer->ip] == 'e' || Buffer->Array[Buffer->ip] == 'E')
    {
        Buffer->ip++;

        Value =  M_E;
    }
    else if (strncasecmp (&(Buffer->Array[Buffer->ip]), "PI", 2) == 0)
    {
        Buffer->ip += 2;

        Value = M_PI;
    }
    else
    {
        Value = get_Int (Buffer);

        double SecondValue = 0;

        double ThirdValue = 0;

        if (Buffer->Array[Buffer->ip] == '.')
        {
            Buffer->ip++;

            SecondValue = get_Frac (Buffer);
        }


        if (Buffer->Array[Buffer->ip] == 'e' || Buffer->Array[Buffer->ip] == 'E')
        {
            Buffer->ip++;

            ThirdValue = get_Int (Buffer);
        }

        Value = Value + SecondValue;

        Value = Value * pow (10, ThirdValue);
    }

    Node->type = TValue;
    Node->priority = VAL_PRIOR;

    Node->data.val = Value;

    return Node;
}

double get_Frac (SBuffer* Buffer)
{
    int counter = 1;

    int in_counter = 0;

    double Value = 0;

    double Add = 0;

    size_t OldPtr = Buffer->ip;

    while (Buffer->Array[Buffer->ip] >= '0' && Buffer->Array[Buffer->ip] <= '9')
    {
        Add = (Buffer->Array[Buffer->ip] - '0');

        in_counter = counter;

        while (in_counter > 0)
        {
            Add /= 10;

            in_counter--;
        }

        Value = Value  + Add;

        counter++;

        Buffer->ip++;
    }

    MLA (Buffer->ip > OldPtr);

    return Value;
}

double get_Int (SBuffer* Buffer)
{
    double Value = 0;

    size_t OldPtr = Buffer->ip;

    while (Buffer->Array[Buffer->ip] >= '0' && Buffer->Array[Buffer->ip] <= '9')
    {
        Value = Value * 10 + (Buffer->Array[Buffer->ip] - '0');

        Buffer->ip++;
    }

    MLA (Buffer->ip > OldPtr);

    return Value;
}

SNode* get_Function (SBuffer* Buffer)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = TOperation;

    Node->left = NULL;

    size_t OldPtr = Buffer->ip;

    if (0) {}
    #define DEF_OP(e_num, e_number, line, size, diff_f, collapse_f, calc_f, prior, ...) \
    else if (strncasecmp (&(Buffer->Array[Buffer->ip]), line, size) == 0) \
    {\
        Buffer->ip += size;\
        \
        Node->data.op = e_num;\
        Node->priority = prior;\
        Node->right = get_Bracket (Buffer);\
    }

    #include "DEF_Operations.h"

    #undef DEF_OP

    MLA (Buffer->ip > OldPtr);

    return Node;
}

void get_$ (SBuffer* Buffer)
{
    size_t OldPtr = Buffer->ip;

    if (Buffer->Array[Buffer->ip] == '\0')
    {
        Buffer->ip++;
    }

    MLA (Buffer->ip > OldPtr);

    return;
}

//=================================================================================================================================================================================================================
//derivative//
//=================================================================================================================================================================================================================

SNode* diff_tree (SNode* Root)
{
    SNode* DifRoot = diff_node (Root);

    return DifRoot;
}

SNode* diff_node (SNode* Node)
{
    SNode* DiffNode = NULL;

    if (Node->type == TValue)
    {
        DiffNode = CR_VAL (ZERO);

        return DiffNode;
    }

    if (Node->type == TVariable)
    {
        DiffNode = (Node->data.var == 'x') ? CR_VAL (ONE) : CR_VAL (ZERO);

        return DiffNode;
    }

    #define DEF_OP(e_num, num, line, size, diff_f, ...) \
        case e_num:\
            diff_f \
            break;

    switch (Node->data.op)
    {
        #include "DEF_Operations.h"

        default:
            printf ("diff default error!\n");
            break;
    }

    #undef DEF_OP

    return DiffNode;
}

SNode* copy_node (SNode* Original)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = Original->type;
    Node->data = Original->data;
    Node->priority = Original->priority;

    if (Original->left != NULL)
    {
        Node->left  = copy_node (Original->left);
    }

    if (Original->right != NULL)
    {
        Node->right  = copy_node (Original->right);
    }

    return Node;
}

void collapse_tree (SNode* Root)
{
    int running = 1;
    while (running)
    {
        running = 0;
        running += clean_const (Root);
        //print_inorder (Root);
        //printf ("\n");
        running += clean_zero  (Root);
        //print_inorder (Root);
        //printf ("\n");
    }

    return;
}

int clean_const (SNode* Node)
{
    if (Node == NULL)
    {
        return 0;
    }

    //print_node (Node);
    //printf ("\n");
    if ((Node->type == TOperation) && ((Node->left != NULL && Node->left->type == TValue) && (Node->right != NULL && Node->right->type == TValue)))
    {
        Node->type = TValue;

        switch (Node->data.op)
        {
            case ADD:
                Node->data.val = Node->left->data.val + Node->right->data.val;
                break;

            case SUB:
                Node->data.val = Node->left->data.val - Node->right->data.val;
                break;

            case MUL:
                Node->data.val = Node->left->data.val * Node->right->data.val;
                break;

            case DIV:
                Node->data.val = Node->left->data.val / Node->right->data.val;
                break;

            // case SIN:
            //     Node->data.val = sin (Node->left->data.val);
            //     break;

            default:
                printf ("clean_const DEFAULT ERROR!\n");
                return 0;
        }

        free (Node->left);

        if (Node->right != NULL)
        {
            free (Node->right);
        }

        Node->left = NULL;
        Node->right = NULL;

        return 1;
    }

    if (Node->left != NULL && Node->right != NULL)
    {
        return (clean_const (Node->left) + clean_const (Node->right));
    }
    else if (Node->left != NULL)
    {
        return clean_const (Node->left);
    }
    else if (Node->right != NULL)
    {
        return clean_const (Node->right);
    }

    return 0;
}

int clean_zero (SNode* Node)
{
    if (Node == NULL)
    {
        return 0;
    }

    #define DEF_OP(e_num, num, line, size, diff_f, collapse_f, ...) \
            case e_num: \
                collapse_f \
                break;

    if ((Node->type == TOperation) && (((Node->left != NULL) && (Node->left->type == TValue)) || ((Node->right != NULL) && (Node->right->type == TValue ))))
    {
        switch (Node->data.op)
        {
            #include "DEF_Operations.h"

            default:
                printf ("clean_zero DEFAULT ERROR!\n");
        }
    }
    #undef DEF_OP

    if (Node->left != NULL || Node->right != NULL)
    {
        return (clean_zero (Node->left) + clean_zero (Node->right));
    }

    return 0;
}

int find_vars (SNode* Node)
{
    if (Node->type == TVariable)
    {
        return 1;
    }

    if (Node->left != NULL && Node->right != NULL)
    {
        return (find_vars (Node->left) + find_vars (Node->right));
    }

    return 0;
}

//=================================================================================================================================================================================================================
//node functions//
//=================================================================================================================================================================================================================

SNode* create_OP_node (EOperations OP, SNode* Left, SNode* Right)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = TOperation;
    Node->data.op = OP;

    switch (OP)
    {

    #define DEF_OP(e_num, num, line, size, diff_f, collapse_f, calc_f, prior, ...) \
        case e_num: \
            Node->priority = prior; \
            break;

    #include "DEF_Operations.h"

    #undef DEF_OP

        default:
            Node->priority = 1000;
    }

    Node->left  = Left;
    Node->right = Right;

    return Node;
}

SNode* create_VAR_node (char Variable)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = TVariable;
    Node->data.var = Variable;
    Node->priority = VAR_PRIOR;

    Node->left  = NULL;
    Node->right = NULL;

    return Node;
}

SNode* create_VAL_node (double Value)
{
    SNode* Node = (SNode*) calloc (1, sizeof (SNode));

    Node->type = TValue;
    Node->data.val = Value;
    Node->priority = VAL_PRIOR;

    Node->left  = NULL;
    Node->right = NULL;

    return Node;
}

void refactor_left (SNode* Node)
{
    SNode* Old = Node->right;

    free (Node->left);

    Node->left  = Old->left;
    Node->right = Old->right;

    Node->type  = Old->type;
    Node->data  = Old->data;
    Node->priority = Old->priority;

    free (Old);

    return;
}

void refactor_right (SNode* Node)
{
    SNode* Old = Node->left;

    free (Node->right);

    Node->left  = Old->left;
    Node->right = Old->right;

    Node->type  = Old->type;
    Node->data  = Old->data;
    Node->priority = Old->priority;

    free (Old);

    return;
}

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

int delete_tree (SNode** Node)
{
    if (*Node == NULL)
    {
        return 0;
    }

    if ((*Node)->left != NULL)
    {
        delete_tree (&((*Node)->left));
    }

    if ((*Node)->right != NULL)
    {
        delete_tree (&((*Node)->right));
    }

    //free (Node->data);

    //printf ("\n%p freed\n", *Node);
    free (*Node);

    *Node = NULL;
    //printf ("\n%p freed^\n", *Node);

    return 0;
}

//=================================================================================================================================================================================================================
//printers//
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

    if ((Node->parent != NULL) && (((Node->left != NULL) && (Node->parent->priority > Node->priority) && (!(Node->parent->type == TOperation && Node->parent->data.op == DIV))) || (Node->type == TOperation && Node->data.op == ADD && (Node->parent->type == TOperation && Node->parent->data.op == SUB) && (Node->parent->right == Node))))
    {
        printf ("(");
    }

    if (Node->left != NULL)
    {
        print_inorder (Node->left);
    }

    print_node (Node);

    if (Node->right != NULL)
    {
        print_inorder (Node->right);
    }

    if ((Node->parent != NULL) && (((Node->left != NULL) && (Node->parent->priority > Node->priority) && (!(Node->parent->type == TOperation && Node->parent->data.op == DIV))) || (Node->type == TOperation && Node->data.op == ADD && (Node->parent->type == TOperation && Node->parent->data.op == SUB) && (Node->parent->right == Node))))
    {
        printf (")");
    }

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
    //printf ("%p\n", Node);
    switch (Node->type)
    {
        case TOperation:
            switch (Node->data.op)
            {
                #define DEF_OP(e_num, num, line, size, ...) \
                case e_num: \
                    printf (" %s ", line); \
                    break;

                #include "DEF_Operations.h"

                #undef DEF_OP

                default:
                    printf (KRED "Op_type default error! '%d'\n" KNRM, Node->data.op);
            }
            break;

        case TValue:
            if (Node->data.val == M_E)
            {
                printf ("E", Node->data.val);
            }
            else if (Node->data.val == M_PI)
            {
                printf ("PI", Node->data.val);
            }
            else
            {
                printf ("%lg", Node->data.val);
            }
            break;

        case TVariable:
            printf ("%c", Node->data.var);
            break;

        default:
            printf (KRED "Node_type Default error!" KNRM);
    }

    //printf ("[%d]", Node->priority);

    return;
}

void find_all_parents (SNode* Root)
{
    if (Root != NULL)
    {
        find_parent (Root->left, Root);
        find_parent (Root->right, Root);
    }

    return;
}

void find_parent (SNode* Node, SNode* Parent)
{
    if (Node == NULL)
    {
        return;
    }

    Node->parent = Parent; //printf ("1\n");

    if (Node->left != NULL)
    {
        find_parent (Node->left, Node);
    }

    if (Node->right != NULL)
    {
        find_parent (Node->right, Node);
    }

    return;
}

//=================================================================================================================================================================================================================
//LaTeX//
//=================================================================================================================================================================================================================

FILE* tex_head (void)
{
    FILE* TEXFile = fopen ("TEX/diff.tex","w");

    fwprintf (TEXFile,
    LR"(
    \documentclass[a4paper, 12pt]{article}
    \usepackage{geometry}
    \geometry{a4paper,
    total={170mm,257mm},left=2cm,right=2cm,
    top=2cm,bottom=2cm}
    \usepackage{setspace}
    \usepackage{color}
    \usepackage{hyperref}
    \usepackage{mathtext}
    \usepackage{amsmath}
    \usepackage[utf8]{inputenc}
    \usepackage[english,russian]{babel}
    \usepackage{graphicx, float}
    \usepackage{tabularx, colortbl}
    \usepackage{textcomp}
    \usepackage{caption}
    \usepackage{wrapfig}
    \usepackage{multirow}
    \usepackage{subfigure}

    \DeclareMathOperator{\sgn}{\mathop{sgn}}
    \newcommand*{\hm}[1]{#1\nobreak\discretionary{}
        {\hbox{$\mathsurround=0pt #1$}}{}}


    \author{Рачков Михаил Васильевич, Б01-201}
    \date{%s}
    \title{\textbf{Отчёт о взятии производной} \\(После взятия производная была положена обратно)}


    \captionsetup{labelsep=period}

    \newcommand{\parag}[1]{\paragraph*{#1:}}
    \newcounter{Points}
    \setcounter{Points}{1}
    \newcommand{\point}{\noindent \arabic{Points}. \addtocounter{Points}{1}}
    \newcolumntype{C}{>{\centering\arraybackslash}X}


    \begin{document}

        \maketitle )"
    , __DATE__);

    return TEXFile;
}

void tex_tail (FILE* TEXFile)
{
    fwprintf (TEXFile,
    LR"(
    \section{Источники}
    Я самоучка.
    )");

    fwprintf (TEXFile,
    LR"(
    \end{document}
    )");

    fclose (TEXFile);
}

void tex_tree (FILE* TEXFile, SNode* Root, ETreeMode Mode)
{
    switch (Mode)
    {
        case MODE_FUN:
            fwprintf (TEXFile,
            LR"(
            \maketitle
            \begin{equation}
            f(x)~=~)");
            break;

        case MODE_DER:
            fwprintf (TEXFile,
            LR"(
            \maketitle
            \begin{equation}
            f'(x)~=~)");
            break;

        case MODE_EQ:
            fwprintf (TEXFile,
            LR"(
            \maketitle
            \begin{equation}
            f(x)~=~)");
            break;

        default:
            fwprintf (TEXFile, L"default mode error!\n");
    }

    tex_node (TEXFile, Root);

    fwprintf (TEXFile,
    LR"(
    \end{equation}
    )");
}

void tex_node (FILE* TEXFile, SNode* Node)
{
    if (Node == NULL)
    {
        return;
    }

    if (Node->type == TOperation && Node->data.op == DIV)
    {
        fwprintf (TEXFile, L"\\dfrac{");
    }

    if ((Node->parent != NULL)
    && (((Node->left != NULL)
    && (Node->parent->priority > Node->priority)
    && (!(Node->parent->type == TOperation && Node->parent->data.op == DIV))
    && (!(Node->parent->type == TOperation && Node->parent->data.op == POW)))
    || (Node->type == TOperation && Node->data.op == ADD
        && (Node->parent->type == TOperation && Node->parent->data.op == SUB)
        && (Node->parent->right == Node))))
    {
        fwprintf (TEXFile, L"(");
    }

    if (Node->left != NULL)
    {
        tex_node (TEXFile, Node->left);
    }

    switch (Node->type)
    {

        case TOperation:
            switch (Node->data.op)
            {
                #define DEF_OP(e_num, num, line, size, diff_f, collapse_f, calc_f, prior, print_f) \
                case e_num: \
                    print_f \
                    break;

                #include "DEF_Operations.h"

                #undef DEF_OP

                default:
                    fwprintf (TEXFile, L"Op_type default error! '%d'\n" KNRM, Node->data.op);
            }
            break;

        case TValue:
            if (Node->data.val == M_E)
            {
                fwprintf (TEXFile, L" e ", Node->data.val);
            }
            else if (Node->data.val == M_PI)
            {
                fwprintf (TEXFile, L" \\pi ", Node->data.val);
            }
            else
            {
                fwprintf (TEXFile, L"%lg", Node->data.val);
            }
            break;

        case TVariable:
            fwprintf (TEXFile, L"%c", Node->data.var);
            break;

        default:
            printf (KRED "Node_type Default error!" KNRM);
    }

    if (Node->type == TOperation && Node->data.op == POW)
    {
        fwprintf (TEXFile, L"{");
    }

    if (Node->right != NULL)
    {
        tex_node (TEXFile, Node->right);
    }

    if (Node->type == TOperation && (Node->data.op == POW || Node->data.op == DIV))
    {
        fwprintf (TEXFile, L"}");
    }
    else if ((Node->parent != NULL)
    && (((Node->left != NULL)
    && (Node->parent->priority > Node->priority)
    && (!(Node->parent->type == TOperation && Node->parent->data.op == DIV))
    && (!(Node->parent->type == TOperation && Node->parent->data.op == POW)))
    || (Node->type == TOperation && Node->data.op == ADD
        && (Node->parent->type == TOperation && Node->parent->data.op == SUB)
        && (Node->parent->right == Node))))
    {
        fwprintf (TEXFile, L")");
    }

    return;
}

void do_pdf (const char* TEXName)
{
    system ("pdftex TEX/diff.tex");

    return;
}

//=================================================================================================================================================================================================================
//TreeViz//
//=================================================================================================================================================================================================================

int make_gv_tree (SNode* Root, const char* FileName)
{
    FILE* gvInputFile = fopen (FileName, "w");
    MLA (gvInputFile != NULL);

    fprintf (gvInputFile,
        R"(digraph {
    rankdir = VR
    graph [splines = curved];
    bgcolor = "white";
    node [shape = "plaintext", style = "solid"];)");

    make_gv_node (gvInputFile, Root);

    fprintf (gvInputFile, "\n}\n");

    fclose (gvInputFile);

    draw_gv_tree (FileName);

    return 0;
}

void make_gv_node (FILE* File, SNode* Node)
{
    MLA (File != NULL);

    if (Node == NULL)
    {
        return;
    }

    fprintf (File,
        R"(

                    node_%p
                    [
                        label=
                        <
                        <table border="0" cellborder="1" cellspacing="0">
                            <tr>)", Node);

    print_gv_node (File, Node);

    fprintf (File,
            R"(</td>
                            </tr>)");

    // if (Node->left != NULL)
    // {
    //     fprintf(File,
    //     R"(
    //                         <tr>
    //                             <td bgcolor = "#61de4b" port="left" > %p   </td>
    //                             <td bgcolor = "#f27798" port="right"> %p   </td>
    //                         </tr>)", Node->left, Node->right);
    // }

    fprintf (File, R"(
                        </table>
                        >
                    ]
                    )");

    if (Node->parent != NULL)
    {
        //wprintf (L"!%d!\n", Node->branch);
        if ((Node->parent != NULL) && (Node->parent->left == Node))
        {
            fprintf (File,  "\n                    node_%p -> node_%p;",
                        Node->parent, Node);
        }
        else if ((Node->parent != NULL) && (Node->parent->right == Node))
        {
            fprintf (File,  "\n                    node_%p -> node_%p;",
                        Node->parent, Node);
        }
    }

    make_gv_node (File, Node->left);
    make_gv_node (File, Node->right);
}

void print_gv_node (FILE* File, SNode* Node)
{
    MLA (File != NULL);

    switch (Node->type)
    {
        case TOperation:
            switch (Node->data.op)
            {

                #define DEF_OP(e_num, num, line, size, diff_f, collapse_f, calc_f, prior, print_f) \
                case e_num: \
                    fprintf (File, "<td colspan=\"2\" bgcolor = \"#39c3ed\">\n" " %s ", line); \
                    break;

                    #include "DEF_Operations.h"

                #undef DEF_OP
            }
            break;

        case TVariable:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"#1cfc03\">\n"" %c ", Node->data.var);
            break;

        case TValue:
            fprintf (File, "<td colspan=\"2\" bgcolor = \"#f21847\">\n"" %lg ", Node->data.val);
            break;
    }

    return;
}

void draw_gv_tree (const char* FileName)
{
    size_t length = strlen (FileName) + 60;

    char* Line_1 = (char*) calloc (sizeof (*FileName), length);

    sprintf (Line_1, "dot -Tpng %s -o %s.png", FileName, FileName);
    //system ("dot -Tpng gvDiff.dot -o gvDiff.png");
    system (Line_1);

    sprintf (Line_1, "xdg-open %s.png", FileName);

    //system ("xdg-open 1.png");
    system (Line_1);

    free (Line_1);

    return;
}

//=================================================================================================================================================================================================================
