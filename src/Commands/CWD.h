#ifndef _CWD_H_
#define _CWD_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class CWD : public FTPCommand
{
public:
    explicit CWD(WiFiClient *const Client, SdFs *const Filesystem) : FTPCommand("CWD", 1, Client, Filesystem) {}

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        FTPPath path = WorkDirectory;
        String line = Line.size() > 1 ? Line[1] : "";

        if (line == "..")
        {
            path.goPathUp();
        }
        else
        {
            if (line != "")
                path.changePath(Line[1]);
        }

        File dir = _Filesystem->open(path.getPath());
        if (dir.isDirectory())
        {
            char name[FTP_DIRNAME_SIZE];
            dir.getName(name, FTP_DIRNAME_SIZE);
            path.goPathUp();
            path.changePath(name);
            WorkDirectory = path;
            SendResponse(250, "CWD command successful.");
        }
        else
        {
            SendResponse(550, "Directory does not exist");
        }
    }
};

#endif
