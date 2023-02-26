#ifndef _XMKD_H_
#define _XMKD_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class XMKD : public FTPCommand
{
public:
    explicit XMKD(WiFiClient *const Client, SdFs *const Filesystem) : FTPCommand("XMKD", 1, Client, Filesystem) {}

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        String filepath = WorkDirectory.getFilePath(Line[1]);
        if (_Filesystem->exists(filepath))
        {
            SendResponse(521, "Can't create \"" + filepath + "\", Directory exists");
            return;
        }
        if (_Filesystem->mkdir(filepath))
        {
            SendResponse(257, "\"" + filepath + "\" created");
        }
        else
        {
            SendResponse(550, "Can't create \"" + filepath + "\"");
        }
    }
};

#endif
