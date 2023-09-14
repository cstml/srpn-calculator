#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#define i32 long int
#define STACK_LIMIT 23
#define BUFFER_SIZE 100

//#define DEBUG 1 // uncomment for debugging
#ifdef DEBUG
#define LOG(X,Y) printf("log: "); printf(X,Y)
#else
#define LOG(X,Y) {}
#endif // DEBUG

typedef enum {
  P_FAILED,
  P_SUCCESS,
} ParseResult;

typedef struct {
  i32 fst;
  i32 snd;
} Tuple;

typedef enum {
  OP_FAILED,
  OP_OK,
} OpStatus;

typedef enum {
  READ_NULL,
  READ_OK,
} ReadResult;

typedef enum {
  OCTAL_FAILED,
  OCTAL_OK,
} OctalConversion;

// Parser Functions.
ParseResult TERM(),NUMBER(),END(),SPACE(),OPP(),INSTRUCTION(),OCTAL(),UNKNOWN();

// Stack manipulation functions.
void stackPush(i32),stackPrint(),stackPrintLast();
OpStatus stackPop2(Tuple*);

// Input reading.
ReadResult readInput();

// REPL
int repl();

// Octal to i32 Conversion.
OctalConversion octalToInt (i32, i32*);

// Guards against integer overflow.
int overflowGuard (i32);

// Buffer Globals.
static char inputBuffer[BUFFER_SIZE];
static char *cursor;

// Stack globals.
static i32 stack[STACK_LIMIT];
static i32 stackSize = 0;
static i32 *stackHead = &stack[0];

// Baked in random stack.
static i32 randomStack[STACK_LIMIT] =
{
  1804289383, 846930886,  1681692777, 1714636915, 1957747793, 424238335,
  719885386,  1649760492, 596516649,  1189641421, 1025202362, 1350490027,
  783368690,  1102520059, 2044897763, 1967513926, 1365180540, 1540383426,
  304089172,  1303455736, 35005211,   521595368
};

static i32 randomStackHeadCounter = 0;

bool reads(char c) {
  return (*cursor == c);
}

i32 randomStackPop()
{
  i32 i = randomStack[randomStackHeadCounter];
  if (randomStackHeadCounter == STACK_LIMIT - 1)
  {
    randomStackHeadCounter = 0;
  }
  else
  {
    randomStackHeadCounter++;
  }
  return i;
}

// Errors.
void signalParseError()
{
  printf("Unrecognised operator or operand \"%c\".\n", *cursor);
}

void signalStackUnderflow()
{
  puts("Stack underflow!");
}

void signalStackOverflow()
{
  puts("Stack overflow!");
}

void signalDivByZero()
{
  puts("Divide by 0.");
}

int repl()
{
  if (READ_OK == readInput())
  {
    if(TERM() && reads('\0'))
    {
      LOG("Parsing success",);
      return repl();
    }
    else
    {
      LOG("Parsing failed ~ shouldn't happen.",);
      return repl();
    }
  }
  return 0;
}

ReadResult readInput()
{
  cursor = &inputBuffer[0];
  int r =scanf("%s", inputBuffer);
  // defend against C-d and terminate orderly.
  if (r == EOF)
  {
    return READ_NULL;
  }
  LOG("%s\n",inputBuffer);
  return READ_OK;
}

// Some Parser helpers.
bool isZero ()
{
  return reads('0');
}

bool isNonZeroDigit ()
{
  return (*cursor >= '1' && *cursor <= '9');
}

bool isDigit ()
{
  return (isZero() || isNonZeroDigit());
}

// Parser + interpreter.
ParseResult TERM ()
{
  if (END())
  {
    return P_SUCCESS;
  }
  if (NUMBER())
  {
    return TERM();
  }
  if (OCTAL())
  {
    return TERM();
  }
  if (OPP())
  {
    return TERM();
  }
  if (SPACE())
  {
    return TERM();
  }
  if (INSTRUCTION())
  {
    return TERM();
  }
  if (UNKNOWN())
  {
    return TERM();
  }
  // shouldn't be reached due to UNKNOWN();
  return P_FAILED;
}

bool isMinus ()
{
  return reads('-');
}

ParseResult END ()
{
  if (reads('\0')) return P_SUCCESS;
  return P_FAILED;
}

ParseResult SPACE ()
{
  if (reads(' ')) return P_SUCCESS;
  return P_FAILED;
}

ParseResult INSTRUCTION ()
{
  if (reads('d'))
  {
    stackPrint();
    cursor++;
    return P_SUCCESS;
  }
  if (reads('='))
  {
    stackPrintLast();
    cursor++;
    return P_SUCCESS;
  }
  if (reads('r'))
  {
    i32 r = randomStackPop();
    stackPush(r);
    cursor++;
    return P_SUCCESS;
  }
  return P_FAILED;
}

