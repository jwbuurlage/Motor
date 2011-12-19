//Singleton
//Virtual file system
//Use this for all file access
//All paths are relative to the applications directory
//It will make sure all file data will be followed by at least one 0 in memory so text files are 0-terminated.

//TODO: Functionality to iterate through (virtual) directory tree

#pragma once
#include <string>
#include <map>

namespace Motor {

	struct File{
		char* data;
		unsigned int size;
	};
	typedef const File* pFile;

	class Filesystem
	{
	public:
		Filesystem();
		~Filesystem();

		static Filesystem& getSingleton();

		//With / appended
		const std::string getApplicationPath(){ return applicationPath; }

		//Will open and load the file if not already opened
		//and close the handle to the file
		//Returns pointer to file in memory or 0 on error
		const File* getFile(std::string filename);

		//Will unload the file from memory
		void unloadFile(const File* file);
		void unloadAllFiles();

	private:
		std::string applicationPath;
		void initApplicationPath();

		//TODO: Have some virtual directory-tree like structure to be able to
		//iterate through all files in a directory.
		std::map<std::string,File*> loadedFiles;
		typedef std::map<std::string,File*>::iterator fileIterator;
		typedef std::map<std::string,File*>::value_type _fileValueType;
	};

}