
#include "cif.h"
#include "cif_rules.h"

namespace cif::rules {

    //reserved words
    struct DATA : TAO_PEGTL_ISTRING("data_") {};
    struct LOOP : TAO_PEGTL_ISTRING("loop_") {};
    struct GLOBAL : TAO_PEGTL_ISTRING("global_") {};
    struct SAVE : TAO_PEGTL_ISTRING("save_") {};
    struct STOP : TAO_PEGTL_ISTRING("stop_") {};
    struct reserved : pegtl::sor< DATA, LOOP, SAVE, GLOBAL, STOP> {};

    struct EOL : pegtl::eol {};
    struct SP  : pegtl::one<' '> {};
    struct HT  : pegtl::one<'\t'> {};

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
    struct ordinarychar : pegtl::ranges< '!','!', '%','&', '(',':', '<','Z', '\\','\\', '^','^', '`','~' > {}; //not: ' " # $ _ ; [ ] SP HT
    struct nonblankchar : pegtl::range< '!', '~' > {};  //not: SP HT
    struct anyprintchar : pegtl::ranges< ' ', '~', '\t'> {}; //ALL THE CHARS!!

    //Whitespace and comments
    //WhiteSpace        : { SP | HT | EOL | TokenizedComments }+ 
    //Comments          : { '#' AnyPrintChar* EOL }+
    //TokenizedComments	 : { SP | HT | EOL }+ Comments
    struct comment    : pegtl::if_must< pegtl::one< '#' >, pegtl::until< EOL > > {};
    struct whitespace : pegtl::plus< pegtl::sor< SP, HT, EOL, comment > > {};
  
    //character strings  and text fields
    //CharString                   : UnquotedString | SingleQuotedString | DoubleQuotedString
    //unquotedstring               : eolunquotedstring | noteolunquotedstring
    //eol UnquotedString           : eol OrdinaryChar NonBlankChar*  --  this is to match a string from the beginning of a line
    //noteol UnquotedString        : noteol {OrdinaryChar | ';'} NonBlankChar*  --  this is to match a string to from the beginning of a line
    //SingleQuotedString WhiteSpace: single_quote AnyPrintChar* single_quote WhiteSpace
    //DoubleQuotedString WhiteSpace: double_quote AnyPrintChar* double_quote WhiteSpace
    //TextField                    : SemiColonTextField	
    //eol SemiColonTextField       : eol ';' { AnyPrintChar* eol { {TextLeadChar AnyPrintChar* } ? eol }* } ';'
    struct field_sep : pegtl::seq< pegtl::bol, pegtl::one<';'> > {};
    struct semicolontextfield : pegtl::if_must< field_sep, pegtl::until< field_sep > > {};
    struct textfield : pegtl::sor< semicolontextfield > {};
    template<typename Q> //what is the end of a quoted string
    struct endq : pegtl::seq<Q, pegtl::at<pegtl::sor< pegtl::one<' ', '\n', '\r', '\t', '#'>, pegtl::eof >>> {};
    template <typename Q> //the entire tail of a quoted string
    struct quoted_tail : pegtl::until<endq<Q>, anyprintchar> {};
    template<typename Q>
    struct quoted : pegtl::if_must<Q, quoted_tail<Q>> {};

    struct singlequotedstring : quoted<pegtl::one<'\''>> {};
    struct doublequotedstring : quoted<pegtl::one<'"'>> {};
    struct unquotedstring : pegtl::seq< pegtl::not_at< reserved >, pegtl::not_at< pegtl::one<'_', '$', '#' > >, pegtl::plus< nonblankchar > > {};
    struct charstring : pegtl::sor< singlequotedstring, doublequotedstring, unquotedstring> {};

    // for pdCIF, most values will be numeric. But we don't care about their specific type, we just suck up their
    //  string representation, and worry about it later.
    struct numeric : pegtl::seq < pegtl::not_at< reserved >, pegtl::plus< ordinarychar >, pegtl::at < pegtl::sor<  pegtl::one<' ', '\n', '\r', '\t', '#' >, pegtl::eof> > > {};

