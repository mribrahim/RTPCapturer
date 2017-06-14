// pcapdeneme.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>
#include <fstream>
#include<conio.h>
#include <pcap.h>

#include "UDPPacket.h"

#define PCAP_OPENFLAG_PROMISCUOUS   1

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")


using namespace std;

void Check_dated_calls(long sec, std::map<std::string, Call> &call_list)
{
	std::map<std::string, Call>::iterator it;

	for (it = call_list.begin(); it != call_list.end(); it++) 
	{
		if (it->second.isfinished(sec))
		{
			it->second.display();		
			it->second.save_to_file();

			call_list.erase(it);
			break;
		}
	}
}


unsigned short get_ethernet_type(const u_char* pkt_data, bool display_MAC=false)
{
	if(display_MAC)
		std::cout << " Destination Mac Address :  ";
	for (u_int i = 0; i <6; i++)
	{
		if (display_MAC)
			std::cout << std::hex << ((int)pkt_data[0]<10 ? "0" : "") << (int)pkt_data[0] << (i != 5 ? ":" : "");
		pkt_data++;
	}

	if (display_MAC)
		std::cout << std::endl;
	if (display_MAC)
		std::cout << " Source Mac Address :   ";
	for (u_int i = 0; i <6; i++)
	{
		if (display_MAC)
			std::cout << std::hex << ((int)pkt_data[0]<10 ? "0" : "") << (int)pkt_data[0] << (i != 5 ? ":" : "");
		pkt_data++;
	}
	if (display_MAC)
		std::cout << std::dec << std::endl;

	unsigned short ethernet_type = pkt_data[0] << 8 | pkt_data[1];
	pkt_data += 2; // **** ethernet type is 2 byte ****
				   //std::cout << " Ethernet Type :   " << (ethernet_type == 0x0800 ? "*** IP ***" : "-----") << std::endl;

	return ethernet_type;
}

std::map<std::string, Call> call_list;
bool vp8payloadheader = true;
std::unordered_map<std::string, std::string> fragmented_rtps;




int main(int argc, char **argv)
{
	pcap_if_t *alldevs, *d;
	pcap_t *fp;
	u_int inum, i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	int res;
	struct pcap_pkthdr *header;
	const u_char *pcap_data;

	if (argc == 1)
	{
		std::string directory = "wav_files";
		std::string folder = "mkdir " + directory;
		system(folder.c_str());

		cout << "\n ************ printing the device list: *************\n";
		/* The user didn't provide a packet source: Retrieve the local device list */
		if (pcap_findalldevs(&alldevs, errbuf) == -1)
		{
			fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
			return -1;
		}

		/* Print the list */
		for (d = alldevs; d; d = d->next)
		{
			cout << ++i << "  " << d->name << ": ";

			if (d->description)
				cout << d->description << endl;
			else
				cout << "No description available" << endl;
		}

		if (i == 0)
		{
			fprintf(stderr, "No interfaces found! Exiting.\n");
			return -1;
		}

		cout << "Enter the interface number (1-" << i << ")" << endl;
		cin>>inum;

		if (inum < 1 || inum > i)
		{
			printf("\nInterface number out of range.\n");

			/* Free the device list */
			pcap_freealldevs(alldevs);
			return -1;
		}

		/* Jump to the selected adapter */
		for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

		/* Open the device */
		if ((fp = pcap_open_live(d->name, 100, PCAP_OPENFLAG_PROMISCUOUS, 100, errbuf)) == NULL)
		{
			fprintf(stderr, "\nError opening adapter\n");
			return -1;
		}

	}
	else  if (0 == strcmp("-f", argv[1]))
	{
		// read from FILE
		//std::string file = "C:\\Users\\IbrahimD\\Desktop\\vakif_pcap\\vakif1.pcap";
		//std::string file = "C:\\Users\\IbrahimD\\Desktop\\pcap_goruntulu\\vp8.pcap";
		std::string file = argv[2];
		fp = pcap_open_offline(file.c_str(), errbuf);
	}





	// get current time
	time_t current_time;
	time(&current_time);

	u_int packetCount = 0;
	unsigned char * pkt_data;
	/* Read the packets */
	while ((res = pcap_next_ex(fp, &header, &pcap_data)) >= 0)
	{
		pkt_data = (unsigned char*) pcap_data;
		++packetCount;
		if (_kbhit())
		{
			//char ch = getchar();
			//if (ch == 27)
				break;
		}
			
		

		////std::cout << " Packet size : " << header->len << std::endl;
		//// Show Epoch Time
		//std::cout << " Epoch Time:   " << header->ts.tv_sec << "." << header->ts.tv_usec << " sn" << std::endl;

		Check_dated_calls(header->ts.tv_sec,call_list);

		if (header->len == 0)
			continue;

		
		unsigned short ethernet_type = get_ethernet_type(pkt_data);
		pkt_data += 14;

		if (ethernet_type == 0x0800)
		{
			unsigned char lms = pkt_data[2];
			unsigned char rms = pkt_data[3];

			uint16_t totallength = (lms << 8) | rms;

			lms = pkt_data[4];
			rms = pkt_data[5];
			unsigned short identification = (lms << 8) | rms;
			unsigned int ip_source = (pkt_data[15] << 24) | (pkt_data[14] << 16) | (pkt_data[13] << 8) | pkt_data[12];
			unsigned int ip_destination = (pkt_data[19] << 24) | (pkt_data[18] << 16) | (pkt_data[17] << 8) | pkt_data[16];
			unsigned short protocol = pkt_data[9];

			std::string source_ip = GetIPString(ip_source);
			std::string destination_ip = GetIPString(ip_destination);

			// If this is an UDP packet
			if (protocol == 17)
			{
				pkt_data = pkt_data + 20;
				unsigned short source_port = (pkt_data[0] << 8) | (pkt_data[1]);;
				unsigned short destination_port = (pkt_data[2] << 8) | (pkt_data[3]);;
				unsigned short udp_data_length = (pkt_data[4] << 8) | (pkt_data[5]);;

				udp_data_length = udp_data_length - 8;
				//std::cout << " udp_data_length  : " << udp_data_length << std::endl;
				pkt_data = pkt_data + 8;

				//std::string str((char*)pkt_data, udp_data_length);


				UDPPacket udp(source_ip, destination_ip, source_port, destination_port, pkt_data, udp_data_length);

				udp.Process(header->ts.tv_sec, header->ts.tv_usec, call_list, fragmented_rtps, vp8payloadheader);
					
				//Sleep(500);
				//std::cout << std::endl << std::endl;


			}
			else
				continue;

			
			//std::cout << " IP Packet Total length	:   " << totallength << std::endl;
			//std::cout << " IP identification number  : " << identification << std::endl;
			//std::cout << " IP Source:    " << source_ip << std::endl;
			//std::cout << " IP Destination:   " << destination_ip << std::endl;
			//std::cout << " IP protocol:   " << (protocol==17 ? "UDP" : "other") << std::endl;
			

		}

	
		
	
	} // end of while

	std::map<std::string, Call>::iterator it;
	for (it = call_list.begin(); it != call_list.end(); it++)
	{

		it->second.save_to_file();
		break;
	}

	std::cout << "Packet Count  : " << packetCount << std::endl;
	cout << "File ended" << endl;
	if (res == -1)
	{
		fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
		return -1;
	}


	getchar();
	return 0;
}