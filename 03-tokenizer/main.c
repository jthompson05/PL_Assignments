#include <stdio.h>
#include "tokenizer.h"
#include "schemeval.h"
#include "linkedlist.h"
#include "talloc.h"

int main() {
    SchemeVal *list = tokenize();
    displayTokens(list);
    tfree();
}
