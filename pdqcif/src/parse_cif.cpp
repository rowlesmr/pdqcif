
#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <assert.h>



namespace cif::helper {


	bool stode(const char* s, double& v, double& e)
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
		if (c == '.') {
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

	bool stode(const std::string s, double& v, double& e)
	{
		return stode(s.c_str(), v, e);
	}

	bool split_val_err(const std::vector<std::string>& ves, std::vector<double>* v, std::vector<double>* e) {
		double val{ 0 };
		double err{ 0 };
		bool has_err = false;
		v->clear();
		e->clear();

		for (const std::string ve : ves) {
			stode(ve, val, err);
			v->push_back(val);
			e->push_back(err);
			if (err > 0.0) has_err = true;
		}
		return has_err;
	}


}

