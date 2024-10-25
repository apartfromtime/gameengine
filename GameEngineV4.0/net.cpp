#include "net.h"

//=============================================================================
// Constructor
//=============================================================================
Net::Net()
{
    // SDL
    datagramSocket = NULL;
    streamSocket = NULL;
    serverSocket = NULL;
    remoteAddr = NULL;
    localAddr = NULL;

    // Net
    SDL_memset(serverIP, 0, sizeof(char) * netNS::IP_SIZE);
    type = netNS::UNCONNECTED;
    mode = netNS::UNINITIALIZED;
    netInitialized = false;
    bound = false;
}

//=============================================================================
// Destructor
//=============================================================================
Net::~Net()
{
    closeSocket();          // close connection, release memory
}

//=============================================================================
// Initialize network
//=============================================================================
int Net::initialize(int mode, int port, int protocol)
{
    int status;

    if (netInitialized)
    {
        // close current network and start over
        closeSocket();
    }

    status = SDLNet_Init();         // initiate the use of SDLNet
    if (status != 0)
    {
        status = netNS::NET_INIT_FAILED;
        return status;
    }

    if (mode == netNS::CLIENT)
    {
        remoteAddr = SDLNet_ResolveHostname(serverIP);

        if (remoteAddr != NULL)
        {
            if (SDLNet_WaitUntilResolved(remoteAddr, -1) < 0)
            {
                SDLNet_UnrefAddress(remoteAddr);
                remoteAddr = NULL;
            }
        }
    }

    switch (protocol)
    {
    case netNS::UDP:            // UDP
    {
        // Create UDP socket and bind it to a local interface and port
        datagramSocket = SDLNet_CreateDatagramSocket(remoteAddr, port);
        if (datagramSocket == NULL)
        {
            SDLNet_Quit();
            status = netNS::NET_INVALID_SOCKET;
            return status;
        }
        type = netNS::UDP;
    } break;
    case netNS::TCP:            // TCP
    {
        // Create TCP socket and bind it to a local interface and port
        if (mode == netNS::CLIENT)
        {
            streamSocket = SDLNet_CreateClient(remoteAddr, port);

            if (streamSocket == NULL)
            {
                SDLNet_Quit();
                status = netNS::NET_INVALID_SOCKET;
                return status;
            }
        }
        if (mode == netNS::SERVER)
        {
            serverSocket = SDLNet_CreateServer(NULL, port);

            if (serverSocket == NULL)
            {
                SDLNet_Quit();
                status = netNS::NET_INVALID_SOCKET;
                return status;
            }
        }
        type = netNS::UNCONNECTED_TCP;
    } break;
    default:            // Invalid type
        return (netNS::NET_INIT_FAILED);
    }

    netInitialized = true;

    return netNS::NET_OK;
}

//=============================================================================
// Setup network for use as server
//=============================================================================
int Net::createServer(int port, int protocol)
{
    int status;

    // ----- Initialize network stuff -----
    status = initialize(netNS::SERVER, port, protocol);
    if (status != netNS::NET_OK)
    {
        return status;
    }

    bound = true;
    mode = netNS::SERVER;

    return status;
}

//=============================================================================
// Setup network for use as a Client
//=============================================================================
int Net::createClient(char* server, int port, int protocol)
{
    char localIP[netNS::IP_SIZE * 2] = { 0 };          // IP as string (e.g. "192.168.1.100");
    int status;

    strncpy_s(serverIP, netNS::IP_SIZE, server, netNS::IP_SIZE);

    // ----- Initialize network stuff -----
    status = initialize(netNS::CLIENT, port, protocol);
    if (status != netNS::NET_OK)
    {
        return status;
    }

    // set local IP address
    getLocalIP(localIP);            // get local IP
    localAddr = SDLNet_ResolveHostname(localIP);            // local IP

    if (localAddr != NULL)
    {
        if (SDLNet_WaitUntilResolved(localAddr, -1) < 0)
        {
            SDLNet_UnrefAddress(localAddr);
            localAddr = NULL;
        }
    }

    mode = netNS::CLIENT;

    return status;
}

//=============================================================================
// Send data to remote IP on current port
//=============================================================================
int Net::sendData(const char* data, int& size, const char* remoteIP)
{
    int ret = 0;
    int status;
    int sendSize = size;

    size = 0;           // assume 0 bytes sent, changed if send successful

    if (mode == netNS::SERVER)
    {
        remoteAddr = SDLNet_ResolveHostname(remoteIP);
        status = SDLNet_WaitUntilResolved(remoteAddr, -1);
    }

    if (type == netNS::UDP)
    {
        void* buf = (void*)SDL_malloc(sendSize * sizeof(char));
        SDL_memcpy(buf, data, sendSize * sizeof(char));
        ret = SDLNet_SendDatagram(datagramSocket, remoteAddr, 0, buf,
            sendSize);

        SDL_free(buf);

        if (ret < 0)
        {
            return netNS::NET_ERROR;
        }
    }

    if (mode == netNS::CLIENT && type == netNS::UNCONNECTED_TCP)
    {
        ret = SDLNet_GetConnectionStatus(streamSocket);

        if (ret == 1)
        {
            ret = 0;
            type = netNS::CONNECTED_TCP;
        }
        else
        {
            if (ret == 0)
            {
                return netNS::NET_OK;          // no connection yet
            }
            else
            {
                return netNS::NET_CONNECT_FAILED;
            }
        }
    }

    if (type == netNS::CONNECTED_TCP)
    {
        if (streamSocket != NULL)
        {
            ret = SDLNet_WriteToStreamSocket(streamSocket, data, sendSize);

            if (ret < 0)
            {
                type = netNS::UNCONNECTED_TCP;

                return netNS::NET_ERROR;
            }
        }
    }

    bound = true;
    size = sendSize;

    return netNS::NET_OK;
}

