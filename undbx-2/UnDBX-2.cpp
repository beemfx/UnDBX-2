// UnDBX-2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "dbxread.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

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

	class UnDBXDatabase
	{
	private:
		struct Message
		{
			dbx_type_t Type = DBX_TYPE_UNKNOWN;
			std::string Text;
			std::size_t TextHash = 0;
			fs::path Folder;

			bool operator == (const Message& Other) const
			{
				return Type == Other.Type && Text == Other.Text;
			}
		};

		std::vector<Message> m_AllMessages;

	public:
		void AddMessage(dbx_type_t Type, const fs::path& DBXFilename, const char* RawText, unsigned int RawTextSize)
		{
			if (Type == DBX_TYPE_EMAIL || Type == DBX_TYPE_OE4)
			{
				Message msg = { Type, std::string(RawText) };
				msg.TextHash = std::hash<std::string>{}(msg.Text);

				msg.Folder = DBXFilename.filename();
				msg.Folder.replace_extension();

				// If a duplicate message is found, ignore it. (This will only
				// keep the first folder it was found in.)
				if (!Contains(msg))
				{
					m_AllMessages.push_back(msg);
				}
			}
		}

		void PrintStats()
		{
			std::cout << "Message Count: " << m_AllMessages.size() << std::endl;
		}

		void SaveFiles(const fs::path& DumpDir)
		{
			if (!fs::exists(DumpDir) || !fs::is_directory(DumpDir))
			{
				std::cout << "Invalid dump directory. " << std::endl;
				return;
			}


			for (std::size_t i = 0; i < m_AllMessages.size(); i++)
			{
				const Message& Msg = m_AllMessages[i];

				const fs::path FullFolderPath = DumpDir / Msg.Folder;

				if (!fs::exists(FullFolderPath) && !fs::is_directory(FullFolderPath))
				{
					fs::create_directories(FullFolderPath);
				}

				const fs::path Filename = std::format("{:05}.eml", i);
				const fs::path FullPath = FullFolderPath / Filename;

				std::ofstream OutFile(FullPath);

				if (OutFile.is_open())
				{
					OutFile << Msg.Text;
					OutFile.close();
				}
			}
		}

	private:
		bool Contains(const Message& Msg)
		{
			for (const auto& CmpMsg : m_AllMessages)
			{
				if (CmpMsg.TextHash == Msg.TextHash && CmpMsg == Msg)
				{
					return true;
				}
			}

			return false;
		}
	};
}

int main(int argc, char** argv)
{
	using namespace UnDBX2;

	std::cout << "UnDBX2" << std::endl;

	if (argc < 3)
	{
		return -1;
	}

	UnDBXDatabase db;

	const fs::path DirToSearch = argv[1];
	const fs::path DumpDir = argv[2];

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
				db.AddMessage(dbx->type, Path, Message, MessageSize);
			}

			dbx_close(dbx);
		}
	}

	db.PrintStats();
	db.SaveFiles(DumpDir);
}
