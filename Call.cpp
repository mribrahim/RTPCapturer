#include "stdafx.h"
#include "Call.h"


Call::Call()
{
	
}

Call::Call(RTPContent rtp, long sec, long usec)
{
	// RTP datanin basina header ekle!!!!!!!!!
	std::string header;
	std::string strdata = rtp.data;
	unsigned int leninbytes = strdata.length();
	unsigned int long timestamp = 0;
	header = int_to_byte_char(leninbytes, "littleendian") + longint_to_byte_char(0, "littleendian");
	strdata = header + strdata;


	ssrc1 = rtp.ssrc;
	callers_data[rtp.ssrc] = strdata;
	tv_sec_start = sec;
	tv_usec_start = usec;
	tv_sec_current = sec;
	tv_usec_current = usec;

	ip_source = rtp.ip_source;
	ip_destination = rtp.ip_destination;
	payload_type = rtp.payload_type;

	numberofframes = 1;
	fark = 0;
}


Call::~Call()
{
}


void Call::add_RTP(RTPContent rtp, long sec,long usec)
{
	std::string strdata = rtp.data;

	if (payload_type == 96)
	{
		numberofframes++;

		if (fark == 0)
			fark = (usec - tv_usec_start) / 100;
		else
			fark = fark + (usec - tv_usec_current) / 100;

		tv_sec_current = sec;
		tv_usec_current = usec;


		// RTP datanin basina header ekle!!!!!!!!!
		std::string header;
		unsigned int leninbytes = strdata.length();
		unsigned int long timestamp = tv_sec_current - tv_sec_start;

		header = int_to_byte_char(leninbytes, "littleendian") + longint_to_byte_char(fark, "littleendian");
		strdata = header + strdata;
	}


	std::map <unsigned int, std::string>::iterator it;

	it = callers_data.find(rtp.ssrc);
	if (it != callers_data.end())
		callers_data[rtp.ssrc] += (strdata);
	else 
	{
		callers_data[rtp.ssrc] = strdata;
		ssrc2 = rtp.ssrc;
	}
}



bool Call::isfinished(long sec)
{
	if(tv_sec_current == 0)
		return false;

	if ((sec - tv_sec_current) > 3)
		return true;

	return false;
}

std::vector<int> Call::decode_data(std::string data)
{
	std::vector<int> decoded_data;
	char* temp = (char*)data.c_str();
	int length = data.length();

	const int size = 80;

	do
	{
		int decoded_data_ptr[size];

		unsigned short decoded_data_length = DecodePacket_G711a(temp, size, decoded_data_ptr);
		decoded_data.insert(decoded_data.end(), decoded_data_ptr, decoded_data_ptr + size);

		temp = temp + size;
		length = length - size;

	} while (length>0);

	return decoded_data;
}

void Call::save_to_file()
{

	if (payload_type == 8)
	{
		std::string str1 = callers_data[ssrc1];
		data_decoded_1 = decode_data(callers_data[ssrc1]);
		data_decoded_2 = decode_data(callers_data[ssrc2]);
		
		std::string temp = directory + ip_source + " to " + ip_destination + " ";

		std::string filename = temp +  std::to_string(ssrc1);

		CreateWavefile(filename + ".wav", data_decoded_1);
		std::cout << filename << " : " << data_decoded_1.capacity() << std::endl;

		std::string filename2 = temp + std::to_string(ssrc2);
		CreateWavefile(filename2 + ".wav", data_decoded_2);
		std::cout << filename2 << " : " << data_decoded_2.capacity() << std::endl;
	}

	else if (payload_type == 96)
	{
		
		std::string xxxx = ip_source;
		std::string data = callers_data[ssrc1];
		
		
		// IVF header -> https://wiki.multimedia.cx/index.php/IVF
		
		std::string convert = "littleendian";
		int time_elapsed = tv_sec_current - tv_sec_start;
		int fark2 = tv_usec_current - tv_usec_start;

		std::string header;

		short header_size = 32;  // (6-7) bytes : length of header in bytes -> 32 
		unsigned int rate = (numberofframes / time_elapsed) ; //  18;
		unsigned int scale = 1;


		header = "DKIF" + shortint_to_byte_char(0, convert) + shortint_to_byte_char(header_size, convert) + "VP80";
		header = header + shortint_to_byte_char(352, convert) + shortint_to_byte_char(288, convert);
		header = header + int_to_byte_char(rate, convert) + int_to_byte_char(scale, convert);
		header = header + int_to_byte_char(numberofframes, convert) + int_to_byte_char(0, convert);

		unsigned char* temp = (unsigned char*)header.c_str();
		unsigned short  lenbyte = char_array_to_short(temp+6,convert);
		unsigned short width = char_array_to_short(temp + 12, "littleendian");
	    rate = char_array_to_int(temp + 16, "littleendian");
		unsigned int framecount = char_array_to_int(temp + 24, "littleendian");

		data = header + data;
		int length = data.length();
		char* input = (char*)data.c_str();

		std::fstream fout;
		fout.open("output1", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
		fout.write(input, length);
		fout.flush();
		fout.close();

		//std::ofstream out2("output2.vp8");
		//int length2 = callers_data[ssrc2].data.length();
		//out2 << callers_data[ssrc2].data;
		//out2.close();

	}


}

void Call::display()
{
	std::cout << ip_source << " <--> " << ip_destination << std::endl;
}


