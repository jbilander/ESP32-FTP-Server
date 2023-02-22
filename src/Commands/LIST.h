#ifndef _LIST_H_
#define _LIST_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class LIST : public FTPCommand
{
public:
    explicit LIST(WiFiClient *const Client, SdFs *const Filesystem, IPAddress *DataAddress, int *DataPort)
        : FTPCommand("LIST", 1, Client, Filesystem, DataAddress, DataPort)
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

        int cnt = 0;
        File f = dir.openNextFile();

        while (f)
        {
            char name[FTP_DIRNAME_SIZE];
            f.getName(name, FTP_DIRNAME_SIZE);
            String filename = String(name);
            filename.remove(0, filename.lastIndexOf('/') + 1);

            if (f.isDirectory())
            {
                data_print("drwxr-xr-x");
            }
            else
            {
                data_print("-rw-r--r--");
            }

            /*

            String format explained here: http://cr.yp.to/ftp/list/binls.html
            -rw-r--r-- 1 owner group           213 Aug 26 16:31 README
            -rw-r--r-- 1 owner group          1383 Apr 10  1997 ip.c

            */

            data_print(" 1 esp32 esp32 ");
            char buf[50];
            sprintf(buf, "%llu", f.size());
            String filesize = String(buf);

            int fill_cnt = 13 - filesize.length();
            for (int i = 0; i < fill_cnt; i++)
            {
                data_print(" ");
            }

            uint16_t pdate;
            uint16_t ptime;
            f.getModifyDateTime(&pdate, &ptime);
            uint16_t monthNmbr = (pdate >> 5) & 0XF;
            String monthStr = MonthToStr<String>(monthNmbr);

            data_println(filesize + " " + monthStr + " " + String(FS_DAY(pdate)) + " " + String(FS_YEAR(pdate)) + " " + filename);
            cnt++;
            f.close();
            f = dir.openNextFile();
        }

        CloseDataConnection();
        SendResponse(226, String(cnt) + " matches total");
    }
};

#endif
