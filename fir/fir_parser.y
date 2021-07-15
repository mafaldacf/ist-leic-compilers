%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include "ast/all.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;    /* float value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::typed_node      *typed;
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  cdk::unary_operation_node *unary;
  fir::block_node      *block_node;
  fir::prologue_node   *prologue_node;
  fir::unless_node     *unless_node;
};

%token <i> tINTEGER
%token <d> tFLOAT
%token <s> tIDENTIFIER tSTRING
%token <expression> tNULL

%token tFINALLY tLEAVE tRESTART tRETURN tWRITE tWRITELN tSIZEOF
%token tTYPE_INT tTYPE_FLOAT tTYPE_STRING tTYPE_VOID
%token tDEFAULT_RETURN tEPILOGUE 

%type <unless_node> unless_node;
%type <node> condInstruction iterInstruction instruction declaration blockDeclaration
%type <sequence> file declarations funcArgs instructions blockDeclarations expressions
%type <expression> expression integer float functionCall defaultReturn
%type <lvalue> lval
%type <typed> varDecl funcDecl funcDef funcArg
%type <i> accessType externalDef
%type <block_node> block epilogue
%type <prologue_node> prologue
%type <s> string
%type <type> dataType voidType

 /* PRECEDÊNCIAS */

%nonassoc tIF tWHILE
%nonassoc tTHEN tDO
%nonassoc tELSE tFINALLY

%right '='
%left tOR
%left tAND
%nonassoc '~'
%left tEQ tNE
%left tGE tLE '<' '>'
%left '+' '-'
%left '*' '/' '%'

%nonassoc tUNARY
%nonassoc '[' ']'

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}

%%       


file           :              { compiler->ast($$ = new cdk::sequence_node(LINE)); }
               | declarations { compiler->ast($$ = $1); }
               ;


/*****************************************************************************************************/
/*                                            DECLARATIONS                                           */
/*****************************************************************************************************/

declarations   : declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
               |              declaration { $$ = new cdk::sequence_node(LINE, $1); }
               ;

declaration    : varDecl ';'   { $$ = $1; }
               | funcDecl      { $$ = $1; }
               | funcDef       { $$ = $1; }
               ; 


/*****************************************************************************************************/
/*                                            VARIABLES                                              */
/*****************************************************************************************************/

varDecl        : dataType accessType  tIDENTIFIER                { $$ = new fir::variable_declaration_node(LINE, $1, $2, *$3, nullptr); delete $3; }
               | dataType externalDef tIDENTIFIER                { $$ = new fir::variable_declaration_node(LINE, $1, $2, *$3, nullptr); delete $3; }
               | dataType accessType  tIDENTIFIER '=' expression { $$ = new fir::variable_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               | dataType externalDef tIDENTIFIER '=' expression { $$ = new fir::variable_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               ;

/*****************************************************************************************************/
/*                                            FUNCTIONS                                              */
/*****************************************************************************************************/

funcDecl       : dataType accessType  tIDENTIFIER '(' funcArgs ')' { $$ = new fir::function_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               | voidType accessType  tIDENTIFIER '(' funcArgs ')' { $$ = new fir::function_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               | dataType externalDef tIDENTIFIER '(' funcArgs ')' { $$ = new fir::function_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               | voidType externalDef tIDENTIFIER '(' funcArgs ')' { $$ = new fir::function_declaration_node(LINE, $1, $2, *$3, $5); delete $3; }
               ;
               
funcDef        : dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue block epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, $9, $10); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue                
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, nullptr, nullptr); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue block          
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, $9, nullptr); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue       epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, nullptr, $9); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn          block          
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, $8, nullptr); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn          block epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, $8, $9); delete $3; }
               | dataType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn                epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, nullptr, $8); delete $3; }

               /* VOID FUNCTIONS */
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue block epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, $9, $10); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue                
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, nullptr, nullptr); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue block          
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, $9, nullptr); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn prologue       epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, $8, nullptr, $9); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn          block          
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, $8, nullptr); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn          block epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, $8, $9); delete $3; }
               | voidType accessType tIDENTIFIER '(' funcArgs ')' defaultReturn                epilogue 
                  { $$ = new fir::function_definition_node(LINE, $1, $2, *$3, $5, $7, nullptr, nullptr, $8); delete $3; }
               ;

