// Java 1.1 Recognizer Grammar
//
// A note about conflicts.  There are four points in this grammar where
//   two tokens of lookahead cannot predict which alternative to select.  These
//   points are:
//   1) standard "else" ambiguity
//   2) index reference after array creation:
//        new int[1][2][3]  // is the "3" an index or dim spec?
//   3) {1,2,3,}    // does the last comma start a new init or is it garbage?
//   4) ((caseLabel|"default")+ (statement)* )*
//      nasty conflict, but proper code is generated
//
// Each of these conflicts are noted in the grammar where they occur, and these
//   are no worry as long as these are the only conflicts reported by ANTLR
//
// Run 'java JavaRecognizer <directory full of java files>'
//
// Terence Parr tested this grammar with antlr 2.20b7 and Scott
// said he tested it under 2.11.
//
// Authors:
//		John Mitchell		johnm@non.net
//		Terence Parr		parrt@magelang.com
//		John Lilley			jlilley@empathy.com
//		Scott Stanchfield	thetick@magelang.com
//
// Version 1.00 December 9, 1997 -- initial release
// Version 1.01 December 10, 1997
//		fixed bug in octal def (0..7 not 0..8)
//
// This grammar is in the PUBLIC DOMAIN
//

// tell ANTLR that we want to generate Java source code
options {
	language="Java";
}

// Import the necessary classes
{
import java.io.*;
}


//-----------------------------------------------------------------------------
// Define a Parser, calling it JavaRecognizer
//-----------------------------------------------------------------------------
class JavaRecognizer extends Parser;
options {
	k = 2;                           // two token lookahead
	tokenVocabulary=Java;            // Call its vocabulary "Java"
	codeGenMakeSwitchThreshold = 2;  // Some optimizations
	codeGenBitsetTestThreshold = 3;
	defaultErrorHandler = false;     // Don't generate parser error handlers
}


// Define some methods and variables to use in the generated parser.
{
	// Define a main
	public static void main(String[] args) {
		// Use a try/catch block for parser exceptions
		try {
			// if we have at least one command-line argument
			if (args.length > 0 ) {
				System.err.println("Parsing...");

				// for each directory/file specified on the command line
				for(int i=0; i< args.length;i++)
					doFile(new File(args[i])); // parse it
			}
			else
				System.err.println("Usage: java JavaRecogizer "+
                                   "<directory name>");

		}
		catch(Exception e) {
			System.err.println("exception: "+e);
			e.printStackTrace(System.err);   // so we can get stack trace
		}
	}


	// This method decides what action to take based on the type of
	//   file we are looking at
	public static void doFile(File f)
							  throws Exception {
		// If this is a directory, walk each file/dir in that directory
		if (f.isDirectory()) {
			String files[] = f.list();
			for(int i=0; i < files.length; i++)
				doFile(new File(f, files[i]));
		}

		// otherwise, if this is a java file, parse it!
		else if ((f.getName().length()>5) &&
				f.getName().substring(f.getName().length()-5).equals(".java")) {
			System.err.println("   "+f.getAbsolutePath());
			parseFile(new FileInputStream(f));
		}
	}

	// Here's where we do the real work...
	public static void parseFile(InputStream s)
								 throws Exception {
		try {
			// Create a scanner that reads from the input stream passed to us
			JavaLexer lexer = new JavaLexer(s);

			// Create a parser that reads from the scanner
			JavaRecognizer parser = new JavaRecognizer(lexer);

			// start parsing at the compilationUnit rule
			parser.compilationUnit();
		}
		catch (Exception e) {
			System.err.println("parser exception: "+e);
			e.printStackTrace();   // so we can get stack trace		
		}
	}
}



// Compilation Unit: In Java, this is a single file.  This is the start
//   rule for this parser
compilationUnit
	:	// A compilation unit starts with an optional package definition
		(	packageDefinition
		|	/* nothing */
		)

		// Next we have a series of zero or more import statements
		( importDefinition )*

		// Wrapping things up with any number of class or interface
		//    definitions
		( typeDefinition )*

		EOF
	;


