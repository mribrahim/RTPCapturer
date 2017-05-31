#include "stdafx.h"
#include "RTPContent.h"

#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

RTPContent::RTPContent(long sec,long usec, std::string  ipa, std::string  ipb, char payloadtype, unsigned short  seqnum, unsigned int  tmpstmp, unsigned int  ssrcnum)
{
	tv_sec = sec;
	tv_usec = usec;

	ip_source = ipa;
	ip_destination = ipb;
	payload_type = payloadtype;
	sequencenumber = seqnum;
	timestamp = tmpstmp;
	ssrc = ssrcnum;
}

RTPContent::RTPContent()
{
}


RTPContent::~RTPContent()
{
}

void RTPContent::set_data(std::string input)
{
	data = input;
}

