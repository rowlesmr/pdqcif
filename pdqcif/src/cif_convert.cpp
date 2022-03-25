

#include "cif_convert.h"


namespace cif::convert {

   std::vector<double> diff(const std::vector<double>& o, const std::vector<double>& c) {
      assert(o.size() == c.size());
      std::vector<double> d;
      d.reserve(o.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         d.push_back(o[i] - c[i]);
      }
      return d;
   }
   void diff_i(std::vector<double>& o, const std::vector<double>& c) {
      for (size_t i{ 0 }; i < o.size(); ++i) {
         o[i] -= c[i];
      }
   }

   std::vector<double> scale(const std::vector<double>& d, const double& s) {
      std::vector<double> r;
      r.reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r.push_back(d[i] * s);
      }
      return r;
   }
   std::vector<double> scale(const double& s, const std::vector<double>& o) {
      return scale(o, s);
   }
   std::vector<double> scale(const std::vector<double>& d, const std::vector<double>& s) {
      assert(d.size() == s.size());
      std::vector<double> r;
      r.reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r.push_back(d[i] * s[i]);
      }
      return r;
   }

   void scale_i(std::vector<double>& o, const double& s) {
      for (size_t i{ 0 }; i < o.size(); ++i) {
         o[i] *= s;
      }
   }
   void scale_i(const double& s, std::vector<double>& o) {
      scale_i(o, s);
   }

   std::vector<double> offset(const std::vector<double>& d, const double& s) {
      std::vector<double> r;
      r.reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r.push_back(d[i] + s);
      }
      return r;
   }
   std::vector<double> offset(const double& s, const std::vector<double>& o) {
      return offset(o, s);
   }

   void offset_i(std::vector<double>& o, const double& s) {
      for (size_t i{ 0 }; i < o.size(); ++i) {
         o[i] += s;
      }
   }
   void offset_i(const double& s, std::vector<double>& o) {
      offset_i(o, s);
   }

   std::vector<double> sqrt(const std::vector<double>& d) {
      std::vector<double> r;
      r.reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r.push_back(std::sqrt(d[i]));
      }
      return r;
   }

   std::vector<double> log10(const std::vector<double>& d) {
      std::vector<double> r;
      r.reserve(d.size());
      for (size_t i{ 0 }; i < d.size(); ++i) {
         r.push_back(std::log10(d[i]));
      }
      return r;
   }




   size_t DiffData::find_tag(const std::string t) const {
      for (size_t i{ 0 }; i < tags.size(); ++i) {
         if (t == tags[i])
            return i;
      }
      return SIZE_MAX;
   }

   std::vector<double>& DiffData::operator[](const std::string t) {
      return values[find_tag(t)];
   }

   bool DiffData::get_values(const std::string t, std::vector<double>* v) {
      size_t i = find_tag(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &values[i];
         return true;
      }
   }

   std::vector<double>* DiffData::get_values(const std::string t) {
      size_t i = find_tag(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &values[i];
      }
   }




   size_t PhaseData::find_phase(const std::string t) const {
      for (size_t i{ 0 }; i < phase_names.size(); ++i) {
         if (t == phase_names[i])
            return i;
      }
      return SIZE_MAX;
   }

   bool PhaseData::get_refl_d(const std::string t, std::vector<double>* v) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &refl_d[i];
         return true;
      }
   }
   bool PhaseData::get_refl_q(const std::string t, std::vector<double>* v) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &refl_q[i];
         return true;
      }
   }
   bool PhaseData::get_refl_th2(const std::string t, std::vector<double>* v) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &refl_th2[i];
         return true;
      }
   }
   bool PhaseData::get_weight_percent(const std::string t, double* v) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &weight_percents[i];
         return true;
      }
   }
   bool PhaseData::get_weight_percent_s(const std::string t, std::string* v) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         v = &weight_percents_s[i];
         return true;
      }
   }

   std::vector<double>* PhaseData::get_refl_d(const std::string t) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &refl_d[i];
      }
   }
   std::vector<double>* PhaseData::get_refl_q(const std::string t) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &refl_q[i];
      }
   }
   std::vector<double>* PhaseData::get_refl_th2(const std::string t) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &refl_th2[i];
      }
   }
   double* PhaseData::get_weight_percent(const std::string t) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &weight_percents[i];
      }
   }
   std::string* PhaseData::get_weight_percent_s(const std::string t) {
      size_t i = find_phase(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &weight_percents_s[i];
      }
   }





}


