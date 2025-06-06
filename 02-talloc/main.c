// Tester for linked list.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "linkedlist.h"
#include "schemeval.h"
#include "talloc.h"

void testForward(SchemeVal *head, int correctLength, bool exemplary) {
  SchemeVal *value = head;
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(1 == car(value)->d);

  if (exemplary) {
    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("2.0s", car(value)->s));

    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("3.0s", car(value)->s));
  }

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(4 == car(value)->d);

  if (exemplary) {
    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("5.0s", car(value)->s));
  }

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(6 == car(value)->d);

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(INT_TYPE == car(value)->type);
  assert(7 == car(value)->i);

  value = cdr(value);
  assert(EMPTY_TYPE == value->type);

  assert(correctLength == length(head));
  assert(!isEmpty(head));
}

void testBackward(SchemeVal *head, int correctLength, bool exemplary) {
  SchemeVal *value = head;

  assert(CONS_TYPE == value->type);
  assert(INT_TYPE == car(value)->type);
  assert(7 == car(value)->i);

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(6 == car(value)->d);

  if (exemplary) {
    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("5.0s", car(value)->s));
  }

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(4 == car(value)->d);

  if (exemplary) {
    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("3.0s", car(value)->s));

    value = cdr(value);
    assert(CONS_TYPE == value->type);
    assert(STR_TYPE == car(value)->type);
    assert(!strcmp("2.0s", car(value)->s));
  }

  value = cdr(value);
  assert(CONS_TYPE == value->type);
  assert(DOUBLE_TYPE == car(value)->type);
  assert(1 == car(value)->d);

  value = cdr(value);
  assert(EMPTY_TYPE == value->type);

  assert(correctLength == length(head));
  assert(!isEmpty(head));
}


int main(int argc, char **argv) {

  bool exemplary = false;
  if (argc == 2 && !strcmp(argv[1], "E")) {
    exemplary = true;
  }

  SchemeVal *head = makeEmpty();
  int correctLength = 0;

  SchemeVal *val1 = talloc(sizeof(SchemeVal));
  val1->type = INT_TYPE;
  val1->i = 7;
  head = cons(val1,head);
  correctLength++;

  SchemeVal *val2 = talloc(sizeof(SchemeVal));
  val2->type = DOUBLE_TYPE;
  val2->d = 6.00;
  head = cons(val2,head);
  correctLength++;

  if (exemplary) {
    SchemeVal *val3 = talloc(sizeof(SchemeVal));
    val3->type = STR_TYPE;
    char *text = "5.0s";
    val3->s = talloc(sizeof(char)*(strlen(text) + 1));
    strcpy(val3->s, text);
    head = cons(val3,head);
    correctLength++;
  }

  SchemeVal *val4 = talloc(sizeof(SchemeVal));
  val4->type = DOUBLE_TYPE;
  val4->d = 4.00000;
  head = cons(val4,head);
  correctLength++;

  if (exemplary) {
    SchemeVal *val5 = talloc(sizeof(SchemeVal));
    val5->type = STR_TYPE;
    char *text = "3.0s";
    val5->s = talloc(sizeof(char)*(strlen(text) + 1));
    strcpy(val5->s, text);
    head = cons(val5,head);
    correctLength++;

    SchemeVal *val6 = talloc(sizeof(SchemeVal));
    val6->type = STR_TYPE;
    text = "2.0s";
    val6->s = talloc(sizeof(char)*(strlen(text) + 1));
    strcpy(val6->s, text);
    head = cons(val6,head);
    correctLength++;
  }

  SchemeVal *val7 = talloc(sizeof(SchemeVal));
  val7->type = DOUBLE_TYPE;
  val7->d = 1.0;
  head = cons(val7,head);
  correctLength++;
    
  display(head);
  testForward(head, correctLength, exemplary);

  SchemeVal *rev = reverse(head);
  display(rev);

  testBackward(rev, correctLength, exemplary);

  // Verify that neither of the lists share identical cons cells
  SchemeVal *headcur = head;
  while (headcur->type != EMPTY_TYPE) {
    SchemeVal *revcur = rev;
    while (revcur->type != EMPTY_TYPE) {
      assert(headcur != revcur);
      revcur = revcur->cdr;
    }
    headcur = headcur->cdr;
  }

  if (exemplary) {
    printf(" -=- EMPTY LIST -=- \n");
    SchemeVal *emptyList = makeEmpty();
    assert(0 == length(emptyList));
    assert(isEmpty(emptyList));

    printf(" -=- REVERSE EMPTY LIST -=- \n");
    SchemeVal *reverseEmpty = reverse(emptyList);
    assert(0 == length(reverseEmpty));
    assert(isEmpty(reverseEmpty));

    SchemeVal *justOneByte = talloc(1);
  }

  tfree();

  SchemeVal *justOneByte = talloc(1);
  texit(0);
}
