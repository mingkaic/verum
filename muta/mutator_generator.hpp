#ifndef PKG_MUTA_MUTATOR_GENERATOR_HPP
#define PKG_MUTA_MUTATOR_GENERATOR_HPP

#include "muta/igenerator.hpp"

namespace muta
{

struct iMutatorEntry
{
	virtual ~iMutatorEntry (void) = default;

	virtual void save (const std::string& s) = 0;

	virtual void save (const int64_t& i) = 0;

	virtual void save (const double& d) = 0;

	virtual void clear (void) = 0;
};

using MutEntryT = std::shared_ptr<iMutatorEntry>;

struct MutatorGenerator final : public iGenerator
{
	MutatorGenerator (iGenerator& generator, MutEntryT entry) :
		generator_(&generator), entry_(entry) {}

	std::string get_str (void) const override
	{
		auto out = generator_->get_str();
		if (entry_ != nullptr)
		{
			entry_->save(out);
		}
		return out;
	}

	int64_t unif_int (const int64_t& lower, const int64_t& upper) const override
	{
		auto out = generator_->unif_int(lower, upper);
		if (entry_ != nullptr)
		{
			entry_->save(out);
		}
		return out;
	}

	double unif_dec (const double& lower, const double& upper) const override
	{
		auto out = generator_->unif_dec(lower, upper);
		if (entry_ != nullptr)
		{
			entry_->save(out);
		}
		return out;
	}

	double norm_dec (const double& mean, const double& stdev) const override
	{
		auto out = generator_->norm_dec(mean, stdev);
		if (entry_ != nullptr)
		{
			entry_->save(out);
		}
		return out;
	}

	GenF<std::string> get_strgen (void) const override
	{
		return [this]()
		{
			return this->get_str();
		};
	}

	GenF<int64_t> unif_intgen (const int64_t& lower, const int64_t& upper) const override
	{
		return [this,lower,upper]()
		{
			return this->unif_int(lower, upper);
		};
	}

	GenF<double> unif_decgen (const double& lower, const double& upper) const override
	{
		return [this,lower,upper]()
		{
			return this->unif_dec(lower, upper);
		};
	}

	GenF<double> norm_decgen (const double& mean, const double& stdev) const override
	{
		return [this,mean,stdev]()
		{
			return this->norm_dec(mean, stdev);
		};
	}

private:
	iGenerator* generator_;

	MutEntryT entry_;
};

}

#endif // PKG_MUTA_MUTATOR_GENERATOR_HPP
