/*
 * bnf.cpp
 *
 *  Created on: 2021年11月22日
 *      Author: nick
 */

#include <bits/stdc++.h>

using namespace std;

// copy from cpplib.h
#define TTYPE_TABLE							\
  OP(EQ,		"=")						\
  OP(NOT,		"!")						\
  OP(GREATER,		">")	/* compare */				\
  OP(LESS,		"<")						\
  OP(PLUS,		"+")	/* math */				\
  OP(MINUS,		"-")						\
  OP(MULT,		"*")						\
  OP(DIV,		"/")						\
  OP(MOD,		"%")						\
  OP(AND,		"&")	/* bit ops */				\
  OP(OR,		"|")						\
  OP(XOR,		"^")						\
  OP(RSHIFT,		">>")						\
  OP(LSHIFT,		"<<")						\
  OP(COMPL,		"~")						\
  OP(AND_AND,		"&&")	/* logical */				\
  OP(OR_OR,		"||")						\
  OP(QUERY,		"?")						\
  OP(COLON,		":")						\
  OP(COMMA,		",")	/* grouping */				\
  OP(OPEN_PAREN,	"(")						\
  OP(CLOSE_PAREN,	")")						\
  OP(EQ_EQ,		"==")	/* compare */				\
  OP(NOT_EQ,		"!=")						\
  OP(GREATER_EQ,	">=")						\
  OP(LESS_EQ,		"<=")						\
  OP(SPACESHIP,		"<=>")						\
  /* These two are unary + / - in preprocessor expressions.  */		\
  OP(PLUS_EQ,		"+=")	/* math */				\
  OP(MINUS_EQ,		"-=")						\
  OP(MULT_EQ,		"*=")						\
  OP(DIV_EQ,		"/=")						\
  OP(MOD_EQ,		"%=")						\
  OP(AND_EQ,		"&=")	/* bit ops */				\
  OP(OR_EQ,		"|=")						\
  OP(XOR_EQ,		"^=")						\
  OP(RSHIFT_EQ,		">>=")						\
  OP(LSHIFT_EQ,		"<<=")						\
  /* Digraphs together, beginning with CPP_FIRST_DIGRAPH.  */		\
  OP(OPEN_SQUARE,	"[")						\
  OP(CLOSE_SQUARE,	"]")						\
  OP(OPEN_BRACE,	"{")						\
  OP(CLOSE_BRACE,	"}")						\
  /* The remainder of the punctuation.	Order is not significant.  */	\
  OP(SEMICOLON,		";")	/* structure */				\
  OP(ELLIPSIS,		"...")						\
  OP(PLUS_PLUS,		"++")	/* increment */				\
  OP(MINUS_MINUS,	"--")						\
  OP(DEREF,		"->")	/* accessors */				\
  OP(DOT,		".")						\
  OP(SINGLE_QUOTE,      "'")	\
  OP(DOUBLE_QUOTE,      "\"")


#define ALIAS_OPERATOR \
  	  	OP("and",	AND_AND) \
		OP("and_eq",	AND_EQ)\
		OP("bitand",	AND)\
		OP("bitor",	OR)\
		OP("compl",	COMPL)\
		OP("not",	NOT)\
		OP("not_eq",	NOT_EQ)\
		OP("or",	OR_OR)\
		OP("or_eq",	OR_EQ)\
		OP("xor",	XOR)\
		OP("xor_eq",	XOR_EQ)

map<string,vector<vector<string>>> selectTree(const map<string, vector<vector<string>>>& rules,
		const string&startRule)
{
	set<string> ruleNames{startRule};
	map<string, vector<vector<string>>> result;
	while (!ruleNames.empty()){
		string str=*ruleNames.begin();
		ruleNames.erase(ruleNames.begin());
		if (rules.contains(str)){
			if (!result.contains(str)){
				auto rule=rules.at(str);
				result.insert(make_pair(str,rule));
				for (auto v: rule){
					for (auto s: v){
						if (rules.contains(s) && !ruleNames.contains(s)){
							ruleNames.insert(s);
						}
					}
				}
			}
		}
	}
	return result;
}

