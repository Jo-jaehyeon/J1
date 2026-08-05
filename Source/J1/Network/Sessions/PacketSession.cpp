#include "PacketSession.h"
#include "NetworkWorker.h"
#include "../J1GameInstance.h"

PacketSession::PacketSession(asio::io_context* io_context)
	: _socket(*io_context)
	, _io_context(io_context)
{
	memset(_recvBuffer, 0, RecvBufferSize);
	GameInstance = GWorld->GetGameInstance();
}

PacketSession::~PacketSession()
{
}

void PacketSession::Run()
{
	NetworkThread = MakeShared<NetworkWorker>(AsShared());
}

void PacketSession::Connect(std::string host, int port)
{
	const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
	_socket.async_connect(endpoint, 
		boost::bind(
			&PacketSession::OnConnect, 
			this, 
			boost::asio::placeholders::error
		)
	);
}

void PacketSession::AsyncRead()
{
	memset(_recvBuffer, 0, RecvBufferSize);
	_socket.async_read_some(
		asio::buffer(
			_recvBuffer,
			RecvBufferSize
		),
		boost::bind(
			&PacketSession::OnRead,
			this,
			asio::placeholders::error,
			asio::placeholders::bytes_transferred
		)
	);
}

void PacketSession::AsyncWrite(asio::mutable_buffer& buffer)
{
	asio::async_write(
		_socket,
		buffer,
		boost::bind(
			&PacketSession::OnWrite,
			this,
			asio::placeholders::error,
			asio::placeholders::bytes_transferred
		)
	);
}

void PacketSession::OnRead(const boost::system::error_code& err, size_t size)
{
	if (!err)
	{
		HandlePacket(_recvBuffer, size);
		AsyncRead();
	}
	else
	{
		// TODO : Error
	}
}

void PacketSession::OnWrite(const boost::system::error_code& err, size_t size)
{
	if (!err)
	{
		UE_LOG(LogTemp, Log, TEXT("OnWrite : %d"), size);
	}
	else
	{
		// TODO : Error Code
	}
}

void PacketSession::HandlePacket(char* ptr, size_t size)
{
	PacketHeader header;
	asio::mutable_buffer buffer = asio::buffer(ptr, size);

	int offset = 0;
	if (PacketUtil::ParseHeader(buffer, &header, offset))
	{
		UE_LOG(LogTemp, Log, TEXT("Header Parsing True, headerCode: %d, Length : %d"), header.Code, header.Length);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Header Parsing False.."));
	}

	SessionPtr session = this->AsShared();


	// 자식 클래스에서 처리
	DispatchPacket(session, header, ptr, size);
}
