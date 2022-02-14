#include "exam/exam.hpp"

#include "muta/mutator.hpp"


static bool trigger_fail = false;


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	muta::Mutator::fail_checker_ = [](){ return trigger_fail; };
	return RUN_ALL_TESTS();
}


struct MUTATOR : public muta::Mutator {};


TEST_F(MUTATOR, NoIssue)
{
	trigger_fail = false;
	TearDown();

	bool upload_called = false;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		upload_called = true; // expect session to not be called
		return grpc::Status::OK;
	});
	EXPECT_FALSE(upload_called);
}


TEST_F(MUTATOR, NoEntrySession)
{
	trigger_fail = true;
	TearDown();

	bool upload_called = false;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		upload_called = true; // expect session to not be called
		return grpc::Status::OK;
	});
	EXPECT_FALSE(upload_called);
}


TEST_F(MUTATOR, DecimalsEntrySession)
{
	trigger_fail = true;
	std::string expected_entry_key = "decimals";
	std::vector<double> generated = generate_decs(expected_entry_key, 10, 0., 1.);
	EXPECT_EQ(10, generated.size());
	TearDown();

	std::string sess_id;
	int64_t num_sessions = -1;
	int64_t num_entries = -1;
	muta::EntryType entry_type = muta::EntryType::UNSET;
	std::vector<double> entry_result;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		auto& sessions = request.sessions();
		num_sessions = sessions.size();
		if (num_sessions > 0)
		{
			auto& sess = sessions.Get(0);
			sess_id = sess.id();
			auto& entries = sess.entries();
			num_entries = entries.size();
			if (num_entries > 0 && entries.contains(expected_entry_key))
			{
				auto& entry = entries.at(expected_entry_key);
				entry_type = entry.type();
				auto& ds = entry.ds();
				entry_result = std::vector<double>(ds.begin(), ds.end());
			}
		}
		return grpc::Status::OK;
	});
	EXPECT_EQ(1, num_sessions);
	EXPECT_STREQ("MUTATOR.DecimalsEntrySession", sess_id.c_str());
	EXPECT_EQ(1, num_entries);
	EXPECT_EQ(muta::EntryType::DOUBLE, entry_type);
	EXPECT_VECEQ(generated, entry_result);
}


TEST_F(MUTATOR, IntsEntrySession)
{
	trigger_fail = true;
	std::string expected_entry_key = "integers";
	std::vector<int64_t> generated = generate_ints(expected_entry_key, 10, 1, 150);
	EXPECT_EQ(10, generated.size());
	TearDown();

	std::string sess_id;
	int64_t num_sessions = -1;
	int64_t num_entries = -1;
	muta::EntryType entry_type = muta::EntryType::UNSET;
	std::vector<int64_t> entry_result;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		auto& sessions = request.sessions();
		num_sessions = sessions.size();
		if (num_sessions > 0)
		{
			auto& sess = sessions.Get(0);
			sess_id = sess.id();
			auto& entries = sess.entries();
			num_entries = entries.size();
			if (num_entries > 0 && entries.contains(expected_entry_key))
			{
				auto& entry = entries.at(expected_entry_key);
				entry_type = entry.type();
				auto& is = entry.is();
				entry_result = std::vector<int64_t>(is.begin(), is.end());
			}
		}
		return grpc::Status::OK;
	});
	EXPECT_EQ(1, num_sessions);
	EXPECT_STREQ("MUTATOR.IntsEntrySession", sess_id.c_str());
	EXPECT_EQ(1, num_entries);
	EXPECT_EQ(muta::EntryType::INT, entry_type);
	EXPECT_VECEQ(generated, entry_result);
}


TEST_F(MUTATOR, StringsEntrySession)
{
	trigger_fail = true;
	std::string expected_entry_key = "strings";
	std::vector<std::string> generated = generate_strs(expected_entry_key, 10);
	EXPECT_EQ(10, generated.size());
	TearDown();

	std::string sess_id;
	int64_t num_sessions = -1;
	int64_t num_entries = -1;
	muta::EntryType entry_type = muta::EntryType::UNSET;
	std::vector<std::string> entry_result;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		auto& sessions = request.sessions();
		num_sessions = sessions.size();
		if (num_sessions > 0)
		{
			auto& sess = sessions.Get(0);
			sess_id = sess.id();
			auto& entries = sess.entries();
			num_entries = entries.size();
			if (num_entries > 0 && entries.contains(expected_entry_key))
			{
				auto& entry = entries.at(expected_entry_key);
				entry_type = entry.type();
				auto& ss = entry.ss();
				entry_result = std::vector<std::string>(ss.begin(), ss.end());
			}
		}
		return grpc::Status::OK;
	});
	EXPECT_EQ(1, num_sessions);
	EXPECT_STREQ("MUTATOR.StringsEntrySession", sess_id.c_str());
	EXPECT_EQ(1, num_entries);
	EXPECT_EQ(muta::EntryType::STRING, entry_type);
	EXPECT_VECEQ(generated, entry_result);
}


TEST_F(MUTATOR, PermuteEntrySession)
{
	trigger_fail = true;
	std::string expected_entry_key = "indices";
	std::vector<int64_t> generated = permute_indices(expected_entry_key, 100, 10);
	EXPECT_EQ(10, generated.size());
	TearDown();

	std::string sess_id;
	int64_t num_sessions = -1;
	int64_t num_entries = -1;
	muta::EntryType entry_type = muta::EntryType::UNSET;
	std::vector<int64_t> entry_result;

	save_sessions(
	[&](const muta::SaveSessionsRequest& request)
	{
		auto& sessions = request.sessions();
		num_sessions = sessions.size();
		if (num_sessions > 0)
		{
			auto& sess = sessions.Get(0);
			sess_id = sess.id();
			auto& entries = sess.entries();
			num_entries = entries.size();
			if (num_entries > 0 && entries.contains(expected_entry_key))
			{
				auto& entry = entries.at(expected_entry_key);
				entry_type = entry.type();
				auto& is = entry.is();
				entry_result = std::vector<int64_t>(is.begin(), is.end());
			}
		}
		return grpc::Status::OK;
	});
	EXPECT_EQ(1, num_sessions);
	EXPECT_STREQ("MUTATOR.PermuteEntrySession", sess_id.c_str());
	EXPECT_EQ(1, num_entries);
	EXPECT_EQ(muta::EntryType::INT, entry_type);
	EXPECT_VECEQ(generated, entry_result);

	// ensure the indices are unique
	std::unordered_set<int64_t> indices(generated.begin(), generated.end());
	EXPECT_EQ(generated.size(), indices.size());
}
