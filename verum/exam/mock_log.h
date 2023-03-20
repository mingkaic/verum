
#ifndef VERUM_EXAM_MOCK_LOG_H
#define VERUM_EXAM_MOCK_LOG_H

#include "gmock/gmock.h"

#include "cisab/logs/ilogs.h"

namespace verum
{

namespace exam
{

struct MockLogger final : public cisab::logs::iLogger
{
	MOCK_CONST_METHOD0(get_log_level, std::string(void));

	MOCK_METHOD1(set_log_level, void(const std::string& log_level));

	MOCK_CONST_METHOD1(supports_level, bool(size_t));

	MOCK_CONST_METHOD1(supports_level, bool(const std::string&));

	MOCK_METHOD3(log, void(const std::string&,const std::string&,const cisab::logs::SrcLocT&));

	MOCK_METHOD3(log, void(size_t,const std::string&,const cisab::logs::SrcLocT&));
};

}

}

#endif // VERUM_EXAM_MOCK_LOG_H
