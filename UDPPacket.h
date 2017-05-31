#pragma once

#include <string>
#include <iostream>
#include <map>
#include <unordered_map>

#include "Call.h"

class UDPPacket
{
public:
	UDPPacket(std::string  ip1, std::string  ip2, unsigned short sport, unsigned short dport, unsigned char* data, int data_length);
	~UDPPacket();

	void Process(long, long, std::map<std::string, Call>& , std::unordered_map<std::string, std::string> &, bool &);

private:
	std::string ip_source;
	std::string  ip_destination;
	unsigned short source_port;
	unsigned short destination_port;
	unsigned short udp_data_length;
	unsigned char* udp_data;



};

