#ifndef UNDBXFILE_H
#define UNDBXFILE_H

#if defined(__cplusplus)
extern "C" {
#endif

	typedef struct _UnDBXFile {
		void* Impl;
	} UnDBXFile;

	static const int UNDBX_OPEN_FILE_TYPE_READ_BINARY=1;

	UnDBXFile* UnDBXFile_Open(const char* Filename, int OpenType);
	void UnDBXFile_Close(UnDBXFile* File);
	size_t UnDBXFile_Read(void* ptr, size_t size, size_t nitems, UnDBXFile* File);
	size_t UnDBXFile_Seek(UnDBXFile* File, long Distance, int Type);
	size_t UnDBXFile_GetSize(const char* Filename);

	void UnDBXFile_Read_long_long(long long int* value, UnDBXFile* file);
	void UnDBXFile_Read_int(int* value, UnDBXFile* file);
	void UnDBXFile_Read_short(short* value, UnDBXFile* file);



#if defined(__cplusplus)
} // extern "C"
#endif

#endif // UNDBXFILE_H
