#include "ESP32-FTP-Server.h"
#include "common.h"
#include "FTPCommand.h"
#include "Commands/CDUP.h"
#include "Commands/CWD.h"
#include "Commands/DELE.h"
#include "Commands/LIST.h"
#include "Commands/MFMT.h"
#include "Commands/MKD.h"
#include "Commands/MLSD.h"
#include "Commands/NLST.h"
#include "Commands/PORT.h"
#include "Commands/PWD.h"
#include "Commands/RETR.h"
#include "Commands/RMD.h"
#include "Commands/RNFR_RNTO.h"
#include "Commands/STOR.h"
#include "Commands/TYPE.h"
#include "Commands/XMKD.h"
#include "Commands/XRMD.h"

FTPConnection::FTPConnection(const WiFiClient &Client, std::list<FTPUser> &UserList, SdFs *const Filesystem)
    : _ClientState(Idle), _Client(Client), _Filesystem(Filesystem), _UserList(UserList), _AuthUsername("")
{

    std::shared_ptr<FTPCommandTransfer> retr = std::shared_ptr<FTPCommandTransfer>(new class RETR(&_Client, _Filesystem, &_DataAddress, &_DataPort));
    std::shared_ptr<FTPCommandTransfer> stor = std::shared_ptr<FTPCommandTransfer>(new class STOR(&_Client, _Filesystem, &_DataAddress, &_DataPort));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class CDUP(&_Client)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class CWD(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class DELE(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class LIST(&_Client, _Filesystem, &_DataAddress, &_DataPort)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class MFMT(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class MKD(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class MLSD(&_Client, _Filesystem, &_DataAddress, &_DataPort)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class NLST(&_Client, _Filesystem, &_DataAddress, &_DataPort)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class PORT(&_Client, &_DataAddress, &_DataPort)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class PWD(&_Client)));
    _FTPCommands.push_back(retr);
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class RMD(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class RNFR_RNTO(&_Client, _Filesystem)));
    _FTPCommands.push_back(stor);
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class TYPE(&_Client)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class XMKD(&_Client, _Filesystem)));
    _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new class XRMD(&_Client, _Filesystem)));
    _FTPCommandsTransfer.push_back(retr);
    _FTPCommandsTransfer.push_back(stor);

    Serial.print("New Connection from ");
    Serial.print(_Client.remoteIP());
    Serial.print(":");
    Serial.println(_Client.remotePort());

    sendResponse(220, "--- Welcome to FTP Server for ESP32 ---");
}

FTPConnection::~FTPConnection()
{
    Serial.println("Connection closed!");
}

void FTPConnection::sendResponse(int Code, String Text)
{
    _Client.println(String(Code) + " " + Text);
}

bool FTPConnection::readUntilLineEnd()
{
    while (_Client.available())
    {
        char c = _Client.read();
        if (c == '\n')
        {
            _LineSplit = Split<std::vector<String>>(_Line, ' ');
            return true;
        }
        if (c >= 32 && c < 127)
        {
            _Line += c;
        }
    }
    return false;
}

bool FTPConnection::handle()
{
    if (!_Client.connected())
    {
        return false;
    }
    for (std::shared_ptr<FTPCommandTransfer> cmd : _FTPCommandsTransfer)
    {
        if (cmd->transferInProgress())
        {
            cmd->workOnData();
        }
    }
    if (!readUntilLineEnd())
    {
        return true;
    }

    String command = _LineSplit[0];
    FTPCommandEnum c = FTPCommandMap()[command];

    switch (c)
    {
    case NONE:
        break;
    case SYST: // Return system type.
        sendResponse(215, "UNIX Type: L8");
        _Line = "";
        return true;

    case OPTS: // Select options for a feature (for example OPTS UTF8 ON).
        sendResponse(502, "Command not implemented.");
        _Line = "";
        return true;

    case NOOP: // No operation (dummy packet; used mostly on keepalives).
        sendResponse(200, "NOOP command successful.");
        _Line = "";
        return true;

    case FEAT: // Get the feature list implemented by the server.
        _Client.println("211-Features:");
        _Client.println("MLSD");
        _Client.println("MFMT");
        sendResponse(211, "End");
        _Line = "";
        return true;

    case QUIT: // Disconnect.
        sendResponse(221, "Goodbye.");
        _Client.stop();
        _Line = "";
        return true;

    case ABOR: // Abort an active file transfer.
        for (std::shared_ptr<FTPCommandTransfer> cmd : _FTPCommandsTransfer)
        {
            cmd->abort();
        }
        sendResponse(226, "Data connection closed.");
        _Line = "";
        return true;

    default:
        break;
    }

    switch (_ClientState)
    {
    case Idle:
        if (c == USER)
        {
            c_USER();
            _Line = "";
            return true;
        }
        break;

    case UsernamePass:
        if (c == PASS)
        {
            c_PASS();
            _Line = "";
            return true;
        }
        break;

    case AuthPass:
    {
        std::vector<std::shared_ptr<FTPCommand>>::const_iterator it;

        if (c == RN)
            break;

        for (it = _FTPCommands.begin(); it != _FTPCommands.end(); it++)
        {
            FTPCommandEnum cmd = it->get()->getCommandEnum();

            if (c == cmd || (cmd == RN && (c == RNFR || c == RNTO)))
            {
                (*it)->run(_WorkDirectory, _LineSplit);
                _Line = "";
                return true;
            }
        };

        break;
    }

    default:
        break;
    }

    sendResponse(500, "Unknown command.");
    _Line = "";
    return true;
}

bool FTPConnection::connected()
{
    return _Client.connected();
}

void FTPConnection::c_USER()
{
    String username = _LineSplit.size() > 1 ? _LineSplit[1] : "";

    for (std::list<FTPUser>::const_iterator it = _UserList.begin(); it != _UserList.end(); it++)
    {
        if (username == it->Username)
        {
            _AuthUsername = username;
            _ClientState = UsernamePass;
            sendResponse(331, "OK. Password required.");
            return;
        }
    }
    sendResponse(530, "User not found.");
}

void FTPConnection::c_PASS()
{
    String password = _LineSplit.size() > 1 ? _LineSplit[1] : "";

    for (std::list<FTPUser>::const_iterator it = _UserList.begin(); it != _UserList.end(); it++)
    {
        if (_AuthUsername == it->Username && password == it->Password)
        {
            _ClientState = AuthPass;
            sendResponse(230, "OK.");
            return;
        }
    };
    sendResponse(430, "Invalid username or password.");
}