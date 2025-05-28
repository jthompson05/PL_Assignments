// interpreter.c

#include "interpreter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "talloc.h" // Use your project's memory manager


// Helper: Make a CONS cell (linked list node)
SchemeVal *makeCons(SchemeVal *car, SchemeVal *cdr) {
    SchemeVal *cell = talloc(sizeof(SchemeVal));
    cell->type = CONS_TYPE;
    cell->car = car;
    cell->cdr = cdr;
    return cell;
}

// Add a binding (var . val) to a frame's bindings list
void addBinding(Frame *frame, SchemeVal *var, SchemeVal *val) {
    SchemeVal *binding = makeCons(var, val);
    frame->bindings = makeCons(binding, frame->bindings);
}

// Look up symbol in chain of frames
SchemeVal *lookUpSymbol(SchemeVal *symbol, Frame *frame) {
    while (frame != NULL) {
        SchemeVal *binding = frame->bindings;
        while (binding != NULL && binding->type == CONS_TYPE) {
            SchemeVal *pair = binding->car;
            if (pair != NULL && pair->type == CONS_TYPE) {
                SchemeVal *bindSymbol = pair->car;
                if (bindSymbol->type == SYMBOL_TYPE && strcmp(bindSymbol->s, symbol->s) == 0) {
                    return pair->cdr;
                }
            }
            binding = binding->cdr;
        }
        frame = frame->parent; // Move to parent frame if not found here
    }
    printf("Evaluation error: unbound variable %s\n", symbol->s);
    tfree(); exit(1);
}

// Evaluate (if test trueExpr [falseExpr])
SchemeVal *evalIf(SchemeVal *args, Frame *frame) {
    // Parse arguments: (testExpr trueExpr [falseExpr])
    if (args == NULL || args->type != CONS_TYPE) goto argerr;
    SchemeVal *testExpr = args->car;
    args = args->cdr;
    if (args == NULL || args->type != CONS_TYPE) goto argerr;
    SchemeVal *trueExpr = args->car;
    args = args->cdr;

    SchemeVal *falseExpr = NULL;
    if (args != NULL && args->type == CONS_TYPE) {
        falseExpr = args->car;
        args = args->cdr;
    }
    // Accept both NULL and EMPTY_TYPE for "no more args"
    if (args != NULL && args->type != EMPTY_TYPE) goto argerr;

    // Evaluate test
    SchemeVal *testResult = eval(testExpr, frame);
    int isTrue = !(testResult->type == BOOL_TYPE && testResult->i == 0);

    if (isTrue) {
        return eval(trueExpr, frame);
    } else {
        if (falseExpr == NULL) {
            printf("Evaluation error: if with no falseExpr\n");
            tfree(); exit(1);
        }
        return eval(falseExpr, frame);
    }

argerr:
    printf("Evaluation error: malformed if\n");
    tfree(); exit(1);
}