    //Tags and values
    //Tag   :  '_' NonBlankChar+ 
    //Value :  '.' | '?' | Numeric | TextField | CharString
    struct tag       : pegtl::seq< pegtl::one< '_' >, pegtl::plus< nonblankchar > > {};
    struct value     : pegtl::sor< pegtl::one< '.' >, pegtl::one< '?' >, numeric, textfield, charstring> {}; // If you get to the end of the pegtl:sor, then the value doesn't match and we raise an error with a custom msg.
    struct itemtag   : tag {};
    struct itemvalue : value {};
    struct looptag   : tag {};
    struct loopvalue : value {};
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
    struct loopheader : pegtl::seq< LOOP, pegtl::plus< pegtl::seq< whitespace, looptag, pegtl::discard > > > {};
    struct loopbody : pegtl::seq< loopvalue, pegtl::star< pegtl::seq< whitespace, loopvalue, pegtl::discard > > > {};
    struct dataitem : pegtl::sor< pegtl::if_must< itemtag, whitespace, pegtl::seq< pegtl::sor< itemvalue, TAO_PEGTL_RAISE_MESSAGE("Malformed or missing value.") > >, pegtl::discard >, pegtl::if_must< loopheader, whitespace, loopbody, pegtl::discard > > {};
    struct blockframecode : pegtl::plus< nonblankchar > {};
    struct saveframeheading : pegtl::seq< SAVE, blockframecode, TAO_PEGTL_RAISE_MESSAGE("Save frames are not supported by this parser.") > {};
    struct saveframe : pegtl::seq< saveframeheading, pegtl::plus< pegtl::seq< whitespace, dataitem > >, whitespace, SAVE > {};
    struct datablockheading : pegtl::seq< DATA, pegtl::seq< pegtl::sor< blockframecode, TAO_PEGTL_RAISE_MESSAGE("Invalid data block name.") > > > {};
    struct datablock : pegtl::seq< datablockheading, pegtl::star< pegtl::seq< whitespace, pegtl::sor< dataitem, saveframe > > > > {};
    struct cif : pegtl::must< pegtl::opt< comment >, 
                             pegtl::opt< whitespace >,
                             pegtl::opt< pegtl::seq< datablock, 
                                                     pegtl::star< pegtl::seq< whitespace, datablock> >, 
                                                     pegtl::opt<whitespace>
                                                   >
                                       >, 
                             pegtl::eof> {};

    
    template<typename Input>
    void print(const std::string&& s, const Input& in) {
       std::cout << s << in.string() << std::endl;
    }
    void print(const std::string&& s, const int in) {
       std::cout << s << in << std::endl;
    }
    void print(const std::string&& s) {
       std::cout << s  << std::endl;
    }

    template<typename Rule> struct Action : pegtl::nothing<Rule> {};

    template<> struct Action< blockframecode > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          print("New block: ", in);
          ::cif::Block & block = out.blocks.emplace_back(in.string());
          out.items_ = &block.items;
          out.blockcode_ = &block.name;
       }
    };

    template<> struct Action< itemtag > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          out.items_->emplace_back(in.string()); 
       }
    };

    template<> struct Action< itemvalue > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          ::cif::Item& item = out.items_->back();
          assert(std::holds_alternative<::cif::Pair>(item.data));
          ::cif::Pair& pair = std::get<::cif::Pair>(item.data);
          pair.value = in.string();
      }
    };

   template<> struct Action< LOOP > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          out.items_->emplace_back(::cif::LoopArg{}); //construct a new Item containing a Loop
       }
    };

    template<> struct Action< looptag > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          ::cif::Item& item = out.items_->back();
          assert(std::holds_alternative<::cif::Loop>(item.data));
          ::cif::Loop& loop = std::get<::cif::Loop>(item.data);
          loop.lpairs.emplace_back(in.string());
       }
    };

   template<> struct Action< loopvalue > {
      template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
         ::cif::Item& item = out.items_->back();
         assert(std::holds_alternative<::cif::Loop>(item.data));
         ::cif::Loop& loop = std::get<::cif::Loop>(item.data);
         loop.lpairs[loop.currentAppend].values.emplace_back(in.string());
         loop.currentAppend = ++loop.currentAppend % loop.lpairs.size();
         ++loop.totalValues;
      }
   };
     
    template<> struct Action< loopheader > {
       template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
          //std::cout << "Just read a loop header." << std::endl;
          //out.push_back(std::string("---\nheader\n") + in.string());
          // All the tags have been read in
          // you now know how many tags there are in the loop
          //construct the correct number of vectors to hold the right amount of values
          //print("New loop header: ", in);
       }
    };
    
   template<> struct Action< loopbody > {
      template<typename Input> static void apply(const Input& in, ::cif::Cif& out) {
         ::cif::Item& item = out.items_->back();
         assert(std::holds_alternative<::cif::Loop>(item.data));
         ::cif::Loop& loop = std::get<::cif::Loop>(item.data);
         size_t should_be_zero = loop.totalValues % loop.lpairs.size();
         if (should_be_zero != 0) {
            std::string too_many{ std::to_string(should_be_zero) };
            std::string too_few{ std::to_string(loop.lpairs.size() - should_be_zero) };
            throw pegtl::parse_error(too_few + " too few, or " + too_many + " too many values in loop." , in);
         }
      }
   };



 
}


