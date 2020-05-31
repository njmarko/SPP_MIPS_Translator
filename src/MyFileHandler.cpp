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

MyFileHandler::MyFileHandler(std::string folder_path):id_size(ZB_ID_SIZE), zbfile_id_cnt(0)
{
	inFolderPath = folder_path;
	get_filenames(folder_path);
	createProgramFromFilenames();
}

MyFileHandler::MyFileHandler(int argc, char * argv[]):id_size(ZB_ID_SIZE), zbfile_id_cnt(0)
{
	checkCommandlineArgs(argc, argv);
	if (argc == 4)
	{
		makeZeroBytesProgram();
	}
	else if (argc == 5)
	{
		get_filenames(inFolderPath);
		createProgramFromFilenames();
	}

}

std::vector<std::string> MyFileHandler::get_filenames(Path path)
{
	
	//DONE: Add support for reading binary code from filenames (zer0s and ones in ascii format) 
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
			fileNames.push_back(iter->path().string().erase(0, path.string().size() + id_size + 1)); 
			// id size is the number of characters in the the identificator; one is added for the separator
	}

	return fileNames;
}

void MyFileHandler::makeZeroBytesProgram()
{
	zbfile_id_cnt = 0;

	std::ifstream in(inFilePath, std::ios::binary);
	//std::ofstream out("../temp/temp.bin", std::ios::binary);

	// clear the temporary directory where zerobytes program will be stored
	std::experimental::filesystem::remove_all("../temp/zeroBytesProgram/");
	std::experimental::filesystem::create_directory("../temp/zeroBytesProgram/");
	std::stringstream stream;
	char d;
	std::string s; /*= std::string(id_size - std::to_string(zbfile_id_cnt).length(), '0') + std::to_string(zbfile_id_cnt) + "_";*/
	std::string empty = "";
	std::string temp = "";
	while (in >> std::noskipws >> d) {

		if (s.length() > 150)
		{
			s = std::string(id_size - std::to_string(zbfile_id_cnt).length(), '0') + std::to_string(zbfile_id_cnt) + "_" + s;
			std::ofstream out("../temp/zeroBytesProgram/"+ s);
			out.write((char*)&empty,0);
			out.close();
			++zbfile_id_cnt;
			// make new string
			s = "";
		}
		if (zb_type == __BIN__)
		{
			s += std::bitset<8>(d).to_string();
		}
		else if (zb_type == __HEX__)
		{
			// using string strim as a way to get hexadecimal representation of the digit
			stream << std::hex << (int)d;
			temp = stream.str(); // helper variable for making sure all hex numbers have 2 digits
			if (temp.size() < 2)
			{
				temp = "0" + temp; // if the number has only one digit, add a leading zero
			}
			s += temp;
			stream.str(""); // clear the stream
		}
		
		//std::cout << (char)std::stoi(std::bitset<8>(d).to_string(),0,2);
		//out.write((char*)&d, sizeof d);
		//std::cout << (int)d << std::endl;
		//std::cout << s << std::endl;
	}
	s = std::string(id_size - std::to_string(zbfile_id_cnt).length(), '0') + std::to_string(zbfile_id_cnt) + "_" + s;
	std::ofstream out("../temp/zeroBytesProgram/" + s);
	out.write((char*)&empty, 0);
	out.close();
	in.close();

	//unsigned int x;
	//unsigned char b;
	//std::ifstream myFile(inFilePath, std::ios::out | std::ios::binary);
	//myFile.seekg(0, std::ios::beg);
	//while (myFile >> std::noskipws >> b)
	//{
	//	x = (unsigned int)b;
	//	std::cout << std::bitset<8>('Š') << std::endl;
	//}
}

std::string MyFileHandler::createProgramFromFilenames()
{
	int inc = zb_type == __HEX__ ? 2 : 8; // ascii code in hexadeximal is saved using only hexadecimal digits per char
	int base = zb_type == __HEX__ ? 16 : 2; // base for string to integer function
	codeFromFilenames = "";
	for (auto& name : fileNames) {
		for (size_t i = 0; i < name.size(); i+=inc)
		{
			codeFromFilenames += (char)std::stoi(name.substr(i,inc),0, base);
		}
	}
	std::ofstream out("../temp/loadedZeroBytesProgram.txt");
	out << codeFromFilenames;
	out.flush();
	out.close();
	return codeFromFilenames;
}

std::string MyFileHandler::getCodeFromFilename()
{
	return codeFromFilenames;
}

void MyFileHandler::checkCommandlineArgs(int argc, char * argv[])
{
	//char* inFilename/inFolername;
	//char* outFilename;
	//char* hex/bin; // used for reading or writing the code in the filenames
	//char* zerobytes // used to signal that program will be loaded from the zerobyte files in the folder

	//DONE: Add support for debug mode option and option for reading code from filenames
	// [inFoldername][outFilename][hex / bin][zerobytes]

	if (argc < 3 || argc > 5) {
		throw std::runtime_error("Wrong number of input arguments!SuportedFormats: [inFoldername][outFilename] optional:[hex / bin] optional:[zerobytes]\n");
	}
	inFilePath = argv[1];
	if (argc != 5)
	{
		std::regex regExp("^.*\.(mavn)$"); // filename has to end with .mavn
		if (!std::regex_match(inFilePath, regExp)) {
			throw std::runtime_error("Incorrect file extension (It has to be .mavn file): " + inFilePath);
		}
	}
	outFilePath = argv[2];
	if (argc >= 4)
	{
		if (std::string(argv[3]) == "hex")
		{
			zb_type = __HEX__;
		}
		else if (std::string(argv[3]) == "bin")
		{
			zb_type = __BIN__;
		}
		else
		{
			throw std::runtime_error("Wrong type of file specified. Choose either hex or bin: [inFilename] [outFilename] [\"hex\" / \"bin\"][\"zerobytes\"]\n");
		}
	}
	if (argc == 5)
	{
		inFolderPath = argv[1];
		if (std::string(argv[4]) != "zerobytes")
		{
			throw std::runtime_error("Wrong parameter specified. Enter zerobytes to load from folder: [inFilename] [outFilename] [\"hex\" / \"bin\"][\"zerobytes\"]\n");
		}
	}
}

std::string MyFileHandler::getInFilepath()
{
	return inFilePath;
}
