#define _USE_MATH_DEFINES
#include <cmath>
#include <format>
#include <stdexcept>
#include "cif_transform.h"


namespace row::cif::transform {


   /***************************************************
   *
   *  DiffDataAxis - contains a single set of x, yobs, ycalc, or ybkg data
   *
   ****************************************************/
   DiffDataAxis::DiffDataAxis(const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s)
      : has_weights{ false } {
      if (values_s.size() != weights_s.size()) {
         throw std::runtime_error("Length of values_s and weights_s must be the same.");
      }

      double val{ 0 };
      double wei{ 0 };

      for (size_t i{ 0 }; i < values_s.size(); ++i) {
         str_to_v(values_s[i], val);
         str_to_v(weights_s[i], wei);

         values.push_back(val);
         if (wei > 0.0) {
            weights.push_back(wei);
            has_weights = true;
         }
         else {
            weights.push_back(0.0);
         }
      }
   }

   DiffDataAxis::DiffDataAxis(const std::vector<std::string>& values_s, const bool weights_from_values)
      : has_weights{ false } {

      double val{ 0 };
      double err{ 0 };

      for (const std::string& s : values_s) {
         str_to_ve(s, val, err);

         values.push_back(val);
         if (err > 0.0 ) {
            weights.push_back(1 / (err * err));
            has_weights = true;
         }
         else {
            weights.push_back(0.0);
         }
      }

      if (!has_weights && weights_from_values) {
         std::transform(weights.begin(), weights.end(), weights.begin(), [](const auto& d) {  if (d > 0) { return 1 / d; } else { return 0; } });
      }
   }

   DiffDataAxis::DiffDataAxis(const std::string& th2_min, const std::string& th2_inc, const std::string& th2_max)
      : has_weights{ false } {
      double start{ 0.0 };
      double step{ 0.0 };
      double stop{ 0.0 };

      str_to_v(th2_min, start);
      str_to_v(th2_inc, step);
      str_to_v(th2_min, stop);

      if (start > stop) {
         throw std::runtime_error("The start angle must be smaller than the stop angle.");
      }

      size_t points = static_cast<size_t>(((stop - start) / step) + 1.000001);
      values.resize(points);

      double i{ 0.0 };
      std::generate(values.begin(), values.end(), [&i, &start, &step]() { return start + step * i++; });
   }

   size_t DiffDataAxis::size() {
      return values.size();
   }

   /***************************************************
   *
   *  DiffData - contains  all the x, yobs, ycalc, and/or ybkg data
   *
   ****************************************************/
   void DiffData::add_axis(const std::string& tag, const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s) {
      axes.emplace(tag, ( values_s, weights_s));
      tags.push_back(tag);
   }
   void DiffData::add_axis(const std::string& tag, const std::vector<std::string>& values_s, const bool weights_from_values) {
      axes.emplace(tag, (values_s, weights_from_values));
      tags.push_back(tag);
   }

   DiffDataAxis& DiffData::operator[](const std::string& t) {
      auto it = axes.find(t);
      if (it == axes.end()) {
         throw std::out_of_range("Tag not found.");
      }
      else {
         return it->second; //where second == value (first == key)
      }
   }

   bool DiffData::has_tag(const std::string& t) const {
      auto it = axes.find(t);
      return it != axes.end();
   }

   /***************************************************
   *
   *  PhaseDataSingle - information about a single phase
   *
   ****************************************************/
  
   void PhaseDataSingle::add_refl_d(const std::vector<std::string>& refl_d_s){
      refl_d.reserve(refl_d_s.size());
      refl_q.reserve(refl_d_s.size());

      double d{ 0 };
      for (size_t i{ 0 }; i < refl_d_s.size(); ++i) {
         str_to_v(refl_d_s[i], d);
         refl_d.push_back(d);
         refl_q.push_back(2 * M_PI / d);
      }
   }

   void PhaseDataSingle::add_refl_hover(const std::vector<std::string>& refl_h_s, const std::vector<std::string>& refl_k_s, const std::vector<std::string>& refl_l_s) {   
      for (size_t i{ 0 }; i < refl_h_s.size(); ++i) {
         refl_hover.push_back(refl_h_s[i] + " " + refl_k_s[i] + " " + refl_l_s[i]); // +" d = " + std::format("{:.5f}", refl_d[i]) + " A");
      }
   }
   void PhaseDataSingle::make_refl_th2(const double& wavelength) {
      std::transform(refl_d.begin(), refl_d.end(), refl_th2.begin(), [&wavelength](const auto& d) {  return 2 * std::asin(wavelength / (2 * d)) * 180 / M_PI; });
   }
   void PhaseDataSingle::add_weight_percent(const std::string& weight_percent_s) {
      this->weight_percent_s = weight_percent_s;
      str_to_v(weight_percent_s, weight_percent);
   }






