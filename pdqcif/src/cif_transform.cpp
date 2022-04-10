#define _USE_MATH_DEFINES
#include <cmath>
//#include <format>
#include <stdexcept>
#include "cif.h"
#include "cif_transform.h"


namespace row::cif::transform {


   bool stode(const char* s, const size_t len, double& v, double& e)
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
      if (c == '.' || c == '?') {
         if (len == 1) { 
            v = NaN; e = 0.0;   
            return false;
         }
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
  
   bool stod(const char* s, const size_t len, double& v)
   {
      v = 0.0; //value

      int sgn = 1; // what is sign of the double?
      int p = 0; // what is the effective power for the value and error terms?
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
      //get the digits after the decimal point. Also checks if the string represent a missing or unknown value
      if (c == '.' || c == '?') {
         if (len == 1) { v = NaN; return false; }
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
      ////scale the value and error
      while (p > 0) {
         v *= 10.0;
         p--;
      }
      while (p < 0) {
         v *= 0.1;
         p++;
      }
      //apply the correct sign to the value
      v *= sgn;

      return false;
   }

   bool str_to_ve(const std::string s, double& v, double& e)
   {
      return stode(s.c_str(), s.size(), v, e);
   }
  
   bool str_to_v(const std::string s, double& v)
   {
      return stod(s.c_str(), s.size(), v);
   }

   void errors_to_weights(std::vector<double> errors) {
      std::transform(errors.begin(), errors.end(), errors.begin(), [](const auto& e) {  if (e != 0.0) { return 1.0 / (e * e); } else { return 0.0; } });
   }
   
   void weights_from_counts(const std::vector<double> counts, std::vector<double>& weights) {
      weights.clear();
      std::transform(counts.begin(), counts.end(), std::back_inserter(weights), [](const auto& d) {  if (d > 0.0) { return 1.0 / d; } else { return 0.0; } });
   }



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
         if (err > 0.0) {
            weights.push_back(1 / (err * err));
            has_weights = true;
         }
         else {
            weights.push_back(0.0);
         }
      }

      if (!has_weights && weights_from_values) {
         std::transform(weights.begin(), weights.end(), weights.begin(), [](const auto& d) {  if (d > 0.0) { return 1.0 / d; } else { return 0.0; } });
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

   DiffDataAxis::DiffDataAxis(const std::vector<double>& values_d, const std::vector<double>& weights_d)
      : values{ values_d }, weights{ weights_d }, has_weights{ true } {
      if (values_d.size() != weights_d.size()) {
         throw std::runtime_error("Length of values_d and weights_d must be the same.");
      }
   }


   DiffDataAxis::DiffDataAxis(const std::vector<double>& values_d, const bool weights_from_values)
      : values{ values_d }, has_weights{ false } {
      if (weights_from_values) {
         weights.reserve(values.size());
         std::transform(values.begin(), values.end(), std::back_inserter(weights), [](const auto& d) {  if (d > 0.0) { return 1.0 / d; } else { return 0.0; } });
      }
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
      DiffDataAxis dda = DiffDataAxis(values_s, weights_s);
      axes.insert({ tag, dda });
      tags.push_back(tag);
   }
   void DiffData::add_axis(const std::string& tag, const std::vector<std::string>& values_s, const bool weights_from_values) {
      DiffDataAxis dda = DiffDataAxis(values_s, weights_from_values);
      axes.insert({ tag, dda });
      tags.push_back(tag);
   }

   void DiffData::add_axis(const std::string& tag, const std::vector<double>& values_d, const std::vector<double>& weights_d) {
      DiffDataAxis dda = DiffDataAxis(values_d, weights_d);
      axes.insert({ tag, dda });
      tags.push_back(tag);
   }
   void DiffData::add_axis(const std::string& tag, const std::vector<double>& values_d, const bool weights_from_values) {
      DiffDataAxis dda = DiffDataAxis(values_d, weights_from_values);
      axes.insert({ tag, dda});
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

   void PhaseDataSingle::add_refl_d(const std::vector<std::string>& refl_d_s) {
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

   File::File(const row::cif::Cif& in) {
      cif = in;
      transform();
   }

   void File::transform() {
      make_up_block_ids();
      expand_multiple_dataloops();
      group_block_names();
      initialise_patterns();
      get_nice_to_have_information();
      get_phase_info();
      calc_xaxis_d_q();
   }



   //If there isn't a block id, make it equal to the data_name
   void File::make_up_block_ids() {
      for (row::cif::Block& block : cif.blocks) {
         if (block.has_tag("_pd_block_id")) {
            block.pd_block_id = block.get_value("_pd_block_id")[0];
         }
      }
   }

   void File::expand_multiple_dataloops() {
      for (size_t i{ 0 }; i < cif.blocks.size(); ++i) {
         row::cif::Block& block = cif.blocks[i];
         if (!std::any_of(X_AXES.begin(), X_AXES.end(), [&block](const auto& t) { return block.has_tag(t); })) {
            continue;
         }
         //if we get here, we have at least one x-axis tag
         //how may different loops contain x-axis tags?
         std::vector<size_t> loop_indicies{ };
         for (size_t j{ 0 }; j < block.items.size(); ++j) {
            const row::cif::Item& item = block.items[j];
            if (item.is_pair())
               continue;
            //all items are now loops

            for (const std::string& ax : X_AXES) {
               if (item.has_tag(ax))
                  loop_indicies.push_back(j);
            }

            if (!(loop_indicies.size() > 1))
               continue;
            //now we have multiple loops with x-axes
            //need to copy the block loop_indicies.size()-1 times 
            for (size_t k{ 0 }; k < loop_indicies.size() - 1; ++k) {
               cif.blocks.insert(cif.blocks.begin() + i + 1, block);
            }
            //and then erase those loops and alter the dataname
            for (size_t k{ 0 }; k < loop_indicies.size(); ++k) {
               cif.blocks[i + k].name = cif.blocks[i + k].name + "_loop" + std::to_string(k);
               cif.blocks[i + k].items.erase(cif.blocks[i + k].items.begin() + loop_indicies[k]);
            }
         }
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
            pattern_block_ids.push_back(block.pd_block_id);
         }
         else if (block.has_tag("_cell_length_a")) {
            block_indices.structures.push_back(i);
         }
         else {
            block_indices.others.push_back(i);
         }
      }
   }

   void File::initialise_patterns() {
      patterns.reserve(block_indices.patterns.size());
      for (const size_t i : block_indices.patterns) {
         const row::cif::Block& block = cif.blocks[i];
         Pattern& pattern = patterns.emplace_back(block.name, block.pd_block_id);

         for (const std::string& ax : X_AXES) {
            if (block.has_tag(ax)) {
               pattern.x_axis.add_axis(ax, block.get_value(ax));
            }
         }

         {
            bool has_weights = block.has_tag("_pd_proc_ls_weight");
            for (const std::string& ax : YOBS_AXES) {
               if (block.has_tag(ax)) {
                  const std::vector<std::string>& yobs = block.get_value(ax);
                  if (has_weights) {
                     const std::vector<std::string>& weights = block.get_value("_pd_proc_ls_weight");
                     if (weights.size() == yobs.size()) {
                        pattern.y_obs.add_axis(ax, yobs, weights);
                     }
                     else {
                        pattern.y_obs.add_axis(ax, yobs, true);
                     }
                  }
                  else {
                     pattern.y_obs.add_axis(ax, yobs, true);
                  }
               }
            }
         }

         for (const std::string& ax : YCALC_AXES) {
            if (block.has_tag(ax)) {
               pattern.y_calc.add_axis(ax, block.get_value(ax));
            }
         }
         for (const std::string& ax : YBKG_AXES) {
            if (block.has_tag(ax)) {
               pattern.y_bkg.add_axis(ax, block.get_value(ax));
            }
         }

         if (block.has_tag("_pd_meas_datetime_initiated")) {
            pattern.datetime.datetime = block.get_value("_pd_meas_datetime_initiated")[0];
         }
      }
   }

   void File::get_nice_to_have_information() {
      std::vector<std::string> linked_structures_id{};
      size_t p{ 0 };
      for (const size_t i : block_indices.patterns) {
         //find all of the structures linked to this pattern
         const row::cif::Block& block = cif.blocks[i];
         Pattern& pattern = patterns[p++];
         linked_structures_id.clear();

         if (block.has_tag("_pd_phase_block_id")) {
            linked_structures_id = block.get_value("_pd_phase_block_id");
         }
         //all of the linked structures, if any, are now in the vector

         //Now to get the values
         for (const std::string& t : NICE_TO_HAVE) {
            bool found_tag = false;
            if (block.has_tag(t)) {
               pattern.nice_data.add_data(t, block.get_value(t)[0]);
               found_tag = true;
            }

            //if we get here, the tag doesn't exist in the block, and we check the linked structures
            if (!found_tag) {
               for (const std::string& link : linked_structures_id) {
                  const row::cif::Block& linked = cif.get_block_by_id(link);
                  if (linked.has_tag(t)) {
                     pattern.nice_data.add_data(t, linked.get_value(t)[0]);
                     found_tag = true;
                     break;
                  }
               }
            }

            //finally, we need to check the "other" blocks
            if (!found_tag) {
               for (const size_t j : block_indices.others) {
                  const row::cif::Block& other = cif.blocks[j];
                  if (other.has_tag(t)) {
                     pattern.nice_data.add_data(t, other.get_value(t)[0]);
                     found_tag = true;
                     break;
                  }
               }
            }
         }//end of looping over nice to have names

         //now for some specific checks and consolidation
         if (pattern.nice_data.has_tag("_cell_measurement_wavelength") && !patterns[p].nice_data.has_tag("_diffrn_radiation_wavelength")) {
            pattern.nice_data.values["_diffrn_radiation_wavelength"] = patterns[p].nice_data.values["_cell_measurement_wavelength"];
            pattern.nice_data.strings["_diffrn_radiation_wavelength"] = patterns[p].nice_data.strings["_cell_measurement_wavelength"];
         }
         if (pattern.nice_data.has_tag("_cell_measurement_temperature") && !patterns[p].nice_data.has_tag("_diffrn_ambient_temperature")) {
            pattern.nice_data.values["_diffrn_ambient_temperature"] = patterns[p].nice_data.values["_cell_measurement_temperature"];
            pattern.nice_data.strings["_diffrn_ambient_temperature"] = patterns[p].nice_data.strings["_cell_measurement_temperature"];
         }
         if (pattern.nice_data.has_tag("_cell_measurement_pressure") && !patterns[p].nice_data.has_tag("_diffrn_ambient_pressure")) {
            pattern.nice_data.values["_diffrn_ambient_pressure"] = patterns[p].nice_data.values["_cell_measurement_pressure"];
            pattern.nice_data.strings["_diffrn_ambient_pressure"] = patterns[p].nice_data.strings["_cell_measurement_pressure"];
         }
      }
   }

   void File::get_phase_info() {
      size_t p{ 0 };
      std::vector<std::string> linked_structures_id{};
      std::vector<std::string> pd_phase_ids{};
      for (size_t i{ 0 }; i < block_indices.patterns.size(); ++i) {
         const row::cif::Block& block = cif.blocks[i];
         Pattern& pattern = patterns[p++];
         linked_structures_id.clear();
         pd_phase_ids.clear();

         if (block.has_tag("_pd_phase_block_id")) { //then we are linked to other structures
            linked_structures_id = block.get_value("_pd_phase_block_id");
            if (block.has_tag("_pd_phase_id")) {
               pd_phase_ids = block.get_value("_pd_phase_id");
            }
            //let's initialise all the linked structures
            for (size_t j{ 0 }; j < linked_structures_id.size(); ++j) {
               const row::cif::Block& structure = cif.get_block_by_id(linked_structures_id[j]);
               std::string phase_name{};
               if (structure.has_tag("_pd_phase_name")) {
                  phase_name = structure.get_value("_pd_phase_name")[0];
               }
               else {
                  phase_name = std::to_string(j + 1);
               }
               pattern.phase_data.add_phase(phase_name);

               if (block.has_tag("_pd_phase_mass_%")) {
                  pattern.phase_data.add_phase_weight_percent(phase_name, block.get_value("_pd_phase_mass_%")[j]);
               }
            }

            //and now go over them and propagate the refln information
            if (!block.has_tag("_refln_d_spacing")) { //then the information should be in other places
               for (size_t j{ 0 }; j < linked_structures_id.size(); ++j) {
                  const row::cif::Block& structure = cif.get_block_by_id(linked_structures_id[j]);
                  const std::string phase_name = pattern.phase_data.phase_names[j];

                  if (structure.has_tag("_refln_d_spacing")) {
                     pattern.phase_data.add_phase_refl_d(phase_name, structure.get_value("_refln_d_spacing"));
                  }
                  if (structure.has_tag("_refln_index_h")) {
                     pattern.phase_data.add_phase_refl_hover(phase_name, structure.get_value("_refln_index_h"),
                        structure.get_value("_refln_index_k"),
                        structure.get_value("_refln_index_l"));
                  }
                  if (pattern.nice_data.has_tag("_diffrn_radiation_wavelength")) {
                     pattern.phase_data.make_phase_refl_th2(phase_name, pattern.nice_data["_diffrn_radiation_wavelength"]);
                  }
               }
            }
            else if (block.has_tag("_pd_refln_phase_id")) { //then there is a big table in the block that has all of the refln info
               //if we get there, then _pd_phase_id must also exist
               for (size_t j{ 0 }; j < pd_phase_ids.size(); ++j) {
                  std::string phase_id = pd_phase_ids[j];
                  std::string phase_name = pattern.phase_data.phase_names[j];

                  if (block.has_tag("_refln_d_spacing")) {
                     pattern.phase_data.add_phase_refl_d(phase_name, mask(phase_id, block.get_value("_pd_refln_phase_id"), block.get_value("_refln_d_spacing")));
                  }
                  if (block.has_tag("_refln_index_h")) {
                     pattern.phase_data.add_phase_refl_hover(phase_name, mask(phase_id, block.get_value("_pd_refln_phase_id"), block.get_value("_refln_index_h")),
                        mask(phase_id, block.get_value("_pd_refln_phase_id"), block.get_value("_refln_index_k")),
                        mask(phase_id, block.get_value("_pd_refln_phase_id"), block.get_value("_refln_index_l")));
                  }
                  if (pattern.nice_data.has_tag("_diffrn_radiation_wavelength")) {
                     pattern.phase_data.make_phase_refl_th2(phase_name, pattern.nice_data["_diffrn_radiation_wavelength"]);
                  }
               }
            }
         }
         else if (block.has_tag("_refln_d_spacing")) { //there are no linked structures, and only a single phases worth of reflections
            std::string phase_name{};
            if (block.has_tag("_pd_phase_name")) {
               phase_name = block.get_value("_pd_phase_name")[0];
            }
            else {
               phase_name = "1";
            }
            pattern.phase_data.add_phase(phase_name);

            if (block.has_tag("_pd_phase_mass_%")) {
               pattern.phase_data.add_phase_weight_percent(phase_name, block.get_value("_pd_phase_mass_%")[0]);
            }
            pattern.phase_data.add_phase_refl_d(phase_name, block.get_value("_refln_d_spacing"));
            if (block.has_tag("_refln_index_h")) {
               pattern.phase_data.add_phase_refl_hover(phase_name, block.get_value("_refln_index_h"),
                  block.get_value("_refln_index_k"),
                  block.get_value("_refln_index_l"));
            }
            if (pattern.nice_data.has_tag("_diffrn_radiation_wavelength")) {
               pattern.phase_data.make_phase_refl_th2(phase_name, pattern.nice_data["_diffrn_radiation_wavelength"]);
            }
         }
      }//end for
   }

   void File::calc_xaxis_d_q() {
      patterns.reserve(block_indices.patterns.size());
      for (Pattern& pattern : patterns) {
         if (!pattern.x_axis.has_tag("_pd_proc_d_spacing") &&
            pattern.x_axis.has_tag("_pd_proc_recip_len_Q")) {
            pattern.x_axis.add_axis("d", calc_dq_from_qd(pattern, "_pd_proc_recip_len_Q"));
         }
         if (!pattern.x_axis.has_tag("_pd_proc_recip_len_Q") &&
            pattern.x_axis.has_tag("_pd_proc_d_spacing")) {
            pattern.x_axis.add_axis("q", calc_dq_from_qd(pattern, "_pd_proc_d_spacing"));
         }

         if (!pattern.nice_data.has_tag("_diffrn_radiation_wavelength"))
            continue;

         double lam = pattern.nice_data["_diffrn_radiation_wavelength"];

         if (!pattern.x_axis.has_tag("_pd_proc_recip_len_Q") &&
            !pattern.x_axis.has_tag("_pd_proc_d_spacing") &&
            pattern.x_axis.has_tag("_pd_meas_2theta_scan")) {
            calc_d_q_from_th2(pattern, "_pd_meas_2theta_scan", lam);
         }
         if (!pattern.x_axis.has_tag("_pd_proc_recip_len_Q") &&
            !pattern.x_axis.has_tag("_pd_proc_d_spacing") &&
            pattern.x_axis.has_tag("_pd_proc_2theta_corrected")) {
            calc_d_q_from_th2(pattern, "_pd_proc_2theta_corrected", lam);
         }
      }
   }

   void File::calc_d_q_from_th2(Pattern& pattern, const std::string& t, double lam) {
      const std::vector<double>& th2 = pattern.x_axis[t].values;
      std::vector<double> d;
      std::vector<double> q;
      d.reserve(th2.size());
      q.reserve(th2.size());
      std::transform(th2.begin(), th2.end(), std::back_inserter(d), [lam](const auto& val) { return lam / (2.0 * std::sin(val * M_PI / 360.0)); });
      std::transform(d.begin(), d.end(), std::back_inserter(q), [](const auto& val) { return 2.0 * M_PI / val; });
      pattern.x_axis.add_axis("d", d);
      pattern.x_axis.add_axis("q", q);
   }

   std::vector<double> File::calc_dq_from_qd(Pattern& pattern, const std::string& t) {
      const std::vector<double>& qd = pattern.x_axis[t].values;
      std::vector<double> dq;
      dq.reserve(qd.size());
      std::transform(qd.begin(), qd.end(), std::back_inserter(dq), [](const auto& val) { return 2.0 * M_PI / val; });
      return dq;
   }





#if 0
   //Helper functions to print things.


   void print(DiffData& dd, PrintVerbosity pv) {
      std::cout << "* DiffData:\n";
      for (std::string& t : dd.tags) {
         std::cout << " - " + t + "\n";
         print(dd.axes[t].values, "DD axis values", pv);
         print(dd.axes[t].weights, "DD axis weights", pv);
      }
      std::cout << std::endl;
   }

   void print(PhaseData& pd, PrintVerbosity pv) {
      std::cout << "* PhaseData:\n";
      for (std::string& p : pd.phase_names) {
         std::cout << " - " + p + "\n";
         print(pd.phases[p].refl_d, "PD refl_d", pv);
         print(pd.phases[p].refl_q, "PD refl_q", pv);
         print(pd.phases[p].refl_th2, "PD refl_th2", pv);
         print(pd.phases[p].refl_hover, "PD refl_hover", pv);
         print(pd.phases[p].weight_percent, "PD weight_percent", pv);
         print(pd.phases[p].weight_percent_s, "PD weight_percent_s", pv);
      }
      std::cout << std::endl;
   }

   void print(DateTime& dt, PrintVerbosity pv) {
      std::cout << "* DateTime:\n";
      print(dt.datetime, "DT datetime", pv);
      std::cout << std::endl;
   }

   void print(NiceData& nd, PrintVerbosity pv) {
      std::cout << "* NiceData:\n";
      print(nd.values, "ND values", pv);
      print(nd.strings, "ND strings", pv);
      std::cout << std::endl;
   }

   void print(Pattern& p, PrintVerbosity pv) {
      std::cout << "* Pattern:\n";
      print(p.block_code, "P block_code", pv);
      print(p.block_id, "P block_id", pv);
      print(p.datetime, "P datetime", pv);
      print(p.phase_data, "P phase_data", pv);
      print(p.x_axis, "P x_axis", pv);
      print(p.y_obs, "P y_obs", pv);
      print(p.y_calc, "P y_calc", pv);
      print(p.y_bkg, "P y_bkg", pv);
      std::cout << std::endl;
   }

   void print(File& file, PrintVerbosity pv) {
      std::cout << "* File:\n";
      for (Pattern& p : file.patterns)
         print(p, "Pattern", pv);
      std::cout << std::endl;
   }
#endif

}