//=============================================================================
// Send data to remoteIP and port
//=============================================================================
int Net::sendData(const char* data, int& size, const char* remoteIP,
    const unsigned short port)
{
    int ret = 0;
    int status;
    int sendSize = size;

    size = 0;           // assume 0 bytes sent, changed if send successful

    if (mode == netNS::SERVER)
    {
        remoteAddr = SDLNet_ResolveHostname(remoteIP);
        status = SDLNet_WaitUntilResolved(remoteAddr, -1);
    }

    if (type == netNS::UDP)
    {
        void* buf = (void*)SDL_malloc(sendSize * sizeof(char));
        SDL_memcpy(buf, data, sendSize * sizeof(char));
        ret = SDLNet_SendDatagram(datagramSocket, remoteAddr, port, buf,
            sendSize);

        SDL_free(buf);

        if (ret < 0)
        {
            return netNS::NET_ERROR;
        }
    }

    if (mode == netNS::CLIENT && type == netNS::UNCONNECTED_TCP)
    {
        ret = SDLNet_GetConnectionStatus(streamSocket);

        if (ret == 1)
        {
            ret = 0;
            type = netNS::CONNECTED_TCP;
        }
        else
        {
            if (ret == 0)
            {
                return netNS::NET_OK;          // no connection yet
            }
            else
            {
                return netNS::NET_CONNECT_FAILED;
            }
        }
    }

    if (type == netNS::CONNECTED_TCP)
    {
        if (streamSocket != 0)
        {
            ret = SDLNet_WriteToStreamSocket(streamSocket, data, sendSize);

            if (ret < 0)
            {
                type = netNS::UNCONNECTED_TCP;

                return netNS::NET_ERROR;
            }
        }
    }

    bound = true;
    size = sendSize;

    return netNS::NET_OK;
}

//=============================================================================
// Read data, return sender's IP
//=============================================================================
int Net::readData(char* data, int& size, char* senderIP)
{
    char* remoteIP = NULL;
    int ret = 0;
    int readSize = size;

    size = 0;           // assume 0 bytes read, changed if read successful
    if (bound == false)         // no receive from unbound socket
    {
        return netNS::NET_OK;
    }

    if (mode == netNS::SERVER && type == netNS::UNCONNECTED_TCP)
    {
        if (serverSocket != NULL)
        {
            SDLNet_StreamSocket* tempSocket = 0;

            ret = SDLNet_AcceptClient(serverSocket, &tempSocket);

            if (ret == 0 && tempSocket == 0)
            {
                return netNS::NET_OK;          // no connection yet
            }

            SDLNet_DestroyStreamSocket(streamSocket);
            streamSocket = tempSocket;          // TCP client connected
            remoteAddr = SDLNet_GetStreamSocketAddress(streamSocket);

            type = netNS::CONNECTED_TCP;
        }
    }

    if (mode == netNS::CLIENT && type == netNS::UNCONNECTED_TCP)
    {
        return netNS::NET_OK;           // no connection yet
    }

    if (datagramSocket != NULL)
    {
        SDLNet_Datagram* dgram = NULL;

        while (((ret = SDLNet_ReceiveDatagram(datagramSocket, &dgram)) == 0) &&
            (dgram != NULL))
        {
            if (mode == netNS::CLIENT)
            {
                if (SDLNet_CompareAddresses(dgram->addr, remoteAddr) != 0)
                {
                    // packet from non-server address.
                    SDLNet_DestroyDatagram(dgram);
                    dgram = NULL;

                    continue;
                }
            }

            remoteAddr = dgram->addr;
            remoteIP = (char*)SDLNet_GetAddressString(remoteAddr);

            if (remoteIP != NULL)
            {
                SDL_memcpy(senderIP, remoteIP, netNS::IP_SIZE);
            }

            if (dgram->buflen < readSize)
            {
                readSize = dgram->buflen;
            }

            SDL_memcpy(data, dgram->buf, readSize);
            size = readSize;

            SDLNet_DestroyDatagram(dgram);
            dgram = NULL;
        }
    }

    if (streamSocket != NULL)
    {
        ret = SDLNet_ReadFromStreamSocket(streamSocket, data, readSize);

        if (ret < 0)
        {
            return netNS::NET_ERROR;
        }

        remoteIP = (char*)SDLNet_GetAddressString(remoteAddr);
        if (remoteIP != NULL)
        {
            SDL_memcpy(senderIP, remoteIP, netNS::IP_SIZE);
        }

        size = ret;
    }

    return netNS::NET_OK;
}

