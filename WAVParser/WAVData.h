#pragma once

#include <string>
#include "WAVUtil.h"

namespace wav
{
	class WAVData
	{
	private:
		WAVHeader header;
		byte_t* data;

		bool ParseHeader(std::ifstream& fileHandle);
		bool ParseListSubchunks(const byte_t* listData, const uint32_t listSize);

	public:
		WAVData();
		WAVData(std::string filename);
		WAVData(const WAVData& obj);
		~WAVData();

		void ClearData();
		void ParseFile(std::string filename);
		void WriteToFile(std::string filename) const;
		int32_t* GetSamples();

		friend std::ostream& operator<<(std::ostream& os, const WAVData& file)
		{
			char* printable = new char[5];
			printable[4] = '\0';

			os << "======== WAV File Data ========" << std::endl;

			WriteXBytes(file.header.id, 0, 4, (byte_t*)printable);
			os << " Header Type: " << printable << std::endl;

			WriteXBytes(file.header.file_format, 0, 4, (byte_t*)printable);
			os << " File Format: " << printable << std::endl;

			if (ByteArrayToUInt16LE(file.header.format.data) == 1)
			{
				os << "    Format type: PCM" << std::endl;
			}

			os << "    Number of Channels: " << file.GetNumberOfChannels() << std::endl;
			os << "    Sampling Rate: " << file.GetSamplingRate() << " Hz" << std::endl;
			os << "    Bits per Sample: " << file.GetBitsPerSample() << std::endl;

			if (file.GetFileSizeMB() != 0)
			{
				os << " File Size: " << file.GetFileSizeMB() << "." << file.GetFileSizeKB() % 1024 << " MB" << std::endl;
			}
			else
			{
				os << " File Size: " << file.GetFileSizeKB() << " KB" << std::endl;
			}

			delete[] printable;
			return os;
		}

		inline WAVHeader GetHeader() const
		{
			return this->header;
		}

		inline byte_t* GetDataPointer() const
		{
			return this->data;
		}

		inline uint16_t GetNumberOfChannels() const
		{
			return ByteArrayToUInt16LE(this->header.format.data + 2);
		}

		inline uint16_t GetBitsPerSample() const
		{
			return ByteArrayToUInt16LE(this->header.format.data + 14);
		}

		inline uint32_t GetFileSize() const
		{
			return (ByteArrayToUInt32LE(this->header.file_size) + 8);
		}

		inline uint32_t GetFileSizeKB() const
		{
			return this->GetFileSize() / 1024;
		}

		inline uint32_t GetFileSizeMB() const
		{
			return this->GetFileSize() / (1024 * 1024);
		}

		inline uint32_t GetSamplingRate() const
		{
			return ByteArrayToUInt32LE(this->header.format.data + 4);
		}

		inline uint32_t GetNumberOfSamples() const
		{
			return ByteArrayToUInt32LE(this->header.data.size) / (this->GetBitsPerSample() / 8);
		}
	};
}
