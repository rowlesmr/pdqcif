#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>


namespace row::cif::plot {
   void diff(const std::vector<double>& o, const std::vector<double>& c, std::vector<double>* r);
   std::vector<double> diff(const std::vector<double>& o, const std::vector<double>& c);

   void scale(const double& s, const std::vector<double>& o, std::vector<double>* r);
   std::vector<double> scale(const double& s, const std::vector<double>& o);
   void scale(const std::vector<double>& s, const std::vector<double>& d, std::vector<double>* r);
   std::vector<double> scale(const std::vector<double>& s, const std::vector<double>& d);

   void offset(const double& s, const std::vector<double>& o, std::vector<double>* r);
   std::vector<double> offset(const double& s, const std::vector<double>& o);

   void sqrt(const std::vector<double>& d, std::vector<double>* r);
   std::vector<double> sqrt(const std::vector<double>& d);

	void log10(const std::vector<double>& d, std::vector<double>* r);
   std::vector<double> log10(const std::vector<double>& d);
 
   /***************************************************
   *
   *  Helper functions to deal with vector scaling and the like
   *
   ****************************************************/
   
   void diff(const std::vector<double>& lhs, const std::vector<double>& rhs, std::vector<double>* r) {
      if (lhs.size() != rhs.size()) {
         r = nullptr;
      }
      else {
         r->clear();
         std::transform(lhs.begin(), lhs.end(), rhs.begin(), std::back_inserter(*r), std::minus());
      }
   }
   std::vector<double> diff(const std::vector<double>& o, const std::vector<double>& c) {
      std::vector<double> d;
      diff(o, c, &d);
      return d;
   }


   //////////////////////////////////////////////////////////////////////////////////
   void scale(const double& s, const std::vector<double>& v, std::vector<double>* r) {
      r->clear();
      std::transform(v.begin(), v.end(), std::back_inserter(*r), [&s](const auto& d) { return s * d; });
   }
   std::vector<double> scale(const double& s, const std::vector<double>& d) {
      std::vector<double> r;
      scale(s, d, &r);
      return r;
   }

   void scale(const std::vector<double>& s, const std::vector<double>& d, std::vector<double>* r) {
      if (s.size() != d.size()) {
         r = nullptr;
      }
      else {
         r->clear();
         std::transform(s.begin(), s.end(), d.begin(), std::back_inserter(*r), std::multiplies());
      }
   }
   std::vector<double> scale(const std::vector<double>& s, const std::vector<double>& d) {
      std::vector<double> r;
      scale(s, d, &r);
      return r;
   }


   //////////////////////////////////////////////////////////////////////////////////
   void offset(const double& s, const std::vector<double>& v, std::vector<double>* r) {
      r->clear();
      std::transform(v.begin(), v.end(), std::back_inserter(*r), [&s](const auto& d) { return s + d; });
   }
   std::vector<double> offset(const double& s, const std::vector<double>& d) {
      std::vector<double> r;
      offset(s, r);
      return r;
   }

 

   //////////////////////////////////////////////////////////////////////////////////
   void sqrt(const std::vector<double>& v, std::vector<double>* r) {
      r->clear();
      std::transform(v.begin(), v.end(), std::back_inserter(*r), [](const auto& d) { return std::sqrt(d); });
   }
   std::vector<double> sqrt(const std::vector<double>& d) {
      std::vector<double> r;
      sqrt(d, &r);
      return r;
   }

   void log10(const std::vector<double>& v, std::vector<double>* r) {
      r->clear();
      std::transform(v.begin(), v.end(), std::back_inserter(*r), [](const auto& d) { return std::log10(d); });
   }
   std::vector<double> log10(const std::vector<double>& d) {
      std::vector<double> r;
      log10(d, &r);
      return r;
   }

}