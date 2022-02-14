#ifndef PKG_MUTA_IGENERATOR_HPP
#define PKG_MUTA_IGENERATOR_HPP

#include <functional>
#include <string>

namespace muta
{

/// Function that returns a generated value
template <typename T>
using GenF = std::function<T()>;

struct iGenerator
{
	virtual ~iGenerator (void) = default;

	/// Return random string
	virtual std::string get_str (void) const = 0;

	/// Return uniformly generate integer between a and b
	virtual int64_t unif_int (const int64_t& lower, const int64_t& upper) const = 0;

	/// Return uniformly generate decimal between a and b
	virtual double unif_dec (const double& lower, const double& upper) const = 0;

	/// Return normally generate decimal with mean and stdev
	virtual double norm_dec (const double& mean, const double& stdev) const = 0;

	/// Return random string generator function
	virtual GenF<std::string> get_strgen (void) const = 0;

	/// Return generator function that uniformly generates integers between a and b
	virtual GenF<int64_t> unif_intgen (const int64_t& lower, const int64_t& upper) const = 0;

	/// Return generator function that uniformly generates decimals between a and b
	virtual GenF<double> unif_decgen (const double& lower, const double& upper) const = 0;

	/// Return generator function that normally generate decimal with mean and stdev
	virtual GenF<double> norm_decgen (const double& mean, const double& stdev) const = 0;
};

}

#endif // PKG_MUTA_IGENERATOR_HPP
