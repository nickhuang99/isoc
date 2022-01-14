/*
 * Copyright (c) 2021 Qingzhe Huang <nickhuang99@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * C lexical analyzer.
 * 
 */

%{
#include <iostream>
#include <string>
#include "driver.h"
#include "isoc.h"
using namespace std;

void yyerror(const string& str);
%}

%option	noyywrap
%option yylineno

universal-character-name			([\\][u][0-9a-fA-F]{4,4}|[\\][U][0-9a-fA-F]{8,8})
identifier							[_a-zA-Z][_a-zA-Z0-9]*

	/********************************start integer constant*******************************/

sign								[+-]										   
digit-sequence						[0-9]+
decimal-constant 					[1-9][0-9]*
octal-constant 						[0][0-7]*
hexadecimal-prefix					[0][xX]
hexadecimal-constant				{hexadecimal-prefix}[0-9a-fA-F]+								
integer-suffix						([uU][lL]?)|([uU]((ll)|(LL)))|([lL][uU]?)|(((ll)|(LL))[uU]?)
integer-constant					({decimal-constant}|{octal-constant}|{hexadecimal-constant}){integer-suffix}?
 
	/********************************start floating constant*******************************/
floating-suffix						[fFlL]

exponent-part						[eE]{sign}?{digit-sequence}
fractional-constant					({digit-sequence}[.])|({digit-sequence}*[.]{digit-sequence})
		
decimal-floating-constant			({digit-sequence}|{fractional-constant}){exponent-part}?{floating-suffix}?

binary-exponent-part				[pP]{sign}?{digit-sequence}

hexadecimal-digit-sequence			[0-9a-fA-F]+	
hexadecimal-fractional-constant		({hexadecimal-digit-sequence}*[.]{hexadecimal-digit-sequence})|([.]{hexadecimal-digit-sequence})

hexadecimal-floating-constant     	{hexadecimal-prefix}({hexadecimal-fractional-constant}|{hexadecimal-digit-sequence}){binary-exponent-part}{floating-suffix}?

floating-constant					{decimal-floating-constant}|{hexadecimal-floating-constant}	
		
	/********************************start enumeration constant*******************************/
enumeration-constant				{identifier}

	/********************************start character constant*******************************/
									 
