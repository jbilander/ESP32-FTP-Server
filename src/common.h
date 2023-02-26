#ifndef _COMMON_H_
#define _COMMON_H_

#include <Arduino.h>
#include <list>
#include <map>
#include <vector>

template <typename T>
T Split(String str, char parser)
{
    T str_array;
    int last_idx = 0;
    int next_idx = str.indexOf(parser, last_idx);
    do
    {
        str_array.push_back(str.substring(last_idx, next_idx));
        last_idx = next_idx + 1;
        next_idx = str.indexOf(parser, last_idx);
        if (next_idx == -1 && last_idx != 0)
        {
            str_array.push_back(str.substring(last_idx, str.length()));
        }
    } while (next_idx != -1);
    return str_array;
}

enum FTPCommandEnum
{
    NONE,
    SYST,
    OPTS,
    NOOP,
    FEAT,
    QUIT,
    ABOR,
    USER,
    PASS,
    CDUP,
    DELE,
    CWD,
    LIST,
    MKD,
    MLSD,
    NLST,
    PORT,
    PWD,
    RMD,
    RN,
    RNFR,
    RNTO,
    TYPE,
    XMKD,
    XRMD,
};

struct FTPCommandMap : public std::map<String, FTPCommandEnum>
{
    FTPCommandMap()
    {
        this->operator[]("NONE") = NONE;
        this->operator[]("SYST") = SYST;
        this->operator[]("OPTS") = OPTS;
        this->operator[]("NOOP") = NOOP;
        this->operator[]("FEAT") = FEAT;
        this->operator[]("QUIT") = QUIT;
        this->operator[]("ABOR") = ABOR;
        this->operator[]("USER") = USER;
        this->operator[]("PASS") = PASS;
        this->operator[]("CDUP") = CDUP;
        this->operator[]("CWD") = CWD;
        this->operator[]("DELE") = DELE;
        this->operator[]("LIST") = LIST;
        this->operator[]("MKD") = MKD;
        this->operator[]("MLSD") = MLSD;
        this->operator[]("NLST") = NLST;
        this->operator[]("PORT") = PORT;
        this->operator[]("PWD") = PWD;
        this->operator[]("RMD") = RMD;
        this->operator[]("RN") = RN;
        this->operator[]("RNFR") = RNFR;
        this->operator[]("RNTO") = RNTO;
        this->operator[]("TYPE") = TYPE;
        this->operator[]("XMKD") = XMKD;
        this->operator[]("XRMD") = XRMD;
    };
    ~FTPCommandMap() {}
};

template <typename T>
T MonthToStr(uint16_t m)
{
    T month;

    switch (m)
    {
    case 1:
        month = "JAN";
        break;
    case 2:
        month = "FEB";
        break;
    case 3:
        month = "MAR";
        break;
    case 4:
        month = "APR";
        break;
    case 5:
        month = "MAY";
        break;
    case 6:
        month = "JUN";
        break;
    case 7:
        month = "JUL";
        break;
    case 8:
        month = "AUG";
        break;
    case 9:
        month = "SEP";
        break;
    case 10:
        month = "OCT";
        break;
    case 11:
        month = "NOV";
        break;
    case 12:
        month = "DEC";
        break;
    default:
        month = "ERR";
        break;
    }

    return month;
};

#endif
