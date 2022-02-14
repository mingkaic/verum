#include "muta/mutator.hpp"

#ifdef PKG_MUTA_MUTATOR_HPP

namespace muta
{

static grpc::Status grpc_upload (const SaveSessionsRequest& request)
{
	grpc::ClientContext context;
	SaveSessionsResponse response;

	std::chrono::time_point deadline = std::chrono::system_clock::now() +
		std::chrono::milliseconds(DEADLINE_MS);
	context.set_deadline(deadline);

	auto channel = grpc::CreateChannel(
		MUTATOR_STORAGE_HOST, grpc::InsecureChannelCredentials());
	auto stub = MutationErrorStorage::NewStub(channel);
	return stub->SaveSessions(&context, request, &response);
}

static bool gtest_fail (void)
{
	return ::testing::Test::HasFailure();
}

SessSaveF Mutator::exit_saver_ = grpc_upload;

FailCheckerF Mutator::fail_checker_ = gtest_fail;

RandomGenerator Mutator::generator_;

}

#endif