void trim(string& str)
{
	while (!str.empty() && (std::isspace(str[str.size()-1], std::locale("C"))
		|| !std::isprint(str[str.size()-1], std::locale("C")))){
		str.resize(str.size()-1);
	}
};
set<string> getNonTerminals(const string& bnfFile){
	ifstream in(bnfFile);
	string str;
	set<string> result;
	while (getline(in, str)){
		trim(str);
		if (str.size()>2 && str[0]!='\t' && str[str.size()-1]==':'){
			string s=str.substr(0, str.size()-1);
			if (!result.contains(s)){
				result.insert(s);
			}else{
				cerr<<"duplicate nonTerminal:"<<s<<endl;
			}
		}
	}
	return result;
};


map<string, vector<vector<string> > > getProduction(const string& bnfFile,
		map<string, string>&terminalTokens)
{
	auto quoteString=[](const string& str){
		stringstream ss;
		//ss<<quoted(str, str.size()==1?'\'':'"');
		ss<<quoted(str);
		return ss.str();
	};
	string str;
	string ruleName;
	vector<vector<string>> productions;
	map<string, vector<vector<string> > > result;
	set<string> non_terminals=getNonTerminals(bnfFile);
	ifstream in(bnfFile);


	auto alphaToken=[](const string& str)->string{
		string token;
#define OP(name, literals) if (str==literals) token=  #name  ;
		// convert operator to token name
		TTYPE_TABLE
#undef OP
		if (token.empty()){
			cout<<"unknown operator:["<<str<<"]"<<endl;
			assert(!token.empty());
		}else{ // debug
			//cout<<"token="<<token<<endl;
		}

		return token;
	};
	auto isTerminal=[&non_terminals](const string& str)->bool{
		return !non_terminals.contains(str);
	};
	auto getTerminalStr=[](const string& str)->string{
		if (str.size()>1 && std::isupper(str[0], std::locale("C"))){
			return str;
		}
		stringstream ss_token;
		ss_token<<quoted(str, str.size()==1?'\'':'"');
		return ss_token.str();
	};


	auto getTerminalToken=[quoteString,alphaToken,getTerminalStr,&terminalTokens]
						   (const string& str)->string{
		auto makeSureCapitalize=[](const string& str, string&token){
			std::transform(str.begin(), str.end(), back_inserter(token), [](auto c){
				if (c=='-')	return '_';
				return std::toupper(c, std::locale("C"));
			});
		};

		string token;
		if (str.size() == 1 && std::isalnum(str[0], std::locale("C"))){
			//token="'"+str+"'";
			token="LITERAL";
			// "R": 'R'
			terminalTokens.insert(make_pair(token, quoteString(str)));
			//return token; //'R'
		}else if (str=="ll"){
			token="LONG_LONG_L";
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}
		else if (str=="LL"){
			token="LONG_LONG_H";
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}
		else if (str=="u8"){
			token="UNICODE_8";
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}
		else if (str[0]=='\\'){
			token="ESCAPE";
			if (str=="\\"){
				//
			}else if (str=="\\x"){
				token+="_LX";
			}else if (str=="\\u"){
				token+="_LU";
			}else if (str=="\\U"){
				token+="_HU";
			}
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}else if(str[0]=='0'){
			token="ZERO";
			if (str=="0b"){
				token+="_LB";
			}else if (str=="0B"){
				token+="_HB";
			}else if (str=="0x"){
				token+="_LX";
			}else if (str=="0X"){
				token+="_HX";
			}
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}
		else if(std::isupper(str[0], std::locale("C"))){
			makeSureCapitalize(str, token);
			terminalTokens.insert(make_pair(token, str));
		}else if (!std::isalnum(str[0], std::locale("C"))){
			token=alphaToken(str);
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}else if (str=="import-keyword"){
			token="IMPORT";
			terminalTokens.insert(make_pair(token, quoteString("import")));
		}else if (str=="export-keyword"){
			token="EXPORT";
			terminalTokens.insert(make_pair(token, quoteString("export")));
		}else if (str=="module-keyword"){
			token="MODULE";
			terminalTokens.insert(make_pair(token, quoteString("module")));
		}else{
			makeSureCapitalize(str, token);
			terminalTokens.insert(make_pair(token, quoteString(str)));
		}
		return token;
	};
	while (std::getline(in, str)){
		trim(str);
		if (str.empty()){
			continue;
		}
		if (str[0]!='\t' && str.size()>2 && str[str.size()-1] == ':'){
			if (!productions.empty()){
				result.insert(make_pair(ruleName, productions));
				productions.clear();
			}
			ruleName=str.substr(0, str.size()-1);
			continue;
		}
		stringstream ss(str);
		string token;
		vector<string> production;
		while (ss>>token){
			trim(token);
			bool bOpt=false;
			string dup=token;
			if (token.size()>=4 && token.substr(token.size()-3)=="opt"){
				token=token.substr(0, token.size()-3);
				dup=token;
				bOpt=true;
			}
			if (isTerminal(token)){
				token=getTerminalToken(token);
				if (token.empty()){
					cout<<dup<<" is empty!"<<endl;
				}
			}
			if (bOpt){
				string tokenOpt=token+"-opt";
				if (!result.contains(tokenOpt)){
					vector<vector<string>> optRules{{"%empty"}, {token}};
					result.insert(make_pair(tokenOpt, optRules));
				}
				non_terminals.insert(tokenOpt);
				production.push_back(tokenOpt);
			}else{
				production.push_back(token);
			}
		}
		productions.push_back(production);
	}
	result.insert(make_pair(ruleName, productions));
	return result;
}


