
#include "commonfunction.h"

#include <fstream>
#include <vector>

std::string GetIPString(uint32_t x)
{
	char buffer[INET_ADDRSTRLEN + 1];
	auto result = inet_ntop(AF_INET, &x, buffer, sizeof(buffer));
	if (result == nullptr) throw std::runtime_error("Can't convert IP4 address");
	return buffer;
}


unsigned short char_array_to_short(unsigned char* input, std::string convert)
{
	unsigned short x;

	if (convert == "bigendian") 
		x =  (input[0] << 8) | input[1];
	else if (convert == "littleendian")
		x = (input[1] << 8) | input[0];

	return x;
}

unsigned int char_array_to_int(unsigned char* input, std::string convert)
{
	unsigned int x;

	if (convert == "bigendian")
		x = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
	else if (convert == "littleendian")
		x = (input[3] << 24) | (input[2] << 16) | (input[1] << 8) | input[0];

	return x;
}

unsigned long int char_array_to_long(unsigned char* input, std::string convert)
{
	unsigned long int x;

	if (convert == "bigendian")
		x = (input[0] << 56) | (input[1] << 48) | (input[2] << 40) | (input[3] << 32) << (input[4] << 24) | (input[5] << 16) | (input[6] << 8) | input[7];
	else if (convert == "littleendian")
		x = (input[7] << 56) | (input[6] << 48) | (input[5] << 40) | (input[4] << 32) << (input[3] << 24) | (input[2] << 16) | (input[1] << 8) | input[0];

	return x;
}


std::string shortint_to_byte_char(unsigned int input, std::string convert)
{
	const int byte_count = 2;
	std::string str;
	std::vector<unsigned char> arrayOfByte(byte_count);

	for (int i = 0; i < byte_count; i++)
		arrayOfByte[(byte_count - 1) - i] = (input >> (i * 8));


	if (convert == "bigendian")
		for (int i = 0; i < byte_count; i++)
			str.push_back(arrayOfByte[i]);

	if (convert == "littleendian")
		for (int i = byte_count - 1; i >= 0; i--)
			str.push_back(arrayOfByte[i]);

	return str;
}


std::string int_to_byte_char(unsigned int input, std::string convert)
{
	const int byte_count = 4;
	std::string str;
	std::vector<unsigned char> arrayOfByte(byte_count);

	for (int i = 0; i < byte_count; i++)
		arrayOfByte[(byte_count - 1) - i] = (input >> (i * 8));


	if (convert == "bigendian")
		for (int i = 0; i < byte_count; i++)
			str.push_back(arrayOfByte[i]);

	if (convert == "littleendian")
		for (int i = byte_count - 1; i >= 0; i--)
			str.push_back(arrayOfByte[i]);

	return str;
}


std::string longint_to_byte_char(unsigned long int input, std::string convert)
{
	const int byte_count = 8;
	std::string str;
	std::vector<unsigned char> arrayOfByte(byte_count);

	for (int i = 0; i < byte_count; i++)
		arrayOfByte[(byte_count - 1) - i] = (input >> (i * 8));


	if (convert == "bigendian")
		for (int i = 0; i < byte_count; i++)
			str.push_back(arrayOfByte[i]);

	if (convert == "littleendian")
		for (int i = byte_count - 1; i >= 0; i--)
			str.push_back(arrayOfByte[i]);

	return str;
}


unsigned short int DecodePacket_G711a( char* packet_data_ptr, unsigned short int packet_data_length, int* decoded_data_ptr)
{

	//double normalizing_ratio=((double)(0x9ffff))/((double)(0x1ffff)); // this will map 13 bit pcm to pseudo 16 bit pcm
	short int quantization_value;
	short int quantization_segment;
	unsigned char alaw_data;
	for (int k = 0; k<packet_data_length; k++)
	{
		alaw_data = *packet_data_ptr++;
		alaw_data ^= 0x55;

		quantization_value = (alaw_data & (0xf)) << 4;
		quantization_segment = ((unsigned)alaw_data & (0x70)) >> (4);
		switch (quantization_segment)
		{
		case 0:
			quantization_value += (0x0008);
			break;
		case 1:
			quantization_value += (0x0108);
			break;
		default:
			quantization_value += (0x0108);
			quantization_value <<= (quantization_segment - 1);
		};

		//*pcm_vector++=((alaw_data & (0x80))?quantization_value:-quantization_value)*normalizing_ratio;
		*decoded_data_ptr++ = (int)(((alaw_data & (0x80)) ? quantization_value : -quantization_value));
	}

	return packet_data_length;
}



