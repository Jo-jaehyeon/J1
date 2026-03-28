#pragma once

#include "J1.h"

/**
 *
 */
class J1_API NetworkWorker : public FRunnable
{
public:
	NetworkWorker(TSharedPtr<PacketSession> Session);
	~NetworkWorker();

	// 가상함수 (Override)
	virtual bool Init() override;	// 스레드 시작
	virtual uint32 Run() override;	// 스레드 실행
	virtual void Exit() override;	// 스레드 실행 끝
	virtual void Stop() override;	// 스레드 정지

	void Destroy();

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;

	// 소켓 & 세션 멤버 
	TWeakPtr<PacketSession> SessionRef;
};