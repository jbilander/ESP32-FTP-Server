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

        String argument = Line.size() > 1 ? Line[1] : "";

        if (argument != "" && !WorkDirectory.getPath().equals("/"))
        {
            argument = "/" + argument;
        }

        File dir = _Filesystem->open(WorkDirectory.getPath() + argument);

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
                data_print("drw-r--r-- 2"); // ToDo: 2 here could be altered to show the actual number of subdirs
                /*
                For directories, it's 2 + the number of subdirectories.
                This is because a directory can be referred to either by its name in the parent directory,
                 . in itself, or .. in each subdirectory.
                */
            }
            else
            {
                data_print("-rw-r--r-- 1");
            }

            /*

            String format explained here: http://cr.yp.to/ftp/list/binls.html
            -rw-r--r-- 1 owner group           213 Aug 26 16:31 README
            -rw-r--r-- 1 owner group          1383 Apr 10  1997 ip.c

            */

            data_print(" esp32 default ");

            char buf[14];
            uint64_t fsize = f.size();
            sprintf(buf, "%13d", fsize);
            String filesize;

            if (fsize > 9999999999999)
            {
                filesize = "9999999999999"; // Size too large to fit in 13 digits
            }
            else
            {
                filesize = String(buf);
            }

            uint16_t pdate;
            uint16_t ptime;
            f.getModifyDateTime(&pdate, &ptime);

            uint16_t monthNmbr = (pdate >> 5) & 0XF;
            String monthStr = MonthToStr<String>(monthNmbr);

            uint16_t dayNmbr = pdate & 0X1F;
            String day = dayNmbr > 9 ? String(dayNmbr) : "0" + String(dayNmbr);

            data_println(filesize + " " + monthStr + " " + day + " " + String(FS_YEAR(pdate)) + " " + filename);
            cnt++;
            f.close();
            f = dir.openNextFile();
        }

        CloseDataConnection();
        SendResponse(226, String(cnt) + " matches total");
    }
};

#endif
