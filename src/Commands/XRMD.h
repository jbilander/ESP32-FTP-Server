#ifndef _XRMD_H_
#define _XRMD_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

//Duplicate of the RMD command. It's implemented because some clients use this, e.g. MS built-in CLI ftp client.
class XRMD : public FTPCommand
{
public:
    explicit XRMD(WiFiClient *const Client, SdFs *const Filesystem) : FTPCommand("XRMD", 1, Client, Filesystem) {}

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        String filepath = WorkDirectory.getFilePath(Line[1]);
        if (!_Filesystem->exists(filepath))
        {
            SendResponse(550, "Folder " + filepath + " not found");
            return;
        }
        if (_Filesystem->rmdir(filepath))
        {
            SendResponse(250, " Deleted \"" + filepath + "\"");
        }
        else
        {
            SendResponse(450, "Can't delete \"" + filepath + "\"");
        }
    }
};

#endif
