

#ifndef CIF_RULES
#define CIF_RULES

#include "cif.h"
#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/trace.hpp"
namespace pegtl = tao::pegtl;


namespace cif::rules {

   //reserved words
   struct DATA;
   struct LOOP;
   struct GLOBAL;
   struct SAVE;
   struct STOP;
   struct reserved;

   struct EOL;
   struct SP ;
   struct HT ;

   //character sets
   //OrdinaryChar:	{ '!' | '%' | '&' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' 
   //              | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | '<' | '=' 
   //              | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' 
   //              | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' 
   //              | 'V' | 'W' | 'X' | 'Y' | 'Z' | '\' | '^' | '`' | 'a' | 'b' | 'c' | 'd' 
   //              | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' 
   //              | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' 
   //              | '}' | '~' }	
   //NonBlankChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' |           ';' | '[' | ']'
   //TextLeadChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' | SP | HT |       '[' | ']'
   //AnyPrintChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' | SP | HT | ';' | '[' | ']'
   struct ordinarychar ;
   struct nonblankchar ;
   struct anyprintchar ;

   //Whitespace and comments
   //WhiteSpace        : { SP | HT | EOL | TokenizedComments }+ 
   //Comments          : { '#' AnyPrintChar* EOL }+
   //TokenizedComments	 : { SP | HT | EOL }+ Comments
   struct comment    ;
   struct whitespace ;
  
   //character strings  and text fields
   //CharString                   : UnquotedString | SingleQuotedString | DoubleQuotedString
   //unquotedstring               : eolunquotedstring | noteolunquotedstring
   //eol UnquotedString           : eol OrdinaryChar NonBlankChar*  --  this is to match a string from the beginning of a line
   //noteol UnquotedString        : noteol {OrdinaryChar | ';'} NonBlankChar*  --  this is to match a string to from the beginning of a line
   //SingleQuotedString WhiteSpace: single_quote AnyPrintChar* single_quote WhiteSpace
   //DoubleQuotedString WhiteSpace: double_quote AnyPrintChar* double_quote WhiteSpace
   //TextField                    : SemiColonTextField	
   //eol SemiColonTextField       : eol ';' { AnyPrintChar* eol { {TextLeadChar AnyPrintChar* } ? eol }* } ';'
   struct field_sep;
   struct semicolontextfield;
   struct textfield;
   template<typename Q> //what is the end of a quoted string
   struct endq;
   template <typename Q> //the entire tail of a quoted string
   struct quoted_tail;
   template<typename Q>
   struct quoted;

   struct singlequotedstring;
   struct doublequotedstring;
   struct unquotedstring;
   struct charstring;

   // for pdCIF, most values will be numeric. But we don't care about their specific type, we just suck up their
   //  string representation, and worry about it later.
   struct numeric;

   //Tags and values
   //Tag   :  '_' NonBlankChar+ 
   //Value :  '.' | '?' | Numeric | TextField | CharString
   struct tag       ;
   struct value     ;
   struct itemtag   ;
   struct itemvalue ;
   struct looptag   ;
   struct loopvalue ;
   //TAO_PEGTL_RAISE_MESSAGE("This is a msg.")

   //CIF Strucure
   //cif               :  Comments? WhiteSpace? { DataBlock { WhiteSpace DataBlock }* WhiteSpace? }?
   //DataBlock         :  DataBlockHeading { WhiteSpace { DataItem | SaveFrame } }*
   //DataBlockHeading  :  DATA NonBlankChar+
   //SaveFrame         :  SaveFrameHeading { WhiteSpace DataItem }+ WhiteSpace SAVE
   //SaveFrameHeading  :  SAVE NonBlankChar+
   //DataItems         :	{ Tag WhiteSpace Value } | { LoopHeader WhiteSpace LoopBody }
   //LoopHeader        :  LOOP { WhiteSpace Tag }+
   //LoopBody          :  Value { WhiteSpace Value }*  , 
   struct loopheader;
   struct loopbody;
   struct dataitem;
   struct blockframecode;
   struct saveframeheading;
   struct saveframe;
   struct datablockheading;
   struct datablock;
   struct cif;

    
   template<typename Input>
   void print(const std::string&& s, const Input& in);
   void print(const std::string&& s, const int in);
   void print(const std::string&& s);

   template<typename Rule> struct Action;

}

#endif