// Evaluate (let ((v1 e1) (v2 e2) ...) body...)
SchemeVal *evalLet(SchemeVal *args, Frame *parent) {
    // Must have both bindings and at least one body expr
    if (args == NULL || args->type != CONS_TYPE) goto argerr;
    SchemeVal *bindingsList = args->car;
    SchemeVal *body = args->cdr;
    if (body == NULL || body->type != CONS_TYPE) goto argerr;

    // Check bindings list is a proper list and no duplicates
    // For each binding, must be (symbol expr)
    // Outer loop: for each binding
    SchemeVal *bindingIter1 = bindingsList;
    while (bindingIter1 != NULL && bindingIter1->type == CONS_TYPE) {
        SchemeVal *binding1 = bindingIter1->car;
        // Binding must be a CONS of symbol and expr, and nothing else
        if (binding1 == NULL || binding1->type != CONS_TYPE ||
            binding1->car == NULL || binding1->car->type != SYMBOL_TYPE ||
            binding1->cdr == NULL || binding1->cdr->type != CONS_TYPE ||
            (binding1->cdr->cdr != NULL && binding1->cdr->cdr->type != EMPTY_TYPE)) {
            goto argerr;
        }
        SchemeVal *var1 = binding1->car;

        // Check for duplicate variable names in the rest of the list
        SchemeVal *bindingIter2 = bindingIter1->cdr;
        while (bindingIter2 != NULL && bindingIter2->type == CONS_TYPE) {
            SchemeVal *binding2 = bindingIter2->car;
            if (binding2 != NULL && binding2->type == CONS_TYPE &&
                binding2->car != NULL && binding2->car->type == SYMBOL_TYPE) {
                SchemeVal *var2 = binding2->car;
                if (strcmp(var1->s, var2->s) == 0) {
                    goto argerr; // Duplicate found
                }
            } else {
                goto argerr;
            }
            bindingIter2 = bindingIter2->cdr;
        }

        bindingIter1 = bindingIter1->cdr;
    }
    // Make sure it's a proper list (ends with NULL or EMPTY_TYPE)
    if (bindingIter1 != NULL && bindingIter1->type != EMPTY_TYPE)
        goto argerr;

    // New frame for let
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->bindings = NULL;
    newFrame->parent = parent;

    // Actually evaluate and add bindings
    SchemeVal *bindingIter = bindingsList;
    while (bindingIter != NULL && bindingIter->type == CONS_TYPE) {
        SchemeVal *binding = bindingIter->car;
        SchemeVal *var = binding->car;
        SchemeVal *expr = binding->cdr->car;
        SchemeVal *val = eval(expr, parent);
        addBinding(newFrame, var, val);
        bindingIter = bindingIter->cdr;
    }

    // Body: must have at least one expr and be a proper list
    SchemeVal *result = NULL;
    SchemeVal *bodyIter = body;
    int any = 0;
    while (bodyIter != NULL && bodyIter->type == CONS_TYPE) {
        result = eval(bodyIter->car, newFrame);
        bodyIter = bodyIter->cdr;
        any = 1;
    }
    if (!any || (bodyIter != NULL && bodyIter->type != EMPTY_TYPE))
        goto argerr;
    return result;

argerr:
    printf("Evaluation error\n");
    tfree(); exit(1);
}



// Recursive core evaluator
SchemeVal *eval(SchemeVal *tree, Frame *frame) {
    if (tree == NULL) return NULL;

    switch (tree->type) {
        case INT_TYPE:
        case DOUBLE_TYPE:
        case STR_TYPE:
        case BOOL_TYPE:
            return tree; // Literal: return self

        case SYMBOL_TYPE:
            return lookUpSymbol(tree, frame);

        case CONS_TYPE: {
            SchemeVal *first = tree->car;
            SchemeVal *args = tree->cdr;
            if (first->type == SYMBOL_TYPE) {
                if (strcmp(first->s, "if") == 0)
                    return evalIf(args, frame);
                else if (strcmp(first->s, "let") == 0)
                    return evalLet(args, frame);
                else {
                    printf("Evaluation error: unknown special form '%s'\n", first->s);
                    tfree(); exit(1);
                }
            }
            printf("Evaluation error: expected special form symbol\n");
            tfree(); exit(1);
        }
        default:
            printf("Evaluation error: unknown type\n");
            tfree(); exit(1);
    }
}

// print result: matches your project output spec
void printResult(SchemeVal *result) {
    if (result == NULL) printf("()\n");
    else if (result->type == INT_TYPE) printf("%d\n", result->i);
    else if (result->type == DOUBLE_TYPE) printf("%f\n", result->d);
    else if (result->type == BOOL_TYPE) printf(result->i ? "#t\n" : "#f\n");
    else if (result->type == STR_TYPE) printf("\"%s\"\n", result->s);
    else printf("?\n");
}

// top level interpreter: evaluate all top-level expressions in a global frame
void interpret(SchemeVal *tree) {
    Frame *globalFrame = talloc(sizeof(Frame));
    globalFrame->bindings = NULL;
    globalFrame->parent = NULL;

    SchemeVal *exprs = tree;
    while (exprs != NULL && exprs->type == CONS_TYPE) {
        SchemeVal *result = eval(exprs->car, globalFrame);
        printResult(result);
        exprs = exprs->cdr;
    }
    tfree(); // Cleans up all memory!
}