// Package statement: "package" followed by an identifier.
packageDefinition
	options {defaultErrorHandler = true;} // let ANTLR handle errors
	:	"package" identifier SEMI
	;


// Import statement: import followed by a package or class name
importDefinition
	options {defaultErrorHandler = true;}
	:	"import" identifierStar SEMI
	;


// A type definition in a file is either a class or interface definition.
typeDefinition
	options {defaultErrorHandler = true;}
	:	modifiers
		( classDefinition
		| interfaceDefinition
		)
	|	SEMI
	;



// A declaration is the creation of a reference or primitive-type variable
declaration
	:	modifiers typeSpec variableDefinitions
	;


// A list of zero or more modifiers.  We could have used (modifier)* in
//   place of a call to modifiers, but I thought it was a good idea to keep
//   this rule separate so they can easily be collected in a Vector if
//   someone so desires
modifiers
	:	( modifier )*
	;


// A type specification is a type name with possible brackets afterwards
//   (which would make it an array type).
typeSpec
	:	type (LBRACK RBRACK )*
	;


// A type name. which is either a (possibly qualified) class name or
//   a primitive (builtin) type
type
	:	identifier
	|	builtInType
	;


// The primitive types.
builtInType
	:	"void"
	|	"boolean"
	|	"byte"
	|	"char"
	|	"short"
	|	"int"
	|	"float"
	|	"long"
	|	"double"
	;


// A (possibly-qualified) java identifier.  We start with the first IDENT
//   and expand its name by adding dots and following IDENTS
identifier
	:	IDENT  ( DOT IDENT )*
	;


identifierStar
	:	IDENT
		( DOT IDENT )*
		( DOT STAR  )?
	;


// modifiers for Java classes, interfaces, class/instance vars and methods
modifier
	:	"private"
	|	"public"
	|	"protected"
	|	"static"
	|	"transient"
	|	"final"
	|	"abstract"
	|	"native"
	|	"threadsafe"
	|	"synchronized"
	|	"const"
	;


// Definition of a Java class
classDefinition
	:	"class" IDENT // aha! a class!

		// it _might_ have a superclass...
		( "extends" identifier )?

		// it might implement some interfaces...
		( implementsClause )?

		// now parse the body of the class
		classBlock
	;



// Definition of a Java Interface
interfaceDefinition
	:	"interface" IDENT // aha! an interface!
			
		// it might extend some other interfaces
		(interfaceExtends)?

		// now parse the body of the interface (looks like a class...)
		classBlock
	;


// This is the body of a class.  You can have fields and extra semicolons,
// That's about it (until you see what a field is...)
classBlock
	:	LCURLY
			( field | SEMI )*
		RCURLY
	;


// An interface can extend several other interfaces...
interfaceExtends
	:	"extends" identifier ( COMMA identifier )*
	;


// A class can implement several interfaces...
implementsClause
	:	"implements" identifier
		( COMMA identifier )*
	;



// Now the various things that can be defined inside a class or interface...
// Note that not all of these are really valid in an interface (constructors,
//   for example), and if this grammar were used for a compiler there would
//   need to be some semantic checks to make sure we're doing the right thing...
field
	:	// method, constructor, or variable declaration
		modifiers
		(	methodHead compoundStatement // constructor

		|	classDefinition              // inner class
		|	interfaceDefinition          // inner interface

		|	typeSpec  // method or variable declaration(s)
			(	methodHead ( compoundStatement | SEMI )

			|	variableDefinitions SEMI
			)
		)

    // "static { ... }" class initializer
	|	"static" compoundStatement

    // "{ ... }" instance initializer
	|	compoundStatement
	;


variableDefinitions
	:	variableDeclarator
		(COMMA variableDeclarator )*
	;


// Declaration of a variable.  This can be a class/instance variable,
//   or a local variable in a method
// It can also include possible initialization.
variableDeclarator
	:	IDENT (LBRACK RBRACK)* ( ASSIGN initializer )?
	;