void outputBisonInput(const string& bisonFile, auto rules, const map<string, string>& terminalTokens,
		const string startRule="translation-unit")
{
	auto outputBisonTokens=[](auto&out, auto& rules, auto& terminalTokens){
		set<string> result;
		for (auto r:rules){
			for (auto p: r.second){
				for (auto s: p){
					if (s.size()>1 && std::isupper(s[0], std::locale("C"))){
						if (!rules.contains(s) && !result.contains(s)){
							const string& strAlias=terminalTokens.at(s);
							if (s != strAlias){
								out<<"%token  "<<s<<"\t"<<terminalTokens.at(s)<<endl;
							}else{
								out<<"%token  "<<s<<endl;
							}
							result.insert(s);
						}
					}
				}
			}
		}
	};
	auto outputOneRule=[](auto& out, auto r, auto rules){
		out<<r.first<<":"<<endl;
		bool bFirst=true;

		for (auto p: r.second){
			if (bFirst){
				bFirst=false;
			}else{
				out<<"\t|";
			}
			stringstream ss;
			ss<<"{$$=new Node(\""<<r.first<<"\"";
			for (size_t i=0; i<p.size(); i++){
				out<<"\t"<<p[i];
				if (rules.contains(p[i])){
					ss<<",$"<<i+1;
				}
			}
			ss<<");}";
			out<<"\t%merge <merge_function>"<<"\t"<<ss.str()<<endl;
		}
		out<<"\t;"<<endl;
	};
	auto outputRules=[outputOneRule](auto&out, auto& rules){
		for (auto r: rules){
			outputOneRule(out, r, rules);
		}
		const string strCode=R"({std::cout << *$1 << '\n';})";
		//const string strCode=R"({$$->print(std::cout);})";
		out<<"result:"<<"\n\t"<<"translation-unit"<<"\t"<<strCode<<"\n\t;"<<endl;
	};
	auto outputNodeType=[](auto&out, auto&rules, auto&terminalTokens){
		out<<"%type  <node> ";
//		for (auto s: terminalTokens){
//			if (s.first[0]!='"'){
//				out<<s.first<<"\t";
//			}
//		}
		for (auto item: rules){
			out<<item.first<<"\t";
		}
		out<<"result";
		out<<endl;
	};

	string strPrologue=R"delim(
%code requires{
#include "ast.h"
}
%code{
#include <iostream>
#include <string>
#include "driver.h"
#include "isoc.h"
using namespace std;
extern int yylineno;
Node* merge_function (yy::parser::value_type x0, yy::parser::value_type x1);
}
)delim";

	string strEpilogue=R"delim(
void yy::parser::error (const std::string& msg)
{
	std::cerr <<"Error["<<yylineno<<"]:"<< msg << endl;
}
Node* merge_function (yy::parser::value_type x0, yy::parser::value_type x1)
{
	Node*result=new Node ("***<OR>***", x0.node, x1.node);
	result->m_bMerged=true;
	return result;
}

