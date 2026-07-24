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

	class UnDBXDatabase
	{
	private:
		struct Message
		{
			dbx_type_t Type = DBX_TYPE_UNKNOWN;
			std::u8string Text;
			std::size_t TextHash = 0;

			bool operator == (const Message& Other) const
			{
				return Type == Other.Type && Text == Other.Text;
			}
		};

		std::vector<Message> m_AllMessages;

	public:
		void AddMessage(dbx_type_t Type, const char* RawText, unsigned int RawTextSize)
		{
			if (Type == DBX_TYPE_EMAIL || Type == DBX_TYPE_OE4)
			{
				Message msg = { Type, std::u8string(reinterpret_cast<const char8_t*>(RawText)) };
				msg.TextHash = std::hash<std::u8string>{}(msg.Text);

				// If a duplicate message is found, ignore it.
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

	if (argc < 2)
	{
		return -1;
	}

	UnDBXDatabase db;

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
				db.AddMessage(dbx->type, Message, MessageSize);
			}

			dbx_close(dbx);
		}
	}

	db.PrintStats();
}
