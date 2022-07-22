#ifndef KEYWORDS_H
#define KEYWORDS_H
 
char *kwords = "and\0...."
               "args\0..."
               "false\0.."
               "get\0...."
               "if\0....."
               "list\0..."
               "list?\0.."
               "nil\0...."
               "nil?\0..."
               "not\0...."
               "or\0....."
               "print\0.."
               "true\0..."
               "zero?\0..";

#define KW_AND       1
#define KW_ARGS      2
#define KW_FALSE     3
#define KW_GET       4
#define KW_IF        5
#define KW_LIST      6
#define KW_LISTP     7
#define KW_NIL       8
#define KW_NILP      9
#define KW_NOT      10 
#define KW_OR       11
#define KW_PRINT    12
#define KW_TRUE     13
#define KW_ZEROP    14

#define keywords_count      14


#define KW_ADD      71     
#define KW_SUB      72   
#define KW_DIV      73  
#define KW_MULT     74   
#define KW_LT       75  
#define KW_LE       76      
#define KW_GT       77  
#define KW_GE       78 

#endif
