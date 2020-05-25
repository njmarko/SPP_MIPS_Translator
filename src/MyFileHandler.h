#pragma once
#include <experimental/filesystem> // http://en.cppreference.com/w/cpp/experimental/fs
#include <vector>
#include <string>
#include "IR.h"
#include "SymbolTable.h"

/*
* Vector of filename strings. It is used for reading the program code from filenames in the folder
*/
typedef std::vector<std::string> Filenames;

/*
* Filenames used by the experimental library filesystem
*/
typedef std::experimental::filesystem::path Path;

class MyFileHandler:public Visitor {

public:

	/*
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	* Here it is used for writing the code into file
	*/
	void visit(SymbolTable& symTab)override;

	/*
	* Default constructor
	*/
	MyFileHandler();

	/*
	* Constructor that takes the folder path where the files are located.
	* It automatically loads the data from those files
	* @param path to the folder where the files with code in their name are located
	*/
	MyFileHandler(std::string folder_path);

	/*
	* Constructor that also checks the command line arguments
	* @param number of arguments 
	* @param value of the arguments
	* @throws runtime_error if the number of arguments is incorrect
	*/
	MyFileHandler(int argc, char* argv[]);

	/*
	* Function that reads the filenames in the folder path and adds them to the vector of strings
	* Solution found at: http://www.cplusplus.com/forum/windows/189681/
	* @param path name
	* @returns vector of strings that contain the filenames
	*/
	Filenames get_filenames(Path path);

	/*
	* Combines the code in the filenames into one string
	* @param vector of filename strings
	* @returns single string representation of the program
	*/
	std::string createProgramFromFilenames();

	/*
	* Getter for the string that represents all the loaded code
	* @returns string with all the loaded code
	*/
	std::string getCodeFromFilename();

	/*
	* Checks if there are 3 or 4 command line arguments
	* @param number of arguments 
	* @param value of the arguments
	* @throws runtime_error if the number of arguments is incorrect
	*/
	void checkCommandlineArgs(int argc, char* argv[]);

	/*
	* Getter for the inFilepath (first argument of the program)
	*/
	std::string getInFilepath();

private:

	/*
	* Name of the folder with the files that contain the code in their names
	*/
	Path folderWithFilenames;

	/*
	* Vector of all the filenames encountered in the folder
	*/
	Filenames fileNames;

	/*
	* Code from all the filenames in the folder in the form of one string
	*/
	std::string codeFromFilenames;

	/*
	* String path to the folder where the files with code in their name is located
	*/
	std::string inFolderPath;

	/*
	* String path to the file where the MIPS .s code will be saved
	*/
	std::string outFilePath;

	/*
	* String path to the flie where the MAVN code is located
	*/
	std::string inFilePath;
};