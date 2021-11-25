#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>

using namespace Sync;

// This thread handles a single connection to a client
class ClientThread : public Thread
{
private:
    Socket& socket; // Reference to the socket that the client is connected to
    ByteArray data; // Data sent to/received from the client

public:
    // Constructor: Sets the thread's socket reference to the one passed as an argument
    ClientThread(Socket& socket) : socket(socket)
    {}

    ~ClientThread()
    {}

    Socket& getSocket()
    {
        return socket;
    }

    virtual long ThreadMain()
    {
        // Loop continuously to read new data from the client
        while (true)
        {
            // Try-catch block will catch the exception generated when the client closes the connection,
            // allowing the program to exit properly instead of crashing
            try
            {
                // Receive data from client (as a byte array)
                socket.Read(data);

                // Convert the byte array to a string
                std::string dataStr = data.ToString();

                // Transform string: Add spaces between characters
                // ex. "Test" becomes "T e s t"
                
                
                // Create empty string for output
                std::string toSend = "";

                // If the received string is empty, send an error message and don't transform it
                if (dataStr == "")
                {
                    toSend = "Empty string received, no processing done.";
                }
                else {
                    // For every character in the input string other than the last one,
                    // append that character plus a space to the output string
                    for (int i = 0; i < dataStr.length() - 1; i++)
                    {
                        toSend.append(1, dataStr[i]);
                        toSend.append(1, ' ');
                    }
                    // Add the last character to the output string
                    toSend += (dataStr[dataStr.length() - 1]);
                }
                

                // Convert the transformed string back to a byte array to send
                data = ByteArray(toSend);

                // Send the new byte array to the client
                socket.Write(data);
                
            }
            catch(...)
            {
                // Print a message to the console when the connection is terminated
                std::cout << "Connection closed" << std::endl;
            }
            
        }

        return 1;
    }
};

// This thread handles the server operations
class ServerThread : public Thread
{
private:
    SocketServer& server;
    bool end = false;
public:
    // Constructor: Sets the thread's socket reference to the one passed as an argument 
    ServerThread(SocketServer& server) : server(server)
    {}

    ~ServerThread()
    {
        // Set end flag to true so the thread will terminate itself
        std::cout << "Terminating server thread" << std::endl;
        end = true;
    }

    virtual long ThreadMain()
    {
        while (!end)
        {
            // Try-catch to handle connection errors without crashing
            try
            {
                // Wait for a client socket connection
                Socket* newConnection = new Socket(server.Accept());

                // Create a reference to the new client socket connection,
                // then pass this reference to a new client thread
                Socket& socketReference = *newConnection;
                ClientThread* clientThread = new ClientThread(socketReference);
            }
            catch(std::exception e)
            {
                // When a connection error occurs, if the thread is set to terminate, then just end the program.
                // Otherwise, display an error message and return to the beginning of the loop to accept new connections.
                if (end)
                {
                    std::cout << "Exiting" << std::endl;
                    return 0;
                }
                std::cout << "Error establishing connection with client" << std::endl;
                std::cout << e.what();
            }
            
            
        }
        
	
	return 1;
    }
};


int main(void)
{
    std::cout << "Server - Press enter to terminate" << std::endl;
    std::cout.flush();  // Clear output buffer
	
    // Create server
    SocketServer server(3000);    

    // Create a thread for server operations
    ServerThread serverThread(server);
	
    // This will wait for input to shutdown the server
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();

    // Shut down and clean up the server
    server.Shutdown();

}