unsigned short int DecodePacket_G711u( char* packet_data_ptr, unsigned short int packet_data_length, int* decoded_data_ptr)
{

	//double normalizing_ratio=((double)(0x9ffff))/((double)(0x1ffff)); // this will map 13 bit pcm to pseudo 16 bit pcm
	short int quantization_value;
	short int quantization_segment;
	unsigned char ulaw_data;
	for (int k = 0; k<packet_data_length; k++)
	{
		ulaw_data = ~(*packet_data_ptr++);

		//        quantization_value= (ulaw_data & (0xf)) << 4;
		quantization_value = (ulaw_data & (0xf)) << 3;
		quantization_segment = ((unsigned)ulaw_data & (0x70)) >> (4);

		quantization_value += 0x0084;
		quantization_value <<= quantization_segment;

		quantization_value -= (32);

		*decoded_data_ptr++ = (int)(((ulaw_data & (0x80)) ? quantization_value : -quantization_value));
	}

	return packet_data_length;
}


template <typename Word>
std::ostream& write_word(std::ostream& outs, Word value, unsigned size = sizeof(Word))
{
	for (; size; --size, value >>= 8)
		outs.put(static_cast <char> (value & 0xFF));
	return outs;
}



int CreateWavefile(std::string filename, std::vector<int> data)
{
	std::ofstream f(filename, std::ios::binary);

	short channels = 2;
	short bitspersample = 16;
	short samplerate = 8000;
	short block_alignment = samplerate * bitspersample * channels / 8;

	// Write the file headers
	f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
	write_word(f, 16, 4);  // no extension data
	write_word(f, 1, 2);  // PCM - integer samples
	write_word(f, channels, 2);  // two channels (stereo file)
	write_word(f, samplerate, 4);  // samples per second (Hz)
	write_word(f, block_alignment, 4);  // (Sample Rate * BitsPerSample * Channels) / 8
	write_word(f, 4, 2);  // data block size (size of two integer samples, one for each channel, in bytes)
	write_word(f, bitspersample, 2);  // number of bits per sample (use a multiple of 8)

						   // Write the data chunk header
	size_t data_chunk_pos = f.tellp();
	f << "data----";  // (chunk size to be filled in later)

	
	const char* pointer = reinterpret_cast<const char*>(&data[0]);
	size_t bytes = data.size() * sizeof(data[0]);
	f.write(pointer, bytes );

	// (We'll need the final file size to fix the chunk sizes above)
	size_t file_length = f.tellp();

	// Fix the data chunk header to contain the data size
	f.seekp(data_chunk_pos + 4);
	write_word(f, file_length - data_chunk_pos + 8);

	// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
	f.seekp(0 + 4);
	write_word(f, file_length - 8, 4);

	return 1;
}




// only an example to create 2sn example.wav file :)
void CreateWaveFile_2sn()
{
	std::ofstream f("example.wav", std::ios::binary);

// Write the file headers
f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
write_word(f, 16, 4);  // no extension data
write_word(f, 1, 2);  // PCM - integer samples
write_word(f, 2, 2);  // two channels (stereo file)
write_word(f, 44100, 4);  // samples per second (Hz)
write_word(f, 176400, 4);  // (Sample Rate * BitsPerSample * Channels) / 8
write_word(f, 4, 2);  // data block size (size of two integer samples, one for each channel, in bytes)
write_word(f, 16, 2);  // number of bits per sample (use a multiple of 8)

					   // Write the data chunk header
size_t data_chunk_pos = f.tellp();
f << "data----";  // (chunk size to be filled in later)

				  // Write the audio samples
				  // (We'll generate a single C4 note with a sine wave, fading from left to right)
constexpr double two_pi = 6.283185307179586476925286766559;
constexpr double max_amplitude = 32760;  // "volume"

double hz = 44100;    // samples per second
double frequency = 261.626;  // middle C
double seconds = 2.5;      // time

int N = hz * seconds;  // total number of samples
for (int n = 0; n < N; n++)
{
	double amplitude = (double)n / N * max_amplitude;
	double value = sin((two_pi * n * frequency) / hz);
	write_word(f, (int)(amplitude  * value), 2);
	write_word(f, (int)((max_amplitude - amplitude) * value), 2);
}

// (We'll need the final file size to fix the chunk sizes above)
size_t file_length = f.tellp();

// Fix the data chunk header to contain the data size
f.seekp(data_chunk_pos + 4);
write_word(f, file_length - data_chunk_pos + 8);

// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
f.seekp(0 + 4);
write_word(f, file_length - 8, 4);

}

