#include <iostream>
#include <fstream>
#include <vector>
#include "WAVData.h"

using namespace wav;

WAVData::WAVData()
{
	this->header = CreateDefaultHeader();
	this->data = nullptr;
}

WAVData::WAVData(std::string filename) : WAVData()
{
	this->ParseFile(filename);
}

WAVData::WAVData(const WAVData& obj) : WAVData()
{
	uint32_t size;

	WriteXBytes(obj.header.id, 0, 4, this->header.id);
	WriteXBytes(obj.header.file_size, 0, 4, this->header.file_size);
	WriteXBytes(obj.header.file_format, 0, 4, this->header.file_format);
	
	WriteXBytes(obj.header.format.id, 0, 4, this->header.format.id);
	WriteXBytes(obj.header.format.size, 0, 4, this->header.format.size);
	if (obj.header.format.data != nullptr)
	{
		size = ByteArrayToUInt32LE(obj.header.format.size);
		try
		{
			this->header.format.data = new byte_t[size];
		}
		catch (std::bad_alloc& ex)
		{
			std::cerr << "[!] Exception in copy_ctr: " << ex.what() << std::endl;
			this->ClearData();
			return;
		}

		for (int i = 0; i < size; i++)
		{
			*(this->header.format.data + i) = *(obj.header.format.data + i);
		}
	}

	WriteXBytes(obj.header.list.id, 0, 4, this->header.list.id);
	WriteXBytes(obj.header.list.size, 0, 4, this->header.list.size);
	WriteXBytes(obj.header.list.type, 0, 4, this->header.list.type);
	if (obj.header.list.data != nullptr)
	{
		size = obj.header.list.numOfChunks;

		try
		{
			this->header.list.data = new RiffChunk[size];
		}
		catch (std::bad_alloc& ex)
		{
			std::cerr << "[!] Exception in copy_ctr: " << ex.what() << std::endl;
			this->ClearData();
			return;
		}

		for (int i = 0; i < obj.header.list.numOfChunks; i++)
		{
			WriteXBytes((obj.header.list.data + i)->id, 0, 4, (this->header.list.data + i)->id);
			WriteXBytes((obj.header.list.data + i)->size, 0, 4, (this->header.list.data + i)->size);
			if ((obj.header.list.data + i)->data != nullptr)
			{
				size = ByteArrayToUInt32LE((obj.header.list.data + i)->size);

				try
				{
					(this->header.list.data + i)->data = new byte_t[size];
				}
				catch (std::bad_alloc& ex)
				{
					std::cerr << "[!] Exception in copy_ctr: " << ex.what() << std::endl;
					this->ClearData();
					return;
				}

				for (int j = 0; j < size; j++)
				{
					*((this->header.list.data + i)->data + j) = *((obj.header.list.data + i)->data + j);
				}
			}
		}

		this->header.list.numOfChunks = obj.header.list.numOfChunks;
	}
	
	WriteXBytes(obj.header.data.id, 0, 4, this->header.data.id);
	WriteXBytes(obj.header.data.size, 0, 4, this->header.data.size);
	this->header.header_size = obj.header.header_size;

	size = ByteArrayToUInt32LE(obj.header.data.size);

	if (obj.data == nullptr)
	{
		this->header.data.data = nullptr;
		this->data = nullptr;
		return;
	}

	try
	{
		this->data = new byte_t[size];
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception in copy_ctr: " << ex.what() << std::endl;
		this->ClearData();
		return;
	}

	this->header.data.data = this->data;

	for (int i = 0; i < size; i++)
	{
		*(this->data + i) = *(obj.data + i);
	}
}

WAVData::~WAVData()
{
	this->ClearData();
}

void WAVData::ClearData()
{
	if (this->header.format.data != nullptr)
	{
		delete[] this->header.format.data;
	}

	if (this->header.list.data != nullptr)
	{
		for (int i = 0; i < this->header.list.numOfChunks; i++)
		{
			if ((this->header.list.data + i)->data != nullptr)
			{
				delete[] (this->header.list.data + i)->data;
			}
		}

		delete[] this->header.list.data;
	}

	this->header = CreateDefaultHeader();

	if (this->data == nullptr)
	{
		return;
	}

	delete[] this->data;
	this->data = nullptr;
}

