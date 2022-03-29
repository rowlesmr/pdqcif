

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




   struct DiffDataAxis {
      std::string tag{};
      std::vector<double> values{};
   };

   struct DiffData {
      std::vector<DiffDataAxis> data{};

      size_t find_tag(const std::string& t) const;
      std::vector<double>& operator[](const std::string& t);
      bool get_values(const std::string& t, std::vector<double>* v);
      std::vector<double>* get_values(const std::string& t);
      std::vector<std::string> get_tags();
   };

   struct PhaseDataSingle {
      std::string phase_name{};					// _pd_phase_name
      double weight_percent{};					// _pd_phase_mass_%
      std::string weight_percent_s{};			// _pd_phase_mass_%
      std::vector<double> refl_d{};				// _refln_d_spacing
      std::vector<double> refl_q{};				// = 2 * M_PI / d 
      std::vector<double> refl_th2{};			// = 2 * asin(lam / (2 * d)) * 180 / M_PI 
      std::vector<std::string> refl_hover{};	// "h + ' ' + k + ' ' + k + ' ' + "d = " + _refln_d_spacing"
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

