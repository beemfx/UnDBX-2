#ifndef UNDBXFILE_H
#define UNDBXFILE_H

#if defined(__cplusplus)
extern "C" {
#endif

	typedef struct _UnDBXFile {
		void* Impl;
	} UnDBXFile;

	static const int UNDBX_OPEN_FILE_TYPE_READ_BINARY=1;

	UnDBXFile* UnDBXFile_Open(char* Filename, int OpenType);
	void UnDBXFile_Close(UnDBXFile* File);
	size_t UnDBXFile_Read(void* ptr, size_t size, size_t nitems, UnDBXFile* File);
	size_t UnDBXFile_Seek(UnDBXFile* File, long Distance, int Type);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // UNDBXFILE_H
