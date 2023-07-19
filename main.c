#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <setjmp.h>
#include <limits.h>
#include <math.h>

#define i32 long int
#define STACK_LIMIT 23
#define BUFFER_SIZE 100
#define SUCCESS 1
#define FAILED 0

//#define DEBUG 1
#ifdef DEBUG
#define LOG(X,Y) printf("log: "); printf(X,Y)
#else
#define LOG(X,Y) {}
#endif // DEBUG

// Jumps.
static jmp_buf fail;
static jmp_buf end;
static jmp_buf fail_octal_conversion;
// Buffer Globals.
static char inputBuffer[BUFFER_SIZE];
static char *cursor;
// Stack globals.
static i32 stack[STACK_LIMIT];
static i32 stackSize = 0;
static i32 *stackHead = &stack[0];

typedef struct {
  i32 fst;
  i32 snd;
} Tuple;

bool TERM(),NUMBER(),END(),SPACE(),OPP(),INSTRUCTION(),OCTAL();
void stackPush(i32),stackPrint(),stackPrintLast();
void readInput();
Tuple stackPop2();
int repl();

// Baked it random stack.
static i32 randomStack[STACK_LIMIT] ={
  1804289383,
  846930886,
  1681692777,
  1714636915,
  1957747793,
  424238335,
  719885386,
  1649760492,
  596516649,
  1189641421,
  1025202362,
  1350490027,
  783368690,
  1102520059,
  2044897763,
  1967513926,
  1365180540,
  1540383426,
  304089172,
  1303455736,
  35005211,
  521595368
};
static i32 randomStackHeadCounter = 0;

i32 randomStackPop() {
  i32 i = randomStack[randomStackHeadCounter];
  if (randomStackHeadCounter == STACK_LIMIT - 1) {
    randomStackHeadCounter = 0;
  } else {
    randomStackHeadCounter++;
  }
  return i;
}

// Errors.
void handleParseError(){
  puts("Parsing error");
  printf("Did not know how to read \"%c\"\n", *cursor);
  printf("All input after has \"%c\" been ignored\n", *cursor);
}

void handleStackUnderflow(){
  puts("Stack underflow!");
  longjmp(fail,0);
}

void handleStackOverflow(){
  puts("Stack overflow!");
  longjmp(fail,0);
}

// Evaluation.
int main(){
  if (!setjmp(end))
    return repl();
  return 0;
}

int repl(){
  setjmp(fail);
  readInput();
  if(TERM() && *cursor == '\0') {
    LOG("Parsing success",);
    return repl();
  } else {
    handleParseError();
    return repl();
  }
  return 0;
}

void readInput(){
  cursor = &inputBuffer[0];
  int r =scanf("%s", inputBuffer);
  // defend against C-d and terminate orderly.
  if (r == EOF) {
    longjmp(end,1);
  }
  LOG("%s\n",inputBuffer);
}

// Some Parser helpers.
bool isZero (){
  return (*cursor == '0');
}

bool isNonZeroDigit () {
  return ((*cursor == '1') || (*cursor == '2') || (*cursor == '3')
          || (*cursor == '4') || (*cursor == '5') || (*cursor == '6')
          || (*cursor == '7') || (*cursor == '8') || (*cursor == '9'));
}

bool isDigit () {
  return (isZero() || isNonZeroDigit());
}

// Parser + interpreter.
bool TERM () {
  if (END())         { return SUCCESS;}
  if (NUMBER())      { return TERM(); }
  if (OCTAL())       { return TERM(); }
  if (OPP())         { return TERM(); }
  if (SPACE())       { return TERM(); }
  if (INSTRUCTION()) { return TERM(); }
  return FAILED;
}

bool isMinus (){
  return *cursor == '-';
}

bool END (){
  if (*cursor == '\0') return SUCCESS;
  return FAILED;
}

bool SPACE() {
  if (*cursor == ' ') return SUCCESS;
  return FAILED;
}

