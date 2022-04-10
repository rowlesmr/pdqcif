

#ifndef ROW_CIF_
#define ROW_CIF_


#include <stdexcept>

#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/trace.hpp"

#include "cif_file.h"


namespace row {
   namespace cif {
      namespace pegtl = tao::pegtl;
      namespace rules {
         struct DATA : TAO_PEGTL_ISTRING("data_") {};

         struct ordinarychar : pegtl::ranges<'!', '!', '%', '&', '(', ':', '<', 'Z', '\\', '\\', '^', '^', '`', '~'> {}; //not: ' " # $ _ ; [ ] SP HT
         struct nonblankchar : pegtl::range<'!', '~'> {};  //not: SP HT
         struct wschar : pegtl::one<' ', '\t', '\n', '\r'> {};

         //Whitespace and comments
         struct comment : pegtl::if_must<pegtl::one<'#'>, pegtl::until<pegtl::eolf>> {};
         struct whitespace : pegtl::plus<pegtl::sor<wschar, comment>> {};
         struct ws_or_eof : pegtl::sor<whitespace, pegtl::eof> {};

         // tags and values, and data pairs
         struct tag : pegtl::seq<pegtl::one<'_'>, pegtl::plus<nonblankchar>> {};
         struct value : pegtl::seq<pegtl::plus<ordinarychar>, pegtl::at<wschar>> {};
         struct pair : pegtl::if_must<tag, whitespace, pegtl::if_then_else<value, ws_or_eof, TAO_PEGTL_RAISE_MESSAGE("Malformed or missing value.")>, pegtl::discard> {};

         //datablock
         struct datablockheading : pegtl::seq<DATA, pegtl::plus<nonblankchar>> {};
         struct datablock : pegtl::seq<datablockheading, ws_or_eof, pegtl::star<pair>> {};

         //the actual CIF file
         struct content : pegtl::plus<pegtl::seq<datablock, pegtl::opt<whitespace>>> {};
         struct file : pegtl::seq<pegtl::opt<comment>, pegtl::opt<whitespace>, pegtl::if_must<pegtl::not_at<pegtl::eof>, content, pegtl::eof>> {};

      } //end namespace rules

      //helper function to print information during debugging.
      template<typename Input>
      void print(const std::string&& s, const Input& in) {
         std::cout << s << in.string() << std::endl;
      }

      //********************
      // Parsing Actions to populate the values in the ciffile
      //********************
      template<typename Rule> struct Action : pegtl::nothing<Rule> {};

      template<> struct Action<rules::datablockheading> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            print("blockframecode: ", in);
         }
      };

      template<> struct Action<rules::tag> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            print("itemtag: ", in);
         }
      };

      template<> struct Action<rules::value> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            print("itemvalue: ", in);
         }
      };
      
   } //end namespace cif
} // end namespace row


#endif