// This is an initializer used to set up an array.
// CONFLICT: does a COMMA after an initializer start a new
//           (...)* or start the (...)?
//           ANTLR generates proper code due to LA(2)
arrayInitializer
	:	LCURLY
			(	initializer ( COMMA initializer )*
				(COMMA)?
			)?
		RCURLY
	;


// The two "things" that can initialize an array element are an expression
//   and another (nested) array initializer.
initializer
	:	expression
	|	arrayInitializer
	;


// This is the header of a method.  It includes the name and parameters
//   for the method.
//   This also watches for a list of exception classes in a "throws" clause.
methodHead
	:	IDENT  // the name of the method

		// parse the formal parameter declarations.
		LPAREN (parameterDeclarationList)? RPAREN

		// again, the array specification is skipped...
		(LBRACK RBRACK)*

		// get the list of exceptions that this method is declared to throw
		(throwsClause)?
	;


// This is a list of exception classes that the method is declared to throw
throwsClause
	:	"throws" identifier ( COMMA identifier )*
	;


// A list of formal parameters
parameterDeclarationList
	:	parameterDeclaration ( COMMA parameterDeclaration )*
	;


// A formal parameter.
parameterDeclaration
	:	("final")? typeSpec IDENT (LBRACK RBRACK)*
	;



// Compound statement.  This is used in many contexts:
//   Inside a class definition prefixed with "static":
//      it is a class initializer
//   Inside a class definition without "static":
//      it is an instance initializer
//   As the body of a method
//   As a completely indepdent braced block of code inside a method
//      it starts a new scope for variable definitions

compoundStatement
	:	LCURLY
			// include the (possibly-empty) list of statements
			(statement)*
		RCURLY
	;


// Here are all the wonderful Java statements...
statement
	// A list of statements in curly braces -- start a new scope!
	:	compoundStatement

	// If it _looks_ like a decl, it's a decl...
	|	(declaration)=> declaration SEMI

	// Attach a label to the front of a statement
	|	IDENT COLON statement

	// An expression statement.  This could be a method call, assignment
	//   statement, or any other expression evaluated for side-effects.
	|	expression SEMI

	// If-else statement
	// CONFLICT: the old "dangling-else" problem...
	//           ANTLR generates proper code by just making the "else"
	//           optional!
	|	"if" LPAREN expression RPAREN statement
		( "else" statement )?

		// the "else" part above is ambiguous.  The intent
		// is to keep it as close to the corresponding "if"
		// as possible.  The generated code will do this,
		// so we can live with the ambiguity.  We could do
		//		(	("else")=> "else" statement 
		//		|	// no else clause
        //		)
		// instead, but that's less efficient...

	// For statement
	|	"for"
			LPAREN
				(forInit)?        SEMI    // initializer
				(expression)?     SEMI    // condition test
				(expressionList)?         // updater
			RPAREN
			statement                     // statement to loop over

	// While statement
	|	"while" LPAREN expression RPAREN statement

	// do-while statement
	|	"do" statement "while" LPAREN expression RPAREN SEMI

	// get out of a loop (or switch)
	|	"break" (IDENT)? SEMI

	// do next iteration of a loop
	|	"continue" (IDENT)? SEMI

	// Return an expression
	|	"return" (expression)? SEMI

	// switch/case statement
	// CONFLICT: to which "cases" does the statement bind?
	//           ANTLR generates proper code as it groups as
	//           many "case"/"default" labels together then
	//           follows them with the statements
	|	"switch" LPAREN expression RPAREN
			LCURLY
				(	(("case" expression | "default") COLON)+ (statement)*
					// ambiguous but proper code will be generated...
				)*
			RCURLY

	// exception try-catch block
	|	tryBlock

	// throw an exception
	|	"throw" expression SEMI

	// synchronize a statement
	|	"synchronized" LPAREN expression RPAREN statement

	// empty statement
	|	SEMI
	;


