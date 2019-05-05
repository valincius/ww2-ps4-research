#include <sys/_types/_uint64_t.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <libnet/nettypes.h>
#include <libnet/inet.h>
#include <libnet/socket.h>
#include <stdarg.h>
#include <_pthread.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "logger.h"


int Logger::server_socket = 0;
ScePthread Logger::server_thread = NULL;
ScePthread Logger::client_threads[MAX_CLIENT_CONNECTIONS];
int Logger::client_connections[MAX_CLIENT_CONNECTIONS];
int Logger::client_connection_count = 0;
bool Logger::server_running = false;

std::vector<std::string> Split(const std::string& str, int splitLength) {
	int NumSubstrings = str.length() / splitLength;
	std::vector<std::string> ret;

	for (auto i = 0; i < NumSubstrings; i++) {
		ret.push_back(str.substr(i * splitLength, splitLength));
	}

	if (str.length() % splitLength != 0) {
		ret.push_back(str.substr(splitLength * NumSubstrings));
	}

	return ret;
}

void Logger::clientThread(int client_socket) {
	while (true) {
		char buffer[1024];
		sceNetRecv(client_socket, buffer, 1024, NULL);

		std::istringstream iss(buffer);
		std::vector<std::string> tokens { std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };

		if (tokens[0] == "write") {
			uint32_t address = std::stoi(tokens[1], nullptr, 16);
			uint32_t offset = std::stoi(tokens[2], nullptr, 16);

			uint32_t index = 0;
			for (auto value : Split(tokens[3], 2)) {
				uint8_t byte = std::stoi(value, nullptr, 16);
				*(char*)(address + offset + index) = byte;
				index++;
			}
		}
	}
}

void Logger::serverThread() {
	int port = 4455;

	sockaddr_in server_addr;

	server_addr.sin_len = sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = 0;
	server_addr.sin_port = sceNetHtons(port);

	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	if ((server_socket = sceNetSocket("Logger", AF_INET, SOCK_STREAM, 0)) == 0) {
		return;
	}

	if (sceNetBind(server_socket, (SceNetSockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		return;
	}

	if (sceNetListen(server_socket, 0) < 0) {
		return;
	}

	while (true) {
		int clientSocket;
		if ((clientSocket = sceNetAccept(server_socket, NULL, NULL)) < 0) {
			continue;
		}
		if (client_connection_count < MAX_CLIENT_CONNECTIONS) {
			client_connections[client_connection_count] = clientSocket;
			scePthreadCreate(&client_threads[client_connection_count], NULL, (void* (*)(void*))clientThread, (void *)clientSocket, "Client Connection Thread");
			
			client_connection_count++;
		}
	}
}

void Logger::initialize() {
	if (!server_running) {
		server_running = true;
		scePthreadCreate(&server_thread, NULL, (void* (*)(void*))serverThread, NULL, "Server Thread");
	}
}

void Logger::destory() {
	sceNetSocketClose(server_socket);
	scePthreadCancel(server_thread);
	
	for (int i = 0; i < client_connection_count; i++) {
		sceNetSocketClose(client_connections[i]);
		scePthreadCancel(client_threads[i]);
	}
}

void Logger::log(const char *message, ...) {
	char msg[1000];
	va_list args;
	va_start(args, message);
	vsnprintf(msg, sizeof(msg), message, args);
	va_end(args);

	char whole_msg[1100];
	snprintf(whole_msg, sizeof(whole_msg), "%s", msg);

	for (int i = 0; i < client_connection_count; i++) {
		sceNetSend(client_connections[i], whole_msg, strlen(whole_msg), 0);
	}
}
