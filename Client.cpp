
#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

// Data to send to/receive from server
ByteArray data;

int main(void)
{
	// Welcome the user 
	std::cout << "Client" << std::endl;

	// String to store user input
	std::string in = "";

	// Create  and open socket on localhost:3000
	Socket socket("127.0.0.1", 3000);
	socket.Open();

	// Loop continuously until the user enters 'done'
	while (true)
	{
		// Collect user input
		std::cout << "Enter a string to transform, or enter 'done' to exit" << std::endl;
		std::cin >> in;

		// Break loop if user types 'done'
		if (in == "done")
		{
			break;
		}

		// Convert input string to a ByteArray so it can be sent to the server
		ByteArray data(in);

		// Send byte array to server
		socket.Write(data);

		// If the server sends no data, then it has already been terminated and the client can terminate itself
		if (socket.Read(data) == 0)
		{
			std::cout << "Server is dead" << std::endl;
			break;
		}

		// If execution gets to this point, that means the server sent a response.
		// So, convert the received byte array back to a string and print it to the console
		std::string serverResponse = data.ToString();
		std::cout << "Response from server: " << serverResponse << std::endl;
	}

	// If execution reaches this point, it means either the user typed 'done' or the server was killed.
	// So, the socket can be closed and the program can terminate.
	socket.Close();

	return 0;
}
