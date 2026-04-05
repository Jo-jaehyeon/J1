#pragma once
#include <google/protobuf/message.h>
#include <boost/asio.hpp>

using namespace boost;

struct PacketHeader
{
	short Length;
	short Code;
};

class PacketUtil
{
public:
	static size_t RequiredSize(const google::protobuf::Message& msg)
	{
		return sizeof(PacketHeader) + msg.ByteSizeLong();
	}

	static bool Serialize(const asio::mutable_buffer& buffer, const short packetCode, const google::protobuf::Message& msg)
	{
		const size_t requiredSize = RequiredSize(msg);
		if (buffer.size() < requiredSize)
			return false;

		PacketHeader header;
		header.Length = static_cast<short>(msg.ByteSizeLong());
		header.Code = packetCode;

		memcpy(buffer.data(), &header, sizeof(PacketHeader));

		char* payloadPtr = static_cast<char*>(buffer.data()) + sizeof(PacketHeader);
		if (!msg.SerializeToArray(payloadPtr, static_cast<int>(buffer.size()) - sizeof(PacketHeader)))
			return false;

		return true;

	}

	static bool ParseHeader(const asio::mutable_buffer& buffer, PacketHeader* header, int& offset)
	{
		if (buffer.size() <= offset)
		{
			UE_LOG(LogTemp, Error, TEXT("Header size is smaller than offset.."));
			//spdlog::error("Header size is smaller than offset..");
			return false;
		}

		const size_t remainedSize = buffer.size() - offset;
		if (remainedSize < sizeof(PacketHeader))
		{
			UE_LOG(LogTemp, Error, TEXT("remainedSize of Header is smaller than PacketHeader' size"));
			//spdlog::error("remainedSize of Header is smaller than PacketHeader' size");
			return false;
		}

		const char* headerPtr = static_cast<char*>(buffer.data()) + offset;
		memcpy(header, headerPtr, sizeof(PacketHeader));
		offset += sizeof(PacketHeader);

		return true;
	}

	static bool Parse(google::protobuf::Message& msg, const asio::mutable_buffer& buffer, const int payloadSize, int& offset)
	{
		const char* payloadPtr = static_cast<char*>(buffer.data()) + offset;
		const size_t remainedSize = buffer.size() - offset;
		UE_LOG(LogTemp, Log, TEXT("payloadSize : %d, offset : %d, remainedSize : %d"), payloadSize, offset, remainedSize);
		if (payloadPtr == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Payload Empty!"));
			return false;
		}

		const bool parseResult = msg.ParseFromArray(payloadPtr, remainedSize);
		if (parseResult)
		{
			offset += static_cast<int>(msg.ByteSizeLong());
			UE_LOG(LogTemp, Log, TEXT("Parsing Success! offset = %d"), offset);
			return true;
		}
		else
		{
			//spdlog::error("Failed to parse data..");
			UE_LOG(LogTemp, Error, TEXT("Parsing Error!"));
			return false;
		}

	}
};