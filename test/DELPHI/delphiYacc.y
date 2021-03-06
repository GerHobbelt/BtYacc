
/* Delphi Pascal Grammar (Delphi 3), 1997 Thierry Coq
   adapted from A.G.'s  Turbo Pascal 4.0 grammar (prototype) 5-1-91 AG
   NOTES :
   DON'T FORGET TO MOVE THE GENERATED CONSTANTS TO THE PLACE INDICATED
   */

%{
unit DelphiYacc;

interface

uses SysUtils, Classes, YaccUtilities, LexLib, YaccLib, DLex,
     UCustomParsedObject, UInterfacedParser, UDelphiObject, UUnitePascal;

//
// The generated constants must be placed here
// HERE !!!!
//

type
  TDelphiParser = class( TInterfacedParser)
    public
      identString : string;
      QualifiedIdentString : string;
      currentProcedure : TDelphiProcedure;
      currentUnit      : TUnitePascal;
      function yyparse : integer; override;
      procedure yyerror(msg : string);
      constructor Create( anOwner : TComponent); override;
  end;

implementation

var filename : String;

constructor TDelphiParser.Create( anOwner : TComponent);
begin
  inherited Create( anOwner);
  identString          := '';
  QualifiedIdentString := '';
  currentProcedure     := nil;
  currentUnit          := nil;
end;


(*----------------------------------------------------------------------------*)
procedure TDelphiParser.yyerror(msg : string);
  begin
    writeln( yyLexer.yyerrorfile,  filename+ ': '+ intToStr(yyLexer.yylineno)+
             ': ' + msg + ' at or before `'+ yyLexer.yytext + '''.');
  end(*yyerror*);

%}


%start compilation_unit

/* Lexical stuff: */

%token

/* Keywords are stropped with underscores to prevent conflicts with Delphi
   Pascal keywords. */

_AND_           _ABSOLUTE_      _ABSTRACT_      _ARRAY_
_AS_            _ASM_           _ASSEMBLER_     _AUTOMATED_
_BEGIN_         _CASE_          _CDECL_         _CLASS_
_CONST_         _CONSTRUCTOR_   _DEFAULT_       _DESTRUCTOR_
_DISPID_        _DISPINTERFACE_ _DIV_           _DO_
_DOWNTO_        _DYNAMIC_       _ELSE_          _END_
_EXCEPT_        _EXPORT_        _EXPORTS_       _EXTERNAL_
_FAR_           _FILE_          _FINALIZATION_  _FINALLY_
_FOR_           _FORWARD_       _FUNCTION_      _GOTO_
_IF_            _IMPLEMENTATION_ _IN_           _INDEX_
_INHERITED_     _INITIALIZATION_ _INLINE_       _INTERFACE_
_IS_            _LABEL_         _LIBRARY_       _MESSAGE_
_MOD_           _NAME_          _NEAR_          _NIL_
_NODEFAULT_     _NOT_           _OBJECT_        _OF_
_OR_            _OUT_           _OVERRIDE_      _PACKED_
_PASCAL_        _PRIVATE_       _PROCEDURE_     _PROGRAM_
_PROPERTY_      _PROTECTED_     _PUBLIC_        _PUBLISHED_
_RAISE_         _READ_          _READONLY_      _RECORD_
_REGISTER_      _REPEAT_        _RESIDENT_      _RESOURCESTRING_
_SAFECALL_      _SET_           _SHL_           _SHR_
_STDCALL_       _STORED_        _STRING_        _STRINGRESOURCE_
_THEN_          _THREADVAR_     _TO_            _TRY_
_TYPE_          _UNIT_          _UNTIL_         _USES_
_VAR_           _VIRTUAL_       _WHILE_         _WITH_
_WRITE_         _WRITEONLY_     _XOR_

%right _THEN_ _ELSE_            /* resolve dangling else */

%token

ID
UNSIGNED_INTEGER
UNSIGNED_REAL
STRING_CONST
ILLEGAL

ASSIGNMENT
COLON
COMMA
DOT
DOTDOT
EQUAL
GE
GT
LBRAC
LE
LT
LPAREN
MINUS
NOTEQUAL
PLUS
RBRAC
RPAREN
SEMICOLON
SLASH
STAR
STARSTAR
UPARROW
AT
DIESE

%token
DIRECTIVE

%token
_ON_

%%

compilation_unit
        : program
        | unit
        | library
        | package /* not implemented yet */
        ;

/* Programs: */
program : program_heading program_uses_clause program_block DOT
        ;
program_heading
        : _PROGRAM_ identifier SEMICOLON
        | _PROGRAM_ identifier LPAREN id_list RPAREN SEMICOLON
        ;
program_uses_clause
        : /* empty */
        | _USES_ unit_reference_list SEMICOLON
        ;
unit_reference_list
        : unit_reference
        | unit_reference_list COMMA unit_reference
        ;
unit_reference
        : identifier
        | identifier _IN_ STRING_CONST
        ;
program_block   : int_decl_sect_list compound_stmt
        ;

/* Units: */
unit    : unit_heading interface_part implementation_part
          initialization_part finalization_part _END_ DOT
        ;
unit_heading
        : _UNIT_ identifier SEMICOLON
        ;
interface_part
        : _INTERFACE_ uses_clause int_decl_sect_list
        ;
uses_clause
        : /* empty */
        | _USES_ uses_list SEMICOLON
        ;
uses_list : identifier
        | uses_list COMMA identifier
        ;
id_list : identifier
        | id_list COMMA identifier
        ;
implementation_part
        : _IMPLEMENTATION_ uses_clause decl_sect_list
        ;
initialization_part
        : /* empty */
        | _INITIALIZATION_ compound_stmt
        ;
finalization_part
        : /* empty */
        | _FINALIZATION_ compound_stmt
        ;

/* Library Sections */
library : library_heading uses_clause block DOT
        ;
library_heading
        : _LIBRARY_ identifier SEMICOLON
        ;

/* Declaration sections: */
decl_sect_list
        : /* empty */
        | decl_sect_list decl_sect
        ;
decl_sect
        : label_decl_sect
        | const_decl_sect
        | type_decl_sect
        | var_decl_sect
        | general_procedure_declaration
        ;

general_procedure_declaration :
          proc_decl
        | func_decl
        | constructor_decl
        | destructor_decl
        ;
label_decl_sect
        : _LABEL_ label_list SEMICOLON
        ;
label_list
        : label
        | label_list COMMA label
        ;
label   : UNSIGNED_INTEGER
                /* must be decimal integer in the range 0..9999 */
        | identifier
        ;
const_decl_sect
        : _CONST_ const_decl
        | const_decl_sect const_decl
        ;
int_type_decl_sect
        : _TYPE_ int_type_decl
        | int_type_decl_sect int_type_decl
        ;
int_type_decl
        : identifier EQUAL int_type SEMICOLON
        ;
int_type
        : type
        | object_type
        | class_type
        | interface_type
        ;
var_decl_sect
        : _VAR_ var_decl
        | var_decl_sect var_decl
        ;

/* Interface declaration sections: */
/* These appear instead of normal declaration sections in the interface
   part of a unit. The difference is that there are no label declarations
   and for procedures and functions only the headings are given. */
int_decl_sect_list
        : /* empty */
        | int_decl_sect_list int_decl_sect
        ;
int_decl_sect
        : const_decl_sect
        | int_type_decl_sect
        | var_decl_sect
        | proc_heading
        | func_heading
        ;

/* Constant declarations: */
const_decl
        : identifier EQUAL const SEMICOLON
        | identifier COLON type EQUAL typed_const SEMICOLON
        ;
const   : simple_const
        | simple_const relop simple_const
        ;
simple_const : const_term
        | sign const_term
        | _NOT_ const_term
        | simple_const addop const_term
        ;
const_term : const_factor
        | const_term mulop const_factor
        ;

const_factor : unsigned_number
        | qualified_identifier
        | general_string_const
        | LPAREN const RPAREN
        | _NIL_
        | AT qualified_identifier
        ;
unsigned_number
        : UNSIGNED_INTEGER
        | UNSIGNED_REAL
        ;
sign    : PLUS | MINUS
        ;
typed_const
        : const
        | array_const
        | record_const
        | set_const
        ;
array_const
        : LPAREN typed_const_list RPAREN
        ;
typed_const_list
        : typed_const
        | typed_const_list COMMA typed_const
        ;
record_const
        : LPAREN const_field_list RPAREN
        ;
const_field_list
        : const_field
        | const_field_list SEMICOLON const_field
        ;
const_field
        : identifier COLON typed_const
        ;
set_const
        : LBRAC const_elem_list RBRAC
        ;
const_elem_list
        : /* empty */
        | const_elem_list1
        ;
const_elem_list1
        : const_elem
        | const_elem_list1 COMMA const_elem
        ;
const_elem
        : const
        | const DOTDOT const
        ;

/* Type declarations: */
type_decl_sect
        : _TYPE_ type_decl
        | type_decl_sect type_decl
        ;
type_decl
        : identifier EQUAL type SEMICOLON
        ;
type    : simple_type
        | pointer_type
        | structured_type
        | string_type
        ;
simple_type
        : identifier
        | const DOTDOT const    /* subrange */
        | LPAREN id_list RPAREN /* enumeration */
        ;
pointer_type
        : UPARROW identifier
        ;
structured_type
        : unpacked_structured_type
        | _PACKED_ unpacked_structured_type
        ;
unpacked_structured_type
        : array_type
        | record_type
        | set_type
        | file_type
        ;
array_type
        : _ARRAY_ LBRAC simple_type_list RBRAC _OF_ type
        ;
simple_type_list
        : simple_type
        | simple_type_list COMMA simple_type
        ;
record_type
        : _RECORD_ field_list _END_
        ;
field_list
        : fixed_part
        | variant_part
        | fixed_part SEMICOLON variant_part
        ;
fixed_part
        : record_section
        | fixed_part SEMICOLON record_section
        ;
record_section
        : /* empty */
        | id_list COLON type
        ;
variant_part
        : _CASE_ tag_field _OF_ variant_list
        ;
tag_field
        : identifier
        | identifier COLON identifier
        ;
variant_list
        : variant
        | variant_list SEMICOLON variant
        ;
variant : /* empty */
        | case_tag_list COLON LPAREN field_list RPAREN
        ;
case_tag_list
        : const
        | case_tag_list COMMA const
        ;
set_type
        : _SET_ _OF_ simple_type
        ;
file_type
        : _FILE_ _OF_ type
        | _FILE_
        ;
string_type
        : _STRING_
        | _STRING_ LBRAC const RBRAC
        ;

/* Object and Class Declaration */
/* ============================ */
class_type
        : class_header class_structure_decl _END_
        ;
class_header
        : _CLASS_ inheritance_class
        | _CLASS_
        ;
inheritance_class
        : /* empty */
        | LPAREN id_list RPAREN  /* inheritance from class and interface(s) */
        ;
class_structure_decl
        : /* empty */
        | class_structure_decl class_component_struct
        ;
class_component_struct
        : visibility_decl class_component_list
        | class_component_list
        ;
visibility_decl
        : _PUBLISHED_ | _PUBLIC_ | _PROTECTED_ | _PRIVATE_ | _AUTOMATED_
        ;
/* The following rule is NOT entirely TRUE : DELPHI DOES NOT ALLOW */
/* TO FREELY MIX Field and procedure declarations                  */
field_or_property_or_method_decl
        : /* empty */
        | class_field_decl
        | property_decl
        | method_definition
        ;
class_field_decl
        : /* empty */
        | id_list COLON type
        ;
class_component_list
        : field_or_property_or_method_decl
        | class_component_list SEMICOLON field_or_property_or_method_decl
        ;
property_decl
        : _PROPERTY_ identifier property_parameter_list COLON type property_accessors SEMICOLON
        ;
property_parameter_list
        : /* empty */
        | LBRAC fp_sect_list RBRAC /* allows VAR in list of parameters! */
        ;
property_accessors
        : read_accessor write_accessor default_accessor
        ;
read_accessor
        : /* empty */
        | DIRECTIVE identifier  /* _READ_ */
        ;
write_accessor
        : /* empty */
        | DIRECTIVE identifier /* _WRITE_*/
        ;
default_accessor
        : /* empty */
        | DIRECTIVE /* _DEFAULT_ */
method_definition
        : method_heading method_directives
        ;
method_heading
        : simple_method_heading
        | class_method_heading
        | constructor_definition
        | destructor_definition
        ;
class_method_heading
        : _CLASS_ simple_method_heading
        ;
simple_method_heading
        : proc_method_heading
        | func_method_heading
        ;
proc_method_heading
        : _PROCEDURE_ identifier fp_list SEMICOLON
        ;
func_method_heading
        : _FUNCTION_ identifier fp_list COLON fptype SEMICOLON
        ;
constructor_definition
        : _CONSTRUCTOR_ qualified_identifier
        fp_list SEMICOLON
        ;
destructor_definition
        : _DESTRUCTOR_ qualified_identifier
        fp_list SEMICOLON
        ;
method_directives
        : static_dynamic_directive call_directive abstract_directive
        ;
static_dynamic_directive
        : /* Empty */
        | DIRECTIVE SEMICOLON /* _VIRTUAL_ or _DYNAMIC_ or _OVERRIDE_ */
        | DIRECTIVE UNSIGNED_INTEGER SEMICOLON /* _MESSAGE_ */
        ;
call_directive
        : /* empty */
        | DIRECTIVE SEMICOLON /* _REGISTER_ or _PASCAL_ or _CDECL_ or _STDCALL_ or _SAFECALL_ */
abstract_directive
        : /* empty */
        | DIRECTIVE  SEMICOLON /* _ABSTRACT_ */
        ;

/* INTERFACE type */
interface_type
        : interface_header interface_component_list _END_
        ;
interface_header
        : _INTERFACE_ inheritance_interface interface_identification
        | _INTERFACE_
        ;
inheritance_interface
        : /* empty */
        | LPAREN IDRPAREN  /* inheritance from another interface */
        ;
interface_identification
        : /* empty */
        | LBRAC STRING_CONST RBRAC /* MORE COMPLEX, BUT IS ENOUGH TO PARSE !!! */
        ;
interface_component_list
        : property_method_list /* same thing ? */
        ;
object_type
        : object_header class_structure_decl _END_ /* GROSS OVER-SIMPLIFICATION !!!*/
        ;
object_header
        : _OBJECT_ inheritance_object
        | _OBJECT
        ;
inheritance_object
        : /* empty */
        | LPAREN IDRPAREN  /* inheritance from another object */
        ;

/* Variable declarations: */
var_decl
        : id_list COLON type absolute_clause SEMICOLON
        ;
absolute_clause
        : /* empty */
        | _ABSOLUTE_ UNSIGNED_INTEGER COLON UNSIGNED_INTEGER
        | _ABSOLUTE_ identifier
        ;

/* Procedure and function declarations: */
proc_decl
        : proc_heading proc_block SEMICOLON
        ;
func_decl
        : func_heading proc_block SEMICOLON
        ;
constructor_decl
        : constructor_heading proc_block SEMICOLON
        ;
destructor_decl
        : destructor_heading proc_block SEMICOLON
        ;
proc_heading
        : _PROCEDURE_ qualified_identifier
          fp_list SEMICOLON
        ;
func_heading
        : _FUNCTION_ qualified_identifier
          fp_list COLON fptype SEMICOLON
        ;
constructor_heading
        : _CONSTRUCTOR_ qualified_identifier
        fp_list SEMICOLON
        ;
destructor_heading
        : _DESTRUCTOR_ qualified_identifier
        fp_list SEMICOLON
        ;
proc_block              /* omitted inline, assembler, etc. and interrupt */
        : block
        | _EXTERNAL_
        | _FORWARD_
        ;
block   : decl_sect_list compound_stmt
        ;
fp_list : /* empty */
        | LPAREN fp_sect_list RPAREN
        ;
fp_sect_list
        : fp_sect
        | fp_sect_list SEMICOLON fp_sect
        ;
fp_sect : id_list COLON fptype
        | _VAR_ id_list COLON fptype
        | _VAR_ id_list
        ;
fptype  : identifier
        | _STRING_
        ;

/* Statements: */
stmt    : unlabelled_stmt
        | label COLON unlabelled_stmt
        ;
unlabelled_stmt
        : /* empty */
        | general_proc_call
        | assignment
        | goto_stmt
        | compound_stmt
        | if_stmt
        | case_stmt
        | repeat_stmt
        | while_stmt
        | for_stmt
        | with_stmt
        | try_statement /* purely Delphi stuff ! */
        ;
assignment
        : variable ASSIGNMENT expr
        ;
general_proc_call
        : _INHERITED_ proc_call
        | _INHERITED_
        | proc_call
        ;
proc_call
        : variable  /* instead if proc_ident param_list */
        | variable DOT proc_call;
        ;
/* object cast followed by method call is here for procedures */
/* RULES REMOVED : */
/* proc_ident : qualified_identifier */
/*        ; */
/* param_list */
/*      : /* empty  */
/*      | LPAREN expr_list RPAREN*/
/*      ;*/

expr_list
        : expr
        | expr_list COMMA expr
        ;
goto_stmt
        : _GOTO_ label
        ;
compound_stmt
        : _BEGIN_ stmt_list _END_
        ;
stmt_list
        : stmt
        | stmt_list SEMICOLON stmt
        ;
if_stmt : _IF_ expr _THEN_ stmt
        | _IF_ expr _THEN_ stmt _ELSE_ stmt
        ;
case_stmt
        : _CASE_ expr _OF_ case_list else_case _END_
        ;
case_list
        : case
        | case_list SEMICOLON case
        ;
case    : /* empty */
        | case_label_list COLON stmt
        ;
case_label_list
        : case_label
        | case_label_list COMMA case_label
        ;
case_label
        : const
        | const DOTDOT const
        ;
else_case
        : /* empty */
        | _ELSE_ stmt
        | _ELSE_ stmt SEMICOLON
        ;
repeat_stmt
        : _REPEAT_ stmt_list _UNTIL_ expr
        ;
while_stmt
        : _WHILE_ expr _DO_ stmt
        ;
for_stmt
        : _FOR_ identifier ASSIGNMENT expr _TO_ expr _DO_ stmt
        | _FOR_ identifier ASSIGNMENT expr _DOWNTO_ expr _DO_ stmt
        ;
with_stmt
        : _WITH_ variable_list _DO_ stmt
        ;
variable_list
        : variable
        | variable_list COMMA variable
        ;

/* TRY STATEMENTS */
try_statement
        : _TRY_ stmt_list try_protected_code
        ;
try_protected_code
        : try_except_protected_code
        | try_finally_protected_code
        ;
try_finally_protected_code
        : _FINALLY_ stmt_list _END_
        ;
try_except_protected_code
        : _EXCEPT_ exception_bloc _END_
        ;
exception_bloc
        : stmt_list
        | general_exception_handlers_list
        ;
general_exception_handlers_list
        : exception_handlers_list exception_else_clause
        ;
exception_handlers_list
        : exception_handler
        | exception_handlers_list SEMICOLON exception_handler
        ;
exception_handler
        : /*empty*/
        | _ON_ exception_selector _DO_ stmt
        ;
exception_selector
        : identifier
        | identifier COLON identifier
        ;
exception_else_clause
        : /* empty */
        | _ELSE_ stmt_list
        ;

/* Variables: */
variable
        : qualified_identifier
        | variable LBRAC expr_list RBRAC                /* array component */
        | variable DOT identifier                       /* record field */
        | variable UPARROW                              /* pointer value */
        /* procedure or type cast with call behind */
        | qualified_identifier LPAREN expr_list RPAREN DOT variable
        /* simple procedure or type cast */
        | qualified_identifier LPAREN expr_list RPAREN
        ;

/* Expressions: */
expr    : simple_expr
        | simple_expr relop simple_expr
        ;
relop   : EQUAL | NOTEQUAL | LT | GT | LE | GE | _IN_ | _IS_
        ;
simple_expr
        : term
        | sign term
        | _NOT_ term
        | AT term
        | simple_expr addop term
        ;
addop   : PLUS | MINUS | _OR_ | _XOR_
        ;
term    : factor
        | term mulop factor
        ;
mulop   : STAR | SLASH | _DIV_ | _MOD_ | _AND_ | _SHL_ | _SHR_ | _AS_
        ;

/* Parameterless function calls, and function calls looking like type
   casts are caught as variables. */
factor  : variable
        | UNSIGNED_INTEGER
        | UNSIGNED_REAL
        | general_string_const
        | LBRAC elem_list RBRAC
        | _NIL_
        | '@' variable
        | LPAREN expr RPAREN
        | qualified_identifier LPAREN expr_list RPAREN
        | _NOT_ factor
        ;
elem_list
        : /* empty */
        | elem_list1
        ;
elem_list1
        : elem
        | elem_list1 COMMA elem
        ;
elem    : expr
        | expr DOTDOT expr
        ;

/* This allows identifiers to have same names as directives. NOT TESTED ! */
identifier : ID
           | DIRECTIVE
           ;
qualified_identifier : identifier
                     | qualified_identifier DOT identifier
                     ;

general_string_const : STRING_CONST
                     | DIESE UNSIGNED_INTEGER
                     ;
