#include "MyFileHandler.h"

void MyFileHandler::visit(SymbolTable & symTab)
{
	//TODO
}

MyFileHandler::MyFileHandler()
{
}

MyFileHandler::MyFileHandler(std::string folder_path)
{
	inFolderPath = folder_path;
	get_filenames(folder_path);
	createProgramFromFilenames();
}

MyFileHandler::MyFileHandler(int argc, char * argv[])
{
	checkCommandlineArgs(argc, argv);
	inFolderPath = inFolderPath;
	get_filenames(inFolderPath);
	createProgramFromFilenames();
}

std::vector<std::string> MyFileHandler::get_filenames(Path path)
{

	namespace stdfs = std::experimental::filesystem;

	fileNames.clear();

	// http://en.cppreference.com/w/cpp/experimental/fs/directory_iterator
	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter)
	{
		// http://en.cppreference.com/w/cpp/experimental/fs/is_regular_file 
		if (stdfs::is_regular_file(*iter)) // comment out if all names (names of directories tc.) are required
			// erases the path part of the full file path,leaving only the filename
			fileNames.push_back(iter->path().string().erase(0, path.string().size()));
	}

	return fileNames;
}

std::string MyFileHandler::createProgramFromFilenames()
{
	codeFromFilenames = "";
	for (auto& name : fileNames) {
		codeFromFilenames += name;
	}
	return codeFromFilenames;
}

std::string MyFileHandler::getCodeFromFilename()
{
	return codeFromFilenames;
}

void MyFileHandler::checkCommandlineArgs(int argc, char * argv[])
{
	//char* inFilename;
	//char* outFilename;
	//char* inFolderPath; // used for reading the code from the filenames

	if (argc < 3 || argc > 4) {
		throw std::runtime_error("Wrong number of input arguments!SuportedFormats: [inFilename] [outFilename] {this one is optional}[inFolderPath]\n");
	}
	inFilePath = argv[1];
	outFilePath = argv[2];
	if (argc == 4)
	{
		inFolderPath = argv[3];
	}
}

std::string MyFileHandler::getInFilepath()
{
	return inFilePath;
}
