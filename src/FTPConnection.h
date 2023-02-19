#ifndef _FTP_CONNECTION_H_
#define _FTP_CONNECTION_H_

#include <vector>
#include <WiFiServer.h>
#include <SdFat.h>
#include "FTPPath.h"
#include "FTPCommand.h"

#define MAX_CLIENTS 1
#define FTP_CTRL_PORT 21
#define FTP_USER_TIME_OUT 5
#define FTP_CMD_SIZE 255 + 8     // max size of a command
#define FTP_DIRNAME_SIZE 255 + 8 // max size of a directory name
#define FTP_FILENAME_SIZE 255    // max size of a file name

class FTPConnection
{
public:
    FTPConnection(const WiFiClient &Client, std::list<FTPUser> &UserList, SdFs *const Filesystem);
    virtual ~FTPConnection();

    bool readUntilLineEnd();
    bool handle();
    bool connected();
    void sendResponse(int Code, String Text);

private:
    enum ClientState
    {
        Idle,
        UsernamePass,
        AuthPass,
    };

    void c_USER();
    void c_PASS();

    ClientState _ClientState;
    WiFiClient _Client;
    SdFs *const _Filesystem;
    String _FilePath;

    String _Line;
    std::vector<String> _LineSplit;
    std::list<FTPUser> _UserList;
    String _AuthUsername;

    IPAddress _DataAddress;
    int _DataPort;

    FTPPath _WorkDirectory;

    std::vector<std::shared_ptr<FTPCommand>> _FTPCommands;
    std::vector<std::shared_ptr<FTPCommandTransfer>> _FTPCommandsTransfer;
};

#endif