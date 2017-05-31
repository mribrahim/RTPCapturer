#pragma once

#include <string>
#include <iostream>
#include <vector>
#include<conio.h>
#include <pcap.h>


#define PCAP_OPENFLAG_PROMISCUOUS   1

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")



std::string GetIPString(uint32_t x);

unsigned short char_array_to_short(unsigned char* input, std::string convert = "bigendian");
unsigned int char_array_to_int(unsigned char* input, std::string convert="bigendian");
unsigned long int char_array_to_long(unsigned char* input, std::string convert = "bigendian");

std::string shortint_to_byte_char(unsigned int input, std::string convert = "bigendian");
std::string int_to_byte_char(unsigned int input, std::string convert = "bigendian");
std::string longint_to_byte_char(unsigned long int input, std::string convert = "bigendian");

unsigned short int DecodePacket_G711a( char* packet_data_ptr, unsigned short int packet_data_length, int* decoded_data_ptr);

unsigned short int DecodePacket_G711u( char* packet_data_ptr, unsigned short int packet_data_length, int* decoded_data_ptr);


int CreateWavefile(std::string filename, std::vector<int> data);

void CreateWaveFile_2sn();