i32 slurpNumber ()
{
  i32 t = 0;
  while(isDigit())
  {
    t *= 10;
    t += *cursor - '0';
    cursor++;
  }
  return t;
}

ParseResult NUMBER ()
{
  if (isNonZeroDigit())   // Positive Number
  {
    i32 t = slurpNumber();
    stackPush(t);
    return P_SUCCESS;
  }

  else if (isMinus())   // Negative Numbers;
  {
    cursor++;
    if(isNonZeroDigit())   // Not An Octal.
    {
      i32 t = slurpNumber();
      stackPush(t * -1);
      return P_SUCCESS;
    }
    else
    {
      cursor--;
      return P_FAILED;
    }
  }
  return P_FAILED;
}

ParseResult OCTAL ()
{
  if (isZero())   // Positive Number
  {
    cursor++;
    i32 temporaryAcc = slurpNumber();
    i32 conversionValue;
    // As per blackbox, we silently fail the octal conversion and skip pushing
    // the number.
    if(OCTAL_OK == octalToInt(temporaryAcc,&conversionValue))
    {
      stackPush(conversionValue);
    }
    return P_SUCCESS;
  }

  else if (isMinus())   // Negative Numbers;
  {
    cursor++;
    if(isZero())
    {
      cursor++;
      i32 temporaryAcc = slurpNumber();
      i32 conversionValue;
      if(OCTAL_OK == octalToInt(temporaryAcc, &conversionValue))
      {
        stackPush(conversionValue * -1);
      }
      return P_SUCCESS;
    }
    else
    {
      cursor--;
      return P_FAILED;
    }
  }
  return P_FAILED;
}

ParseResult OPP ()
{
  if (reads('+'))
  {
    cursor++;
    Tuple t;
    if(OP_OK ==  stackPop2(&t))
    {
      i32 a = t.fst;
      i32 b = t.snd;
      stackPush(a + b);
    }
    return P_SUCCESS;
  }

  if (reads('-'))
  {
    cursor++;
    Tuple t;
    if (OP_OK == stackPop2(&t))
    {
      i32 a = t.fst;
      i32 b = t.snd;
      stackPush(a - b);
    }
    return P_SUCCESS;
  }

  if (reads('/'))
  {
    cursor++;
    Tuple t;
    if(OP_OK == stackPop2(&t))
    {
      i32 a = t.fst;
      i32 b = t.snd;
      if (a == 0)
      {
        // Signal and push back the numbers;
        signalDivByZero();
        stackPush(b);
        stackPush(a);
      }
      else
      {
        stackPush(a / b);
      }
    }
    return P_SUCCESS;
  }

  if (reads('*'))
  {
    cursor++;
    Tuple t;
    if(OP_OK == stackPop2(&t))
    {
      i32 a = t.fst;
      i32 b = t.snd;
      stackPush(a * b);
    }
    return P_SUCCESS;
  }

  return P_FAILED;
}

// Should always pass.
ParseResult UNKNOWN ()
{
  signalParseError();
  cursor++;
  return P_SUCCESS;
}

// Stack Management.
void stackPush (i32 x)
{
  if (stackSize == STACK_LIMIT - 1 )
  {
    signalStackOverflow();
  }
  else
  {
    *stackHead = overflowGuard(x);
    stackHead++;
    stackSize++;
  }
}

OpStatus stackPop2 (Tuple *result)
{
  if (stackSize <= 1)
  {
    signalStackUnderflow();
    return OP_FAILED;
  }
  else
  {
    --stackHead;
    i32 t1 = *stackHead;
    --stackHead;
    i32 t2 = *stackHead;
    stackSize-=2;
    result->fst = t1;
    result->snd = t2;
    return OP_OK;
  }
}

OctalConversion octalToInt (i32 octal, i32 *result)
{
  int decimalValue = 0;
  int base = 8;
  int power = 0;
  while (octal)
  {
    int lastDigit = octal % 10;

    if (lastDigit > 7)
    {
      return OCTAL_FAILED;
    }

    octal /= 10;
    decimalValue += lastDigit * pow(base,power);
    power += 1;
  }
  *result = decimalValue;
  return OCTAL_OK;
}

// Overflow guard for saturating values that get pushed.
int overflowGuard (i32 x)
{
  if (x >= INT_MAX) return INT_MAX;
  if (x <= INT_MIN) return INT_MIN;
  return x;
}

// Stack Printers.
void stackPrint ()
{
  for (i32 i = 0 ; i < stackSize ; i++)
  {
    printf("%ld\n",stack[i]);
  }
}

void stackPrintLast ()
{
  switch (stackSize)
  {
  case 0:
    puts("Stack empty.\n");
    break;
  default:
    printf("%li\n",*(stackHead - 1));
  }
}
