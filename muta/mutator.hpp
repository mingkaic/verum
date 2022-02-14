///
/// mutator.hpp
/// muta
///
/// Purpose:
/// Define test mutator setup
///

#ifndef PKG_MUTA_MUTATOR_HPP
#define PKG_MUTA_MUTATOR_HPP

#include <iostream>
#include <list>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <google/protobuf/util/time_util.h>

#include "gtest/gtest.h"

#include "logs/logs.hpp"

#include "muta/random.hpp"
#include "muta/random_generator.hpp"
#include "muta/mutator_generator.hpp"
#include "muta/mutation.grpc.pb.h"
#include "muta/mutation.pb.h"

namespace muta
{

struct ProtobufMutatorEntry final : public iMutatorEntry
{
	ProtobufMutatorEntry (MutatorEntry* entry) : entry_(entry) {}

	void save (const std::string& s) override
	{
		auto type = entry_->type();
		assert(type == EntryType::UNSET || type == EntryType::STRING);
		entry_->set_type(EntryType::STRING);
		entry_->add_ss(s);
	}

	void save (const int64_t& i) override
	{
		auto type = entry_->type();
		assert(type == EntryType::UNSET || type == EntryType::INT);
		entry_->set_type(EntryType::INT);
		entry_->add_is(i);
	}

	void save (const double& d) override
	{
		auto type = entry_->type();
		assert(type == EntryType::UNSET || type == EntryType::DOUBLE);
		entry_->set_type(EntryType::DOUBLE);
		entry_->add_ds(d);
	}

	void clear (void) override
	{
		switch (entry_->type())
		{
		case EntryType::STRING:
			entry_->mutable_ss()->Clear();
			break;
		case EntryType::INT:
			entry_->mutable_is()->Clear();
			break;
		case EntryType::DOUBLE:
			entry_->mutable_ds()->Clear();
			break;
		default:
			break;
		}
		entry_->set_type(EntryType::UNSET);
	}

	MutatorEntry* entry_;
};

using PbMutEntryT = std::shared_ptr<ProtobufMutatorEntry>;

struct ProtobufMutatorSession final
{
	ProtobufMutatorSession (const std::string& id)
	{
		session_.set_id(id);
		*session_.mutable_created() = google::protobuf::util::TimeUtil::GetCurrentTime();
	}

	std::string to_string (void) const
	{
		return session_.id();
	}

	PbMutEntryT add_entry (const std::string& id)
	{
		auto entry = &(*session_.mutable_entries())[id];
		return std::make_shared<ProtobufMutatorEntry>(entry);
	}

	MutatorSession* get_session (void)
	{
		return &session_;
	}

	bool has_entry (void) const
	{
		return session_.entries().size() > 0;
	}

private:
	MutatorSession session_;
};

static std::string get_envar (const std::string& envar, const std::string& fallback)
{
	if (auto env = std::getenv(envar.c_str()))
	{
		return std::string(env);
	}
	return fallback;
}

using PbMutSessT = std::shared_ptr<ProtobufMutatorSession>;

static const std::string STORAGE_HOST_ENVAR = "MUTATOR_STORAGE_URL";

static const std::string MUTATOR_STORAGE_HOST = get_envar(STORAGE_HOST_ENVAR,  "localhost:50051");

static const int DEADLINE_MS = 200;

static const size_t MIN_SHUFFLE = 5;

using SessSaveF = std::function<grpc::Status(const SaveSessionsRequest&)>;

using FailCheckerF = std::function<bool()>;

static std::string get_full_test_name (void)
{
	std::string buffer;
	if (const auto* info = testing::UnitTest::GetInstance()->current_test_info())
	{
		if (info->test_case_name())
			buffer += info->test_case_name();
		if (info->name())
		{
			if (!buffer.empty())
				buffer += '.';
			buffer += info->name();
		}
		if (info->type_param())
		{
			if (!buffer.empty())
				buffer += '.';
			buffer += info->type_param();
		}
	}
	return buffer;
}

struct Mutator : public ::testing::Test
{
	static SessSaveF exit_saver_;

