
#include "UnDBXFile.h"
#include <filesystem>
#include <fstream>

class UnDBXFileReader
{
private:
	std::ifstream m_Stream;

public:
	UnDBXFileReader(const char* Filename)
		: m_Stream(Filename, std::ios::binary)
	{
	}

	size_t Read(void* ptr, size_t size, size_t nitems)
	{
		const std::streamsize PrePos = m_Stream.tellg();
		m_Stream.read(reinterpret_cast<char*>(ptr), size*nitems);
		const std::streamsize PostPos = m_Stream.tellg();
		return PostPos - PrePos;
	}

	int Seek(long Distance, int Type)
	{
		std::ios::seekdir SeekDir = std::ios::cur;

		switch (Type)
		{
			case SEEK_SET:
				SeekDir = std::ios::beg;
				break;
			case SEEK_CUR:
				SeekDir = std::ios::cur;
				break;
			case SEEK_END:
				SeekDir = std::ios::end;
				break;
		}

		m_Stream.seekg(Distance, SeekDir);
		return Distance;
	}
};

UnDBXFile* UnDBXFile_Open(const char* Filename, int OpenType)
{
	if (!std::filesystem::exists(Filename))
	{
		return nullptr;
	}

	if (OpenType != UNDBX_OPEN_FILE_TYPE_READ_BINARY)
	{
		return nullptr;
	}

	UnDBXFileReader* Reader = new UnDBXFileReader(Filename);

	UnDBXFile* Out = new UnDBXFile;
	Out->Impl = reinterpret_cast<void*>(Reader);
	return Out;
}

void UnDBXFile_Close(UnDBXFile* File)
{
	if (!File)
	{
		return;
	}

	UnDBXFileReader* Reader = reinterpret_cast<UnDBXFileReader*>(File->Impl);
	if (Reader)
	{
		delete Reader;
	}

	delete File;
}

size_t UnDBXFile_Read(void* ptr, size_t size, size_t nitems, UnDBXFile* File)
{
	if (!File)
	{
		return 0;
	}

	UnDBXFileReader* Reader = reinterpret_cast<UnDBXFileReader*>(File->Impl);
	return Reader ? Reader->Read(ptr, size, nitems) : 0;
}

size_t UnDBXFile_Seek(UnDBXFile* File, long Distance, int Type)
{
	if (!File)
	{
		return 0;
	}

	UnDBXFileReader* Reader = reinterpret_cast<UnDBXFileReader*>(File->Impl);
	return Reader ? Reader->Seek(Distance, Type) : 0;
}

size_t UnDBXFile_GetSize(const char* Filename)
{
	return std::filesystem::file_size(Filename);
}

void UnDBXFile_Read_long_long(long long int* value, UnDBXFile* file)
{
#ifndef WORDS_BIGENDIAN
	UnDBXFile_Read(value, 1, sizeof(long long int), file);
#else
	/* the following code is endianness neutral */
	long long int llw = 0;
	llw = (long long int) (fgetc(file) & 0xFF);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x08);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x10);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x18);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x20);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x28);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x30);
	llw |= ((long long int) (fgetc(file) & 0xFF) << 0x38);
	*value = llw;
#endif
}

void UnDBXFile_Read_int(int* value, UnDBXFile* file)
{
#ifndef WORDS_BIGENDIAN
	UnDBXFile_Read(value, 1, sizeof(int), file);
#else
	/* the following code is endianness neutral */
	int dw = 0;
	dw = (int)(fgetc(file) & 0xFF);
	dw |= ((int)(fgetc(file) & 0xFF) << 0x08);
	dw |= ((int)(fgetc(file) & 0xFF) << 0x10);
	dw |= ((int)(fgetc(file) & 0xFF) << 0x18);
	*value = dw;
#endif
}

void UnDBXFile_Read_short(short* value, UnDBXFile* file)
{
#ifndef WORDS_BIGENDIAN
	UnDBXFile_Read(value, 1, sizeof(short), file);
#else
	/* the following code is endianness neutral */
	short w = 0;
	w = (short)(fgetc(file) & 0xFF);
	w |= ((short)(fgetc(file) & 0xFF) << 0x08);
	*value = w;
#endif
}
