

#ifndef ROW_CIF_CONVERT_
#define ROW_CIF_CONVERT_

#include <vector>
#include <string>

#include "cif.h"

namespace row::cif::convert {

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

   //convert a string (of vector of) to a double (or vector or), with the corresponding weights
   void s_dw(const std::string& s, double* val, double* weight);
   void s_d(const std::string& s, double* val);
   void vs_dw(const std::vector<std::string>& s, std::vector<double>* val, std::vector<double>* weight);
   void vs_d(const std::string& s, std::vector<double>* val);


   //takes a string representing a number and gives two doubles containing the value and error. The function returns true.
   // If there is not an error, then the error is given as the sqrt of the value. The function returns false.
   bool split_val_err(const std::string& ve, double& v, double& e) {
      size_t fb = ve.find('(');
      if (fb == std::string::npos) { //there is no error in the string
         if (ve == "." || ve == "?") {
            v = std::numeric_limits<double>::quiet_NaN();
            e = std::numeric_limits<double>::quiet_NaN();
         }
         else {
            v = std::stod(ve);
            e = sqrt(abs(v));
         }
         return false;
      }

      size_t lb = ve.find(')');
      std::string val = ve.substr(0, fb);
      std::string err = ve.substr(fb + 1, lb - fb - 1);
      size_t d = val.find('.');
      int pow{ 0 };

      if (d != std::string::npos) {
         pow = static_cast<int>(val.size() - d) - 1;
      }

      v = std::stod(val);
      e = std::stod(err) / std::pow(10, pow);
      return true;
   }

   void split_val_err(const std::vector<std::string>& ves, std::vector<double>& v, std::vector<double>& e) {
      double val{ 0 };
      double err{ 0 };

      for (const std::string ve : ves) {
         split_val_err(ve, val, err);
         v.push_back(val);
         e.push_back(err);
      }
   }



   class DiffDataAxis {
   private:
      std::string m_tag;
      std::vector<double> m_values;
      std::vector<double> m_weights;

   public:
      DiffDataAxis(const std::string& tag, const std::vector<std::string>& values_s, const std::vector<std::string>& weight_s);
      DiffDataAxis(const std::string& tag, const std::vector<std::string>& values_s);

      bool has_weights();

      const std::vector<double>* get_values() const;
      const std::vector<double>* get_weights() const;
      const std::string* get_tag() const;

      bool get_values(const std::vector<double>* v) const;
      bool get_weights(const std::vector<double>* v) const;
      bool get_tag(const std::string* t) const;
   };

   class DiffData {
   private:
      std::vector<std::string> m_tags;
      std::vector<DiffDataAxis> m_data;
   public:
      DiffData()
         : m_tags{}, m_data{} {}

      void add_axis(const std::string& tag, const std::vector<std::string>& values_s, const std::vector<std::string>& weight_s);
      void add_axis(const std::string& tag, const std::vector<std::string>& values_s);

      size_t find_tag(const std::string& t) const;
      const std::vector<double>* get_values(const std::string& t) const;
      const std::vector<std::string> get_tags() const;

      bool has_tag(const std::string& t) const;
      bool get_values(const std::string& t, const std::vector<double>* v) const;
      bool get_tags(const std::vector<double>* v) const;
   };

   struct PhaseDataSingle {
   private:
      std::string phase_name{};					// _pd_phase_name
      double weight_percent{};					// _pd_phase_mass_%
      std::string weight_percent_s{};			// _pd_phase_mass_%
      std::vector<double> refl_d{};				// _refln_d_spacing
      std::vector<double> refl_q{};				// = 2 * M_PI / d 
      std::vector<double> refl_th2{};			// = 2 * asin(lam / (2 * d)) * 180 / M_PI 
      std::vector<std::string> refl_hover{};	// "h + ' ' + k + ' ' + k + ' ' + "d = " + _refln_d_spacing"
   
   public:
      PhaseDataSingle(const std::string& phase_name_s,
         const std::string& weight_percent_s,
         const std::vector<std::string>& refl_d_s);
      PhaseDataSingle(const std::string& phase_name_s,
         const std::string& weight_percent_s,
         const std::vector<std::string>& refl_d_s,
         const double& wavelength);
      PhaseDataSingle(const std::string& phase_name_s,
         const std::string& weight_percent_s,
         const std::vector<std::string>& refl_d_s,
         const std::vector<std::string>& refl_h_s,
         const std::vector<std::string>& refl_k_s,
         const std::vector<std::string>& refl_l_s);
      PhaseDataSingle(const std::string& phase_name_s,
         const std::string& weight_percent_s,
         const std::vector<std::string>& refl_d_s,
         const std::vector<std::string>& refl_h_s,
         const std::vector<std::string>& refl_k_s,
         const std::vector<std::string>& refl_l_s,
         const double& wavelength);


   };

   struct PhaseData {

      std::vector<PhaseDataSingle> data{};

      size_t find_phase(const std::string& p) const;
      bool get_refl_d(const std::string& p, std::vector<double>* v);
      bool get_refl_q(const std::string& p, std::vector<double>* v);
      bool get_refl_th2(const std::string& p, std::vector<double>* v);
      bool get_weight_percent(const std::string& tp, double* v);
      bool get_weight_percent_s(const std::string& p, std::string* v);

      std::vector<double>* get_refl_d(const std::string& p);
      std::vector<double>* get_refl_q(const std::string& p);
      std::vector<double>* get_refl_th2(const std::string& p);
      double* get_weight_percent(const std::string& p);
      std::string* get_weight_percent_s(const std::string& p);

      std::vector<std::string> get_phase_names();					// _pd_phase_name
      std::vector<double> get_weight_percents();					// _pd_phase_mass_%
      std::vector<std::string> get_weight_percents_s();			// _pd_phase_mass_%

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





   struct CifBlock {
      std::string block_code{};		// data_#####
      std::string block_id{};			// _pd_block_id

      DateTime datetime{};				// _pd_meas_datetime_initiated

      double wavelength{ 0 };			// _diffrn_radiation_wavelength
      double temperature{ 0 };		// _diffrn_ambient_temperature
      double pressure{ 0 };			// _diffrn_ambient_pressure
      double gof{ 0 };					// _refine_ls_goodness_of_fit_all
      double rwp{ 0 };					// _pd_proc_ls_prof_wR_factor
      double rexp{ 0 };					// _pd_proc_ls_prof_wr_expected

      std::string wavelength_s{};
      std::string gof_s{};
      std::string rwp_s{};
      std::string rexp_s{};
      std::string temperature_s{};
      std::string pressure_s{};

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

   struct CifFile {
      std::vector<CifBlock> blocks{};
      CifBlock* current_block{ nullptr };
   };

}




#endif

