#include <sstream>
#include <iomanip>

#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "verum/muta/mutation.grpc.pb.h"

template <typename T>
static void to_string (std::ostream& out, const google::protobuf::RepeatedField<T>& vec)
{
	if (vec.size() > 0)
	{
		out << std::setprecision(std::numeric_limits<T>::digits10 + 1) << vec.at(0);
	}
	for (size_t i = 1, n = vec.size(); i < n; ++i)
	{
		out << "," << vec.at(i);
	}
}

static void to_string (std::ostream& out, const google::protobuf::RepeatedPtrField<std::string>& vec)
{
	if (vec.size() > 0)
	{
		out << vec.at(0);
	}
	for (size_t i = 1, n = vec.size(); i < n; ++i)
	{
		out << "," << vec.at(i);
	}
}

static void print_entry (std::ostream& out,
	const std::string& name, const muta::MutatorEntry& entry)
{
	std::string type;
	std::stringstream ss;
	switch (entry.type())
	{
		case muta::EntryType::INT:
			type = "int64_t";
			to_string(ss, entry.is());
			break;
		case muta::EntryType::DOUBLE:
			type = "double";
			to_string(ss, entry.ds());
			break;
		case muta::EntryType::STRING:
			type = "std::string";
			to_string(ss, entry.ss());
			break;
		default:
			break;
	}
	out << "std::vector<" << type << "> " << name << " = {" << ss.str() << "};";
}

class MutationErrorStorageImpl final : public muta::MutationErrorStorage::Service
{
	grpc::Status SaveSessions (grpc::ServerContext* context,
		const muta::SaveSessionsRequest* request, muta::SaveSessionsResponse* response) override
	{
		auto& sessions = request->sessions();
		for (auto& session : sessions)
		{
			std::cout << "// failed " << session.id() << "\n";
			auto& entries = session.entries();
			for (auto& entry : entries)
			{
				print_entry(std::cout, entry.first, entry.second);
				std::cout << "\n";
			}
			std::cout << std::endl;
		}
		return grpc::Status::OK;
	}
};

void RunServer (void)
{
	std::string server_address("0.0.0.0:50051");
	MutationErrorStorageImpl service;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}

int main (int argc, char** argv)
{
	RunServer();
	return 0;
}
