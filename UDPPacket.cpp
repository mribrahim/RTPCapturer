
#include "UDPPacket.h"
#include <string> 

UDPPacket::UDPPacket(std::string ip1 , std::string ip2, unsigned short sport, unsigned short dport, unsigned char* data, int data_length)
{
	ip_source = ip1;
	ip_destination = ip2;
	source_port = sport; 
	destination_port = dport;
	udp_data = data;
	udp_data_length = data_length;
}


UDPPacket::~UDPPacket()
{
}

void UDPPacket::Process( long tv_sec, long tv_usec, std::map<std::string, Call> &call_list, std::unordered_map<std::string, std::string> &fragmented_rtps, bool &vp8payloadheader)
{

	unsigned char payload_type = udp_data[1] & 127; // get first 7 bit
													//std::cout << " RTP packet-> payload  G.711 " << std::endl;
	char rtp_markerbit = (udp_data[1] & 128) >> 7;
	unsigned short  sequencenumber = (udp_data[2] << 8) | (udp_data[3]);
	//char *cstr = (char*)udp_data.c_str();
	unsigned int  timestamp = char_array_to_int(udp_data + 4);
	unsigned int  ssrc = char_array_to_int(udp_data + 8);

	// Process RTP packets 
	if ( payload_type == 8 && udp_data_length == 172) // 172 = 160 (G711- 20ms) + 12 byte RTP header )
	{
		
		//std::cout << " rtp_markerbit  : " << (int)rtp_markerbit << std::endl;
		//std::cout << " payload_type  : " << (int)payload_type << std::endl;
		//std::cout << " sequencenumber  : " << sequencenumber << std::endl;
		//std::cout << " timestamp  : " << timestamp << std::endl;
		//std::cout << " ssrc  : " << ssrc << std::endl;

		RTPContent rtp(tv_sec, tv_usec, ip_source, ip_destination, payload_type, sequencenumber, timestamp, ssrc);
		
		unsigned char* rtp_data = udp_data + 12;  // skip RTP header
		unsigned int rtp_data_length = udp_data_length - 12;
		std::string rtp_data_str((char*)rtp_data, rtp_data_length);

		rtp.set_data(rtp_data_str);

		// check call list to find call
		std::map <std::string, Call>::iterator it;
		std::string key = ip_source + ":" + std::to_string(source_port);

		// ilk olarak kaynak ip-porta gore ara
		it = call_list.find(key);
		if (it != call_list.end())
			call_list[key].add_RTP(rtp);
		else 
		{
			// kaynak ip port icin rtp bulunumazsa hedef ip-port icin ara
			key = ip_destination + ":" + std::to_string(destination_port);
			it = call_list.find(key);
			if (it != call_list.end())
				call_list[key].add_RTP(rtp);
			else
			{
				// Create new CALL
				Call c1(rtp);
				call_list[key] = c1;
			}				
		}

	}

	else if (payload_type == 96)
		{
			RTPContent rtp(tv_sec, tv_usec, ip_source, ip_destination, payload_type, sequencenumber, timestamp, ssrc);

			unsigned char* rtp_data = udp_data + 12;  // skip RTP header
			unsigned int rtp_data_length = udp_data_length - 12;
			std::string rtp_data_str((char*)rtp_data, rtp_data_length);

			uint8_t Xbit = (rtp_data[0] & 0x80) >> 7;
			uint8_t Sbit = (rtp_data[0] & 0x10) >> 4;
			char PartID = (rtp_data[0] & 15);
			unsigned short int extended_pic_ID = rtp_data[2];

			// skip payload descriptor!!!!
			rtp_data += 4;
			rtp_data_length -= 4;


			if (vp8payloadheader)
			{
				char keyframe = (rtp_data[0] & 1);
				int i = 3;
				if (rtp_data[i] == 0x9d && rtp_data[i + 1] == 0x01 && rtp_data[i + 2] == 0x2a)
				{
						unsigned short  width = (rtp_data[i + 4] << 8) | (rtp_data[i + 3]);
						unsigned short  height = (rtp_data[i + 6] << 8) | (rtp_data[i + 5]);
						std::cout << " width  : " << width << std::endl;
						std::cout << " height  : " << height << std::endl;
						std::cout << " Sbit  : " << (int)Sbit << std::endl;
						std::cout << " PartID  : " << (int)PartID << std::endl;
						std::cout << " extended_pic_ID  : " << extended_pic_ID << std::endl << std::endl;

						// skip keyframe header
						//rtp_data += 6;
						//rtp_data_length -= 6;
				}
			}


			//VP8 payload header
			if (Sbit == 1 && PartID == 0) // no payload header exist next frames
				vp8payloadheader = false;



			std::string key = ip_source + ":" + std::to_string(source_port);

			std::unordered_map<std::string, std::string>::iterator it;
			std::string combined_data = "";

			if (rtp_markerbit == 0)
			{
				std::string str_data((char*)rtp_data, rtp_data_length);
				it = fragmented_rtps.find(key);

				if (it != fragmented_rtps.end())
				{
					it->second = it->second + str_data;
				}
				else
					fragmented_rtps.insert(std::pair< std::string, std::string>(key, str_data));

				return;
			}
			else if (rtp_markerbit == 1)
			{
				// Check if fragmentation is ended?
				bool concat_packets = false;
				it = fragmented_rtps.find(key);

				if (it != fragmented_rtps.end())
					concat_packets = true;

				// If  fragmentation ended, concat packet contents
				if (concat_packets)
				{
					std::string str_data((char*)rtp_data, rtp_data_length);

					it = fragmented_rtps.find(key);
					if (it != fragmented_rtps.end())
						combined_data = it->second;

					combined_data += str_data;
					rtp_data_length = combined_data.length();


					rtp_data_str = combined_data;


					fragmented_rtps.erase(key);
							vp8payloadheader = true;

				}

			} // else if (rtp_markerbit == 1)


			 rtp.set_data(rtp_data_str);

			// check call list to find call
			std::map <std::string, Call>::iterator itcall;

			// search for source ip-port
			itcall = call_list.find(key);
			if (itcall != call_list.end())
				call_list[key].add_RTP(rtp);
			else
			{
				//search call for destination ip-port 
				key = ip_destination + ":" + std::to_string(destination_port);
				itcall= call_list.find(key);
				if (itcall != call_list.end())
					call_list[key].add_RTP(rtp);
				else
				{
					// Create new CALL
					Call c1(rtp);
					call_list[key] = c1;
				}
			}

		}



}
