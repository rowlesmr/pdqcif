

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
#include "plot_helper.h"
#include <chrono>
#include <cmath>
#include <random>


using namespace row::cif::plot;

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


bool stode(const char* s, double& v, double& e)
{
   v = 0.0; //value
   e = 0.0; //the error in the value

   int sgn = 1; // what is sign of the double?
   int p = 0; // what is the effective power for the value and error terms?
   bool has_e = false; // does the string have an error term?
   int c = *s;

   //get the sign of the double
   if (c == '-') {
      sgn = -1;
      s++;
   }
   else if (c == '+') {
      s++;
   }
   //get the digits before the decimal point
   while ((c = *s++) != '\0' && std::isdigit(c)) {
      v = v * 10.0 + (c - '0');
   }
   //get the digits after the decimal point
   if (c == '.') {
      while ((c = *s++) != '\0' && std::isdigit(c)) {
         v = v * 10.0 + (c - '0');
         p--;
      }
   }
   //get the digits that belong to the exponent
   if (c == 'e' || c == 'E') {
      int sign = 1;
      int m = 0;
      c = *s++;
      if (c == '+')
         c = *s++;
      else if (c == '-') {
         c = *s++;
         sign = -1;
      }
      while (isdigit(c)) {
         m = m * 10 + (c - '0');
         c = *s++;
      }
      p += sign * m;
   }
   // get the digits that belong to the error
   if (c == '(') {
      while ((c = *s++) != '\0' && std::isdigit(c)) { //implicitly breaks out of loop on the trailing ')'
         e = e * 10.0 + (c - '0');
      }
      has_e = true;
   }
   ////scale the value and error
   while (p > 0) {
      v *= 10.0;
      e *= 10.0;
      p--;
   }
   while (p < 0) {
      v *= 0.1;
      e *= 0.1;
      p++;
   }
   //apply the correct sign to the value
   v *= sgn;

   return has_e;
}

bool my_atof(const std::string s, double& v, double& e)
{
   return my_atof(s.c_str(), v, e);
}







constexpr double pow10(int e) {
   double d{ 1 };
   while (e > 0) {
      d *= 10;
      --e;
   }
   while (e < 0) {
      d /= 10;
      ++e;
   }
   return d;
}

struct pow10s {

   const std::vector<double> pows{ 0.000001, 0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000, 1000000 };

   double operator[](int exp) {
      return pows[(exp + static_cast<int>(6))];
   }
};



int main() {
   double v, e;

   std::vector<std::string> ves{ "34.5(1)", "435.348(12)", "456(5546)", "00.321(12345)",
                               "5.123E+004(12)", "-0.05234e-3(24)", "3450e-2(5)"};

   size_t len{ 0 };
   for (const std::string& s : ves) {
      if (s.length() > len)
         len = s.length();
   }


   for (const std::string& s : ves) {
      //std::cout<< '\n' << s  << std::endl;
      my_atof(s, v, e);
      std::cout << std::setw(len-s.length()) << " " << s << " --> " << v << " +/- " << e << std::endl;
   }


   return 0;
}

#if 0
int main(int argc, char** argv)
{
   std::random_device rd_d;
   std::random_device rd_i;
   std::mt19937 e2_d(rd_d());
   std::mt19937 e2_i(rd_i());
   std::uniform_real_distribution<> dbl_dist(-10.0, 10.0);
   std::uniform_int_distribution<> int_dist(-6, 6);

   size_t num{ 1000000 };
   size_t loop{ 100 };

   pow10 pwer{};


      std::vector<double> val;
      std::vector<int> exp;
   for (size_t k{ 0 }; k < loop; ++k) {
      val.clear();
      exp.clear();
      val.reserve(num);
      exp.reserve(num);

      for (size_t i{ 0 }; i < num; ++i) {
         val.push_back(dbl_dist(e2_d));
         exp.push_back(int_dist(e2_i));
      }

      auto start = std::chrono::high_resolution_clock::now();

      double v{ 0 };
      double p{ 0 };
      double e1{ 0 };
      for (size_t i{ 0 }; i < num; ++i) {
         v = val[i];
         p = exp[i];
         e1 = v / std::pow(10, p);
      }


      //e = std::stod(err) / std::pow(10, pow);
      auto div = std::chrono::high_resolution_clock::now();

      double e2{ 0 };
      for (size_t i{ 0 }; i < num; ++i) {
         v = val[i];
         p = exp[i];
         e2 = v * std::pow(10, -p);
      }

      auto mul = std::chrono::high_resolution_clock::now();

      double e3{ 0 };
      for (size_t i{ 0 }; i < num; ++i) {
         v = val[i];
         p = exp[i];
         e3 = v / pwer[p];
      }

      auto loop = std::chrono::high_resolution_clock::now();

      double e4{ 0 };
      for (size_t i{ 0 }; i < num; ++i) {
         v = val[i];
         p = exp[i];
         e4 = v * pwer[-p];
      }
      auto stop = std::chrono::high_resolution_clock::now();

      auto div_time = std::chrono::duration_cast<std::chrono::microseconds>(div - start);
      auto mul_time = std::chrono::duration_cast<std::chrono::microseconds>(mul - div);
      auto look1_time = std::chrono::duration_cast<std::chrono::microseconds>(loop - mul);
      auto look2_time = std::chrono::duration_cast<std::chrono::microseconds>(stop - loop);

      std::cout << "Multiply " << mul_time.count() << 
              " us. Division " << div_time.count() << 
              " us. Lookup div " << look1_time.count() << 
              " us. Lookup mul " << look2_time.count() << " us.\t"<<e1 << " " << e2 << " " << e3 << e4 << std::endl;
   }
   return 0;
}
#endif