// The initializer for a for loop
forInit
	// if it looks like a declaration, it is
	:	(declaration)=> declaration
	// otherwise it could be an expression list...
	|	expressionList
	;


// an exception handler try/catch block
tryBlock
	:	"try" compoundStatement
		(handler)*
		( "finally" compoundStatement )?
	;


// an exception handler
handler
	:	"catch" LPAREN parameterDeclaration RPAREN compoundStatement
	;


// expressions -- the FUN stuff!
// Note that most of these expressions follow the pattern
//   thisLevelExpression :
//       nextHigherPrecedenceExpression
//           (OPERATOR nextHigherPrecedenceExpression)*
// which is a standard recursive definition for a parsing an expression.
// The operators in java have the following precedences:
//    lowest  (13)  = *= /= %= += -= <<= >>= >>>= &= ^= |=
//            (12)  ?:
//            (11)  ||
//            (10)  &&
//            ( 9)  |
//            ( 8)  ^
//            ( 7)  &
//            ( 6)  == !=
//            ( 5)  < <= > >=
//            ( 4)  << >>
//            ( 3)  +(binary) -(binary)
//            ( 2)  * / %
//            ( 1)  ++ -- +(unary) -(unary)  ~  !  (type)
//                  []   () (method call)  . (dot -- identifier qualification)
//                  new   ()  (explicit parenthesis)
//
// the last two are not usually on a precedence chart; I put them in
// to point out that new has a higher precedence than '.', so you
// can validy use
//     new Frame().show()
// 
// Note that the above precedence levels map to the rules below...
// Once you have a precedence chart, writing the appropriate rules as below
//   is usually very straightfoward



// the mother of all expressions
expression
	:	assignmentExpression
	;


// This is a list of expressions.
expressionList
	:	expression (COMMA expression)*
	;


// assignment expression (level 13)
assignmentExpression
	:	conditionalExpression
		(	(	ASSIGN
            |   PLUS_ASSIGN
            |   MINUS_ASSIGN
            |   STAR_ASSIGN
            |   DIV_ASSIGN
            |   MOD_ASSIGN
            |   SR_ASSIGN
            |   BSR_ASSIGN
            |   SL_ASSIGN
            |   BAND_ASSIGN
            |   BXOR_ASSIGN
            |   BOR_ASSIGN
            )
			assignmentExpression
		)?
	;


// conditional test (level 12)
conditionalExpression
	:	logicalOrExpression
		( QUESTION conditionalExpression COLON conditionalExpression )?
	;


// logical or (||)  (level 11)
logicalOrExpression
	:	logicalAndExpression (LOR logicalAndExpression)*
	;


// logical and (&&)  (level 10)
logicalAndExpression
	:	inclusiveOrExpression (LAND inclusiveOrExpression)*
	;


// bitwise or non-short-circuiting or (|)  (level 9)
inclusiveOrExpression
	:	exclusiveOrExpression (BOR exclusiveOrExpression)*
	;


// exclusive or (^)  (level 8)
exclusiveOrExpression
	:	andExpression (BXOR andExpression)*
	;


// bitwise or non-short-circuiting and (&)  (level 7)
andExpression
	:	equalityExpression (BAND equalityExpression)*
	;


// equality/inequality (==/!=) (level 6)
equalityExpression
	:	relationalExpression ((NOT_EQUAL | EQUAL) relationalExpression)*
	;


// boolean relational expressions (level 5)
relationalExpression
	:	shiftExpression
		(	(	LT
			|	GT
			|	LE
			|	GE
			)
			shiftExpression
		)*
	;


// bit shift expressions (level 4)
shiftExpression
	:	additiveExpression ((SL | SR | BSR) additiveExpression)*
	;


// binary addition/subtraction (level 3)
additiveExpression
	:	multiplicativeExpression ((PLUS | MINUS) multiplicativeExpression)*
	;


// multiplication/division/modulo (level 2)
multiplicativeExpression
	:	castExpression ((STAR | DIV | MOD ) castExpression)*
	;


