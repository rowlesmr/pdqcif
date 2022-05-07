

#ifndef ROW_CIF_CONVERT_
#define ROW_CIF_CONVERT_

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <vector>
#include <unordered_map>
#include <string>
#include < algorithm >
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

 
   template <typename T, typename U>
   std::vector<U> mask(const T& mask_value, const std::vector<T>& mask_vector, const std::vector<U> input) {
      if (mask_vector.size() != input.size())
         throw std::runtime_error("Vectors to be masked must be the same length.");
            
      std::vector<U> output{};
      output.reserve(mask_vector.size());

      for (size_t i{ 0 }; i < input.size(); ++i) {
         if (mask_vector[i] == mask_value)
            output.push_back(input[i]);
      }
      return output;
   }

   bool stode(const char* s, const size_t len, double& v, double& e);
   bool stod(const char* s, const size_t len, double& v);

   bool str_to_ve(const std::string s, double& v, double& e);
   bool str_to_v(const std::string s, double& v);


   void errors_to_weights(std::vector<double> errors);
   void weights_from_counts(const std::vector<double> counts, std::vector<double>& weights);

   struct DiffDataAxis {
   
      std::vector<double> values{};
      std::vector<double> weights{};
      bool has_weights{};
         
      DiffDataAxis(const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s);
      DiffDataAxis(const std::vector<std::string>& values_s, const bool weights_from_values = false);
      DiffDataAxis(const std::string& th2_min, const std::string& th2_inc, const std::string& th2_max);

      DiffDataAxis(const std::vector<double>& values_s, const std::vector<double>& weights_s);
      DiffDataAxis(const std::vector<double>& values_s, const bool weights_from_values = false);

      size_t size();
   };

   struct DiffData {
      std::vector<std::string> tags{};
      std::unordered_map<std::string, DiffDataAxis> axes{};

      void add_axis(const std::string& tag, const std::vector<std::string>& values_s, const std::vector<std::string>& weights_s);
      void add_axis(const std::string& tag, const std::vector<std::string>& values_s, const bool weights_from_values = false);
      void add_axis(const std::string& tag, const std::string& th2_min, const std::string& th2_inc, const std::string& th2_max);

      void add_axis(const std::string& tag, const std::vector<double>& values_s, const std::vector<double>& weights_s);
      void add_axis(const std::string& tag, const std::vector<double>& values_s, const bool weights_from_values = false);
      void add_axis(const std::string& tag, const double th2_min, const double th2_inc, const double th2_max);

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

   const std::vector<std::string> YBKG_AXES{ "_pd_meas_counts_background", "_pd_meas_counts_container", "_pd_meas_intensity_background", 
                                              "_pd_meas_intensity_container", "_pd_proc_intensity_bkg_calc", "_pd_proc_intensity_bkg_fix" };

   const std::vector<std::string> Y_AXES = YOBS_AXES + YCALC_AXES + YBKG_AXES;

   const std::vector<std::string> NICE_TO_HAVE{ "_diffrn_radiation_wavelength", "_cell_measurement_wavelength", 
                                                "_diffrn_ambient_temperature", "_cell_measurement_temperature", 
                                                "_diffrn_ambient_pressure", "_cell_measurement_pressure",
                                                "_refine_ls_goodness_of_fit_all", "_pd_proc_ls_prof_wR_factor", 
                                                "_pd_proc_ls_prof_wr_expected" };


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

      Pattern(const std::string& name, const std::string& id)
         : block_code{ name }, block_id{ id } {}

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

      std::vector<std::string> pattern_block_ids{};
      std::vector<Pattern> patterns{};
      BlockIndices block_indices;
      row::cif::Cif cif;

      File(const row::cif::Cif& in);


   private:
       //calls the private methods in the correct order
      // to make the Patterns
      void transform();

     //principal methods
      void remove_empties();
      void make_up_block_ids();
      void expand_multiple_dataloops();
      void group_block_names();
      void initialise_patterns();
      void get_nice_to_have_information();
      void get_phase_info();
      void calc_xaxis_d_q();

      //helper methods
      void calc_d_q_from_th2(Pattern& pattern, const std::string& t, double lam);
      std::vector<double> calc_dq_from_qd(Pattern& pattern, const std::string& t);

   };


#if 0
   //Helper functions to print things.

   enum class PrintVerbosity {
      None,
      Some, // prints first 3 and last 3 lines
      All
   };

   template<typename T>
   void print(std::vector<T>& v, const std::string& title, PrintVerbosity pv) {
      if (v.size() < 6 && pv == PrintVerbosity::Some)
         pv = PrintVerbosity::All;

      std::cout << "* " << title << "\n";
      if (pv == PrintVerbosity::None) {
         //don't print anything.
      }
      else if (pv == PrintVerbosity::All) {
         for (size_t i{ 0 }; i < v.size(); ++i) {
            std::cout << i << ": " << v[i] << "\n";
         }
      }
      else if (pv == PrintVerbosity::Some) {
         for (size_t i{ 0 }; i < 3; ++i) {
            std::cout << i << ": " << v[i] << "\n";
         }
         std::cout << "   ...   \n";
         for (size_t i{ v.size() - 3 }; i < v.size(); ++i) {
            std::cout << i << ": " << v[i] << "\n";
         }
      }
      std::cout << std::endl;
   }

   template<typename T>
   void print(T& v, const std::string& title, PrintVerbosity pv) {
      std::cout << "* " << title << "\n";
      if (pv == PrintVerbosity::None) {
         //don't print anything.
      }
      else if (pv == PrintVerbosity::All || pv == PrintVerbosity::Some) {
            std::cout << v << "\n";
      }
      std::cout << std::endl;
   }

   template<typename K, typename V>
   void print(const std::unordered_map<K, V>& m, const std::string& title, PrintVerbosity pv)
   {
      std::cout << "* " << title << "\n";
      if (pv == PrintVerbosity::None) {
         //don't print anything.
      }
      else if (pv == PrintVerbosity::All || pv == PrintVerbosity::Some) {
         for (auto const& pair : m) {
            std::cout << pair.first << ": " << pair.second << "\n";
         }
      }
      std::cout << std::endl;
   }

   void print(DiffData& dd, PrintVerbosity pv);
   void print(PhaseData& pd, PrintVerbosity pv);
   void print(DateTime& dt, PrintVerbosity pv);
   void print(NiceData& nd, PrintVerbosity pv);
   void print(Pattern& p, PrintVerbosity pv);
   void print(File& file, PrintVerbosity pv);

#endif


} //end namespace row::cif::transform


#endif


