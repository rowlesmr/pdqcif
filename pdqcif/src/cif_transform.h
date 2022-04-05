

#ifndef ROW_CIF_CONVERT_
#define ROW_CIF_CONVERT_

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <vector>
#include <unordered_map>
#include <string>
//#include <chrono> // really needs C++20 to do everything that I want...

#include "cif.h"

namespace row::cif::transform {

   static constexpr double NaN = std::numeric_limits<double>::quiet_NaN();

   // to make it easy to concatenate vectors. https://www.techiedelight.com/concatenate-multiple-vectors-in-cpp/
   template <typename T>
   std::vector<T> operator+(const std::vector<T>& x, const std::vector<T>& y)
   {
      std::vector<T> vec;
      vec.reserve(x.size() + y.size());
      vec.insert(vec.end(), x.begin(), x.end());
      vec.insert(vec.end(), y.begin(), y.end());
      return vec;
   }

   template <typename T>
   std::vector<T>& operator+=(std::vector<T>& x, const std::vector<T>& y)
   {
      x.reserve(x.size() + y.size());
      x.insert(x.end(), y.begin(), y.end());
      return x;
   }

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
         if (len == 1) { v = NaN; e = 0.0; }  return false;
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
         if (len == 1) v = NaN; return false;
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


   void errors_to_weights(const std::vector<double> errors) {
      std::transform(errors.begin(), errors.end(), errors.begin(), [](const auto& e) {  if (e != 0.0) { return 1 / (e * e); } else { return 0; } });
   }
   void weights_from_counts(const std::vector<double> counts, std::vector<double>& weights) {
      weights.clear();
      std::transform(counts.begin(), counts.end(), std::back_inserter(weights), [](const auto& d) {  if (d > 0) { return 1 / d; } else { return 0; } });
   }


   struct DiffDataAxis {
   
      std::vector<double> values{};
      std::vector<double> weights{};
      bool has_weights{};
         
      DiffDataAxis(const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s);
      DiffDataAxis(const std::vector<std::string>& values_s, const bool weights_from_values = false);
      DiffDataAxis(const std::string& th2_min, const std::string& th2_inc, const std::string& th2_max);

      size_t size();
   };

   struct DiffData {
      std::vector<std::string> tags{};
      std::unordered_map<std::string, DiffDataAxis> axes{};

      void add_axis(const std::string& tag, const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s);
      void add_axis(const std::string& tag, const std::vector<std::string>& values_s, const bool weights_from_values = false);

      bool has_tag(const std::string& t) const;
      DiffDataAxis& operator[](const std::string& t) ;

   };

   struct PhaseDataSingle {
      std::vector<double> refl_d{};				// _refln_d_spacing
      std::vector<double> refl_q{};				// = 2 * M_PI / d 
      std::vector<double> refl_th2{};			// = 2 * asin(lam / (2 * d)) * 180 / M_PI 
      std::vector<std::string> refl_hover{};	// "h + ' ' + k + ' ' + k + ' ' + "d = " + _refln_d_spacing"
      double weight_percent{ NaN };				// _pd_phase_mass_%
      std::string weight_percent_s{};			// _pd_phase_mass_%

      void add_refl_d(const std::vector<std::string>& refl_d_s);
      void add_refl_hover(const std::vector<std::string>& refl_h_s, const std::vector<std::string>& refl_k_s, const std::vector<std::string>& refl_l_s);
      void make_refl_th2(const double& wavelength);
      void add_weight_percent(const std::string& weight_percent_s);
   };

   struct PhaseData {
      std::vector<std::string> phase_names{};
      std::unordered_map<std::string, PhaseDataSingle> phases{};
      std::vector<double> weight_percents{};

      void add_phase(const std::string& phase_name_s);

      void add_phase_refl_d(const std::string& phase_name_s, const std::vector<std::string>& refl_d_s);
      void add_phase_refl_hover(const std::string& phase_name_s, const std::vector<std::string>& refl_h_s, const std::vector<std::string>& refl_k_s, const std::vector<std::string>& refl_l_s);
      void make_phase_refl_th2(const std::string& phase_name_s, const double& wavelength);
      void add_phase_weight_percent(const std::string& phase_name_s, const std::string& weight_percent_s);

