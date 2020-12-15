#include "WAVUtil.h"

using namespace wav;

WAVHeader wav::CreateDefaultHeader()
{
	WAVHeader ret = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			nullptr
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			nullptr
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			nullptr
		},
		0
	};

	return ret;
}

uint32_t wav::ByteArrayToUInt32LE(const byte_t byte[4])
{
	return byte[0] | byte[1] << 8 | byte[2] << 16 | byte[3] << 24;
}

uint32_t wav::ByteArrayToUInt32BE(const byte_t byte[4])
{
	return byte[3] | byte[2] << 8 | byte[1] << 16 | byte[0] << 24;
}

uint16_t wav::ByteArrayToUInt16LE(const byte_t byte[2])
{
	return byte[0] | byte[1] << 8;
}

uint16_t wav::ByteArrayToUInt16BE(const byte_t byte[2])
{
	return byte[1] | byte[0] << 8;
}

int32_t wav::ByteArrayToInt32LE(const byte_t byte[4])
{
	return byte[0] | byte[1] << 8 | byte[2] << 16 | byte[3] << 24;
}

int32_t wav::ByteArrayToInt32BE(const byte_t byte[4])
{
	return byte[3] | byte[2] << 8 | byte[1] << 16 | byte[0] << 24;
}

int16_t wav::ByteArrayToInt16LE(const byte_t byte[2])
{
	return byte[0] | byte[1] << 8;
}

int16_t wav::ByteArrayToInt16BE(const byte_t byte[2])
{
	return byte[1] | byte[0] << 8;
}

void wav::WriteXBytes(const byte_t* data, const int32_t offset, const uint32_t numOfBytes, byte_t* outContainer)
{
	for (int i = 0; i < numOfBytes; i++)
	{
		*(outContainer + i) = *(data + offset + i);
	}
}