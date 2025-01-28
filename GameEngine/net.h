#pragma once
#include <SDL3_net\SDL_net.h>
#include "constants.h"
#include "gameError.h"

//-----------------------------------------------------------------------------
//
// NETWORK
//
//-----------------------------------------------------------------------------

namespace netNS
{
    const unsigned short DEFAULT_PORT = 48161;
    const int MIN_PORT = 1024;
    const int BUFFER_LENGTH = 4096;
    const int IP_SIZE = 16;         // size of "nnn.nnn.nnn.nnn"

    // Mode
    const int UNINITIALIZED = 0;
    const int SERVER = 1;
    const int CLIENT = 2;

    // Connection type
    const int UNCONNECTED = -1;
    const int UDP = 0;
    const int TCP = 1;
    const int UNCONNECTED_TCP = 2;
    const int CONNECTED_TCP = 3;

    // Status codes
    enum
    {
        NET_OK = 0,
        NET_ERROR = 1,
        NET_INIT_FAILED = 2,
        NET_INVALID_SOCKET = 3,
        NET_GET_HOST_BY_NAME_FAILED = 4,
        NET_BIND_FAILED = 5,
        NET_CONNECT_FAILED = 6,
        NET_ADDR_IN_USE = 7,
        NET_DOMAIN_NOT_FOUND = 8,
        NET_REMOTE_DISCONNECT = 9,
        NET_UNKNOWN_NETWORK_ERROR = 10,
        NET_ERROR_CODES
    };

    // Network error codes
    static const char* codes[NET_ERROR_CODES] = {
        "No errors reported",
        "General network error: ",
        "Network init failed: ",
        "Invalid socket: ",
        "Get host by name failed: ",
        "Bind failed: ",
        "Connect failed: ",
        "Port already in use: ",
        "Domain not found: ",
        "Remote Disconnect: "
        "Unknown network error: "
    };

    const int PACKETS_PER_SEC = 30;         // Number of packets to send per second
    const float NET_TIME = 1.0f / PACKETS_PER_SEC;          // time between net transmissions
    const int MAX_ERRORS = PACKETS_PER_SEC * 30;            // Packets/Sec * 30 Sec
    const int MAX_COMM_WARNINGS = 10;           // max packets out of sync before time reset

    // Connection response messages, ===== MUST BE SAME SIZE =====
    const int RESPONSE_SIZE = 12;
    const char CLIENT_ID[RESPONSE_SIZE] = "Client v1.0";            // client ID
    const char SERVER_ID[RESPONSE_SIZE] = "Server v1.1";            // server ID
    const char SERVER_FULL[RESPONSE_SIZE] = "Server Full";          // server full
    const int REMOTE_DISCONNECT = 0x2775;
}

class Net
{
private:
    // SDL
    SDLNet_DatagramSocket* datagramSocket;          // UDP\DATAGRAM socket client\server
    SDLNet_StreamSocket* streamSocket;          // TCP\STREAM socket client\server
    SDLNet_Server* serverSocket;
    SDLNet_Address* remoteAddr;         // remote address
    SDLNet_Address* localAddr;          // local address

    // net properties
    char    serverIP[netNS::IP_SIZE];           // IP as string (e.g. "192.168.1.100");
    int     type;
    char    mode;
    bool    netInitialized;
    bool    bound;

    //=============================================================================
    // Initialize network (for class use only)
    // Called by createServer and creatClient
    //   port = Port number.
    //   protocol = UDP or TCP.
    // 
    //   Returns int code on error.
    //=============================================================================
    int initialize(int mode, int port, int protocol);

public:
    Net();
    ~Net();

    //=============================================================================
    // Setup network for use as server
    // May not be configured as Server and Client at the same time. 
    //   port = Port number to listen on.
    //     Port numbers 0-1023 are used for well-known services.
    //     Port numbers 1024-65535 may be freely used.
    //   protocol = UDP or TCP
    // 
    //   Returns NET_OK on success
    //   Returns int code on error.
    //=============================================================================
    int createServer(int port, int protocol);

	//=============================================================================
    // Setup network for use as a Client
    //   *server = IP address of server to connect to as null terminated
    //     string (e.g. "192.168.1.100") or null terminated hostname
    //     (e.g. "www.programming2dgames.com").
    //   port = Port number. Port numbers 0-1023 are used for well-known services.
    //     Port numbers 1024-65535 may be freely used.
    //   protocol = UDP or TCP
    // 
    //   Returns NET_OK on success
    //   Returns int code on error.
    //=============================================================================
    int createClient(char* server, int port, int protocol);

    //=============================================================================
    // Send data
    // 
    //   *data = Data to send
    //   size = Number of bytes to send, 0 if no data sent
    //   *remoteIP = Destination IP address as null terminated char array
    //   port = Destination port number.
    // 
    //   Returns NET_OK on success. Success does not indicate data was sent.
    //   Returns int code on error.
    //=============================================================================
    int sendData(const char* data, int& size, const char* remoteIP);
    int sendData(const char* data, int& size, const char* remoteIP,
        const unsigned short port);

    //=============================================================================
    // Read data
    // 
    //   *data = Buffer for received data.
    //   size = Number of bytes to receive. may be 0.
    //   *senderIP = NULL
    //   port = port number of sender
    // 
    //   Returns NET_OK on success.
    //   Returns int code on error.
    //=============================================================================
    int readData(char* data, int& size, char* senderIP);
    int readData(char* data, int& size, char* senderIP, unsigned short& port);

    //=============================================================================
    // Close socket and free resources
    // 
    //   Socket is closed and buffer memory is released.
    //   Returns int code on error.
    //=============================================================================
    int closeSocket();

    //=============================================================================
    // Get the IP address of this computer as a string
    // 
    //   *localIP = IP address of local computer as null terminated string on success.
    // 
    //   Returns int code on error.
    //=============================================================================
    int getLocalIP(char* localIP);

    //=============================================================================
    // Return mode
    // Valid modes are: UNINITIALIZED, SERVER, CLIENT
    //=============================================================================
    char getMode() const;

    //=============================================================================
    // Returns detailed error message from error code
    //=============================================================================
    std::string getError(int error) const;
};
