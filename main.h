#define i32 long int

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
}OctalConversion;

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
