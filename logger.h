#pragma once

#include <vector>

#define MAX_CLIENT_CONNECTIONS 10

class Logger {
private:
	Logger() {}
	static void serverThread();
	static void clientThread(int client_socket);

	static int server_socket;
	static ScePthread server_thread;
	static ScePthread client_threads[];
	static int client_connections[MAX_CLIENT_CONNECTIONS];
	static int client_connection_count;
	static bool server_running;

public:
	static void initialize();
	static void log(const char *message, ...);
	static void destory();
};