int main(int argc, char**argv){		
	extern FILE *yyin;
	extern int yydebug;
	yydebug=1;
	if (argc!=2){
		fprintf(stderr, "usage: %s <source>\n", argv[0]);
		return -1;
	}
	yyin=fopen(argv[1], "r");
	if (yyin){	
		yy::parser parser;	
		if (parser.parse()==0){
				printf("success!\n");
			}else{
				printf("failure\n");
			}
			fclose(yyin);
		}else{
			perror(argv[1]);
		return -2;
	}
	return 0;
}
)delim";

	string strDeclaration=R"delim(
%require "3.2"
%glr-parser
		/*%no-lines */
%skeleton "glr.cc"
%header
%define parse.error detailed
%define api.token.prefix {TOK_}
%union {Node* node;};
%printer { if (yyvaluep) yyo << $$; } <node>
)delim";


//	%left COMMA
//		/*%right AND_EQ XOR_EQ OR_EQ LSHIFT_EQ RSHIFT_EQ MULT_EQ DIV_EQ MOD_EQ PLUS_EQ MINUS_EQ EQ CO_YIELD THROW*/
//	%left OR_OR
//	%left AND_AND
//	%left OR
//	%left XOR
//	%left AND
//	%left EQ_EQ NOT_EQ
//	%left LESS LESS_EQ GREATER GREATER_EQ
//	%left SPACESHIP
//	%left LSHIFT RSHIFT
//	%left PLUS MINUS
//	%left MULT DIV MOD
//	%left DOT_STAR DEREF_STAR
//		/*%right DELETE NEW CO_WAIT SIZEOF AND MULT*/
//
//	%left DOT DEREF PLUS_PLUS MINUS_MINUS
//	%left SCOPE
	string strPrecedence=R"delim(
)delim";
	ofstream out(bisonFile);
	out<<strDeclaration;
	out<<strPrologue;
	outputNodeType(out, rules, terminalTokens);
	outputBisonTokens(out, rules, terminalTokens);
	out<<strPrecedence;
	//out<<"%start "<<startRule<<endl;
	out<<"%start "<<"result"<<endl;
	out<<"%%"<<endl;

	outputRules(out, rules);
	out<<"%%"<<endl;
	out<<strEpilogue<<endl;
}

void bnf(){

	const string bnfFile="grammar.txt";
	map<string, vector<vector<string> > > rules;
	map<string, string> terminalTokens;
	rules=getProduction(bnfFile, terminalTokens);
//	cout<<"=================printout all non-terminals===================="<<endl;
//	[](auto r){
//		for (auto item: r){
//			cout<<item<<endl;
//		}
//	}(non_terminals);
	//cout<<"=================printout all non-terminals===================="<<endl;
	//cout<<"===================print rules====================="<<endl;

	const string bisonFile="isoc.y";
	outputBisonInput(bisonFile, rules, terminalTokens);

//	cout<<"=================print all terminals======================"<<endl;
//	set<string> terminals=[](auto rules){
//		set<string> result;
//		for (auto r:rules){
//			for (auto p:r.second){
//				for (auto s:p){
//					if (!rules.contains(s)){
//						result.insert(s);
//					}
//				}
//			}
//		}
//		return result;
//	}(rules);
//	auto printTerminals=[](auto s){
//		cout<<"=============terminals=================="<<endl;
//		for (auto item: s){
//			cout<<item<<endl;
//		}
//		cout<<"=============terminals=================="<<endl;
//	};
//	printTerminals(terminals);
}
int main(int argc, char**argv){
	if (argc==1){
		bnf();
	}else if (argc==2){
		const string bnfFile="grammar.txt";
		map<string, vector<vector<string> > > rules;
		map<string, string> terminalTokens;
		rules=getProduction(bnfFile, terminalTokens);
		auto subTree=selectTree(rules, argv[1]);
		cout<<"=================printout all non-terminals===================="<<endl;
		[](auto r){
			for (auto item: r){
				cout<<item.first<<endl;
			}
		}(subTree);

		const string bisonFile="isoc.y";
		outputBisonInput(bisonFile, subTree, terminalTokens, argv[1]);
	}
}
