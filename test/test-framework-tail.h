/*
Tail Section, which includes all the scaffolding to compile and run the generated parser
*/


#if defined(YYERROR_DETAILED)

void yyerror_detailed(const char *msg, int lexed_token, YYSTYPE yylval, YYPOSN yyposn, unsigned int yystate)
{
	int ri, si, ci;
	int i;

#if defined(YYDEBUG)
	fprintf(stderr, "%s (when parsing token '%s' at input position %d)\n", msg, ((lexed_token >= 0 && lexed_token < YYMAXTOKEN) ? yyname[lexed_token] : "illegal-token"), yyposn);
#else
	fprintf(stderr, "%s (when parsing token '%d' at input position %d)\n", msg, lexed_token, yyposn);
#endif

	/*
	print EXPECT set:
	*/
	if (yystate >= 0)
	{
		fprintf(stderr, "EXPECTED input:\n");

		ri = yyrindex[yystate];
		si = yysindex[yystate];
		ci = yycindex[yystate];
		for (i = 1; i < YYMAXTOKEN; i++)
		{
			if (ri != 0 && (ri + i <= YYTABLESIZE) && yycheck[ri + i] == i)
			{
#if defined(YYDEBUG)
				fprintf(stderr, "%s (%d) -> %s\n", yyname[i], i, "reduction");
#else
				fprintf(stderr, "(%d) -> %s\n", i, "reduction");
#endif
			}
			if (si != 0 && (si + i <= YYTABLESIZE) && yycheck[si + i] == i)
			{
#if defined(YYDEBUG)
				fprintf(stderr, "%s (%d) -> %s\n", yyname[i], i, "shift");
#else
				fprintf(stderr, "(%d) -> %s\n", i, "shift");
#endif
			}
			if (ci != 0 && (ci + i <= YYTABLESIZE) && yycheck[ci + i] == i)
			{
#if defined(YYDEBUG)
				fprintf(stderr, "%s (%d) -> %s\n", yyname[i], i, "CONFLICT");
#else
				fprintf(stderr, "(%d) -> %s\n", i, "CONFLICT");
#endif
			}
		}
	}
}

#else

