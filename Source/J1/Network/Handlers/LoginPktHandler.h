#pragma once
#include "../Protocol/LoginProtocol.pb.h"
#include "Packet.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "J1.h"
#endif

using LoginHandlerFunc = std::function<bool(SessionPtr&, boost::asio::mutable_buffer&, int32&)>;
extern LoginHandlerFunc GLoginPacketHandler[UINT16_MAX];

// Custom Handler
bool Handle_Login_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset);
bool Handle_RES_LOGIN(SessionPtr& session, Login::RES_LOGIN&pkt);
bool Handle_RES_CHECK_ID(SessionPtr& session, Login::RES_CHECK_ID&pkt);
bool Handle_RES_JOIN(SessionPtr& session, Login::RES_JOIN&pkt);

class LoginPktHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GLoginPacketHandler[i] = Handle_Login_INVALID;
		GLoginPacketHandler[Login::PacketType::PKT_RES_LOGIN] = [](SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset) {
			return DispatchPacket<Login::RES_LOGIN>(Handle_RES_LOGIN, session, buffer, offset);
			};
		GLoginPacketHandler[Login::PacketType::PKT_RES_CHECK_ID] = [](SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset) {
			return DispatchPacket<Login::RES_CHECK_ID>(Handle_RES_CHECK_ID, session, buffer, offset);
			};
		GLoginPacketHandler[Login::PacketType::PKT_RES_JOIN] = [](SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset) {
			return DispatchPacket<Login::RES_JOIN>(Handle_RES_JOIN, session, buffer, offset);
			};
	}

	static bool HandlePacket(SessionPtr& session, const PacketHeader& header, char* ptr, size_t size)
	{
		boost::asio::mutable_buffer buffer = boost::asio::buffer(ptr, size);
		int offset = 4;

		return GLoginPacketHandler[header.Code](session, buffer, offset);
	}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool DispatchPacket(ProcessFunc func, SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
	{
		PacketType pkt;
		if (!PacketUtil::Parse(pkt, buffer, buffer.size(), offset))
		{
			#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
				UE_LOG(LogTemp, Warning, TEXT("Failed to Handle Packet"))
			#else
				spdlog::error("Failed to Handle Packet");
			#endif
			return false;
		}

		return func(session, pkt);
	}
};