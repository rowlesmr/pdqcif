

#ifndef ROW_CIF_DB_
#define ROW_CIF_DB_
#define ROW_CIF_

#include <stdexcept>

#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/trace.hpp"


namespace row {
    namespace cif {

        namespace pegtl = tao::pegtl;


        namespace rules {
            //see https://www.iucr.org/resources/cif/spec/version1.1/cifsyntax#bnf for the specification

            //reserved words
            struct DATA : TAO_PEGTL_ISTRING("data_") {};
            struct LOOP : TAO_PEGTL_ISTRING("loop_") {};
            struct GLOBAL : TAO_PEGTL_ISTRING("global_") {};
            struct SAVE : TAO_PEGTL_ISTRING("save_") {};
            struct STOP : TAO_PEGTL_ISTRING("stop_") {};
            struct reserved : pegtl::sor<DATA, LOOP, SAVE, GLOBAL, STOP> {};

            // character sets
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
            struct ordinarychar : pegtl::ranges<'!', '!', '%', '&', '(', ':', '<', 'Z', '\\', '\\', '^', '^', '`', '~'> {}; //not: ' " # $ _ ; [ ] SP HT
            struct nonblankchar : pegtl::range<'!', '~'> {};  //not: SP HT
            struct textleadchar : pegtl::ranges<' ', ':', '<', '~', '\t'> {}; //all chars except ';'
            struct anyprintchar : pegtl::ranges<' ', '~', '\t'> {}; //ALL THE CHARS!!

            //Whitespace and comments
            struct comment : pegtl::if_must<pegtl::one<'#'>, pegtl::until<pegtl::eolf>> {};
            struct ws : pegtl::one<' ', '\t'> {};
            struct wschar : pegtl::space {}; 
            struct whitespace : pegtl::plus<pegtl::sor<wschar, comment>> {};
            struct ws_or_eof : pegtl::sor<whitespace, pegtl::eof> {};

            //character textfields and strings
            struct field_sep : pegtl::seq<pegtl::bol, pegtl::one<';'>> {};
            struct end_field_sep :  pegtl::seq<pegtl::star<ws>, pegtl::plus<pegtl::eol>, field_sep> {};

            struct leading_ws : pegtl::star<pegtl::not_at<pegtl::sor<nonblankchar, end_field_sep>>, pegtl::sor<ws, pegtl::eol>> {};
            struct sctf_text : pegtl::star<pegtl::not_at<end_field_sep>, pegtl::sor<anyprintchar, pegtl::eol>> {};

            struct semicolontextfield : pegtl::if_must<field_sep, leading_ws, sctf_text, end_field_sep> {};
            //struct semicolontextfield : pegtl::if_must<field_sep, pegtl::until<end_field_sep>> {};
            struct textfield : semicolontextfield {};

            template<typename Q> //what is the end of a quoted string
            struct endq : pegtl::seq<Q, pegtl::at<pegtl::sor<pegtl::one<' ', '\n', '\r', '\t', '#'>, pegtl::eof>>> {};
            
            template<typename Q>
            struct quote_text : pegtl::seq<pegtl::star<pegtl::not_at<endq<Q>>, anyprintchar>> {};
            struct unquoted_text : pegtl::plus<nonblankchar> {};

            template <typename Q> //the entire tail of a quoted string
            struct quoted_tail : pegtl::seq<quote_text<Q>, endq<Q>> {};

            template<typename Q>
            struct quoted : pegtl::if_must<Q, quoted_tail<Q>> {};

            struct singlequotedstring : quoted<pegtl::one<'\''>> {};
            struct doublequotedstring : quoted<pegtl::one<'"'>> {};
            struct unquotedstring : pegtl::seq<pegtl::not_at<reserved>, pegtl::not_at<pegtl::one<'_', '$', '#'>>, unquoted_text> {};
            struct charstring : pegtl::sor<singlequotedstring, doublequotedstring, unquotedstring> {};

            // for pdCIF, most values will be numeric. But we don't care about their specific type, we just suck up their
            //  string representation, and worry about it later.
            struct numeric : pegtl::seq<pegtl::plus<ordinarychar>, pegtl::at<wschar>> {};

            //Tags and values
            struct tag : pegtl::seq<pegtl::one<'_'>, pegtl::plus<nonblankchar>> {};
            struct value : pegtl::sor<numeric, textfield, charstring> {};
            struct itemtag : tag {};
            struct itemvalue : value {};
            struct looptag : tag {};
            struct loopvalue : value {};

            //CIF Structure
            //loop
            struct loopend : pegtl::opt<STOP, ws_or_eof> {};
            struct looptags : pegtl::plus<pegtl::seq<looptag, whitespace, pegtl::discard>> {};
            struct loopvalues : pegtl::sor<pegtl::plus<pegtl::seq<loopvalue, ws_or_eof, pegtl::discard>>, pegtl::at<pegtl::sor<reserved, pegtl::eof>>> {};
            struct loopstart : pegtl::seq<LOOP, whitespace> {};
            struct loop : pegtl::if_must<loopstart, looptags, loopvalues, loopend> {};

