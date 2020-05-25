#include "MyFileHandler.h"

void MyFileHandler::visit(SymbolTable & symTab)
{
	//TODO
}

std::vector<std::string> MyFileHandler::get_filenames(Path path)
{
	namespace stdfs = std::experimental::filesystem;

	std::vector<std::string> filenames;

	// http://en.cppreference.com/w/cpp/experimental/fs/directory_iterator
	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		// http://en.cppreference.com/w/cpp/experimental/fs/is_regular_file 
		if (stdfs::is_regular_file(*iter)) // comment out if all names (names of directories tc.) are required
			// erases the path part of the full file path,leaving only the filename
			filenames.push_back(iter->path().string().erase(0, path.string().size())); 
	}

	return filenames;
}

std::string MyFileHandler::createProgramFromFilenames(Filenames fnames)
{
	for (auto& name : fileNames) {
		codeFromFilename += name;
	}
	return codeFromFilename;
}

std::string MyFileHandler::getCodeFromFilename()
{
	return codeFromFilename;
}