void yyerror(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

#endif

#define TILDE					'~'
#define BACKQUOTE				'`'
#define EXCLAMATION_MARK		'!'
#define AT						'@'
#define HASHTAG					'#'
#define DOLLAR					'$'
#define PERCENT					'%'
#define CIRCONFLEX				'^'
#define AMPERSAND				'&'
#define STAR					'*'
#define OPEN_BRACE				'('
#define CLOSE_BRACE				')'
#define UNDERSCORE				'_'
#define MINUS					'-'
#define PLUS					'+'
#define IS						'='
#define OPEN_SQUARE_BRACKET		'['
#define CLOSE_SQUARE_BRACKET	']'
#define OPEN_CURLY_BRACKET		'{'
#define CLOSE_CURLY_BRACKET		'}'
#define COLON					':'
#define SEMICOLON				';'
#define QUOTE					'\''
#define DOUBLEQUOTE				'"'
#define LESS_THAN				'<'
#define GREATER_THAN			'>'
#define COMMA					','
#define PERIOD					'.'
#define QUESTION_MARK			'?'
#define SLASH					'/'
#define BACKSLASH				'\\'
#define PIPE					'|'

#ifndef YYSYMBOL_TILDE
#define YYSYMBOL_TILDE							0
#endif
#ifndef YYSYMBOL_BACKQUOTE
#define YYSYMBOL_BACKQUOTE						0
#endif
#ifndef YYSYMBOL_EXCLAMATION_MARK
#define YYSYMBOL_EXCLAMATION_MARK				0
#endif
#ifndef YYSYMBOL_AT
#define YYSYMBOL_AT								0
#endif
#ifndef YYSYMBOL_HASHTAG
#define YYSYMBOL_HASHTAG						0
#endif
#ifndef YYSYMBOL_DOLLAR
#define YYSYMBOL_DOLLAR							0
#endif
#ifndef YYSYMBOL_PERCENT
#define YYSYMBOL_PERCENT						0
#endif
#ifndef YYSYMBOL_CIRCONFLEX
#define YYSYMBOL_CIRCONFLEX						0
#endif
#ifndef YYSYMBOL_AMPERSAND
#define YYSYMBOL_AMPERSAND						0
#endif
#ifndef YYSYMBOL_STAR
#define YYSYMBOL_STAR							0
#endif
#ifndef YYSYMBOL_OPEN_BRACE
#define YYSYMBOL_OPEN_BRACE						0
#endif
#ifndef YYSYMBOL_CLOSE_BRACE
#define YYSYMBOL_CLOSE_BRACE					0
#endif
#ifndef YYSYMBOL_UNDERSCORE
#define YYSYMBOL_UNDERSCORE						0
#endif
#ifndef YYSYMBOL_MINUS
#define YYSYMBOL_MINUS							0
#endif
#ifndef YYSYMBOL_PLUS
#define YYSYMBOL_PLUS							0
#endif
#ifndef YYSYMBOL_IS
#define YYSYMBOL_IS								0
#endif
#ifndef YYSYMBOL_OPEN_SQUARE_BRACKET
#define YYSYMBOL_OPEN_SQUARE_BRACKET			0
#endif
#ifndef YYSYMBOL_CLOSE_SQUARE_BRACKET
#define YYSYMBOL_CLOSE_SQUARE_BRACKET			0
#endif
#ifndef YYSYMBOL_OPEN_CURLY_BRACKET
#define YYSYMBOL_OPEN_CURLY_BRACKET				0
#endif
#ifndef YYSYMBOL_CLOSE_CURLY_BRACKET
#define YYSYMBOL_CLOSE_CURLY_BRACKET			0
#endif
#ifndef YYSYMBOL_COLON
#define YYSYMBOL_COLON							0
#endif
#ifndef YYSYMBOL_SEMICOLON
#define YYSYMBOL_SEMICOLON						0
#endif
#ifndef YYSYMBOL_QUOTE
#define YYSYMBOL_QUOTE							0
#endif
#ifndef YYSYMBOL_DOUBLEQUOTE
#define YYSYMBOL_DOUBLEQUOTE					0
#endif
#ifndef YYSYMBOL_LESS_THAN
#define YYSYMBOL_LESS_THAN						0
#endif
#ifndef YYSYMBOL_GREATER_THAN
#define YYSYMBOL_GREATER_THAN					0
#endif
#ifndef YYSYMBOL_COMMA
#define YYSYMBOL_COMMA							0
#endif
#ifndef YYSYMBOL_PERIOD
#define YYSYMBOL_PERIOD							0
#endif
#ifndef YYSYMBOL_QUESTION_MARK
#define YYSYMBOL_QUESTION_MARK					0
#endif
#ifndef YYSYMBOL_SLASH
#define YYSYMBOL_SLASH							0
#endif
#ifndef YYSYMBOL_BACKSLASH
#define YYSYMBOL_BACKSLASH						0
#endif
#ifndef YYSYMBOL_PIPE
#define YYSYMBOL_PIPE							0
#endif

#ifndef YYSYMBOL_PLUS_IS
#define YYSYMBOL_PLUS_IS											0
#endif

#ifndef YYSYMBOL_MINUS_IS
#define YYSYMBOL_MINUS_IS											0
#endif

#ifndef YYSYMBOL_MINUS_GREATER_THAN
#define YYSYMBOL_MINUS_GREATER_THAN									0
#endif

#ifndef YYSYMBOL_STAR_IS
#define YYSYMBOL_STAR_IS											0
#endif

#ifndef YYSYMBOL_STAR_STAR
#define YYSYMBOL_STAR_STAR											0
#endif

#ifndef YYSYMBOL_SLASH_IS
#define YYSYMBOL_SLASH_IS											0
#endif

#ifndef YYSYMBOL_LESS_THAN_IS
#define YYSYMBOL_LESS_THAN_IS										0
#endif

#ifndef YYSYMBOL_LESS_THAN_GREATER_THAN
#define YYSYMBOL_LESS_THAN_GREATER_THAN								0
#endif

#ifndef YYSYMBOL_LESS_THAN_LESS_THAN
#define YYSYMBOL_LESS_THAN_LESS_THAN								0
#endif

#ifndef YYSYMBOL_LESS_THAN_LESS_THAN_IS
#define YYSYMBOL_LESS_THAN_LESS_THAN_IS								0
#endif

#ifndef YYSYMBOL_LESS_THAN_LESS_THAN_LESS_THAN
#define YYSYMBOL_LESS_THAN_LESS_THAN_LESS_THAN						0
#endif

#ifndef YYSYMBOL_LESS_THAN_LESS_THAN_LESS_THAN_IS
#define YYSYMBOL_LESS_THAN_LESS_THAN_LESS_THAN_IS					0
#endif

#ifndef YYSYMBOL_LESS_THAN_MINUS
#define YYSYMBOL_LESS_THAN_MINUS									0
#endif

#ifndef YYSYMBOL_GREATER_THAN_IS
#define YYSYMBOL_GREATER_THAN_IS									0
#endif

#ifndef YYSYMBOL_GREATER_THAN_LESS_THAN
#define YYSYMBOL_GREATER_THAN_LESS_THAN								0
#endif

#ifndef YYSYMBOL_GREATER_THAN_GREATER_THAN
#define YYSYMBOL_GREATER_THAN_GREATER_THAN							0
#endif

#ifndef YYSYMBOL_GREATER_THAN_GREATER_THAN_IS
#define YYSYMBOL_GREATER_THAN_GREATER_THAN_IS						0
#endif

#ifndef YYSYMBOL_GREATER_THAN_GREATER_THAN_GREATER_THAN
#define YYSYMBOL_GREATER_THAN_GREATER_THAN_GREATER_THAN				0
#endif

#ifndef YYSYMBOL_GREATER_THAN_GREATER_THAN_GREATER_THAN_IS
#define YYSYMBOL_GREATER_THAN_GREATER_THAN_GREATER_THAN_IS			0
#endif

#ifndef YYSYMBOL_EXCLAMATION_MARK_IS
#define YYSYMBOL_EXCLAMATION_MARK_IS								0
#endif

#ifndef YYSYMBOL_TILDE_IS
#define YYSYMBOL_TILDE_IS											0
#endif

#ifndef YYSYMBOL_PERCENT_IS
#define YYSYMBOL_PERCENT_IS											0
#endif

#ifndef YYSYMBOL_AMPERSAND_IS
#define YYSYMBOL_AMPERSAND_IS										0
#endif

#ifndef YYSYMBOL_AMPERSAND_AMPERSAND
#define YYSYMBOL_AMPERSAND_AMPERSAND								0
#endif

#ifndef YYSYMBOL_AMPERSAND_AMPERSAND_IS
#define YYSYMBOL_AMPERSAND_AMPERSAND_IS								0
#endif

#ifndef YYSYMBOL_PIPE_IS
#define YYSYMBOL_PIPE_IS											0
#endif

#ifndef YYSYMBOL_PIPE_PIPE
#define YYSYMBOL_PIPE_PIPE											0
#endif

#ifndef YYSYMBOL_PIPE_PIPE_IS
#define YYSYMBOL_PIPE_PIPE_IS										0
#endif

#ifndef YYSYMBOL_COLON_IS
#define YYSYMBOL_COLON_IS											0
#endif

#ifndef YYSYMBOL_COLON_COLON
#define YYSYMBOL_COLON_COLON										0
#endif

#ifndef YYSYMBOL_COLON_COLON_IS
#define YYSYMBOL_COLON_COLON_IS										0
#endif

#ifndef YYSYMBOL_CIRCONFLEX_IS
#define YYSYMBOL_CIRCONFLEX_IS										0
#endif

#ifndef YYSYMBOL_IS_IS
#define YYSYMBOL_IS_IS												0
#endif

#ifndef YYSYMBOL_IS_IS_IS
#define YYSYMBOL_IS_IS_IS											0
#endif

#ifndef YYSYMBOL_PERIOD_IS
#define YYSYMBOL_PERIOD_IS											0
#endif

#ifndef YYSYMBOL_PERIOD_PERIOD
#define YYSYMBOL_PERIOD_PERIOD										0
#endif

#ifndef YYSYMBOL_PERIOD_PERIOD_PERIOD
#define YYSYMBOL_PERIOD_PERIOD_PERIOD								0
#endif

#ifndef YYSYMBOL_BACKQUOTE_BACKQUOTE_BACKQUOTE
#define YYSYMBOL_BACKQUOTE_BACKQUOTE_BACKQUOTE						0
#endif


#ifndef NUMBER
#define NUMBER					0
#endif

#ifndef ID
#define ID						0
#endif

#ifndef WORD
#define WORD					0
#endif


static int yylex(void)
{
    short int ch;
    static short int previous_ch[8] = {0};
	static short int previous_ch_pos = 0;
#define POP_CH(ch)										\
			do											\
			{											\
				if (previous_ch[previous_ch_pos])		\
				{										\
					ch = previous_ch[previous_ch_pos];	\
					previous_ch[previous_ch_pos] = 0;	\
					previous_ch_pos++;					\
					previous_ch_pos &= 8 - 1;			\
				}										\
				else									\
				{										\
					ch = getchar();						\
				}										\
			} while(0)
#define LA_CH(ch, n)									\
			do											\
			{											\
				int ch_q_pos = previous_ch_pos + n;		\
				ch_q_pos &= 8 - 1;						\
				if (previous_ch[ch_q_pos])				\
				{										\
					ch = previous_ch[ch_q_pos];			\
				}										\
				else									\
				{										\
					ch = previous_ch[ch_q_pos]			\
					   = getchar();						\
				}										\
			} while(0)
#define PUSH_CH(ch)										\
			do											\
			{											\
				int ch_q_pos = previous_ch_pos;			\
				while (previous_ch[ch_q_pos])			\
				{										\
					ch_q_pos++;							\
					ch_q_pos &= 8 - 1;					\
				}										\
				previous_ch[ch_q_pos] = (ch);			\
			} while(0)
#define PUSHBACK_CH(ch)									\
			do											\
			{											\
				int ch_q_pos = previous_ch_pos;			\
				while (previous_ch[ch_q_pos])			\
				{										\
					ch_q_pos += 8 - 1;					\
					ch_q_pos &= 8 - 1;					\
				}										\
				previous_ch[ch_q_pos] = (ch);			\
			} while(0)
	YYSTYPE val = 0;
	YYSTYPE val_power = 0;
	int val_sign = +1;
	struct mstring *strbuf = NULL;
    enum yylex_state
    {
        LEX_NEW_TOKEN,
        LEX_NUMERIC_VALUE,
        LEX_WORD,
        LEX_VARIABLE_ID,
		LEX_OPERATOR
    } state = LEX_NEW_TOKEN;

	yylval = 0;
    for (;;)
    {
		POP_CH(ch);
        switch (ch)
        {
        case 0:
        case EOF:
        case '\x04': /* Control-D */ 
            if (state != LEX_NEW_TOKEN)
			{
				PUSHBACK_CH(ch);
                break;
			}
			PUSH_CH(EOF);
            return 0;

        case ' ':
        case '\t':
        case '\r':
        case '\n':
            if (state != LEX_NEW_TOKEN)
			{
				PUSHBACK_CH(ch);
                break;
			}
            continue;

        case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
            if (state == LEX_NEW_TOKEN && NUMBER)
			{
                state = LEX_NUMERIC_VALUE;
			}
            if (state == LEX_NUMERIC_VALUE)
            {
                val = val * 10 + (ch - '0');
				val_power *= 10;
                continue;
            }
            else
            {
				PUSHBACK_CH(ch);
            }
            break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
        case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
        case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            if (state == LEX_NEW_TOKEN && (ID || WORD))
                state = (ID ? LEX_VARIABLE_ID : LEX_WORD);
            if (state == LEX_VARIABLE_ID || state == LEX_WORD)
            {
				if (!strbuf)
					strbuf = msnew();
				mputchar(strbuf, ch);
                continue;
            }
			else
			{
				PUSHBACK_CH(ch);
			}
            break;
			
		case '_':
			if (state == LEX_NEW_TOKEN && YYSYMBOL_UNDERSCORE)
				return YYSYMBOL_UNDERSCORE;
            if ((state == LEX_NEW_TOKEN || state == LEX_WORD) && ID)
                state = LEX_VARIABLE_ID;
            if (state == LEX_VARIABLE_ID)
            {
				if (!strbuf)
					strbuf = msnew();
				mputchar(strbuf, ch);
                continue;
            }
			else
			{
				PUSHBACK_CH(ch);
			}
			break;	

#define SUPPORT_SYMBOL_1(s1)					-1, YYSYMBOL_##s1, s1
#define SUPPORT_SYMBOL_2(s1, s2)				-2, YYSYMBOL_##s1##_##s2, s1, s2
#define SUPPORT_SYMBOL_3(s1, s2, s3)			-3, YYSYMBOL_##s1##_##s2##_##s3, s1, s2, s3
#define SUPPORT_SYMBOL_4(s1, s2, s3, s4)		-4, YYSYMBOL_##s1##_##s2##_##s3##_##s4, s1, s2, s3, s4
			
		case '.':
            if (state == LEX_NUMERIC_VALUE && !val_power)
            {
				// decimal point in a floating point value...
				val_power = 10;
				continue;

		case '-':
			if (state == LEX_NEW_TOKEN && NUMBER)
			{
				// see if this one is followed by a digit:
				POP_CH(ch);
				PUSHBACK_CH(ch);
				if (isdigit(ch))
				{
					state = LEX_NUMERIC_VALUE;
					val_sign = -1;
					continue;

		case '+':
			if (state == LEX_NEW_TOKEN && NUMBER)
			{
				// see if this one is followed by a digit:
				POP_CH(ch);
				PUSHBACK_CH(ch);
				if (isdigit(ch))
				{
					state = LEX_NUMERIC_VALUE;
					val_sign = +1;
					continue;
				}
			}
					}
				}
			}
			/* fall through */
        default:
			if (state != LEX_NEW_TOKEN)
			{
				PUSHBACK_CH(ch);
				break;
			}
			else
			{
				static const short int deflist[] = 
				{
					// +  +=  (NOT ++ as predecrement is LR(2) without a 'lexer hack')
					SUPPORT_SYMBOL_1(PLUS),
					SUPPORT_SYMBOL_2(PLUS, IS),
					// -  -=  ->
					SUPPORT_SYMBOL_1(MINUS),
					SUPPORT_SYMBOL_2(MINUS, IS),
					SUPPORT_SYMBOL_2(MINUS, GREATER_THAN),
					// *  *=  ** (would be ambiguous in LR(1) when * can be pointer deref as in C)  
					SUPPORT_SYMBOL_1(STAR),
					SUPPORT_SYMBOL_2(STAR, IS),
					SUPPORT_SYMBOL_2(STAR, STAR),
					// /  /=
					SUPPORT_SYMBOL_1(SLASH),
					SUPPORT_SYMBOL_2(SLASH, IS),
					// <  <=  <>  <<  <<=  <<<  <<<=  <-
					SUPPORT_SYMBOL_1(LESS_THAN),
					SUPPORT_SYMBOL_2(LESS_THAN, IS),
					SUPPORT_SYMBOL_2(LESS_THAN, GREATER_THAN),
					SUPPORT_SYMBOL_2(LESS_THAN, LESS_THAN),
					SUPPORT_SYMBOL_3(LESS_THAN, LESS_THAN, IS),
					SUPPORT_SYMBOL_3(LESS_THAN, LESS_THAN, LESS_THAN),
					SUPPORT_SYMBOL_4(LESS_THAN, LESS_THAN, LESS_THAN, IS),
					SUPPORT_SYMBOL_2(LESS_THAN, MINUS),
					// >  >=  ><  >>  >>=  >>>  >>>=  
					SUPPORT_SYMBOL_1(GREATER_THAN),
					SUPPORT_SYMBOL_2(GREATER_THAN, IS),
					SUPPORT_SYMBOL_2(GREATER_THAN, LESS_THAN),
					SUPPORT_SYMBOL_2(GREATER_THAN, GREATER_THAN),
					SUPPORT_SYMBOL_3(GREATER_THAN, GREATER_THAN, IS),
					SUPPORT_SYMBOL_3(GREATER_THAN, GREATER_THAN, GREATER_THAN),
					SUPPORT_SYMBOL_4(GREATER_THAN, GREATER_THAN, GREATER_THAN, IS),
					// !  !=
					SUPPORT_SYMBOL_1(EXCLAMATION_MARK),
					SUPPORT_SYMBOL_2(EXCLAMATION_MARK, IS),
					// ~  ~=
					SUPPORT_SYMBOL_1(TILDE),
					SUPPORT_SYMBOL_2(TILDE, IS),
					// %  %=
					SUPPORT_SYMBOL_1(PERCENT),
					SUPPORT_SYMBOL_2(PERCENT, IS),
					// &  &=  &&  &&=
					SUPPORT_SYMBOL_1(AMPERSAND),
					SUPPORT_SYMBOL_2(AMPERSAND, IS),
					SUPPORT_SYMBOL_2(AMPERSAND, AMPERSAND),
					SUPPORT_SYMBOL_3(AMPERSAND, AMPERSAND, IS),
					// |  |=  ||  ||=
					SUPPORT_SYMBOL_1(PIPE),
					SUPPORT_SYMBOL_2(PIPE, IS),
					SUPPORT_SYMBOL_2(PIPE, PIPE),
					SUPPORT_SYMBOL_3(PIPE, PIPE, IS),
					// :  :=  ::  ::=
					SUPPORT_SYMBOL_1(COLON),
					SUPPORT_SYMBOL_2(COLON, IS),
					SUPPORT_SYMBOL_2(COLON, COLON),
					SUPPORT_SYMBOL_3(COLON, COLON, IS),
					// ^  ^= 
					SUPPORT_SYMBOL_1(CIRCONFLEX),
					SUPPORT_SYMBOL_2(CIRCONFLEX, IS),
					// =  ==  ===
					SUPPORT_SYMBOL_1(IS),
					SUPPORT_SYMBOL_2(IS, IS),
					SUPPORT_SYMBOL_3(IS, IS, IS),
					// .  .=  ..  ...
					SUPPORT_SYMBOL_1(PERIOD),
					SUPPORT_SYMBOL_2(PERIOD, IS),
					SUPPORT_SYMBOL_2(PERIOD, PERIOD),
					SUPPORT_SYMBOL_3(PERIOD, PERIOD, PERIOD),
					// `  ```
					SUPPORT_SYMBOL_1(BACKQUOTE),
					SUPPORT_SYMBOL_3(BACKQUOTE, BACKQUOTE, BACKQUOTE),
					// '@'
					SUPPORT_SYMBOL_1(AT),
					// '#'
					SUPPORT_SYMBOL_1(HASHTAG),
					// '$'
					SUPPORT_SYMBOL_1(DOLLAR),
					// '('
					SUPPORT_SYMBOL_1(OPEN_BRACE),
					// ')'
					SUPPORT_SYMBOL_1(CLOSE_BRACE),
					// '['
					SUPPORT_SYMBOL_1(OPEN_SQUARE_BRACKET),
					// ']'
					SUPPORT_SYMBOL_1(CLOSE_SQUARE_BRACKET),
					// '{'
					SUPPORT_SYMBOL_1(OPEN_CURLY_BRACKET),
					// '}'
					SUPPORT_SYMBOL_1(CLOSE_CURLY_BRACKET),
					// ';'
					SUPPORT_SYMBOL_1(SEMICOLON),
					// '\''
					SUPPORT_SYMBOL_1(QUOTE),
					// '"'
					SUPPORT_SYMBOL_1(DOUBLEQUOTE),
					// ','
					SUPPORT_SYMBOL_1(COMMA),
					// '?'
					SUPPORT_SYMBOL_1(QUESTION_MARK),
					// '\\'
					SUPPORT_SYMBOL_1(BACKSLASH),
					// sentinel:
					0
				};
				
				// compile the above support table into a lookup table:
				static int compiled = 0;
				static short int first[256] = {0};
				static short int follow[sizeof(deflist) / sizeof(deflist[0])] = {0};
				static short int span = 0;
				static short int offset = 256;
				
				if (!compiled)
				{
					int i = 0;
					short int max_depth = 0;
					
					while (deflist[i] /* symbol length */ )
					{
						short int len = -deflist[i++];
						short int first_sym = deflist[i++];
						short int first_ch = deflist[i++];
						
						if (first_sym == 0)
						{
							i += len - 1;
							continue;
						}
						if (max_depth < len)
							max_depth = len;
						if (span <= first_ch)
							span = first_ch + 1;
						if (offset > first_ch)
							offset = first_ch;
						if (len == 1 && !first[first_ch])
						{
							first[first_ch] = -first_sym;
						}
						else if (len > 1 && first[first_ch] <= 0)
						{
							first[first_ch] = i - 3;
						}
						
						--len;
						while (len > 0)
						{
							short int c = deflist[i++];
							
							--len;
							if (span <= c)
								span = c + 1;
							if (offset > c)
								offset = c;
						}
					}
					span = span - offset;

					/* now calculate the follow sets: */
					if (max_depth > 1)
					{
						/*
 						first[] now contains the set of ACTIVE symbols: it marks all
						ACTIVE 'first characters' so we use that knowledge to rescan
						the ordered definition list to calculate the LA/follow info.
						*/
						for (i = 0; i < sizeof(first) / sizeof(first[0]); i++)
						{
							short int pos = first[i];
							short int first_pos = pos;
							
							if (pos <= 0)
								continue;
								
							/*
							We are now looking at a 'first character' which also includes
							ACTIVE 2/3/... character wide symbols. We need to determine
							the minimum required amount of LA for each of these.
							*/
							while (deflist[pos] /* symbol length */ )
							{
								short int len = -deflist[i++];
								short int first_sym = deflist[i++];
								short int first_ch = deflist[i++];
							}
						}
					}
				}
				
				// now that the above table has been compiled, we can use it to see if we have a match, and if so, with what:
				PUSHBACK_CH(ch);
				if (first[ch])
				{
					// do we need look-ahead?
					short int sympos = first[ch];
					short int la_qpos = 1;
					short int prev_ch = ch;
					
					if (sympos < 0)	
					{
						POP_CH(ch);
						return -sympos;
					}
				
					while (deflist[sympos] == prev_ch)
					{
						LA_CH(ch, la_qpos);
						if (ch < offset)
							break;
						if (ch >= offset + span)
							break;

						if (deflist[sympos + 1] != ch)
						{
							/* check the next posibility in this set: */
							sympos = follow[sympos];
							if (sympos <= 0)
								break;
							continue;
						}
						else
						{
							/* we got a hit on the LA character for this rule: see if we're there yet: SHIFT */
							sympos = sympos + 1;
							prev_ch = ch;
							++la_qpos;
							continue;
						}
					}
				}
				// ERROR when we get here!
			}
			break;
        }
    }

	/*
	When we arrive here, we're either at the end of a NUMBER, ID, WORD, OPERATOR or ERROR,
	where we'd have pushed the offending character in the latter case.
	*/
	switch (state)
	{
	case LEX_NUMERIC_VALUE:
		if (val_power)
			val /= val_power;
		val *= val_sign;
		yylval = val;
		return NUMBER;
		
	case LEX_WORD:
	case LEX_VARIABLE_ID:
		assert(strbuf);
		{
			char *str = msdone(strbuf);
			struct symbol *sym;

			assert(str);
			strbuf = NULL;
			sym = lookup_symbol(str);
			yylval = sym->index;		// this allows the grammar action code to retrieve this word via a simple number.
			free(str);
		}
		return (state == LEX_VARIABLE_ID ? ID : WORD);

	default:
		// we shouldn't really be here...
		POP_CH(ch);
		yylval = ch;
		return YYERRCODE;
	}
}

int main(void)
{
    int ch;

	create_symbol_table();
    yydebug = 1;
    do
    {
#if !defined(SHOW_USAGE)
        fprintf(stderr, "Enter a sequence\n");
#else
        SHOW_USAGE();
#endif

        yylval = 0;
        fprintf(stderr, "yyparse = %d, yylval = ", yyparse());
        YYDBPR(yylval);
        fprintf(stderr, "\n\n*** Hit [Y] for another round:\n");
        ch = getchar();
    } while (ch == 'y' || ch == 'Y');

    fprintf(stderr, "Exiting test application...\n");
	free_symbol_table();
    return 0;
}



/*
---------------------------------------------------------------------------------

support code

---------------------------------------------------------------------------------
*/



/**
parameters about string length.  START is the starting size and
START+TAIL should be a power of two 
*/
#define START   24
#define TAIL    8

static void msprintf(struct mstring *s, const char *fmt, ...)
{
	static char buf[4096];      /* a big static buffer */
	va_list     args;
	int         len;

    if (!s || !s->base) return;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    len = (int)strlen(buf);
    if (len > (int)(s->end - s->ptr))
    {
        int     cp = (int)(s->ptr - s->base), cl = (int)(s->end - s->base), nl = cl;
        while (len > (nl - cp))
            nl = nl + nl + TAIL;
        s->base = (char *)realloc(s->base, nl);
        if (s->base)
        {
            s->ptr = s->base + cp;
            s->end = s->base + nl;
        }
        else
        {
            s->ptr = s->end = 0;
			no_space();
            return;
        }
    }
    memcpy(s->ptr, buf, len);
    s->ptr += len;
}

static int mputchar(struct mstring *s, int ch)
{
    if (!s || !s->base) return ch;
    if (s->ptr == s->end)
    {
        int len = (int)(s->end - s->base);
        s->base = (char *)realloc(s->base, len + len + TAIL);
        if (s->base)
        {
            s->ptr = s->base + len;
            s->end = s->base + len + len + TAIL;
        }
        else
        {
            s->ptr = s->end = 0;
			no_space();
            return ch;
        }
    }
    *s->ptr++ = ch;
    return ch;
}

static struct mstring *msnew(void) 
{
    struct mstring *n = (struct mstring *)malloc(sizeof(struct mstring));

    if (n && (n->base = n->ptr = (char *)malloc(START)))
	{
        n->end = n->base + START;
		if (!n->base) no_space();
	}
    else
    {
        free(n);
        n = 0;
		no_space();
    }
    return n;
}

static char *msdone(struct mstring *s)
{
    char *r = 0;

    if (s)
    {
        mputc(s, 0);
        r = s->base;
        free(s);
    }
    return r;
}


static void no_space(void)
{
    if (fputs("fatal - out of heap space\n", stderr) == EOF)
    {
       perror("no_space: fputs");
       abort();
    }
    exit(2);
}


static unsigned int strhash(char const * name)
{
    register unsigned char const * s;
    register unsigned int c, k;

    assert(name && *name);
    s = (unsigned char const *)name;
    k = *s;
    while ((c = *++s))
        k = 31 * k + c;

    return k;
}


/* TABLE_SIZE is the number of entries in the symbol table. */
/* TABLE_SIZE must be a power of two.                       */

#define TABLE_SIZE 1024


static struct symbol **symbol_table = NULL;
static struct symbol *first_symbol = NULL;
static struct symbol *last_symbol = NULL;


#define SYMTAB_HASH(s)		(strhash(s) & (TABLE_SIZE - 1))


static struct symbol *lookup_symbol(char const * name)
{
    register struct symbol *bp, **bpp;
	register unsigned int count;
	register unsigned int h;

	assert(name);
	h = SYMTAB_HASH(name);

    bpp = symbol_table + h;
    bp = *bpp;

    for (count = 0; bp; count++)
    {
		assert(bp->index == h + count * TABLE_SIZE);
        if (strcmp(name, bp->name) == 0) 
			return (bp);
        bpp = &bp->link;
        bp = *bpp;
    }

	// create bucket:
    bp = (struct symbol *) calloc(1, sizeof(bp[0]));
    if (bp == 0) no_space();
    bp->link = 0;
    bp->next = 0;
    bp->name = strdup(name);
    if (bp->name == 0) no_space();
	bp->index = h + count * TABLE_SIZE;

    *bpp = bp;
	if (last_symbol)
		last_symbol->next = bp;
    last_symbol = bp;
    if (!first_symbol)
	    first_symbol = bp;

    return (bp);
}


static struct symbol *lookup_symbol_by_index(unsigned int index)
{
    register struct symbol *bp;
	register unsigned int h;

	h = index & (TABLE_SIZE - 1);

    bp = symbol_table[h];

    while (bp)
    {
        if (bp->index == index) 
			return (bp);
        bp = bp->link;
    }

    return NULL;
}


static void create_symbol_table(void)
{
    symbol_table = (struct symbol **)calloc(TABLE_SIZE, sizeof(symbol_table[0]));
    if (symbol_table == 0) no_space();

    first_symbol = NULL;
    last_symbol = NULL;
}


static void free_symbol_table(void)
{
    register struct symbol *p, *q;

    for (p = first_symbol; p; p = q)
    {
        q = p->next;
		free(p->name);
        free(p);
    }
    free(symbol_table);
    symbol_table = NULL;
    first_symbol = NULL;
    last_symbol = NULL;
}


