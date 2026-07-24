// UnDBX-2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "dbxread.h"
#include <iostream>
#include <vector>
#include <filesystem>

namespace UnDBX2
{
	namespace fs = std::filesystem;

	static std::vector<fs::path> GetAllFilesRecursive(const fs::path& targetDir)
	{
		const fs::path ExtToFind(".dbx");

		std::vector<fs::path> filePaths;

		// Check if path exists and is a valid directory
		if (!fs::exists(targetDir) || !fs::is_directory(targetDir))
		{
			return filePaths;
		}

		// Iterate through all files and subdirectories recursively
		for (const auto& entry : fs::recursive_directory_iterator(targetDir))
		{
			// Filter to only collect regular files (skips folders, symlinks, etc.)
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ExtToFind)
			{
				filePaths.push_back(entry.path());
			}
		}

		return filePaths;
	}
}

int main(int argc, char** argv)
{
	using namespace UnDBX2;

	std::cout << "UnDBX2" << std::endl;

	if (argc < 2)
	{
		return -1;
	}

	const fs::path DirToSearch = argv[1];
	const std::vector<fs::path> AllFiles = GetAllFilesRecursive(DirToSearch);

	dbx_options_t options = { 0 };

	options.verbosity = DBX_VERBOSITY_INFO;
	options.recover = 0;
	options.safe_mode = 0;
	options.delete_deleted = 0;
	options.ignore0 = 0;
	options.debug = 0;
	
	for (const auto& Path : AllFiles)
	{
		std::cout << Path.string() << std::endl;

		if (dbx_t* dbx = dbx_open(reinterpret_cast<const char*>(Path.u8string().c_str()), &options))
		{
			for (int i = 0; i < dbx->message_count; i++)
			{
				unsigned int MessageSize = 0;
				const char* Message = dbx_message(dbx, i, &MessageSize);
				MessageSize = MessageSize;
			}

			dbx_close(dbx);
		}
	}
}
