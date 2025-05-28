#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "linkedlist.h"
#include "talloc.h"

// helper function to print a SchemeVal tree recursively
void printTreeHelper(SchemeVal *tree);

// adds a token to the parse tree using the stack method described
SchemeVal *addToParseTree(SchemeVal *tree, int *depth, SchemeVal *token) {
    if (token->type == CLOSE_TYPE) {
        SchemeVal *subtree = makeEmpty();
        while (!isEmpty(tree) && car(tree)->type != OPEN_TYPE) {
            subtree = cons(car(tree), subtree);
            tree = cdr(tree);
        }
        if (isEmpty(tree)) {
            printf("Syntax error: too many close parentheses\n");
            tfree();
            exit(1);
        }
        // Pop the OPEN_TYPE
        tree = cdr(tree);

        SchemeVal *newTree = talloc(sizeof(SchemeVal));
        newTree->type = CONS_TYPE;
        newTree->car = subtree;
        newTree->cdr = makeEmpty();

        // Handle quote before list
        if (!isEmpty(tree) && car(tree)->type == QUOTE_TYPE) {
            tree = cdr(tree);
            SchemeVal *quoteSym = talloc(sizeof(SchemeVal));
            quoteSym->type = SYMBOL_TYPE;
            quoteSym->s = talloc(sizeof(char) * 6);
            strcpy(quoteSym->s, "quote");

            SchemeVal *innerList = cons(newTree, makeEmpty());
            innerList = cons(quoteSym, innerList);

            SchemeVal *quotedTree = talloc(sizeof(SchemeVal));
            quotedTree->type = CONS_TYPE;
            quotedTree->car = innerList;
            quotedTree->cdr = makeEmpty();

            tree = cons(quotedTree, tree);
        } else {
            tree = cons(newTree, tree);
        }
        (*depth)--;
    } else if (token->type == OPEN_TYPE) {
        tree = cons(token, tree);
        (*depth)++;
    } else if (token->type == QUOTE_TYPE) {
        tree = cons(token, tree);
    } else {
        // Handle quote before atom
        if (!isEmpty(tree) && car(tree)->type == QUOTE_TYPE) {
            tree = cdr(tree);
            SchemeVal *quoteSym = talloc(sizeof(SchemeVal));
            quoteSym->type = SYMBOL_TYPE;
            quoteSym->s = talloc(sizeof(char) * 6);
            strcpy(quoteSym->s, "quote");

            SchemeVal *innerList = cons(token, makeEmpty());
            innerList = cons(quoteSym, innerList);

            SchemeVal *newTree = talloc(sizeof(SchemeVal));
            newTree->type = CONS_TYPE;
            newTree->car = innerList;
            newTree->cdr = makeEmpty();

            tree = cons(newTree, tree);
        } else {
            tree = cons(token, tree);
        }
    }
    return tree;
}

// The main parse function
SchemeVal *parse(SchemeVal *tokens) {
    SchemeVal *tree = makeEmpty();
    int depth = 0;
    SchemeVal *current = tokens;

    assert(current != NULL && "Error (parse): null pointer");

    while (current->type != EMPTY_TYPE) {
        SchemeVal *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }

    if (depth != 0) {
        printf("Syntax error: not enough close parentheses\n");
        tfree();
        exit(1);
    }

    return reverse(tree);
}

// Helper function for printTree that handles formatting
void printTreeHelper(SchemeVal *tree) {
    if (tree->type == CONS_TYPE) {
        printf("(");
        SchemeVal *curr = tree->car;
        while (curr->type == CONS_TYPE) {
            printTreeHelper(car(curr));
            curr = cdr(curr);
            if (curr->type != EMPTY_TYPE) {
                printf(" ");
            }
        }
        if (curr->type != EMPTY_TYPE) {
            printTreeHelper(curr);
        }
        printf(")");
    } else {
        switch (tree->type) {
            case INT_TYPE:
                printf("%d", tree->i);
                break;
            case DOUBLE_TYPE:
                printf("%g", tree->d);
                break;
            case STR_TYPE:
                printf("%s", tree->s);
                break;
            case BOOL_TYPE:
                printf("%s", tree->s);
                break;
            case SYMBOL_TYPE:
                printf("%s", tree->s);
                break;
            default:
                break;
        }
    }
}

// Public interface for printing the parse tree
void printTree(SchemeVal *tree) {
    while (tree->type != EMPTY_TYPE) {
        printTreeHelper(car(tree));
        if (cdr(tree)->type != EMPTY_TYPE) {
            printf(" ");
        }
        tree = cdr(tree);
    }
    printf("\n");
}