//=============================================================================
// Read data, return sender's IP and port
//=============================================================================
int Net::readData(char* data, int& size, char* senderIP, unsigned short& port)
{
    char* remoteIP = NULL;
    int ret = 0;
    int readSize = size;

    size = 0;           // assume 0 bytes read, changed if read successful
    if (bound == false)         // no receive from unbound socket
    {
        return netNS::NET_OK;
    }

    if (mode == netNS::SERVER && type == netNS::UNCONNECTED_TCP)
    {
        if (serverSocket != NULL)
        {
            SDLNet_StreamSocket* tempSocket = 0;

            ret = SDLNet_AcceptClient(serverSocket, &tempSocket);

            if (ret == 0 && tempSocket == 0)
            {
                return netNS::NET_OK;          // no connection yet
            }

            SDLNet_DestroyStreamSocket(streamSocket);
            streamSocket = tempSocket;          // TCP client connected
            remoteAddr = SDLNet_GetStreamSocketAddress(streamSocket);

            type = netNS::CONNECTED_TCP;
        }
    }

    if (mode == netNS::CLIENT && type == netNS::UNCONNECTED_TCP)
    {
        return netNS::NET_OK;           // no connection yet
    }

    if (datagramSocket != NULL)
    {
        SDLNet_Datagram* dgram = NULL;

        while (((ret = SDLNet_ReceiveDatagram(datagramSocket, &dgram)) == 0) &&
            (dgram != NULL))
        {
            if (mode == netNS::CLIENT)
            {
                if (SDLNet_CompareAddresses(dgram->addr, remoteAddr) != 0)
                {
                    // packet from non-server address.
                    SDLNet_DestroyDatagram(dgram);
                    dgram = NULL;

                    continue;
                }
            }

            remoteAddr = dgram->addr;
            remoteIP = (char*)SDLNet_GetAddressString(remoteAddr);

            if (remoteIP != NULL)
            {
                SDL_memcpy(senderIP, remoteIP, netNS::IP_SIZE);
            }

            if (dgram->buflen < readSize)
            {
                readSize = dgram->buflen;
            }

            SDL_memcpy(data, dgram->buf, readSize);
            size = readSize;
            port = dgram->port;

            SDLNet_DestroyDatagram(dgram);
            dgram = NULL;
        }
    }

    if (streamSocket != NULL)
    {
        ret = SDLNet_ReadFromStreamSocket(streamSocket, data, readSize);

        if (ret < 0)
        {
            return netNS::NET_ERROR;
        }

        remoteIP = (char*)SDLNet_GetAddressString(remoteAddr);
        if (remoteIP != NULL)
        {
            SDL_memcpy(senderIP, remoteIP, netNS::IP_SIZE);
        }

        size = ret;
    }

    return netNS::NET_OK;
}

//=============================================================================
// Close socket and free resources.
//=============================================================================
int Net::closeSocket()
{
    type = netNS::UNCONNECTED;
    bound = false;
    netInitialized = false;

    SDLNet_DestroyDatagramSocket(datagramSocket);
    SDLNet_DestroyStreamSocket(streamSocket);
    SDLNet_DestroyServer(serverSocket);
    SDLNet_Quit();

    return netNS::NET_OK;
}

#ifndef _WIN32
#include <unistd.h>
#else
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#endif

//=============================================================================
// Get the IP address of this computer as a string
//=============================================================================
int Net::getLocalIP(char* localIP)
{
    char hostName[netNS::IP_SIZE * 4] = { 0 };
    SDLNet_Address* host;
    const char* result = NULL;
    int status;

    if (localIP == NULL)
    {
        return netNS::NET_ERROR;
    }

    gethostname(hostName, (netNS::IP_SIZE * 4));

    host = SDLNet_ResolveHostname(hostName);

    if (host != NULL)
    {
        status = SDLNet_WaitUntilResolved(host, -1);
        if (status < 0)
        {
            SDLNet_UnrefAddress(host);
            host = NULL;
        }
    }

    result = SDLNet_GetAddressString(host);

    if (result != NULL)
    {
        SDL_memcpy(localIP, result, SDL_strlen(result));
    }

    return netNS::NET_OK;
}

//=============================================================================
// Return mode
//=============================================================================
char Net::getMode()
{
    return mode;
}

//=============================================================================
// Returns detailed error message from two part error code
//=============================================================================
std::string Net::getError(int error)
{
    std::string errorStr;

    if (error < 0 || error > netNS::NET_ERROR_CODES)
    {
        error = netNS::NET_UNKNOWN_NETWORK_ERROR;
    }

    errorStr += mode == netNS::CLIENT ? "CLIENT" : "SERVER";
    errorStr += ':';
    errorStr += ' ';
    errorStr += netNS::codes[error];
    errorStr += SDL_GetError();

    return errorStr;
}
