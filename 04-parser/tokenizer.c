#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "schemeval.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "talloc.h"

#define MAX_TOKEN_LEN 300

// Reads Scheme tokens from stdin and returns a reversed linked list of SchemeVals
SchemeVal *tokenize();

// Skips over comments starting with ';' until the end of the line
void skipComment(FILE *input);

// Prints an error message and exits the program when an untokenizable sequence is encountered
void syntaxError();

// Reads the next token from input, classifies and returns it as a SchemeVal*
SchemeVal *readToken(FILE *input, int *charRead);

// Checks if the character is valid as the first character of a Scheme symbol
bool isInitialChar(char c);

// Checks if the character is valid as a non-initial character of a Scheme symbol
bool isSubsequentChar(char c);

// Returns true if a given string conforms to Scheme symbol rules, otherwise false
bool isValidSymbol(const char *s);

// Main tokenize function implementation
SchemeVal *tokenize() {
    SchemeVal *list = makeEmpty();
    int charRead = fgetc(stdin);

    while (charRead != EOF) {
        if (isspace(charRead)) {
            charRead = fgetc(stdin);
            continue;
        }
        if (charRead == ';') {
            skipComment(stdin);
            charRead = fgetc(stdin);
            continue;
        }
        if (charRead == '(') {
            SchemeVal *tok = talloc(sizeof(SchemeVal));
            tok->type = OPEN_TYPE;
            list = cons(tok, list);
            charRead = fgetc(stdin);
        } else if (charRead == ')') {
            SchemeVal *tok = talloc(sizeof(SchemeVal));
            tok->type = CLOSE_TYPE;
            list = cons(tok, list);
            charRead = fgetc(stdin);
        } else if (charRead == '"') {
            char buffer[MAX_TOKEN_LEN] = {'\0'};
            int idx = 0;
            buffer[idx++] = charRead;
            charRead = fgetc(stdin);
            while (charRead != EOF && charRead != '"' && idx < MAX_TOKEN_LEN - 1) {
                buffer[idx++] = charRead;
                charRead = fgetc(stdin);
            }
            if (charRead == '"') {
                buffer[idx++] = charRead;
                buffer[idx] = '\0';
                SchemeVal *tok = talloc(sizeof(SchemeVal));
                tok->type = STR_TYPE;
                tok->s = talloc(strlen(buffer) + 1);
                strcpy(tok->s, buffer);
                list = cons(tok, list);
                charRead = fgetc(stdin);
            } else {
                syntaxError();
            }
        } else if (charRead == '\'') {
            SchemeVal *tok = talloc(sizeof(SchemeVal));
            tok->type = QUOTE_TYPE;
            list = cons(tok, list);
            charRead = fgetc(stdin);
        } else {
            ungetc(charRead, stdin);
            SchemeVal *tok = readToken(stdin, &charRead);
            list = cons(tok, list);
        }
    }

    return reverse(list);
}

// Prints each token in the list along with its type label
void displayTokens(SchemeVal *list) {
    while (!isEmpty(list)) {
        SchemeVal *val = car(list);
        switch (val->type) {
            case INT_TYPE:
                printf("%d:integer\n", val->i);
                break;
            case DOUBLE_TYPE:
                printf("%g:double\n", val->d);
                break;
            case STR_TYPE:
                printf("%s:string\n", val->s);
                break;
            case BOOL_TYPE:
                printf("%s:boolean\n", val->s);
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", val->s);
                break;
            case OPEN_TYPE:
                printf("(:open\n");
                break;
            case CLOSE_TYPE:
                printf("):close\n");
                break;
            case QUOTE_TYPE:
                printf("':quote\n");
                break;
            default:
                syntaxError();
        }
        list = cdr(list);
    }
}

// Skips over characters until end of line or EOF when a comment is encountered
void skipComment(FILE *input) {
    int c;
    while ((c = fgetc(input)) != '\n' && c != EOF);
}

// Prints standardized syntax error message and exits
void syntaxError() {
    printf("Syntax error: untokenizeable\n");
    tfree();
    exit(1);
}

// Reads and classifies a single token from the input file stream
SchemeVal *readToken(FILE *input, int *charRead) {
    char buffer[MAX_TOKEN_LEN];
    int idx = 0;
    int c = fgetc(input);

    while (c != EOF && !isspace(c) && c != '(' && c != ')' && c != '\'' && c != ';' && idx < MAX_TOKEN_LEN - 1) {
        buffer[idx++] = c;
        c = fgetc(input);
    }
    buffer[idx] = '\0';
    *charRead = c;

    if (strcmp(buffer, "#t") == 0 || strcmp(buffer, "#f") == 0) {
        SchemeVal *tok = talloc(sizeof(SchemeVal));
        tok->type = BOOL_TYPE;
        tok->s = talloc(strlen(buffer) + 1);
        strcpy(tok->s, buffer);
        return tok;
    }

    if (buffer[0] == '#') {
        syntaxError();
    }

    char *endptr;
    long intVal = strtol(buffer, &endptr, 10);
    if (*endptr == '\0') {
        SchemeVal *tok = talloc(sizeof(SchemeVal));
        tok->type = INT_TYPE;
        tok->i = intVal;
        return tok;
    }

    double dblVal = strtod(buffer, &endptr);
    if (*endptr == '\0') {
        SchemeVal *tok = talloc(sizeof(SchemeVal));
        tok->type = DOUBLE_TYPE;
        tok->d = dblVal;
        return tok;
    }

    if (!isValidSymbol(buffer)) {
        syntaxError();
    }

    SchemeVal *tok = talloc(sizeof(SchemeVal));
    tok->type = SYMBOL_TYPE;
    tok->s = talloc(strlen(buffer) + 1);
    strcpy(tok->s, buffer);
    return tok;
}

// Checks if a character is allowed at the start of a symbol
bool isInitialChar(char c) {
    return isalpha(c) || strchr("!$%&*/:<=>?~_", c);
}

// Checks if a character is allowed after the first in a symbol
bool isSubsequentChar(char c) {
    return isInitialChar(c) || isdigit(c) || c == '.' || c == '+' || c == '-';
}

// Validates that a string is a legal Scheme symbol
bool isValidSymbol(const char *s) {
    if (!s || s[0] == '\0') return false;
    if (strcmp(s, "+") == 0 || strcmp(s, "-") == 0) return true;
    if (!isInitialChar(s[0])) return false;
    for (int i = 1; s[i] != '\0'; i++) {
        if (!isSubsequentChar(s[i])) return false;
    }
    return true;
}
