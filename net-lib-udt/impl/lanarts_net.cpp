/*
 * lanarts_net.cpp:
 *  Fundamental routines for the lanarts_net library
 */

#include <udt/udt.h>

#include "../lanarts_net.h"

#include "ClientConnection.h"
#include "ServerConnection.h"

bool lanarts_net_init(bool print_error) {
	return UDT::startup() == 0;
}

// NB: does not abort
void lanarts_net_quit() {
	UDT::cleanup();
}

/**
 * Create a server at 'port'.
 * Note that you must call initialize_connection().
 */
NetConnection* create_server_connection(int port) {
	return new ServerConnection(port);
}

/**
 * Create a client that will connect to a server with 'host' via 'port'.
 * Note that you must call initialize_connection().
 */
NetConnection* create_client_connection(const char* host, int port) {
	return new ClientConnection(host, port);
}
