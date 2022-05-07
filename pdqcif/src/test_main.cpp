
#include <iostream>
#include <chrono>

//#include "plot_helper.h"
//#include "cif_transform.h"

#include "cif_db.h"

#include <tao/pegtl/contrib/trace.hpp>
#include <tao/pegtl/contrib/analyze.hpp>


namespace pegtl = tao::pegtl;


int main() {
    try {
        auto start = std::chrono::high_resolution_clock::now();

        pegtl::file_input in("c:\\data\\test.cif");

        auto file_open = std::chrono::high_resolution_clock::now();
      

        //pegtl::complete_trace<cif::rules::file>(in);
        //pegtl::standard_trace< cif::rules::file >(in);
      
        //pegtl::tracer< pegtl::tracer_traits< true, true, true > > tr(in);
        //tr.parse< cif::rules::file >(in);

        //const std::size_t issues = tao::pegtl::analyze< cif::rules::file >(1);

        row::cif::parse_input(in);
      

        auto file_parsed = std::chrono::high_resolution_clock::now();

        auto file_read = std::chrono::duration_cast<std::chrono::microseconds>(file_open - start);
        auto file_parse = std::chrono::duration_cast<std::chrono::microseconds>(file_parsed - file_open);

        std::cout << "File reading took " << file_read.count() << " us" << std::endl;
        std::cout << "File parsing took " << file_parse.count() << " us" << std::endl;

    }
    catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        return 1; // a parse error is fatal, so return here to terminate program.
    }
        //cif::print(cif, true);
        //cif::transform::File file(cif);
        //print(file, cif::transform::PrintVerbosity::Some);

   

        return 0;
    }
