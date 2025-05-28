#include <stdio.h>
#include "tokenizer.h"
#include "schemeval.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"

/* Main for parser, by Dave Musicant
   Fairly self-explanatory. Tokenizes, parses, and prints out a parsed Racket program. */

int main() {

    SchemeVal *list = tokenize();
    SchemeVal *tree = parse(list);

    printTree(tree);
    printf("\n");

    tfree();
    return 0;
}
