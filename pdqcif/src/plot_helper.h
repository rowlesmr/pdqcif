#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <assert.h>


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
   
   void diff(const std::vector<double>& o, const std::vector<double>& c, std::vector<double>* d) {
      if (o.size() != c.size()) {
         d = nullptr;
      }
      else {
         d->clear();
         d->reserve(o.size());
         for (size_t i{ 0 }; i < o.size(); ++i) {
            d->push_back(o[i] - c[i]);
         }
      }
   }
   std::vector<double> diff(const std::vector<double>& o, const std::vector<double>& c) {
      std::vector<double> d;
      diff(o, c, &d);
      return d;
   }


   //////////////////////////////////////////////////////////////////////////////////
   void scale(const double& s, const std::vector<double>& d, std::vector<double>* r) {
      r->clear();
      r->reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r->push_back(s * d[i]);
      }
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
         r->reserve(d.size());
         for (size_t i{ 0 }; i < d.size(); ++i) {
            r->push_back(s[i] * d[i]);
         }
      }
   }
   std::vector<double> scale(const std::vector<double>& s, const std::vector<double>& d) {
      std::vector<double> r;
      scale(s, d, &r);
      return r;
   }


   //////////////////////////////////////////////////////////////////////////////////
   void offset(const double& s, const std::vector<double>& d, std::vector<double>* r) {
      r->clear();
      r->reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r->push_back(d[i] + s);
      }
   }
   std::vector<double> offset(const double& s, const std::vector<double>& d) {
      std::vector<double> r;
      offset(s, r);
      return r;
   }

 

   //////////////////////////////////////////////////////////////////////////////////
   void sqrt(const std::vector<double>& d, std::vector<double>* r) {
      r->clear();
      r->reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r->push_back(std::sqrt(d[i]));
      }
   }
   std::vector<double> sqrt(const std::vector<double>& d) {
      std::vector<double> r;
      sqrt(d, &r);
      return r;
   }

   void log10(const std::vector<double>& d, std::vector<double>* r) {
      r->clear();
      r->reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r->push_back(std::log10(d[i]));
      }
   }
   std::vector<double> log10(const std::vector<double>& d) {
      std::vector<double> r;
      log10(d, &r);
      return r;
   }

}