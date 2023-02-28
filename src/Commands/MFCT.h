#ifndef _MFCT_H_
#define _MFCT_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class MFCT : public FTPCommand
{
public:
    explicit MFCT(WiFiClient *const Client, SdFs *const Filesystem) : FTPCommand("MFCT", 1, Client, Filesystem) {}

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        File f = _Filesystem->open(WorkDirectory.getFilePath(Line[2]));

        if (f.isFile() || f.isDir())
        {
            uint16_t YYYY = atoi(Line[1].substring(0, 4).c_str());
            uint8_t MM = atoi(Line[1].substring(4, 6).c_str());
            uint8_t DD = atoi(Line[1].substring(6, 8).c_str());
            uint8_t HH = atoi(Line[1].substring(8, 10).c_str());
            uint8_t MI = atoi(Line[1].substring(10, 12).c_str());
            uint8_t SS = atoi(Line[1].substring(12, 14).c_str());

            f.timestamp(T_CREATE, YYYY, MM, DD, HH, MI, SS);
            f.close();
            SendResponse(253, "Date/time changed okay");
            return;
        }

        SendResponse(550, "File or Folder \"" + Line[2] + "\" not found");
        return;
    }
};

#endif