functionCall    : tIDENTIFIER '(' expressions ')' { $$ = new fir::function_call_node(LINE, *$1, $3); delete $1; }
                | tIDENTIFIER '('             ')' { $$ = new fir::function_call_node(LINE, *$1, new cdk::sequence_node(LINE)); delete $1; }
                ;

funcArgs       :              funcArg     { $$ = new cdk::sequence_node(LINE, $1);     }
               | funcArgs ',' funcArg     { $$ = new cdk::sequence_node(LINE, $3, $1); }
               | /* empty */              { $$ = new cdk::sequence_node(LINE);         }
               ;

funcArg        : dataType tIDENTIFIER { $$ = new fir::variable_declaration_node(LINE, $1, 'p', *$2, nullptr); delete $2; }
               ;

defaultReturn  : tDEFAULT_RETURN tINTEGER { $$ = new cdk::integer_node(LINE, $2); }
               | tDEFAULT_RETURN tFLOAT   { $$ = new cdk::double_node(LINE, $2);  }
               | tDEFAULT_RETURN string   { $$ = new cdk::string_node(LINE, $2); }
               |       /* empty */         { $$ = nullptr; }
               ;

/*****************************************************************************************************/
/*                                          FUNCTION BODY                                            */
/*****************************************************************************************************/

prologue       : '@' block   { $$ = new fir::prologue_node(LINE, $2); }
               ;

epilogue       : tEPILOGUE block  { $$ = $2; }
               ;    

block          : '{' blockDeclarations instructions '}' { $$ = new fir::block_node(LINE, $2, $3);           }
               | '{' blockDeclarations              '}' { $$ = new fir::block_node(LINE, $2, nullptr);      }
               | '{'                   instructions '}' { $$ = new fir::block_node(LINE, nullptr, $2);      }
               | '{'                                '}' { $$ = new fir::block_node(LINE, nullptr, nullptr); }
               ;

blockDeclarations : blockDeclarations blockDeclaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
                  |                   blockDeclaration { $$ = new cdk::sequence_node(LINE, $1);     }
                  ;

blockDeclaration  : varDecl ';'   { $$ = $1; }
                  ;



/*****************************************************************************************************/
/*                                            TYPES                                                  */
/*****************************************************************************************************/

