#ifndef _MLSD_H_
#define _MLSD_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class MLSD : public FTPCommand
{
public:
    explicit MLSD(WiFiClient *const Client, SdFs *const Filesystem, IPAddress *DataAddress, int *DataPort)
        : FTPCommand("MLSD", 1, Client, Filesystem, DataAddress, DataPort)
    {
    }

    void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override
    {
        if (!ConnectDataConnection())
        {
            return;
        }

        File dir = _Filesystem->open(WorkDirectory.getPath());

        if (!dir || !dir.isDirectory())
        {
            CloseDataConnection();
            SendResponse(550, "Can't open directory " + WorkDirectory.getPath());
            return;
        }

        /*
        MLST type*;size*;modify*;UNIX.mode*;UNIX.owner*;UNIX.group*;
        ------------------------------------------------------------
        type=cdir;size=0;modify=20050308020346;UNIX.mode=0644;UNIX.owner=esp32;UNIX.group=default; tmp
        type=pdir;size=0;modify=20050308020346;UNIX.mode=0644;UNIX.owner=esp32;UNIX.group=default; ..
        type=dir;size=0;modify=20230308020346;UNIX.mode=0644;UNIX.owner=esp32;UNIX.group=default; myfolder
        type=file;size=4879403;modify=20040412054240;UNIX.mode=0644;UNIX.owner=esp32;UNIX.group=default; 01.mp3
        */

        File f = dir.openNextFile();

        while (f)
        {
            data_print("type=");

            if (f.isDirectory())
                data_print("dir;");
            else
            {
                char buf[22];
                sprintf(buf, "%llu;", f.size());
                data_print("file;size=" + String(buf));
            }

            uint16_t pdate;
            uint16_t ptime;
            f.getModifyDateTime(&pdate, &ptime);
            char buf[23];
            sprintf(buf, "modify=%4d%02d%02d%02d%02d%02d;",
                    FS_YEAR(pdate),
                    FS_MONTH(pdate),
                    FS_DAY(pdate),
                    FS_HOUR(ptime),
                    FS_MINUTE(ptime),
                    FS_SECOND(ptime));

            data_print(String(buf));

            char name[FTP_DIRNAME_SIZE];
            f.getName(name, FTP_DIRNAME_SIZE);
            String filename = String(name);
            filename.remove(0, filename.lastIndexOf('/') + 1);
            data_println("UNIX.mode=0644;UNIX.owner=esp32;UNIX.group=default; " + filename);

            f.close();
            f = dir.openNextFile();
        }

        CloseDataConnection();
        SendResponse(226, "MLSD completed");
    }
};

#endif
