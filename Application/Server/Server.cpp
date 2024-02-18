#include "Server.h"

int Server::init(uint16_t port)
{
	if (startup())
		return STARTUP_ERROR;
	skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (skt == INVALID_SOCKET)
	{
		return BIND_ERROR;
	}
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	int bind_return = bind(skt, (struct sockaddr*)&addr, sizeof(addr));
	int error = WSAGetLastError();
	if (bind_return == SOCKET_ERROR)
		return BIND_ERROR;
	int listen_return = listen(skt, SOMAXCONN);
	if (listen_return == SOCKET_ERROR)
		return SETUP_ERROR;
	struct sockaddr_in addr_client = { 0 };
	int addr_client_length = sizeof(addr_client);
	accept_return = accept(skt, (struct sockaddr*)&addr_client, &addr_client_length);
	if (accept_return == SOCKET_ERROR)
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return CONNECT_ERROR;
	return SUCCESS;
}
int Server::readMessage(char* buffer, int32_t size)
{
	unsigned short length;
	int recv_return = recv(accept_return, (char*)&length, 2, 0);
	int error = WSAGetLastError();
	if (recv_return == 0)
		return SHUTDOWN;
	if (recv_return == SOCKET_ERROR)
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	if (length > size)
		return PARAMETER_ERROR;
	recv_return = recv(accept_return, buffer, length, 0);
	if (recv_return == SOCKET_ERROR)
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	return SUCCESS;
}
int Server::sendMessage(char* data, int32_t length)
{
	if(length != (unsigned char)length)
		return PARAMETER_ERROR;
	unsigned char* packet = new unsigned char[length + 1];
	memcpy(packet + 1, data, length);
	packet[0] = (char)(unsigned char)length;
	int send_return = send(accept_return, (char*)packet, length + 1, 0);
	if (send_return == 0)
		return SHUTDOWN;
	if (send_return == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	}
	return SUCCESS;
}
void Server::stop()
{
	shutdown(skt, SD_BOTH);
	closesocket(skt);
}