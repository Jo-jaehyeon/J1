#include "NetworkWorker.h"
#include "PacketSession.h"

using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;


NetworkWorker::NetworkWorker(TSharedPtr<PacketSession> Session)
	: SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("NetworkWorker"));
}

NetworkWorker::~NetworkWorker()
{
}

bool NetworkWorker::Init()
{
	return true;
}

uint32 NetworkWorker::Run()
{
	try
	{
		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			work_guard_type work_guard(Session->GetIoContext().get_executor());
			Session->GetIoContext().run();
		}
	}
	catch(const std::exception& e)
	{
		FString formattedString = FString::Printf(TEXT("%s"), *FString(UTF8_TO_TCHAR(e.what())));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *formattedString);
	}

	return 0;
}

void NetworkWorker::Exit()
{
	Running = false;
	Thread->Kill();
}

void NetworkWorker::Stop()
{
}