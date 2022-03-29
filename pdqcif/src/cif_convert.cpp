

#include "cif_convert.h"


namespace row::cif::convert {



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


