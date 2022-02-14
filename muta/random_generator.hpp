#ifndef PKG_MUTA_RANDOM_GENERATOR_HPP
#define PKG_MUTA_RANDOM_GENERATOR_HPP

#include <random>

#include "muta/igenerator.hpp"

namespace muta
{

struct iRandGenerator : public iGenerator
{
	virtual ~iRandGenerator (void) = default;

	/// Seed the random engine using seed specified
	virtual void seed (size_t s) = 0;
};

struct RandomGenerator final : public iRandGenerator
{
	static size_t string_length_;

	static const std::string char_set_;

	/// Implementation of iGenerator
	std::string get_str (void) const override
	{
		const auto indexgen = this->unif_intgen(0, char_set_.size());
		std::string out(string_length_, 'a');
		std::generate(out.begin(), out.end(), [&indexgen]() { return char_set_[indexgen()]; });
		return out;
	}

	/// Implementation of iGenerator
	int64_t unif_int (const int64_t& lower, const int64_t& upper) const override
	{
		std::uniform_int_distribution<int64_t> dist(lower, upper);
		return dist(rengine_);
	}

	/// Implementation of iGenerator
	double unif_dec (const double& lower, const double& upper) const override
	{
		std::uniform_real_distribution<double> dist(lower, upper);
		return dist(rengine_);
	}

	/// Implementation of iGenerator
	double norm_dec (const double& mean, const double& stdev) const override
	{
		std::normal_distribution<double> dist(mean, stdev);
		return dist(rengine_);
	}

	/// Implementation of iGenerator
	GenF<std::string> get_strgen (void) const override
	{
		return [this]{ return this->get_str(); };
	}

	/// Implementation of iGenerator
	GenF<int64_t> unif_intgen (
		const int64_t& lower, const int64_t& upper) const override
	{
		std::uniform_int_distribution<int64_t> dist(lower, upper);
		return std::bind(dist, rengine_);
	}

	/// Implementation of iGenerator
	GenF<double> unif_decgen (
		const double& lower, const double& upper) const override
	{
		std::uniform_real_distribution<double> dist(lower, upper);
		return std::bind(dist, rengine_);
	}

	/// Implementation of iGenerator
	GenF<double> norm_decgen (
		const double& mean, const double& stdev) const override
	{
		std::normal_distribution<double> dist(mean, stdev);
		return std::bind(dist, rengine_);
	}

	/// Implementation of iRandGenerator
	void seed (size_t s) override
	{
		rengine_.seed(s);
	}

	mutable std::default_random_engine rengine_;
};

}

#endif // PKG_MUTA_RANDOM_GENERATOR_HPP