bool WAVData::ParseListSubchunks(const byte_t* listData, const uint32_t listSize)
{
	uint16_t chunkCount = 0;
	uint16_t byteCount = 4;
	uint32_t chunkSize = 0;

	while (byteCount < listSize)
	{
		chunkCount++;

		chunkSize = ByteArrayToUInt32LE((listData + byteCount));
		chunkSize += (chunkSize) % 2;

		byteCount += 8 + chunkSize;
	}

	this->header.list.numOfChunks = chunkCount;
	try
	{
		this->header.list.data = new RiffChunk[chunkCount];
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception while parsing header: " << ex.what() << std::endl;
		return false;
	}

	byteCount = 0;

	for (int i = 0; i < chunkCount; i++)
	{
		WriteXBytes(listData + byteCount, 0, 4, (this->header.list.data + i)->id);
		WriteXBytes(listData + byteCount + 4, 0, 4, (this->header.list.data + i)->size);
		chunkSize = ByteArrayToUInt32LE((this->header.list.data + i)->size);
		chunkSize += (chunkSize) % 2;

		try
		{
			(this->header.list.data + i)->data = new byte_t[chunkSize];
		}
		catch (std::bad_alloc& ex)
		{
			std::cerr << "[!] Exception while parsing header: " << ex.what() << std::endl;
			return false;
		}

		WriteXBytes(listData + byteCount, 8, chunkSize, (this->header.list.data + i)->data);
		byteCount += 8 + chunkSize;
	}

	if (byteCount != listSize)
	{
		std::cerr << "[!] Warning: List data read: " << byteCount << "\tList data size: " << listSize << std::endl;
	}

	return true;
}

bool WAVData::ParseHeader(std::ifstream& fileHandle)
{
	fileHandle.read((char*)this->header.id, 4);
	fileHandle.read((char*)this->header.file_size, 4);
	fileHandle.read((char*)this->header.file_format, 4);

	if (ByteArrayToUInt32BE(this->header.id) != FOURCCRIFF)
	{
		return false;
	}

	if (ByteArrayToUInt32BE(this->header.file_format) != FOURCCWAVE)
	{
		return false;
	}

	byte_t* chunkId;
	try
	{
		chunkId = new byte_t[4];
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception while parsing header: " << ex.what() << std::endl;
		return false;
	}

	WriteXBytes(this->header.id, 0, 4, chunkId);
	uint32_t chunkIdVal = ByteArrayToUInt32BE(chunkId);
	uint32_t headerSize = 12;

	while (chunkIdVal != FOURCCDATA)
	{
		fileHandle.read((char*)chunkId, 4);
		chunkIdVal = ByteArrayToUInt32BE(chunkId);
		uint32_t dataSize;
		byte_t* listData = nullptr;

		switch (chunkIdVal)
		{
		case FOURCCFMT:
			WriteXBytes(chunkId, 0, 4, this->header.format.id);
			fileHandle.read((char*)this->header.format.size, 4);
			dataSize = ByteArrayToUInt32LE(this->header.format.size);

			try
			{
				this->header.format.data = new byte_t[dataSize];
			}
			catch (std::bad_alloc& ex)
			{
				std::cerr << "[!] Exception while parsing header: " << ex.what() << std::endl;
				return false;
			}

			fileHandle.read((char*)this->header.format.data, dataSize);
			headerSize += 8 + dataSize;
			break;

		case FOURCCDATA:
			WriteXBytes(chunkId, 0, 4, this->header.data.id);
			fileHandle.read((char*)this->header.data.size, 4);
			headerSize += 8;
			break;

		case FOURCCLIST:
			WriteXBytes(chunkId, 0, 4, this->header.list.id);
			fileHandle.read((char*)this->header.list.size, 4);
			fileHandle.read((char*)this->header.list.type, 4);
			dataSize = ByteArrayToUInt32LE(this->header.list.size) - 4;

			try
			{
				listData = new byte_t[dataSize];
			}
			catch (std::bad_alloc& ex)
			{
				std::cerr << "[!] Exception while parsing header: " << ex.what() << std::endl;
				return false;
			}

			fileHandle.read((char*)listData, dataSize);
			if (!this->ParseListSubchunks(listData, dataSize))
			{
				return false;
			}
			delete[] listData;
			headerSize += 12 + dataSize;
			break;

		default:
			delete[] chunkId;
			return false;
		}
	}

	this->header.header_size = headerSize;
	delete[] chunkId;
	return true;
}

