#include "MyFileHandler.h"

void MyFileHandler::visit(SymbolTable & symTab)
{
	// TODO: Add support for debug mode printing messages

	std::ofstream out(outFilePath);

	for each (std::string fun in symTab.getFunctions())
	{
		out << ".globl\t" << fun << std::endl;
	}
	out << "\n.data\n";
	for each (Variable* var in symTab.getMemVariables())
	{
		out << var->getName() << ":\t\t" << ".word " << var->getValue() << std::endl;
	}

	Instructions::const_iterator cit = symTab.getInstructions().cbegin();
	Instructions::const_iterator end = symTab.getInstructions().cend();
	out << "\n.text\n";

	// if there was were instructions in the text section before any label or function
	while (cit != end  && (*cit)->getParentLabel()=="")
	{
		out << (*cit++)->convertToMIPSString() << std::endl;
	}
	for each (std::pair<std::string,int> var in symTab.getLabels())
	{
		out << var.first << ":" << std::endl;
		if (var.second != -1) // if the label or function has at least one associated instructions
		{
			while (cit != end && (*cit)->getParentLabel() == var.first)
			{
				out << (*cit++)->convertToMIPSString() << std::endl;
			}
		}
	}
	// exit call so the qtspim doesn't compalin
	out << "\tli  $v0, 10\n\tsyscall";

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
	
	//TODO: Add support for reading binary code from filenames (zeors and ones in ascii format) 
	//as well as reading id's of those files(example: 13/01001001....)
	//222 chars limit for google cloud
	//long filenames can be enabled in windows for up to 32000 characters
	
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

	//TODO: Add support for debug mode option and option for reading code from filenames

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
