

#include "cif_convert.h"


namespace row::cif::convert {

   /***************************************************
   *
   *  Helper functions to deal with vector scaling and the like
   *
   ****************************************************/

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



   /***************************************************
   *
   *  DiffData - contains x, yobs, ycalc, or ybkg data
   *
   ****************************************************/

   size_t DiffData::find_tag(const std::string& t) const {
      for (size_t i{ 0 }; i < data.size(); ++i) {
         if (t == data[i].tag)
            return i;
      }
      return SIZE_MAX;
   }

   std::vector<double>& DiffData::operator[](const std::string& t) {
      size_t i = find_tag(t);
      return data[i].values;
   }

   bool DiffData::get_values(const std::string& t, std::vector<double>* v) {
      v = get_values(t);
      return v != nullptr;
   }

   std::vector<double>* DiffData::get_values(const std::string& t) {
      size_t i = find_tag(t);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].values;
      }
   }

   std::vector<std::string> DiffData::get_tags() {
      std::vector<std::string> r;
      r.reserve(data.size());
      for (const DiffDataAxis& ax : data) {
         r.push_back(ax.tag);
      }
      return r;
   }


   /***************************************************
   *
   *  PhaseData - information about all of the phases
   *
   ****************************************************/

   size_t PhaseData::find_phase(const std::string& p) const {
      for (size_t i{ 0 }; i < data.size(); ++i) {
         if (p == data[i].phase_name)
            return i;
      }
      return SIZE_MAX;
   }

   bool PhaseData::get_refl_d(const std::string& p, std::vector<double>* v) {
      v = get_refl_d(p);
      return v != nullptr;
   }
   bool PhaseData::get_refl_q(const std::string& p, std::vector<double>* v) {
      v = get_refl_q(p);
      return v != nullptr;
   }
   bool PhaseData::get_refl_th2(const std::string& p, std::vector<double>* v) {
      v = get_refl_th2(p);
      return v != nullptr;
   }
   bool PhaseData::get_weight_percent(const std::string& p, double* v) {
      v = get_weight_percent(p);
      return v != nullptr;
   }
   bool PhaseData::get_weight_percent_s(const std::string& p, std::string* v) {
      v = get_weight_percent_s(p);
      return v != nullptr;
   }

   std::vector<double>* PhaseData::get_refl_d(const std::string& p) {
      size_t i = find_phase(p);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].refl_d;
      }
   }
   std::vector<double>* PhaseData::get_refl_q(const std::string& p) {
      size_t i = find_phase(p);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].refl_q;
      }
   }
   std::vector<double>* PhaseData::get_refl_th2(const std::string& p) {
      size_t i = find_phase(p);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].refl_th2;
      }
   }
   double* PhaseData::get_weight_percent(const std::string& p) {
      size_t i = find_phase(p);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].weight_percent;
      }
   }
   std::string* PhaseData::get_weight_percent_s(const std::string& p) {
      size_t i = find_phase(p);
      if (i == SIZE_MAX) {
         return nullptr;
      }
      else {
         return &data[i].weight_percent_s;
      }
   }

   std::vector<std::string> PhaseData::get_phase_names() {
      std::vector<std::string> r;
      r.reserve(data.size());
      for (const auto& ax : data) {
         r.push_back(ax.phase_name);
      }
      return r;
   }

   std::vector<double> PhaseData::get_weight_percents() {
      std::vector<double> r;
      r.reserve(data.size());
      for (const auto& ax : data) {
         r.push_back(ax.weight_percent);
      }
      return r;
   }

   std::vector<std::string> PhaseData::get_weight_percents_s() {
      std::vector<std::string> r;
      r.reserve(data.size());
      for (const auto& ax : data) {
         r.push_back(ax.weight_percent_s);
      }
      return r;
   }


}


