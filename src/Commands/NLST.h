#ifndef _NLST_H_
#define _NLST_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class NLST : public FTPCommand
{
public:
    explicit NLST(WiFiClient *const Client, SdFs *const Filesystem, IPAddress *DataAddress, int *DataPort)
        : FTPCommand("NLST", 1, Client, Filesystem, DataAddress, DataPort)
    {
    }

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        if (!ConnectDataConnection())
        {
            return;
        }

        String argument = Line.size() > 1 ? Line[1] : "";

        if (argument != "" && !WorkDirectory.getPath().equals("/"))
        {
            argument = "/" + argument;
        }

        File dir = _Filesystem->open(WorkDirectory.getPath() + argument);

        if (!dir || !dir.isDirectory())
        {
            CloseDataConnection();
            SendResponse(550, "Can't open directory " + WorkDirectory.getPath() + argument);
            return;
        }

        int cnt = 0;
        File f = dir.openNextFile();
        while (f)
        {
            if (f.isFile())
            {
                char name[FTP_DIRNAME_SIZE];
                f.getName(name, FTP_DIRNAME_SIZE);
                String filename = String(name);
                filename.remove(0, filename.lastIndexOf('/') + 1);
                data_println(filename);
                cnt++;
            }

            f.close();
            f = dir.openNextFile();
        }
        CloseDataConnection();
        SendResponse(226, String(cnt) + " matches total");
    }
};

#endif
