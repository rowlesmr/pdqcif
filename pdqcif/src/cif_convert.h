

#ifndef CIF_CONVERT
#define CIF_CONVERT

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <iostream>
#include <string>
#include <vector>

#include <assert.h>


namespace cif::convert {

   std::vector<double> diff(const std::vector<double>& o, const std::vector<double>& c);
   void diff_i(std::vector<double>& o, const std::vector<double>& c);

   std::vector<double> scale(const std::vector<double>& d, const double& s);
   std::vector<double> scale(const double& s, const std::vector<double>& o);
   std::vector<double> scale(const std::vector<double>& d, const std::vector<double>& s);

   void scale_i(std::vector<double>& o, const double& s);
   void scale_i(const double& s, std::vector<double>& o);

   std::vector<double> offset(const std::vector<double>& d, const double& s);
   std::vector<double> offset(const double& s, const std::vector<double>& o);

   void offset_i(std::vector<double>& o, const double& s);
   void offset_i(const double& s, std::vector<double>& o);

   std::vector<double> sqrt(const std::vector<double>& d);

   std::vector<double> log10(const std::vector<double>& d);


   struct DiffData {
      std::vector<std::string> tags{};
      std::vector<std::vector<double>> values{};

      size_t find_tag(const std::string t) const;
      std::vector<double>& operator[](const std::string t);
      bool get_values(const std::string t, std::vector<double>* v);
      std::vector<double>* get_values(const std::string t);
   };


   struct PhaseData {
      std::vector<std::string> phase_names{};					// _pd_phase_name
      std::vector<double> weight_percents{};						// _pd_phase_mass_%
      std::vector<std::string> weight_percents_s{};						// _pd_phase_mass_%
      std::vector<std::vector<double>> refl_d{};				// _refln_d_spacing
      std::vector<std::vector<double>> refl_q{};				// = 2 * M_PI / d 
      std::vector<std::vector<double>> refl_th2{};				// = 2 * asin(lam / (2 * d)) * 180 / M_PI 
      std::vector<std::vector<std::string>> refl_hover{};	// "h + ' ' + k + ' ' + k + ' ' + "d = " + _refln_d_spacing"


      size_t find_phase(const std::string t) const;
      bool get_refl_d(const std::string t, std::vector<double>* v);
      bool get_refl_q(const std::string t, std::vector<double>* v);
      bool get_refl_th2(const std::string t, std::vector<double>* v);
      bool get_weight_percent(const std::string t, double* v);
      bool get_weight_percent_s(const std::string t, std::string* v);

      std::vector<double>* get_refl_d(const std::string t);
      std::vector<double>* get_refl_q(const std::string t);
      std::vector<double>* get_refl_th2(const std::string t);
      double* get_weight_percent(const std::string t);
      std::string* get_weight_percent_s(const std::string t);
   };


   struct DateTime {
      std::string datetime{};
   };

   struct CifBlock {
      std::string block_code{};		// data_#####
      std::string block_id{};			// _pd_block_id

      DateTime datetime{};				// _pd_meas_datetime_initiated

      double wavelength{ 0 };			// _diffrn_radiation_wavelength
      double gof{ 0 };					// _refine_ls_goodness_of_fit_all
      double rwp{ 0 };					// _pd_proc_ls_prof_wR_factor
      double rexp{ 0 };					// _pd_proc_ls_prof_wr_expected
      double temperature{ 0 };		// _diffrn_ambient_temperature
      double pressure{ 0 };			// _diffrn_ambient_pressure

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

   struct CifCif {
      std::vector<CifBlock> blocks{};
      CifBlock* current_block{ nullptr };
   };

}




#endif