void WAVData::ParseFile(std::string filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "[!] Error opening file '" << filename << "'" << std::endl;
		return;
	}

	if (!this->ParseHeader(file))
	{
		std::cerr << "[!] Corrupt file header..." << std::endl;
		this->ClearData();
		file.close();
		return;
	}

	uint32_t dataSize = ByteArrayToUInt32LE(this->header.data.size);

	try
	{
		this->data = new byte_t[dataSize];
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception while parsing file: " << ex.what() << std::endl;
		this->ClearData();
		file.close();
		return;
	}

	char* dataTemp = (char*)this->data;
	uint32_t counter = 0;

	while (file.good())
	{
		file.read((char*)dataTemp + counter, 1);
		counter++;
	}

	dataTemp = nullptr;

	if (!file.eof())
	{
		std::cerr << "[!] Unexpected error while reading file data. Could read [" << file.gcount() - header.header_size << "] bytes out of [" << dataSize << "]" << std::endl;
		this->ClearData();
		file.close();
		return;
	}

	this->header.data.data = this->data;
}

void WAVData::WriteToFile(std::string filename) const
{
	if (this->data == nullptr)
	{
		std::cout << "[+] No data to write..." << std::endl;
		return;
	}

	std::ofstream file(filename, std::ios::out | std::ios::app | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "[!] Error opening file '" << filename << "'" << std::endl;
		return;
	}

	try
	{
		file.write((char*)this->header.id, 4);
		file.write((char*)this->header.file_size, 4);
		file.write((char*)this->header.file_format, 4);

		file.write((char*)this->header.format.id, 4);
		file.write((char*)this->header.format.size, 4);
		for (int i = 0; i < ByteArrayToUInt32LE(this->header.format.size); i++)
		{
			file.write((char*)this->header.format.data + i, 1);
		}

		if (this->header.list.size != 0)
		{
			file.write((char*)this->header.list.id, 4);
			file.write((char*)this->header.list.size, 4);
			file.write((char*)this->header.list.type, 4);
			for (int i = 0; i < this->header.list.numOfChunks; i++)
			{
				file.write((char*)(this->header.list.data + i)->id, 4);
				file.write((char*)(this->header.list.data + i)->size, 4);
				for (int j = 0; j < ByteArrayToUInt32LE((this->header.list.data + i)->size); j++)
				{
					file.write((char*)((this->header.list.data + i)->data + j), 1);
				}
			}
		}

		file.write((char*)this->header.data.id, 4);
		file.write((char*)this->header.data.size, 4);
		for (int i = 0; i < ByteArrayToUInt32LE(this->header.data.size); i++)
		{
			file.write((char*)this->data + i, 1);
		}
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception while writing to file: " << ex.what() << std::endl;
		file.close();
		return;
	}

	file.close();
}

// If 2 channels exist the pattern is: L R L R L R...
// The caller is responsible for parsing the channels
int32_t* WAVData::GetSamples()
{
	uint16_t bps = this->GetBitsPerSample();
	uint32_t numOfSamples = this->GetNumberOfSamples();

	int32_t* ret;

	try
	{
		ret = new int32_t[numOfSamples];
	}
	catch (std::bad_alloc& ex)
	{
		std::cerr << "[!] Exception while parsing file: " << ex.what() << std::endl;
		return nullptr;
	}

	for (int i = 0; i < numOfSamples; i++)
	{
		if (bps == 16)
		{
			*(ret + i) = ByteArrayToInt16LE(this->data + (i * 2));
		}
		else if (bps == 32)
		{
			*(ret + i) = ByteArrayToInt32LE(this->data + (i * 4));
		}
	}

	return ret;
}