	static FailCheckerF fail_checker_;

	virtual ~Mutator (void)
	{
		save_sessions(exit_saver_);
	}

	virtual void SetUp (void)
	{
		std::string sess_id(get_full_test_name());
		sessions_.push_back(std::make_shared<ProtobufMutatorSession>(sess_id));
	}

	virtual void TearDown (void)
	{
		if (sessions_.empty())
		{
			return;
		}
		if (false == fail_checker_())
		{
			sessions_.pop_back();
		}
		else if (false == sessions_.back()->has_entry())
		{
			auto sess = sessions_.back();
			logs::warnf("failed session %s has no entry: ignoring...", sess->to_string().c_str());
			sessions_.pop_back();
		}
	}

	std::vector<double> generate_decs (const std::string& id, size_t n, double lower, double upper)
	{
		std::vector<double> out;
		out.reserve(n);
		entry_session(id,
		[&](MutatorGenerator& generator)
		{
			for (size_t i = 0; i < n; ++i)
			{
				out.push_back(generator.unif_dec(lower, upper));
			}
		});
		return out;
	}

	std::vector<int64_t> generate_ints (const std::string& id,
		size_t n, int64_t lower, int64_t upper)
	{
		std::vector<int64_t> out;
		out.reserve(n);
		entry_session(id,
		[&](MutatorGenerator& generator)
		{
			for (size_t i = 0; i < n; ++i)
			{
				out.push_back(generator.unif_int(lower, upper));
			}
		});
		return out;
	}

	std::vector<std::string> generate_strs (const std::string& id, size_t n)
	{
		std::vector<std::string> out;
		out.reserve(n);
		entry_session(id,
		[&](MutatorGenerator& generator)
		{
			for (size_t i = 0; i < n; ++i)
			{
				out.push_back(generator.get_str());
			}
		});
		return out;
	}

	// Return random k unique indices from 0 to n
	std::vector<int64_t> permute_indices (const std::string& id, size_t n, size_t k)
	{
		std::vector<int64_t> out(n);
		auto begin = out.begin();
		auto end = out.end();
		std::iota(begin, end, 0);
		fisher_yates_shuffle(begin, end, std::min(
			std::max(k+1, MIN_SHUFFLE), n-1), this->generator_);
		out.erase(begin + k, end);
		auto entry = make_entry(id);
		for (auto i : out)
		{
			entry->save(i);
		}
		return out;
	}

	template <typename T>
	void write_entry (const std::string& id, const std::vector<T>& entry_values)
	{
		assert(false == sessions_.empty());
		auto entry = sessions_.back()->add_entry(id);
		entry->clear();
		for (auto e : entry_values)
		{
			entry->save(e);
		}
	}

	void save_sessions (const SessSaveF& uploader)
	{
		if (sessions_.size() > 0)
		{
			SaveSessionsRequest request;
			for (auto sess : sessions_)
			{
				request.add_sessions()->Swap(sess->get_session());
			}
			grpc::Status status = uploader(request);
			if (false == status.ok())
			{
				logs::fatalf("failed to save session to %s: %s",
					MUTATOR_STORAGE_HOST.c_str(), status.error_message().c_str());
			}
			else
			{
				sessions_.clear();
			}
		}
	}

private:
	void entry_session (const std::string& id, std::function<void(MutatorGenerator&)> f)
	{
		MutatorGenerator mgen(generator_, make_entry(id));
		f(mgen);
	}

	PbMutEntryT make_entry (const std::string& id)
	{
		assert(false == sessions_.empty());
		return sessions_.back()->add_entry(id);
	}

	std::list<PbMutSessT> sessions_;

	RandomGenerator generator_;
};

}

#endif // PKG_MUTA_MUTATOR_HPP
