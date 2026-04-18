#include "LoginPktHandler.h"
#include "J1GameInstance.h"
#include "PacketSession.h"
#include "J1EnumTypes.h"


LoginHandlerFunc GLoginPacketHandler[UINT16_MAX];

bool Handle_Login_INVALID(SessionPtr& session, boost::asio::mutable_buffer& buffer, int32& offset)
{
	return false;
}

bool Handle_RES_LOGIN(SessionPtr& session, Login::RES_LOGIN& pkt)
{
	int account = pkt.player_id();
	bool result = (account > 0);

	AsyncTask(ENamedThreads::GameThread, [result, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnLoginResult.Broadcast(ELoginMode::Login, result);
		}
	});

	return true;
}

bool Handle_RES_CHECK_ID(SessionPtr& session, Login::RES_CHECK_ID& pkt)
{
	bool result = pkt.result();

	AsyncTask(ENamedThreads::GameThread, [result, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnLoginResult.Broadcast(ELoginMode::CheckId, result);
		}
		});

	return true;
}

bool Handle_RES_JOIN(SessionPtr& session, Login::RES_JOIN& pkt)
{
	bool result = pkt.result();

	AsyncTask(ENamedThreads::GameThread, [result, session]() {
		if (auto* GI = Cast<UJ1GameInstance>(session->GetGameInstance()))
		{
			GI->OnLoginResult.Broadcast(ELoginMode::SignUp, result);
		}
		});
	return true;
}
