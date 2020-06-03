#pragma once
#include <experimental/filesystem> // http://en.cppreference.com/w/cpp/experimental/fs
#include <vector>
#include <string>
#include "IR.h"
#include "SymbolTable.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>
#include <stdio.h>
#include <io.h>
#include "Constants.h"
#include <regex>

/**
* Vector of filename strings. It is used for reading the program code from filenames in the folder
*/
typedef std::vector<std::string> Filenames;

/**
* Filenames used by the experimental library filesystem
*/
typedef std::experimental::filesystem::path Path;

class MyFileHandler:public Visitor {

public:

	/**
	* Visitor class has pure virtual function visit that has to be implemented.
	* Function visit is called by the class that this Visitor visits
	* Here it is used for writing the MIPS code into file
	*/
	void visit(SymbolTable& symTab)override;

	/**
	* Default constructor
	*/
	MyFileHandler();

	/**
	* Constructor that takes the folder path where the files are located.
	* It automatically loads the data from those files
	* @param folder_path path to the folder where the files with code in their name are located
	*/
	MyFileHandler(std::string folder_path);

	/**
	* Constructor that also checks the command line arguments
	* @param argc number of arguments 
	* @param argv value of the arguments
	* @throw runtime_error if the number of arguments is incorrect
	*/
	MyFileHandler(int argc, char* argv[]);

	/**
	* Function that reads the filenames in the folder path and adds them to the vector of strings
	* Solution found at: http://www.cplusplus.com/forum/windows/189681/
	* @param path name
	* @returns vector of strings that contain the filenames
	*/
	Filenames get_filenames(Path path);

	/**
	* Tries to make a zerobytes program that stores all of the .mavn code into the name of the files.
	* Name of those files start with id number and are followed by _ and then data in either bin or hex format
	* There can be multiple files created for one program.
	* Because of that files generated from one program are placed inside one folder
	* Default folder is: ../temp/zeroBytesProgram/
	*/
	void makeZeroBytesProgram();

	/**
	* Combines the code in the filenames into one string
	* it stores the resulting program inside the temporary file:
	* ../temp/loadedZeroBytesProgram.txt
	* @returns single string representation of the program
	*/
	std::string createProgramFromFilenames();

	/**
	* Getter for the string that represents all the loaded code
	* @returns string with all the loaded code
	*/
	std::string getCodeFromFilename();

	/**
	* Checks if there are 3,4 or 5 command line arguments and if they are valid
	* Valid arguments are:
	*	[inFilename.mavn] [outFilename]  for reading the program from the .mavn file and outputing MIPS code
	*	[inFilename.mavn] [outFilename] [hex/bin] same as above, but also creates temporary zerobytes program in temp directory
	*	[inFoldername] [outFilename] [hex/bin] [zerobytes] loads the MAVN program from the zerobytes hex/binary file names in the folder
	*		and creates the MIPS output file.
	* @param argc number of arguments 
	* @param argv value of the arguments
	* @throw runtime_error if the number of arguments is incorrect
		or if the individual arguments are not correct
	*/
	void checkCommandlineArgs(int argc, char* argv[]);

	/**
	* Getter for the inFilepath (first argument of the program)
	* @returns string full path for the in file
	*/
	std::string getInFilepath();

private:
	/**
	* Type of data that is stored in zerobytes program
	* If it is hexadecimal file then the value should be 1
	* For binary file value should be 0
	*/
	int zb_type;

	/**
	* Size of the id number of zerobytes files
	* Used to determine number of leading zeors
	*/
	int id_size;

	/**
	* Id counter for the zero bytes program
	* it is used when a new zero bytes program is created because it can 
	* spread accross multiple files
	*/
	int zbfile_id_cnt;

	/**
	* Name of the folder with the files that contain the code in their names
	*/
	Path folderWithFilenames;

	/**
	* Vector of all the filenames encountered in the folder
	*/
	Filenames fileNames;

	/**
	* Code from all the filenames in the folder in the form of one string
	*/
	std::string codeFromFilenames;

	/**
	* String path to the folder where the files with code in their name is located
	*/
	std::string inFolderPath;

	/**
	* String path to the file where the MIPS .s code will be saved
	*/
	std::string outFilePath;

	/**
	* String path to the flie where the MAVN code is located
	*/
	std::string inFilePath;
};