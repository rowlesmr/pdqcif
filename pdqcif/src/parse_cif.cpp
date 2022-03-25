
#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <assert.h>



namespace pcif {

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
	void scale_i(const double& s, std::vector<double>& o ) {
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



	struct DiffData {
		std::vector<std::string> tags{};
		std::vector<std::vector<double>> values{};

		size_t find_tag(const std::string t) const {
			for (size_t i{ 0 }; i < tags.size(); ++i) {
				if (t == tags[i])
					return i;
			}
			return SIZE_MAX;
		}

		std::vector<double>& operator[](const std::string t) {
			return values[find_tag(t)];
		}

		bool get_values(const std::string t, std::vector<double>* v) {
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

		std::vector<double>* get_values(const std::string t) {
			size_t i = find_tag(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &values[i];
			}
		}

	};


	struct PhaseData {
		std::vector<std::string> phase_names{};					// _pd_phase_name
		std::vector<double> weight_percents{};						// _pd_phase_mass_%
		std::vector<std::string> weight_percents_s{};						// _pd_phase_mass_%
		std::vector<std::vector<double>> refl_d{};				// _refln_d_spacing
		std::vector<std::vector<double>> refl_q{};				// = 2 * M_PI / d 
		std::vector<std::vector<double>> refl_th2{};				// = 2 * asin(lam / (2 * d)) * 180 / M_PI 
		std::vector<std::vector<std::string>> refl_hover{};	// "h + ' ' + k + ' ' + k + ' ' + "d = " + _refln_d_spacing"


		size_t find_phase(const std::string t) const {
			for (size_t i{ 0 }; i < phase_names.size(); ++i) {
				if (t == phase_names[i])
					return i;
			}
			return SIZE_MAX;
		}

		bool get_refl_d(const std::string t, std::vector<double>* v) {
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
		bool get_refl_q(const std::string t, std::vector<double>* v) {
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
		bool get_refl_th2(const std::string t, std::vector<double>* v) {
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
		bool get_weight_percent(const std::string t, double* v) {
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
		bool get_weight_percent_s(const std::string t, std::string* v) {
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

		std::vector<double>* get_refl_d(const std::string t) {
			size_t i = find_phase(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &refl_d[i];
			}
		}
		std::vector<double>* get_refl_q(const std::string t) {
			size_t i = find_phase(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &refl_q[i];
			}
		}
		std::vector<double>* get_refl_th2(const std::string t) {
			size_t i = find_phase(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &refl_th2[i];
			}
		}
		double* get_weight_percent(const std::string t) {
			size_t i = find_phase(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &weight_percents[i];
			}
		}
		std::string* get_weight_percent_s(const std::string t) {
			size_t i = find_phase(t);
			if (i == SIZE_MAX) {
				return nullptr;
			}
			else {
				return &weight_percents_s[i];
			}
		}

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

namespace cif::helper {

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


}