            // pair
            struct pair : pegtl::if_must<itemtag, whitespace, pegtl::if_then_else<itemvalue, ws_or_eof, TAO_PEGTL_RAISE_MESSAGE("Malformed or missing value.")>, pegtl::discard> {};

            //dataitem
            struct dataitem : pegtl::sor<pair, loop> {};

            
            struct blockframecode : pegtl::plus<nonblankchar> {};

            //saveframe
            struct saveframeend : SAVE {};
            struct saveframeheading : pegtl::seq<SAVE, blockframecode> {};
            struct saveframe : pegtl::if_must<saveframeheading, whitespace, pegtl::star<dataitem>, saveframeend, ws_or_eof> {};

            //datablock
            struct datablockheading : pegtl::seq<DATA, blockframecode> {};
            struct datablock : pegtl::seq<datablockheading, ws_or_eof, pegtl::star<pegtl::sor<dataitem, saveframe>>> {};

            //the actual CIF file
            struct content : pegtl::plus<pegtl::seq<datablock, pegtl::opt<whitespace>>> {};
            struct file : pegtl::seq<pegtl::opt<comment>, pegtl::opt<whitespace>, pegtl::if_must<pegtl::not_at<pegtl::eof>, content, pegtl::eof>> {};

        } //end namespace rules


        //helper function to print information during debugging.
        template<typename Input>
        void print(const char* s, const Input& in) {
            std::cout << s << "|" << in.string() << "|" << std::endl;
        }

        //to keep track of whether the quote string has been sent to file
        struct Status {
            bool is_loop{ false };
            bool is_quote{ false };
            bool is_printed{ false };

            void reset() {
                is_loop = false;
                is_quote = false;
                is_printed = false;
            }

            void get_ready_to_print() {
                is_quote = true;
                is_printed = false;
            }

            void just_printed() {
                is_printed = true;
            }

            void finished_printing() {
                is_quote = false;
                is_printed = false;
            }
            
            void loop() {
                is_loop = !is_loop;
            }

        };

        template<typename Input>
        void divert_action(const Input& in, Status& status) {
            status.get_ready_to_print();
            if (status.is_loop) {
                Action<rules::loopvalue>::apply(in, status);
            }
            else {
                Action<rules::itemvalue>::apply(in, status);
            }

        }



        template<typename Rule> struct Action : pegtl::nothing<Rule> {};

        template<> struct Action<rules::blockframecode> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                print("\n###\nblockframecode: ", in);
                status.reset();
            }
        };

        template<> struct Action<rules::saveframeheading> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                throw pegtl::parse_error("Saveframes are not yet supported by this parser. ", in);
            }
        };

        template<> struct Action<rules::itemtag> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                print("itemtag: ", in);
            }
        };

        template<> struct Action<rules::itemvalue> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                if (!status.is_quote || (status.is_quote && !status.is_printed)) [[likely]] {
                    print("itemvalue: ", in);
                    status.just_printed();
                }
                else {
                    status.finished_printing();
                }
            }
        };


        // This is the place to construct a new loop in the list
        template<> struct Action<rules::loopstart> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                status.loop();
               // print("loopstart: ", in);
            }
        };

        template<> struct Action<rules::looptag> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                print("looptag: ", in);
            }
        };

        template<> struct Action<rules::loopvalue> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                if (!status.is_quote || (status.is_quote && !status.is_printed)) [[likely]] {
                    print("loopvalue: ", in);
                    status.just_printed();
                }
                else {
                    status.finished_printing();
                }
            }
        };

        // This is called at the end of a loop.
        template<> struct Action<rules::loop> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                status.loop();
               // print("loopend: ", in);
            }
        };


        template<> struct Action<rules::quote_text<pegtl::one<'\''>>> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                divert_action(in, status);
            }
        };

        template<> struct Action<rules::quote_text<pegtl::one<'\"'>>> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                divert_action(in, status);
            }
        };

        template<> struct Action<rules::sctf_text> {
            template<typename Input> static void apply(const Input& in, Status& status) {
                divert_action(in, status);
            }
        };



      template<typename Input> void parse_input(Input&& in) {
          try {
              Status status{};
              pegtl::parse<rules::file, Action>(in, status);
          }
          catch (pegtl::parse_error& e) {
              const auto p = e.positions().front();
              //pretty-print the error msg and the line that caused it, with an indicator at the token what done it ;)
              std::cerr << e.what() << '\n'
                  << in.line_at(p) << '\n'
                  << std::setw(p.column) << '^' << std::endl;
              throw std::runtime_error("Parsing error.");
          }
      }


   } //end namespace cif
} // end namespace row


#endif