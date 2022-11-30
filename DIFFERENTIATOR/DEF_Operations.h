DEF_OP (ADD, 1, "+", 1,
{
    DiffNode = UNITE (ADD, DL, DR);
},
{
    if (Node->left->data.val == ZERO)
    {
        refactor_left (Node);

        return 1;
    }

    if (Node->right->data.val == ZERO)
    {
        refactor_right (Node);

        return 1;
    }
},
{

}, ADD_PRIOR,
{
    fwprintf (TEXFile, L" + ");
})

DEF_OP (SUB, 2, "-", 1,
{
    DiffNode = UNITE (SUB, DL, DR);
},
{
    if (Node->left->data.val == ZERO)
    {
        Node->left->type = TVariable;

        Node->left->data.var = ' ';

        return 1;
    }

    if (Node->right->data.val == ZERO)
    {
        refactor_right (Node);

        return 1;
    }
},
{

}, SUB_PRIOR,
{
    fwprintf (TEXFile, L" - ");
})

DEF_OP (MUL, 3, "*", 1,
{
    DiffNode = UNITE (ADD, UNITE (MUL, DL, CR), UNITE (MUL, DR, CL));
},
{
    //print_node (Node);

    if (Node->left->data.val == ONE)
    {
        refactor_left (Node);

        return 1;
    }

    if (Node->right->data.val == ONE)
    {
        refactor_right (Node);

        return 1;
    }

    if (Node->left->data.val == ZERO)
    {
        delete_tree (&(Node->left));
        delete_tree (&(Node->right));

        Node->type = TValue;
        Node->data.val = ZERO;

        return 1;
    }

    if (Node->right->data.val == ZERO)
    {
        delete_tree (&(Node->left));
        delete_tree (&(Node->right));

        Node->type = TValue;
        Node->data.val = ZERO;

        return 1;
    }
},
{

}, MUL_PRIOR,
{
    fwprintf (TEXFile, L" \\cdot ");
})

DEF_OP (DIV, 4, "/", 1,
{
    DiffNode = UNITE (DIV, UNITE (SUB, UNITE (MUL, DL, CR), UNITE (MUL, DR, CL)) , UNITE (POW, CR, CR_VAL (2)));
},
{
    if (Node->right->data.val == ONE)
    {
        refactor_right (Node);

        return 1;
    }
    else if (Node->left->data.val == ZERO)
    {
        delete_tree (&(Node->left));
        delete_tree (&(Node->right));

        Node->type = TValue;
        Node->data.val = ZERO;

        return 1;
    }
},
{

}, DIV_PRIOR,
{
        fwprintf (TEXFile, L" }{ ");
})

DEF_OP (POW, 5, "^", 1,
{
    if (find_vars (Node->right) == 0)
    {
        DiffNode = UNITE (MUL, UNITE (MUL, CR, UNITE (POW, CL, UNITE (SUB, CR, CR_VAL (1)))), DL);
    }
    else if (find_vars (Node->left) == 0)
    {
        DiffNode = UNITE (MUL, UNITE (MUL, UNITE (LN, NULL, CL), UNITE (POW, CL, CR)), DR);
    }
    else
    {
        SNode* DerNode = UNITE (MUL, CR, UNITE (LN, NULL, CL));
        DiffNode = UNITE (MUL, UNITE (POW, CL, CR), diff_node (DerNode));

        delete_tree (&DerNode);
    }
},
{
    if (Node->right->data.val == ONE)
    {
        refactor_right (Node);

        return 1;
    }
    else if (Node->right->data.val == ZERO)
    {
        delete_tree (&(Node->left));
        delete_tree (&(Node->right));

        Node->type = TValue;
        Node->data.val = ONE;

        return 1;
    }
},
{

}, POW_PRIOR,
{
    fwprintf (TEXFile, L" ^ ");
})

DEF_OP (SIN, 6, "sin", 3,
{
    DiffNode = UNITE (MUL, UNITE (COS, NULL, CR), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\sin ");
})

DEF_OP (COS, 7, "cos", 3,
{
    DiffNode = UNITE (MUL, UNITE (SUB, CR_VAL (0), UNITE (SIN, NULL, CR)), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\cos ");
})

DEF_OP (TG, 8, "tg",  2,
{
    DiffNode = UNITE (DIV, DR, UNITE (POW, UNITE (COS, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\tan ");
})

DEF_OP (TAN, 9, "tan", 3,
{
    DiffNode = UNITE (DIV, DR, UNITE (POW, UNITE (COS, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\tan ");
})

DEF_OP (CTG, 10, "ctg", 3,
{
    DiffNode = UNITE (DIV, UNITE (SUB, CR_VAL (0), DR), UNITE (POW, UNITE (SIN, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\cot ");
})

DEF_OP (COT, 11, "cot", 3,
{
    DiffNode = UNITE (DIV, UNITE (SUB, CR_VAL (0), DR), UNITE (POW, UNITE (SIN, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\cot ");
})

//=============================================================================================================================================================================

DEF_OP (SH, 12, "sh",  2,
{
    DiffNode = UNITE (MUL, UNITE (CH, NULL, CR), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\sinh ");
})

DEF_OP (SINH, 13, "sinh", 4,
{
    DiffNode = UNITE (MUL, UNITE (CH, NULL, CR), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\sinh ");
})

DEF_OP (CH, 14, "ch",  2,
{
    DiffNode = UNITE (MUL, UNITE (SH, NULL, CR), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\cosh ");
})

DEF_OP (COSH, 15, "cosh", 4,
{
    DiffNode = UNITE (MUL, UNITE (SH, NULL, CR), DR);
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\cosh ");
})

DEF_OP (TH, 16, "th",  2,
{
    DiffNode = UNITE (DIV, DR, UNITE (POW, UNITE (CH, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\tanh ");
})

DEF_OP (TANH, 17, "tanh", 4,
{
    DiffNode = UNITE (DIV, DR, UNITE (POW, UNITE (CH, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\tanh ");
})

DEF_OP (CTH, 18, "cth", 3,
{

},
{

},
{

}, FUNC_PRIOR,
{

})

DEF_OP (COTH, 19, "coth", 4,
{

},
{

},
{

}, FUNC_PRIOR,
{

})

//=============================================================================================================================================================================

DEF_OP (SQRT, 20, "sqrt", 4,
{

},
{

},
{

}, FUNC_PRIOR,
{

})

DEF_OP (SQR, 21, "sqr",  3,
{
    DiffNode = UNITE (DIV, DR, UNITE (POW, UNITE (CH, NULL, CR), CR_VAL (2)));
},
{

},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\tanh ");
})

//=============================================================================================================================================================================

DEF_OP (LN, 22, "ln", 2,
{
    DiffNode = UNITE (DIV, DR, CR);
},
{
    if (Node->right->data.val == M_E)
    {
        delete_tree (&(Node->left));
        delete_tree (&(Node->right));

        Node->type = TValue;
        Node->data.val = ONE;

        return 1;
    }
},
{

}, FUNC_PRIOR,
{
    fwprintf (TEXFile, L" \\ln ");
})

//=============================================================================================================================================================================

