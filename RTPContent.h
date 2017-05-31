#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "commonfunction.h"

class RTPContent
{
public:
	RTPContent(long ,long, std::string, std::string, char, unsigned short, unsigned int, unsigned int);
	RTPContent();
	~RTPContent();

	void set_data(std::string);

	std::string  ip_source;
	std::string  ip_destination;
	char payload_type;
	unsigned short  sequencenumber;
	unsigned int  timestamp;
	unsigned int  ssrc;


	long tv_sec;
	long tv_usec;


	std::string data;

};

