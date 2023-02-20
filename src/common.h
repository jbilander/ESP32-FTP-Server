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
	CWD,
	PWD,
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
		this->operator[]("PWD") = PWD;
	};
	~FTPCommandMap() {}
};

#endif