// cast/unary (level 1)
castExpression
	// if it _looks_ like a cast, it _is_ a cast
	:	( LPAREN typeSpec RPAREN castExpression )=>
			LPAREN typeSpec RPAREN c:castExpression

	// otherwise it's a unary expression
	|	INC castExpression
	|	DEC castExpression
	|	MINUS castExpression
	|	BNOT castExpression
	|	LNOT castExpression
	|	postfixExpression ( "instanceof" typeSpec )?
		// instanceof should not allow just primitives (x instanceof int)
		// need a semantic check if we're compiling...
	;


// qualified names, array expressions, method invocation, post inc/dec
postfixExpression
	:	primaryExpression // start with a primary

		
		(	// qualified id (id.id.id.id...) -- buid the name
			DOT ( IDENT
				| "this"
				| "class"
				)
			// the above line needs a semantic check to make sure "class"
			//   is the _last_ qualifier.

		// an array indexing operation
		|	LBRACK expression RBRACK

		// method invocation
		// The next line is not strictly proper; it allows x(3)(4) or
		//   x[2](4) which are not valid in Java.  If this grammar were used
		//   to validate a Java program a semantic check would be needed, or
		//   this rule would get really ugly...
		|	LPAREN
				( expressionList
				| /*nothing*/
				)
			RPAREN
		)*

		// possibly add on a post-increment or post-decrement
		(	INC
	 	|	DEC
		|	// nothing
		)
	;


// the basic element of an expression
primaryExpression
	:	IDENT
	|	builtInType DOT "class"
	|	newExpression
	|	constant
	|	"super"
	|	"true"
	|	"false"
	|	"this"
	|	"null"
	|	LPAREN expression RPAREN
	;


// object instantiation.
newExpression
	:	"new" type
		(	LPAREN
				( expressionList
				| /*nothing*/
				)
			RPAREN 
				// java 1.1
				(classBlock)?

		//java 1.1
		// Note: This will allow bad constructs like
		//    new int[4][][3] {exp,exp}.
		//    There needs to be a semantic check here...
		// to make sure:
		//   a) [ expr ] and [ ] are not mixed
		//   b) [ expr ] and an init are not used together
		|	(
				// CONFLICT:
				// newExpression is a primaryExpression which can be
				// followed by an array index reference.  This is ok,
				// as the generated code will stay in this loop as
				// long as it sees an LBRACK (proper behavior)
				LBRACK
					(expression)?
				RBRACK
			)+
			(arrayInitializer)?

		)
	;


constant
	:	NUM_INT
	|	CHAR_LITERAL
	|	STRING_LITERAL
	|	NUM_FLOAT
	;


//----------------------------------------------------------------------------
// The Java scanner
//----------------------------------------------------------------------------
class JavaLexer extends Lexer;

options {
	tokenVocabulary=Java;  // call the vocabulary "Java"
	longestPossible=true;  // gobble characters to find the longest match
	testLiterals=false;    // don't automatically test for literals
	k=4;                   // four characters of lookahead
}



// OPERATORS
QUESTION		:	'?'		;
LPAREN			:	'('		;
RPAREN			:	')'		;
LBRACK			:	'['		;
RBRACK			:	']'		;
LCURLY			:	'{'		;
RCURLY			:	'}'		;
COLON			:	':'		;
COMMA			:	','		;
//DOT				:	'.'		;
ASSIGN			:	'='		;
EQUAL			:	"=="	;
LNOT			:	'!'		;
BNOT			:	'~'		;
NOT_EQUAL		:	"!="	;
DIV				:	'/'		;
DIV_ASSIGN		:	"/="	;
PLUS			:	'+'		;
PLUS_ASSIGN		:	"+="	;
INC				:	"++"	;
MINUS			:	'-'		;
MINUS_ASSIGN	:	"-="	;
DEC				:	"--"	;
STAR			:	'*'		;
STAR_ASSIGN		:	"*="	;
MOD				:	'%'		;
MOD_ASSIGN		:	"%="	;
SR				:	">>"	;
SR_ASSIGN		:	">>="	;
BSR				:	">>>"	;
BSR_ASSIGN		:	">>>="	;
GE				:	">="	;
GT				:	">"		;
SL				:	"<<"	;
SL_ASSIGN		:	"<<="	;
LE				:	"<="	;
LT				:	'<'		;
BXOR			:	'^'		;
BXOR_ASSIGN		:	"^="	;
BOR				:	'|'		;
BOR_ASSIGN		:	"|="	;
LOR				:	"||"	;
BAND			:	'&'		;
BAND_ASSIGN		:	"&="	;
LAND			:	"&&"	;
SEMI			:	';'		;


