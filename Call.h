#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "commonfunction.h"
#include "RTPContent.h"

class Call
{
public:
	Call();
	Call(RTPContent);
	~Call();

	void add_RTP(RTPContent);
	bool isfinished(long);
	void save_to_file();
	void display();

private:
	std::string directory = "wav_files/";
	std::string  ip_source;
	std::string  ip_destination;
	char payload_type;
	unsigned int  ssrc1;
	unsigned int  ssrc2;
	unsigned int  ssrc1_vp8;
	unsigned int  ssrc2_vp8;

	void Call::save_VP8(std::string filename,std::string data, unsigned int framecount);

	unsigned int numberofframes;
	long tv_sec_start;
	long tv_usec_start;
	long tv_sec_current;
	long tv_usec_current;

	unsigned long fark;

	std::vector<int> data_decoded_1;
	std::vector<int> data_decoded_2;

	std::map<unsigned int, std::string> callers_data;
	std::vector<int> decode_data(std::string);

};