dataType       : tTYPE_STRING      { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
               | tTYPE_INT         { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT);    }
               | tTYPE_FLOAT       { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
               | '<' dataType '>'  { $$ = cdk::reference_type::create(4, $2);               }
               ;

voidType       : tTYPE_VOID        { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
               ;

accessType     : '*'               { $$ = '*'; }
               | /* empty */       { $$ = 'p'; }
               ;

externalDef    : '?'               { $$ = '?'; }
               ;

integer         : tINTEGER { $$ = new cdk::integer_node(LINE, $1); }
                ;

float           : tFLOAT { $$ = new cdk::double_node(LINE, $1); }
                ;

string          :        tSTRING { $$ = $1; }
                | string tSTRING { $$ = $1; $$->append(*$2); delete $2; }
                ;

/*****************************************************************************************************/
/*                                            INSTRUCTIONS                                           */
/*****************************************************************************************************/

instructions      : instructions instruction { $$ = new cdk::sequence_node(LINE, $2, $1); }
                  |              instruction { $$ = new cdk::sequence_node(LINE, $1);     }
                  ;

instruction       : expression           ';'                               { $$ = $1; }
                  | tWRITE   expressions ';'                               { $$ = new fir::write_node(LINE, $2, false); }
                  | tWRITELN expressions ';'                               { $$ = new fir::write_node(LINE, $2, true); }
                  | tLEAVE   tINTEGER    ';'                               { $$ = new fir::leave_node(LINE, $2); }
                  | tLEAVE               ';'                               { $$ = new fir::leave_node(LINE); }
                  | tRESTART tINTEGER    ';'                               { $$ = new fir::restart_node(LINE, $2); }
                  | tRESTART             ';'                               { $$ = new fir::restart_node(LINE); }
                  | tRETURN                                                { $$ = new fir::return_node(LINE); }
                  | condInstruction                                        { $$ = $1; }
                  | iterInstruction                                        { $$ = $1; }
                  | block                                                  { $$ = $1; }
                  | unless_node                                            { $$ = $1; }
                  ;



condInstruction   : tIF expression tTHEN instruction                       { $$ = new fir::if_node(LINE, $2, $4); }
                  | tIF expression tTHEN instruction tELSE instruction     { $$ = new fir::if_else_node(LINE, $2, $4, $6);}
                  ;

iterInstruction   : tWHILE expression tDO instruction                      { $$ = new fir::while_node(LINE, $2, $4); }
                  | tWHILE expression tDO instruction tFINALLY instruction { $$ = new fir::while_finally_node(LINE, $2, $4, $6); }
                  ;


/*****************************************************************************************************/
/*                                            EXPRESSIONS                                            */
/*****************************************************************************************************/


expressions    : expressions ',' expression             { $$ = new cdk::sequence_node(LINE, $3, $1); }
               |                 expression             { $$ = new cdk::sequence_node(LINE, $1);     }
               ;

               /* TYPES */
expression     : integer                     { $$ = $1; }
	            | float                       { $$ = $1; }
               | string                      { $$ = new cdk::string_node(LINE, $1); }
               | tNULL                       { $$ = new fir::null_node(LINE); }

               /* UNITARY EXPRESSIONS */
               | tSIZEOF '(' expression ')'  { $$ = new fir::sizeof_node(LINE, $3); }
               | '+' expression %prec tUNARY { $$ = new fir::identity_node(LINE, $2); }
               | '-' expression %prec tUNARY { $$ = new cdk::neg_node(LINE, $2); }
               | '~' expression              { $$ = new cdk::not_node(LINE, $2); }

               /* ARITHMETIC EXPRESSIONS */
               | expression '+' expression	 { $$ = new cdk::add_node(LINE, $1, $3); }
               | expression '-' expression	 { $$ = new cdk::sub_node(LINE, $1, $3); }
               | expression '*' expression	 { $$ = new cdk::mul_node(LINE, $1, $3); }
               | expression '/' expression	 { $$ = new cdk::div_node(LINE, $1, $3); }
               | expression '%' expression	 { $$ = new cdk::mod_node(LINE, $1, $3); }

               /* LOGICAL EXPRESSIONS */
               | expression '<' expression	 { $$ = new cdk::lt_node(LINE, $1, $3);  }
               | expression '>' expression	 { $$ = new cdk::gt_node(LINE, $1, $3);  }
               | expression tGE expression	 { $$ = new cdk::ge_node(LINE, $1, $3);  }
               | expression tLE expression   { $$ = new cdk::le_node(LINE, $1, $3);  }
               | expression tNE expression	 { $$ = new cdk::ne_node(LINE, $1, $3);  }
               | expression tEQ expression	 { $$ = new cdk::eq_node(LINE, $1, $3);  }
               | expression tAND expression  { $$ = new cdk::and_node(LINE, $1, $3); }
               | expression tOR expression   { $$ = new cdk::or_node(LINE, $1, $3); }

               /* PRIMARY EXPRESSIONS */
               | '(' expression ')'          { $$ = $2; }

               /* ASSIGNMENTS */
               | lval '=' expression         { $$ = new cdk::assignment_node(LINE, $1, $3); }

               /* OTHER EXPRESSIONS */
               | '[' expression ']'          { $$ = new fir::stack_alloc_node(LINE, $2); }
               | lval '?'                    { $$ = new fir::address_of_node(LINE, $1); }
               | lval                        { $$ = new cdk::rvalue_node(LINE, $1); }
               | '@'                         { $$ = new fir::read_node(LINE); }
               | functionCall                { $$ = $1; }
               ;

lval            : tIDENTIFIER                           { $$ = new cdk::variable_node(LINE, $1); delete $1; }
                | lval               '[' expression ']' { $$ = new fir::indexation_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
                | '(' expression ')' '[' expression ']' { $$ = new fir::indexation_node(LINE, $2, $5); }
                |    functionCall    '[' expression ']' { $$ = new fir::indexation_node(LINE, $1, $3); }                
                ;

%%
