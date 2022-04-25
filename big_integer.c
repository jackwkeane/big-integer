/*
 *  CSE202: Big Integer Manipulation Program
 *  Full name: Jack Keane
 *  Full Lehigh Email Address: jwk324@lehigh.edu
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// big_integer structure
struct two_quadwords{
  unsigned long lsq;
  long msq;
};
// big_integer typedef
typedef struct two_quadwords big_integer;

// Union to store big integers as 16 characters or two quadwords
union value{
  big_integer binary;
  char hex[16];
};

// Prototypes of the methods to manipulate big integers

// reads a big integer from string input and stores it in the union v
// returns 0 if the hexadecimal number is invalid, 1 otherwise
int read_big_integer(union value *v, char *input);

// writes the value of b to standard output as 32 hex characters
void write_big_integer(union value b);

// performs b1 & b2 and stores the result in b1_and_b2
void and_big_integers(big_integer b1, big_integer b2, big_integer *b1_and_b2);

// performs b1 | b2 and stores the result in b1_or_b2
void or_big_integers(big_integer b1, big_integer b2, big_integer *b1_or_b2);

// performs b1 ^ b2 and stores the result in b1_xor_b2
void xor_big_integers(big_integer b1, big_integer b2, big_integer *b1_xor_b2);

// performs ~b and stores the result in b
void not_big_integer(big_integer *b);

// performs b << k and stores the result in b
void shift_big_integer_left(big_integer *b, unsigned k);

// performs b >> k and stores the result in b
void shift_big_integer_right(big_integer *b, unsigned k);

// performs b1+b2 and stores the result in sum
// returns 1 if overflow occurs, 0 otherwise
int add_big_integers(big_integer b1, big_integer b2, big_integer *sum);

// prints the message and value followed by the program usage message
void print_usage(const char *message, const char* value);

// Char to int
int charToInt(char c);

// Check overflow
int check_overflow(big_integer b1, big_integer b2);

// main method
int main(int argc, char* argv[]){
    // Checking to see if their are an invalid number of arguments
    if(argc < 3 || argc > 4){
      print_usage("Too many or too few arguments", "");
      return 0;
    }

    // 3-arg command
    if(argc == 3){ // NOT operation (3 arguments)
      if(!(strcmp(argv[1], "not"))){ // check if operation is 'not'
        union value v;
        // Passing hex string and union to function. 
        if (read_big_integer(&v, argv[2]) == 0) {
          print_usage("Invalid input:", argv[2]);
          return 0;
        } 
        // Formatting for printing
        fprintf(stdout, "~ ");
        write_big_integer(v);
        fprintf(stdout, "  = ");
        not_big_integer(&v.binary);
        write_big_integer(v);
        fprintf(stdout, "\n\n");

      } else {
        print_usage("Invalid operation", argv[1]);
      }
      return 0;
    }
    
    if(argc == 4){ //and, or, xor, sl, sr, add operations (4 arguments)
      // Checking to see if the cmd involves shifting.
      if(!(strcmp(argv[1], "sl")) || !(strcmp(argv[1], "sr"))) {
        union value v;
        unsigned int k = strtol(argv[2], NULL, 10);
        // Validating hex & reading big int.
        if (read_big_integer(&v, argv[3]) == 0) {
          print_usage("Invalid input:", argv[3]);
          return 0;
        } 
        // Beginning of printing.
        write_big_integer(v);
        // Checking to see if its a left or right shift.
        if(!(strcmp(argv[1], "sl"))) {
          fprintf(stdout, "   << %s = ", argv[2]);
          shift_big_integer_left(&v.binary, k);
        } else if(!(strcmp(argv[1], "sr"))) {
          fprintf(stdout, "   >> %s = ", argv[2]);
          shift_big_integer_right(&v.binary, k);
        } 
        // Finish printing.
        write_big_integer(v);
        fprintf(stdout, "\n\n");

      } else if(!(strcmp(argv[1], "xor")) || !(strcmp(argv[1], "or")) || !(strcmp(argv[1], "and")) || !(strcmp(argv[1], "add"))) {
        // ^ Checks to see if argument is valid: xor, or, and, add
        union value b1; // Union for first input.
        // Validating hex & reading big int.
        if (read_big_integer(&b1, argv[2]) == 0) {
          print_usage("Invalid input:", argv[2]);
          return 0;
        } 
        union value b2; // Union for second input.
        // Validating hex & reading big int.
        if (read_big_integer(&b2, argv[3]) == 0) {
          print_usage("Invalid input:", argv[3]);
          return 0;
        } 
        // Declaring variables.
        big_integer b1_b2; // Resulting big int.
        union value b1_b2_union; // Resulting union.
        int add_overflow = 0; // int var holding return of the add_big_integer function.
        // Begin printing.
        write_big_integer(b1);
        // Checks to see which command and performs corresponding actions. 
        if(!(strcmp(argv[1], "xor"))) {
          
          fprintf(stdout, "  ^\n");
          xor_big_integers(b1.binary, b2.binary, &b1_b2);
          b1_b2_union.binary = b1_b2;

        } else if(!(strcmp(argv[1], "or"))) {
          
          fprintf(stdout, "  |\n");
          or_big_integers(b1.binary, b2.binary, &b1_b2);

        } else if(!(strcmp(argv[1], "add"))) {
          // Stores return value to see if " - Overflow" needs to be added at the end.
          fprintf(stdout, "  +\n");
          add_overflow = add_big_integers(b1.binary, b2.binary, &b1_b2); 

        } else { 
          // No if-statement for 'and' needed because the outer if-statement ensures
          // that once the code makes it to this point, it will always be using the 'and' command.
          fprintf(stdout, "  &\n");
          and_big_integers(b1.binary, b2.binary, &b1_b2);

        }
        // Finishes printing format.
        write_big_integer(b2);
        fprintf(stdout, "  =\n");
        b1_b2_union.binary = b1_b2;
        write_big_integer(b1_b2_union);
        // Checking to see if there is overflow.
        if (add_overflow) {
          fprintf(stdout, "  - Overflow\n\n");
        } else fprintf(stdout, "\n\n");
        
      } else {
        print_usage("Invalid operation:", argv[1]);
      }

    }
    return 0;
}
// DONE – reads 32 hex characters // returns 0 if the hexadecimal number is invalid, 1 otherwise
int read_big_integer(union value *v, char *input){
  // Checking to see if hexidecimal number is invalid
  if (!(input[strspn(input, "0123456789abcdefABCDEF")] == 0) || strlen(input) != 32) {
    return 0; 
  }
  
  // For loop to iterate over hex array
  int j = 31;
  for (size_t i = 0; i < sizeof(v->hex); i++) {  
    char res;
    int i1 = charToInt(input[j-1]); 
    int i2 = charToInt(input[j]);
    j-=2;
    res = i1 << 4; //bit,bit,bit,bit, 0,0,0,0.
    res |= i2; // Adds the c1 and c2 together.
    v->hex[i] = res;
  }

  // return 1 for success
  return 1;
}

// DONE – writes the 32 hex characters b to standard output
void write_big_integer(union value b){
  int j = 0;
  // Working backwards since we are working in little-endian.
  for (size_t i = sizeof(b.hex) -1; i > 0; i--) {
    if (j != 0 && j % 2 == 0) {
      fprintf(stdout, " ");
    }
    j++;
    fprintf(stdout, "%02x", (unsigned char)b.hex[i]);
  } 
  fprintf(stdout, "%02x", (unsigned char)b.hex[0]);
  
}

// DONE – performs b1 & b2 and stores the result in b1_and_b2
void and_big_integers(big_integer b1, big_integer b2, big_integer *b1_and_b2){
  unsigned long x1 = b1.lsq;
  long x2 = b1.msq;
  unsigned long y1 = b2.lsq;
  long y2 = b2.msq;

  b1_and_b2->lsq = x1 & y1;
  b1_and_b2->msq = x2 & y2;
}

// DONE – performs b1 | b2 and stores the result in b1_or_b2
void or_big_integers(big_integer b1, big_integer b2, big_integer *b1_or_b2){
  unsigned long x1 = b1.lsq;
  long x2 = b1.msq;
  unsigned long y1 = b2.lsq;
  long y2 = b2.msq;

  b1_or_b2->lsq = x1 | y1;
  b1_or_b2->msq = x2 | y2;
}

// DONE – performs b1 ^ b2 and stores the result in b1_xor_b2
void xor_big_integers(big_integer b1, big_integer b2, big_integer *b1_xor_b2){
  unsigned long x1 = b1.lsq;
  long x2 = b1.msq;
  unsigned long y1 = b2.lsq;
  long y2 = b2.msq;

  b1_xor_b2->lsq = x1 ^ y1;
  b1_xor_b2->msq = x2 ^ y2;
}

// DONE – performs ~b and stores the result in b
void not_big_integer(big_integer *b){
  b->lsq = ~(b->lsq);
  b->msq = ~(b->msq);
}

// DONE – performs b << k and stores the result in b – 100%
void shift_big_integer_left(big_integer *b, unsigned k){
  // Shifts msq if greater than 64 and then makes lsq all 0s. 
  if (k >= 64) {

    b->msq = b->msq << (k-64);
    b->lsq = b->msq & 0;

  } else {
    
    b->lsq = b->lsq << k;
  }
}

// DONE – performs b >> k and stores the result in b
void shift_big_integer_right(big_integer *b, unsigned k){
  int bits = sizeof(b->msq) * 8; // Get warning unless from next line unless I do this. 
  int operand = 1 << (bits - 1); // Operand to be used for seeing the msq of each big int.
  long signedness = operand & b->msq; // Checks to see if first bit is '1'.
  
  if (k < 64) {
    // Simple shifting op.
    b->msq = b->msq >> k;

  } else if (signedness != -2147483648) {
    //shifts by '0'
    long temp_long = b->msq >> (k - 64);
    b->msq = b->lsq & 0;
    b->lsq = temp_long;

  } else if (signedness == -2147483648) {
    // Casting to signed so that the long shifts by 'f'
    b->lsq = (signed) b->lsq >> (k - 64);
    b->msq = b->msq & -1; 

  }
}

// performs b1+b2 and stores the result in sum
// returns 1 if overflow occurs, 0 otherwise
int add_big_integers(big_integer b1, big_integer b2, big_integer *sum){
  // Calculating sum.
  sum->lsq = (b1.lsq + b2.lsq);
  sum->msq = (b1.msq + b2.msq);

  // Calculating overflow.
  if ((check_overflow(b1, b2) == 1 )|| (check_overflow(b1, b2) == 0)) {
    return 1;
  } else return 0;

}

// prints the message and value followed by the program usage message
void print_usage(const char *message, const char* value){
  // Formatting string
  if (strcmp(value, "")) {
    printf("%s '%s'\n", message, value);
  } else printf("%s %s\n", message, value);

  // Standard usage message
  fprintf(stdout, "Usage: \n");
  fprintf(stdout, "\t./prog0 op [k] number1 [number2]\n");
  fprintf(stdout, "\t\top: operation from {not, sl, sr, and, or, xor, add}\n");
  fprintf(stdout, "\t\tk: shift positions for operations 'sl' and 'sr'\n");
  fprintf(stdout, "\t\tnumber1: operand in hex for unary operations 'not', 'sl', 'sr'\n");
  fprintf(stdout, "\t\tor first operand in hex for binary operations 'and', 'or', 'xor', 'add'\n");
  fprintf(stdout, "\t\tnumber2: second operand in hex for binary operations\n");
  
  // Exits code
  exit(0);
}

// Char to int helper method
int charToInt(char c) {
  // Standard char to int conversion procedure. 
  switch(c) {
    case '0'...'9':
      c -= 48;
      break;
    case 'A'...'F':
      c -=55;
      break;
    case 'a'...'f':
      c -= 87;
      break;
  }
  return c;
}

// Check overflow helper method
int check_overflow(big_integer b1, big_integer b2) {
  unsigned long operand = 1UL << (sizeof(b1.msq) * 8 - 1);
  // Checking conditons required for overflow. 
  if ((b1.msq & operand) && (b2.msq & operand)) { // b1.msq & operand will either be 9223372036854775808 or 0.
    return 1;
  } else if ((b1.msq & operand) == 0 && (b2.msq & operand) == 0){
    return 0;
  }
  return -1;
}