simple-escape-sequence				[\\]['"\?\\abfnrtv]
octal-escape-sequence				[\\][0-7]{1,3}								  
hexadecimal-escape-sequence			[\\][x][0-9a-fA-F]+								  
escape-sequence						{simple-escape-sequence}|{octal-escape-sequence}|{hexadecimal-escape-sequence}|{universal-character-name}										  

c-char								[^\\'\n]|{escape-sequence}
c-char-sequence						{c-char}+								  
character-constant					[']{c-char-sequence}[']
	/********************************start constant*******************************/
constant							{integer-constant}|{floating-constant}|{character-constant}
	/********************************start string literal*******************************/

s-char								[^"\\\n]|{escape-sequence}
s-char-sequence						{s-char}+
string-literal						[L]?[\"]{s-char-sequence}?[\"]

spacetab							[ \t]*
word								[^)(,]+
parameter-list      				{word}([,]{word})*
%%


	/******************keyword and terminal used by parser********************/

"bitand"		|
"&"				{ return yy::parser::token::TOK_AND;}
"and"			|
"&&"			{ return yy::parser::token::TOK_AND_AND;}
"and_eq"		|
"&="			{ return yy::parser::token::TOK_AND_EQ;}
"auto"			{ return yy::parser::token::TOK_AUTO;}
"break"			{ return yy::parser::token::TOK_BREAK;}
"case"			{ return yy::parser::token::TOK_CASE;}

"char"			{ return yy::parser::token::TOK_CHAR;}

"}"				{ return yy::parser::token::TOK_CLOSE_BRACE;}
")"				{ return yy::parser::token::TOK_CLOSE_PAREN;}
":>"			|
"]"				{ return yy::parser::token::TOK_CLOSE_SQUARE;}
":"				{ return yy::parser::token::TOK_COLON;}
","				{ return yy::parser::token::TOK_COMMA;}
"compl"			|
"~"				{ return yy::parser::token::TOK_COMPL;}

"const"			{ return yy::parser::token::TOK_CONST;}

"continue"		{ return yy::parser::token::TOK_CONTINUE;}

"default"		{ return yy::parser::token::TOK_DEFAULT;}

"->"			{ return yy::parser::token::TOK_DEREF;}

"/"				{ return yy::parser::token::TOK_DIV;}
"/="			{ return yy::parser::token::TOK_DIV_EQ;}
"do"			{ return yy::parser::token::TOK_DO;}
"."				{ return yy::parser::token::TOK_DOT;}

"double"		{ return yy::parser::token::TOK_DOUBLE;}

"..."			{ return yy::parser::token::TOK_ELLIPSIS;}
"else"			{ return yy::parser::token::TOK_ELSE;}
"enum"			{ return yy::parser::token::TOK_ENUM;}
"="				{ return yy::parser::token::TOK_EQ;}
"=="			{ return yy::parser::token::TOK_EQ_EQ;}

"extern"		{ return yy::parser::token::TOK_EXTERN;}

"float"			{ return yy::parser::token::TOK_FLOAT;}
"for"			{ return yy::parser::token::TOK_FOR;}

"goto"			{ return yy::parser::token::TOK_GOTO;}
">"				{ return yy::parser::token::TOK_GREATER;}
">="			{ return yy::parser::token::TOK_GREATER_EQ;}
	
"if"			{ return yy::parser::token::TOK_IF;}

"inline"		{ return yy::parser::token::TOK_INLINE;}
"int"			{ return yy::parser::token::TOK_INT;}
"<"				{ return yy::parser::token::TOK_LESS;}
"<="			{ return yy::parser::token::TOK_LESS_EQ;}
"long"			{ return yy::parser::token::TOK_LONG;}
"<<"			{ return yy::parser::token::TOK_LSHIFT;}
"<<="			{ return yy::parser::token::TOK_LSHIFT_EQ;}
"-"				{ return yy::parser::token::TOK_MINUS;}
"-="			{ return yy::parser::token::TOK_MINUS_EQ;}
"--"			{ return yy::parser::token::TOK_MINUS_MINUS;}
"%"				{ return yy::parser::token::TOK_MOD;}

"%="			{ return yy::parser::token::TOK_MOD_EQ;}
"*"				{ return yy::parser::token::TOK_MULT;}
"*="			{ return yy::parser::token::TOK_MULT_EQ;}

"not"			|
"!"				{ return yy::parser::token::TOK_NOT;}
"not_eq"		|
"!="			{ return yy::parser::token::TOK_NOT_EQ;}

"{"				{ return yy::parser::token::TOK_OPEN_BRACE;}
"("				{ return yy::parser::token::TOK_OPEN_PAREN;}

"["				{ return yy::parser::token::TOK_OPEN_SQUARE;}

"bitor"			|
"|"				{ return yy::parser::token::TOK_OR;}
"or_eq"			|
"|="			{ return yy::parser::token::TOK_OR_EQ;}
"or"			|
"||"			{ return yy::parser::token::TOK_OR_OR;}

"+"				{ return yy::parser::token::TOK_PLUS;}
"+="			{ return yy::parser::token::TOK_PLUS_EQ;}
"++"			{ return yy::parser::token::TOK_PLUS_PLUS;}

"?"				{ return yy::parser::token::TOK_QUERY;}
"register"		{ return yy::parser::token::TOK_REGISTER;}
"restrict"		{ return yy::parser::token::TOK_RESTRICT;}
"return"		{ return yy::parser::token::TOK_RETURN;}
">>"			{ return yy::parser::token::TOK_RSHIFT;}

">>="			{ return yy::parser::token::TOK_RSHIFT_EQ;}

";"				{ return yy::parser::token::TOK_SEMICOLON;}
"short"			{ return yy::parser::token::TOK_SHORT;}
"signed"		{ return yy::parser::token::TOK_SIGNED;}
"sizeof"		{ return yy::parser::token::TOK_SIZEOF;}
"struct"		{ return yy::parser::token::TOK_STRUCT;}
"static"		{ return yy::parser::token::TOK_STATIC;}
"switch"		{ return yy::parser::token::TOK_SWITCH;}

"typedef"		{ return yy::parser::token::TOK_TYPEDEF;}

"union"			{ return yy::parser::token::TOK_UNION;}
"unsigned"		{ return yy::parser::token::TOK_UNSIGNED;}

"void"			{ return yy::parser::token::TOK_VOID;}
"volatile"		{ return yy::parser::token::TOK_VOLATILE;}

"while"			{ return yy::parser::token::TOK_WHILE;}
"xor"			|
"^"				{ return yy::parser::token::TOK_XOR;}
"xor_eq"		|
"^="			{ return yy::parser::token::TOK_XOR_EQ;}

	/*ignore GNU attribute */
"__attribute__"{spacetab}\({spacetab}\({spacetab}\){spacetab}\) 							{ ; }
"__attribute__"{spacetab}\({spacetab}\({spacetab}{identifier}{spacetab}\){spacetab}\) 		{ ; }
"__attribute__"{spacetab}\({spacetab}\({spacetab}{identifier}{spacetab}\({spacetab}{parameter-list}{spacetab}\){spacetab}\){spacetab}\) 	{ ; }
"__attribute__"{spacetab}\({spacetab}\({spacetab}{parameter-list}{spacetab}\){spacetab}\) 	{ ; }
"__restrict"																				{ ; }
"__asm__"{spacetab}[(][^)]+[)]																{ ; }
{constant}				{ return yy::parser::token::TOK_CONSTANT;}
{string-literal}		{ return yy::parser::token::TOK_STRING_LITERAL;}	
		
{identifier}			{ return yy::parser::token::TOK_IDENTIFIER;}
	

[[:space:][:blank:]]+          { ; /* Ignore whitespace */}



.				{ fprintf(stderr, "%d: ERROR! unexpected character `%c'\n", yylineno, yytext[0]); 
					return yy::parser::token::TOK_YYerror;}
<<EOF>>			{ return yy::parser::token::TOK_YYEOF;}
%%
void yyerror(const string& str){
	cerr<<"error:"<<str<<endl;
}

#ifdef LEXER_TEST
#include "isoc.h"
#include "driver.h"
int main(int argc, char**argv){
	using namespace yy;	
	extern FILE *yyin;
	if (argc==2){
		yyin=fopen(argv[1], "r");
		if (yyin){
			parser::token_kind_type tk;			
			parser::value_type node;
			do{				
				tk=yylex(&node);			
//				if (tk==parser::token_kind_type::TOK_IDENTIFIER){
//					cout<<"token:\t"<<yytext<<"\tIDENTIFIER";
//				}else if (tk==parser::token_kind_type::TOK_INTEGER_LITERAL){
//					cout<<"token:\t"<<yytext<<"\tINTEGER_LITERAL";
//				}else if (tk==parser::token_kind_type::TOK_STRING_LITERAL){
//					cout<<"token:\t"<<yytext<<"\tSTRING_LITERAL";
//				}else if (tk==parser::token::TOK_HEADER_NAME){
//					cout<<"token:\t"<<yytext<<"\tHEADER_NAME";					
//				}else 
				if (tk==parser::token::TOK_YYerror){
					cout<<"["<<yylineno<<"]:\t"<<yytext<<"\tYYerror"<<endl;						
				}else{
					cout<<yytext<<endl;
				}
				
										
			}while (tk != parser::token_kind_type::TOK_YYEOF);
		}
	}
}
#endif