   /***************************************************
   *
   *  PhaseData - information about all of the phases
   *
   ****************************************************/

   void PhaseData::add_phase_refl_d(const std::string& phase_name_s, const std::vector<std::string>& refl_d_s) {
      phases[phase_name_s].add_refl_d(refl_d_s);
   }
   void PhaseData::add_phase_refl_hover(const std::string& phase_name_s, const std::vector<std::string>& refl_h_s, const std::vector<std::string>& refl_k_s, const std::vector<std::string>& refl_l_s) {
      phases[phase_name_s].add_refl_hover(refl_h_s, refl_k_s, refl_l_s);
   }
   void PhaseData::make_phase_refl_th2(const std::string& phase_name_s, const double& wavelength) {
      phases[phase_name_s].make_refl_th2(wavelength);
   }
   void PhaseData::add_phase_weight_percent(const std::string& phase_name_s, const std::string& weight_percent_s) {
      phases[phase_name_s].add_weight_percent(weight_percent_s);
   }

   void PhaseData::add_phase(const std::string& phase_name_s) {
      phases[phase_name_s]; //initialises an empty PhaseDataSingle
      phase_names.emplace_back(phase_name_s);
   }
    
   PhaseDataSingle& PhaseData::operator[](const std::string& p) {
      auto it = phases.find(p);
      if (it == phases.end()) {
         throw std::out_of_range("Phase not found.");
      }
      else {
         return it->second;
      }
   }

   bool PhaseData::has_phase(const std::string& p) const {
      auto it = phases.find(p);
      return it != phases.end();
   }

 
   /***************************************************
    *
    *  NiceData - a struct holding a couple of maps of data I've said are nice to have
    *
    ****************************************************/
   
   void NiceData::add_data(const std::string& t, const std::string& v) {
      strings[t] = v;
      double d;
      str_to_v(v, d);
      values[t] = d;
   }
   
   double NiceData::operator[](const std::string& t) const {
      auto it = values.find(t);
      if (it == values.end()) {
         throw std::out_of_range("Tag not found.");
      }
      else {
         return it->second;
      }
   }

   std::string NiceData::get_string(const std::string& t) const {
      auto it = strings.find(t);
      if (it == strings.end()) {
         throw std::out_of_range("Tag not found.");
      }
      else {
         return it->second;
      }
   }

   bool NiceData::has_tag(const std::string& t) const {
      auto it = values.find(t);
      return it != values.end();
   }

   /***************************************************
   *
   *  Pattern - a single Block from a Cif that represents a diffraction pattern
   *
   ****************************************************/



   /***************************************************
   *
   *  File - a ciffile that has been transformed for plotting
   *
   ****************************************************/

  
   //If there isn't a block id, make it equal to the data_name
   void File::make_up_block_ids() {
      for (row::cif::Block& block : cif.blocks) {
         const std::string* block_id{};
         block.get_value("_pd_block_id", block_id);
         if (block_id == nullptr)
            block.pd_block_id = block.name;
         else
            block.pd_block_id = *block_id;
      }
   }

   //looks through all the Blocks in a given Cif and categorises them into ones that are:
   //  1 - diffraction patterns
   //  2 - crystal structures
   //  3 - other
   void File::group_block_names() {
      for (size_t i{ 0 }; i < cif.blocks.size(); ++i) {
         const row::cif::Block& block = cif.blocks[i];
         if (std::any_of(X_AXES.begin(), X_AXES.end(), [&block](const auto& t) { return block.has_tag(t); })) {
            block_indices.patterns.push_back(i);
         }
         else if (block.has_tag("_cell_length_a")) {
            block_indices.structures.push_back(i);
         }
         else {
            block_indices.others.push_back(i);
         }
      }
   }



   void File::get_nice_to_have_information() {
      for (const size_t i : block_indices.patterns) {
         //find all of the structures linked to this pattern
         const row::cif::Block& block = cif.blocks[i];
         std::vector<std::string> linked_structures{};
         std::string ls{};
         
         if (block.has_tag("_pd_phase_block_id")) {
            if (block.is_loop("_pd_phase_block_id")) {
               block.get_value("_pd_phase_block_id", &linked_structures);
            }
            else { //is pair
               block.get_value("_pd_phase_block_id", &ls);
               linked_structures.push_back(ls);
            }
         }
         //all of the linked structures, if any, are now in the vector

         //Now to get the values
         for (const std::string& t : NICE_TO_HAVE) {
            if(block.has_tag)
         }




      }
   }








}


