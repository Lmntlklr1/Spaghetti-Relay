#include "Client.h"

int Client::init(uint16_t port, char* address)
{
	if (startup())
		return STARTUP_ERROR;
	skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(skt == INVALID_SOCKET)
		return SETUP_ERROR;
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(address);
	if (addr.sin_addr.S_un.S_addr == INADDR_NONE)
		return ADDRESS_ERROR;
	addr.sin_port = htons(port);
	int cnct = connect(skt, (struct sockaddr*)&addr, sizeof(addr));
	if (cnct == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return CONNECT_ERROR;
	}
	return SUCCESS;
}
int Client::readMessage(char* buffer, int32_t size)
{
	unsigned short length;
	int recv_result;
	recv_result = recv(skt, (char*)&length, 2, 0);
	length = htons(length);
	if (recv_result == 0)
		return SHUTDOWN;
	if (recv_result == SOCKET_ERROR)
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	if (length >= size)
		return PARAMETER_ERROR;
	recv_result = recv(skt, buffer, length, 0);
	if (recv_result == SOCKET_ERROR)
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	buffer[length] = '\0';
	return SUCCESS;
}
int Client::sendMessage(char* data, int32_t length)
{
	char* buffer = new char[2 + length];
	if (length != (unsigned char)length)
		return PARAMETER_ERROR;
	*(unsigned short*)buffer = htons(length);
	// Then the string
	memcpy(buffer + 2, data, length);
	//cajbcoa isjbdocivsbao
	int send_result = send(skt, buffer, length + 2, 0);
	if (send_result == 0)
		return SHUTDOWN;
	if (send_result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;
		else
			return DISCONNECT;
	}
	return SUCCESS;
}
void Client::stop()
{
	shutdown(skt, SD_BOTH);
	closesocket(skt);
}