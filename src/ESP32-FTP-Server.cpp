#include "ESP32-FTP-Server.h"

FTPServer::FTPServer() : _Server(FTP_CTRL_PORT, MAX_CLIENTS)
{
}

FTPServer::~FTPServer()
{
}

void FTPServer::addUser(const String &Username, const String &Password)
{
    FTPUser user(Username, Password);
    _UserList.push_back(user);
}

void FTPServer::addUser(const FTPUser &User)
{
    _UserList.push_back(User);
}

void FTPServer::addFilesystem(SdFs *const Filesystem)
{
    _Filesystem = Filesystem;
}

bool FTPServer::begin()
{
    _Server.begin();
    return true;
}

bool isNotConnected(const std::shared_ptr<FTPConnection> &con)
{
    return !con->connected();
}

void FTPServer::handle()
{
    if (_Server.hasClient())
    {
        std::shared_ptr<FTPConnection> connection = std::shared_ptr<FTPConnection>(
            new FTPConnection(_Server.available(), _UserList, _Filesystem));
        _Connections.push_back(connection);
    }
    for (std::shared_ptr<FTPConnection> con : _Connections)
    {
        con->handle();
    }
    _Connections.remove_if(isNotConnected);
}

size_t FTPServer::countConnections() const
{
    return _Connections.size();
}