      PhaseDataSingle& operator[](const std::string& t);
      bool has_phase(const std::string& phase_name_s) const;
   };


   struct DateTime {
      std::string datetime{};
   };



   const std::vector<std::string> X_AXES{ "_pd_meas_2theta_scan", "_pd_proc_2theta_corrected", "_pd_meas_time_of_flight",
                                          "_pd_meas_position", "_pd_proc_energy_incident", "_pd_proc_d_spacing",
                                          "_pd_proc_recip_len_Q", "_pd_meas_2theta_range_inc", "_pd_proc_2theta_range_inc" };
   
   const std::vector<std::string> YOBS_AXES{ "_pd_meas_counts_total", "_pd_meas_intensity_total", "_pd_proc_intensity_total", 
                                             "_pd_proc_intensity_net", "_pd_proc_ls_weight" };

   const std::vector<std::string> YCALC_AXES{ "_pd_calc_intensity_total", "_pd_calc_intensity_net" };

   const std::vector<std::string> YCALC_AXES{ "_pd_meas_counts_background", "_pd_meas_counts_container", "_pd_meas_intensity_background", 
                                              "_pd_meas_intensity_container", "_pd_proc_intensity_bkg_calc", "_pd_proc_intensity_bkg_fix" };

   const std::vector<std::string> Y_AXES = YOBS_AXES + YCALC_AXES + YCALC_AXES;

   const std::vector<std::string> NICE_TO_HAVE{ "_diffrn_radiation_wavelength", "_diffrn_ambient_temperature", "_diffrn_ambient_pressure", 
                                                "_refine_ls_goodness_of_fit_all", "_pd_proc_ls_prof_wR_factor", "_pd_proc_ls_prof_wr_expected" };


   struct BlockIndices {
      std::vector<size_t> patterns{};
      std::vector<size_t> structures{};
      std::vector<size_t> others{};
   };



   struct NiceData {
      std::unordered_map<std::string, double> values{};
      std::unordered_map<std::string, std::string> strings{};

      void add_data(const std::string& t, const std::string& v);
      std::string get_string(const std::string& t) const;
      double operator[](const std::string& t) const;
      bool has_tag(const std::string& t) const;
   };

   struct Pattern {
      std::string block_code{};		// data_#####
      std::string block_id{};			// _pd_block_id

      DateTime datetime{};				// _pd_meas_datetime_initiated

      // _diffrn_radiation_wavelength
      // _diffrn_ambient_temperature
      // _diffrn_ambient_pressure
      // _refine_ls_goodness_of_fit_all
      // _pd_proc_ls_prof_wR_factor
      // _pd_proc_ls_prof_wr_expected
      NiceData nice_data{};

      PhaseData phase_data{};

      //_pd_meas_2theta_scan
      //_pd_proc_2theta_corrected
      //_pd_meas_time_of_flight
      //_pd_meas_position
      //_pd_proc_energy_incident
      //_pd_proc_d_spacing
      //_pd_proc_recip_len_Q
      //_pd_meas_2theta_range_*  min max inc
      //_pd_proc_2theta_range_*  min max inc
      DiffData x_axis{};

      //_pd_meas_counts_total
      //_pd_meas_intensity_total
      //_pd_proc_intensity_total
      //_pd_proc_intensity_net
      //_pd_proc_ls_weight
      DiffData y_obs{};

      //_pd_calc_intensity_total
      //_pd_calc_intensity_net
      DiffData y_calc{};

      //_pd_meas_counts_background
      //_pd_meas_counts_container
      //_pd_meas_intensity_background
      //_pd_meas_intensity_container
      //_pd_proc_intensity_bkg_calc
      //_pd_proc_intensity_bkg_fix
      DiffData y_bkg{};
   };

   struct File {

      std::vector<Pattern> patterns{};
      BlockIndices block_indices;
      row::cif::Cif cif;

      

      void transform();


   private:
      //principal methods
      void make_up_block_ids();
      void group_block_names();
      void get_nice_to_have_information();
      void expand_multiple_dataloops();
      void get_hkl_ticks();
      void make_just_patterns_cif();
      void calc_extra_data();
      void put_phase_mass_pct_in_strs();
      void rename_datablock_from_blockid();


   };

} //end namespace row::cif::convert




#endif

