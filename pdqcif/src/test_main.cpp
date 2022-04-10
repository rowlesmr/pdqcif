

#if 0

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

#endif

#include <iostream>
#include <iomanip>

#include <chrono>
#include <cmath>
#include <random>
#include <algorithm>
#include <vector>

#include "plot_helper.h"
#include "cif_transform.h"
#include "cif.h"

#include <tao/pegtl/contrib/trace.hpp>


using namespace row;
namespace pegtl = tao::pegtl;

#if 0
template<typename T, typename A>
void print(const std::vector<T, A>& v, const std::string& header="") {
   std::cout << "\n-- " << header <<" --" << std::endl;
   for (size_t i{ 0 }; i < v.size(); ++i)
      std::cout << i << ":\t" << v[i] << std::endl;
}

template<typename T>
void print(const T& t) {
   std::cout << t << std::endl;
}
template<typename T, typename U>
void print(const T& t, const U& c) {
   std::cout << c << ": " << t << std::endl;
}
#endif


int main() {

   
   auto start = std::chrono::high_resolution_clock::now();
   //pegtl::file_input in("C:\\data\\ideal.cif");
   //pegtl::file_input in("C:\\data\\ian.cif");
   //pegtl::file_input in("C:\\data\\test.cif");


   auto file_open = std::chrono::high_resolution_clock::now();
   cif::Cif cif{};
   try {
      //pegtl::file_input in("C:\\data\\test.cif");
      
      //pegtl::complete_trace<cif::rules::file>(in);
      //pegtl::standard_trace< cif::rules::cif >(in);
      //pegtl::tracer< pegtl::tracer_traits< true, true, true > > tr(in);
      //tr.parse< cif::rules::file >(in);

      //parse_input(cif, in);
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

   //cif::print(cif, true);


   cif::transform::File file(cif);

 

   //print(file, cif::transform::PrintVerbosity::All);




   return 0;
}
