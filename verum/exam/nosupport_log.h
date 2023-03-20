
#ifndef VERUM_EXAM_NOSUPPORT_H
#define VERUM_EXAM_NOSUPPORT_H

#include "cisab/logs/ilogs.h"

namespace verum
{

namespace exam
{

struct NoSupportLogger final : public cisab::logs::iLogger
{
	/// Implementation of iLogger
	std::string get_log_level (void) const override { return ""; }

	/// Implementation of iLogger
	void set_log_level (const std::string&) override {}

	/// Implementation of iLogger
	bool supports_level (size_t) const override { return false; }

	/// Implementation of iLogger
	bool supports_level (const std::string&) const override { return false; }

	/// Implementation of iLogger
	void log (size_t, const std::string&,
		const cisab::logs::SrcLocT& = cisab::logs::SrcLocT::current()) override
	{
		called_ = true;
	}

	/// Implementation of iLogger
	void log (const std::string&, const std::string&,
		const cisab::logs::SrcLocT& = cisab::logs::SrcLocT::current()) override
	{
		called_ = true;
	}

	bool called_ = false;
};

}

}

#endif // VERUM_EXAM_NOSUPPORT_H
