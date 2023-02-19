#ifndef _ESP32_FTP_SERVER_H_
#define _ESP32_FTP_SERVER_H_

#include <list>
#include <Arduino.h>
#include <SdFat.h>

#include "FTPUser.h"
#include "FTPConnection.h"

class FTPServer
{
public:
    FTPServer();
    virtual ~FTPServer();

    void addUser(const String &Username, const String &Password);
    void addUser(const FTPUser &User);
    void addFilesystem(SdFs *const Filesystem);

    bool begin();
    void handle();

    size_t countConnections() const;

private:
    WiFiServer _Server;
    SdFs *_Filesystem;

    std::list<FTPUser> _UserList;
    std::list<std::shared_ptr<FTPConnection>> _Connections;
};

#endif