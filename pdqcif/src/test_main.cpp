
#include <iostream>
#include <chrono>
#include "cif.h"

#include <tao/pegtl/contrib/trace.hpp>

namespace cif = row::cif;

int main(int argc, char** argv)
{
   auto start = std::chrono::high_resolution_clock::now();
   //pegtl::file_input in("C:\\data\\ideal.cif");
   //pegtl::file_input in("C:\\data\\ian.cif");
   //pegtl::file_input in("C:\\data\\test.cif");
  

   auto file_open = std::chrono::high_resolution_clock::now();
   cif::Cif cif{};
   try {
      //pegtl::complete_trace<cif::rules::file>(in);
      //pegtl::standard_trace< cif::rules::cif >(in);
     //pegtl::tracer< pegtl::tracer_traits< true, true, true > > tr(in);
      //tr.parse< rules::cif >(in);
      cif = cif::read_file("C:\\data\\test.cif");
   }
   //catch (pegtl::parse_error& e) {
   catch (std::runtime_error e) {
      std::cerr << e.what() << std::endl;
   //   const auto p = e.positions().front();
   //   //pretty-print the error msg and the line that caused it, with an indicator at the token that done it.
   //   std::cerr << e.what() << '\n'
   //      << in.line_at(p) << '\n'
   //      << std::setw(p.column) << '^' << std::endl;
      return 1; // a parse error is fatal, so return here to terminate program.
   }
   auto file_parsed = std::chrono::high_resolution_clock::now();

   auto file_read = std::chrono::duration_cast<std::chrono::microseconds>(file_open - start);
   auto file_parse = std::chrono::duration_cast<std::chrono::microseconds>(file_parsed - file_open);

   std::cout << "File reading took " << file_read.count() << " us" << std::endl;
   std::cout << "File parsing took " << file_parse.count() << " us" << std::endl;

   cif::print(cif, false);

 

   return 0;
}

