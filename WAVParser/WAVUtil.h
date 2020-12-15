#pragma once

// Big Endian definitions
#define FOURCCRIFF 0x52494646	// RIFF
#define FOURCCFMT  0x666D7420	// fmt 
#define FOURCCWAVE 0x57415645	// WAVE
#define FOURCCDATA 0x64617461	// data
#define FOURCCLIST 0x4C495354	// LIST

#include <cstdint>

namespace wav
{
	typedef uint8_t byte_t;

	struct RiffChunk
	{
		byte_t id[4];
		byte_t size[4];
		byte_t* data;			
	};

	struct ListChunk
	{
		byte_t id[4];
		byte_t size[4];
		byte_t type[4];
		RiffChunk* data;
		uint16_t numOfChunks;
	};

	/*
	* Struct based on the RIFF WAV format
	*/
	struct WAVHeader
	{
		byte_t id[4];
		byte_t file_size[4];
		byte_t file_format[4];
		RiffChunk format;
		RiffChunk data;
		ListChunk list;

		uint32_t header_size;
	};

	/*
	* Default WAV Header creator for initialization purposes
	*/
	WAVHeader CreateDefaultHeader();

	/*
	* Convert a Little Endian byte array to uint32_t
	*/
	uint32_t ByteArrayToUInt32LE(const byte_t byte[4]);

	/*
	* Convert a Big Endian byte array to uint32_t
	*/
	uint32_t ByteArrayToUInt32BE(const byte_t byte[4]);

	/*
	* Convert a Little Endian byte array to uint16_t
	*/
	uint16_t ByteArrayToUInt16LE(const byte_t byte[2]);

	/*
	* Convert a Big Endian byte array to uint16_t
	*/
	uint16_t ByteArrayToUInt16BE(const byte_t byte[2]);

	/*
	* Convert a Little Endian byte array to int32_t
	*/
	int32_t ByteArrayToInt32LE(const byte_t byte[4]);

	/*
	* Convert a Big Endian byte array to int32_t
	*/
	int32_t ByteArrayToInt32BE(const byte_t byte[4]);

	/*
	* Convert a Little Endian byte array to int16_t
	*/
	int16_t ByteArrayToInt16LE(const byte_t byte[2]);

	/*
	* Convert a Big Endian byte array to int16_t
	*/
	int16_t ByteArrayToInt16BE(const byte_t byte[2]);

	/*
	* Read X bytes starting at (data + offset) and write them into the outContainer
	* Caller is responsible for validly allocated container
	*/
	void WriteXBytes(const byte_t* data, const int32_t offset, const uint32_t numOfBytes, byte_t* outContainer);
}