bool INSTRUCTION() {
  if (*cursor == 'd') {
    stackPrint();
    cursor++;
    return SUCCESS;
  }
  if (*cursor == '=') {
    stackPrintLast();
    cursor++;
    return SUCCESS;
  }
  if (*cursor == 'r') {
    i32 r = randomStackPop();
    stackPush(r);
    cursor++;
    return SUCCESS;
  }
  return FAILED;
}

bool NUMBER () {
  // Positive Number
  if (isNonZeroDigit()) {
    i32 t = 0;
    while(isDigit()){
      t *= 10;
      t += *cursor - '0';
      cursor++;
    }
    stackPush(t);
    return SUCCESS;
  }
  // Negative Numbers;
  else if (isMinus()) {
    cursor++;
    if(isNonZeroDigit()){
      i32 t = 0;
      while(isDigit()){
        t *= 10;
        t += *cursor - '0';
        cursor++;
      }
      stackPush(t * -1);
      return SUCCESS;
    } else {
      cursor--;
      return FAILED;
    }
  }
  return FAILED;
}

i32 intToOctal (i32 octal) {
  int decimalValue = 0;
  int base = 8;
  int power = 0;
  while (octal) {
    int lastDigit = octal % 10;

    if (lastDigit > 7) {
      longjmp(fail_octal_conversion,1);
    }

    octal /= 10;
    decimalValue += lastDigit * pow(base,power);
    power += 1;
  }
  return decimalValue;
}

bool OCTAL () {
  // Positive Number
  if (isZero()) {
    i32 t = 0;
    while(isDigit()){
      t *= 10;
      t += *cursor - '0';
      cursor++;
    }

    // As per implementation, we silently fail the octal conversion and skip
    // pushing the number.
    if(!(setjmp(fail_octal_conversion))){
      i32 v = intToOctal(t);
      stackPush(v);
    }

    return SUCCESS;
  }
  // Negative Numbers;
  else if (isMinus()) {
    cursor++;
    if(isZero()){
      i32 t = 0;
      while(isDigit()){
        t *= 10;
        t += *cursor - '0';
        cursor++;
      }
      stackPush(t * -1);
      return SUCCESS;
    } else {
      cursor--;
      return FAILED;
    }
  }
  return FAILED;
}

bool OPP(){
  if (*cursor == '+') {
    cursor++;
    Tuple t = stackPop2();
    i32 a = t.fst;
    i32 b = t.snd;
    stackPush(a + b);
    return SUCCESS;
  }

  if (*cursor == '-') {
    cursor++;
    Tuple t = stackPop2();
    i32 a = t.fst;
    i32 b = t.snd;
    stackPush(a - b);
    return SUCCESS;
  }

  if (*cursor == '/') {
    cursor++;
    Tuple t = stackPop2();
    i32 a = t.fst;
    i32 b = t.snd;
    if (a == 0) {
      puts("Divide by 0.");
      stackPush(b);
      stackPush(a);
    } else {
      stackPush(a / b);
    }
    return SUCCESS;
  }

  if (*cursor == '*') {
    cursor++;
    Tuple t = stackPop2();
    i32 a = t.fst;
    i32 b = t.snd;
    stackPush(a * b);
    return SUCCESS;
  }

  return FAILED;
}

// Overflow guard for saturating values that get pushed.
int overflowGuard (i32 x) {
  if (x >= INT_MAX) return INT_MAX;
  if (x <= INT_MIN) return INT_MIN;
  return x;
}

// Stack Management.
void stackPush(i32 x){
  if (stackSize == STACK_LIMIT + 1 ) handleStackOverflow();
  *stackHead = overflowGuard(x);
  stackHead++;
  stackSize++;
}

Tuple stackPop2(){
  if (stackSize <= 1) handleStackUnderflow();
  --stackHead;
  i32 t1 = *stackHead;
  --stackHead;
  i32 t2 = *stackHead;
  stackSize-=2;
  Tuple r;
  r.fst = t1;
  r.snd = t2;
  return r;
}

void stackPrint (){
  for (i32 i = 0 ; i < stackSize ; i++) {
    printf("%ld\n",stack[i]);
  }
}

void stackPrintLast (){
  switch (stackSize) {
  case 0: puts("Stack empty.\n");break;
  default: printf("%li\n",*(stackHead - 1));
  }
}