// Whitespace -- ignored
WS
	:	(	' '
		|	'\t'
		|	'\f'
		// handle newlines
		|	(	"\r\n"  // Evil DOS
			|	'\r'    // Macintosh
			|	'\n'    // Unix (the right way)
			)
			{ newline(); }
		)
		{ _ttype = Token.SKIP; }
	;


// Single-line comments
SL_COMMENT
	:	"//" (~'\n')* '\n'
			{ _ttype = Token.SKIP; newline(); }
	;


// multiple-line comments
ML_COMMENT
	:	"/*"
		(	{ LA(2)!='/' }? '*'
		|	'\n' { newline(); }
		|	~('*'|'\n')
		)*
		"*/"
			{ _ttype = Token.SKIP; }
	;


// character literals
CHAR_LITERAL
	:	'\'' ( ESC | ~'\'' ) '\''
	;

// string literals
STRING_LITERAL
	:	'"' (ESC|~'"')* '"'
	;


// escape sequence -- note that this is protected; it can only be called
//   from another lexer rule -- it will not ever directly return a token to
//   the parser
protected
ESC
	:	'\\'
		(	'n'
		|	'r'
		|	't'
		|	'b'
		|	'f'
		|	'"'
		|	'\''
		|	'\\'
		|	('u')+ HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT 
		|	('0'..'3') ( ('0'..'9') ('0'..'9')? )?
		|	('4'..'7') ('0'..'9')?
		)
	;


// hexadecimal digit (again, note it's protected!)
protected
HEX_DIGIT
	:	('0'..'9'|'A'..'F'|'a'..'f')
	;


// a dummy rule to force vocabulary to be all characters (except special
//   ones that ANTLR uses internally (0 to 2)
protected
VOCAB
	:	'\3'..'\377'
	;


// an identifier.  Note that testLiterals is set to true!  This means
// that after we match the rule, we look in the literals table to see
// if it's a literal or really an identifer
IDENT
	options {testLiterals=true;}
	:	('a'..'z'|'A'..'Z'|'_'|'$') ('a'..'z'|'A'..'Z'|'_'|'0'..'9'|'$')*
	;


// a numeric literal
NUM_INT
	{boolean isDecimal=false;}
	:	'.' {_ttype = DOT;}
			(('0'..'9')+ (EXPONENT)? (FLOAT_SUFFIX)? { _ttype = NUM_FLOAT; })?
	|	(	'0' {isDecimal = true;} // special case for just '0'
			(	('x'|'X') (HEX_DIGIT)+						// hex
			|	('0'..'7')+									// octal
			)?
		|	('1'..'9') ('0'..'9')*  {isDecimal=true;}		// non-zero decimal
		)
		(	('l'|'L')
		
		// only check to see if it's a float if looks like decimal so far
		|	{isDecimal}?
			(	'.' ('0'..'9')* (EXPONENT)? (FLOAT_SUFFIX)?
			|	EXPONENT (FLOAT_SUFFIX)?
			|	FLOAT_SUFFIX
			)
			{ _ttype = NUM_FLOAT; }
		)?
	;


// a couple protected methods to assist in matching floating point numbers
protected
EXPONENT
	:	('e'|'E') ('+'|'-')? ('0'..'9')+
	;


protected
FLOAT_SUFFIX
	:	'f'|'F'|'d'|'D'
	;

