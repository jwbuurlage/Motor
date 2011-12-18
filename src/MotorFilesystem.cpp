#include "MotorFileSystem.h"
#include <fstream>
#include <algorithm>

Motor::Filesystem filesystemSingleton;

namespace Motor {

	Filesystem::Filesystem()
	{
		initApplicationPath();
	}

	Filesystem::~Filesystem()
	{
		unloadAllFiles();
	}

	Filesystem& Filesystem::getSingleton()
	{
		return filesystemSingleton;
	}

	//getApplicationPath
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	void Filesystem::initApplicationPath()
	{
		char ExePath[MAX_PATH] = {0};
		GetModuleFileName(0, ExePath, MAX_PATH);
		std::string Directory(ExePath);
		applicationPath = Directory.substr(0, Directory.find_last_of('\\') + 1);
		return;
	}
#elif __APPLE__
	std::string Filesystem::initApplicationPath()
	{
		char path[1024];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0){
			std::string Directory(path);
			applicationPath = Directory.substr(0, Directory.find_last_of('/') + 1);
		}
		applicationPath = "./";
		return;
	}
#elif __linux__
	#include <unistd.h>
	std::string Filesystem::initApplicationPath()
	{
		char path[1024] = {0};
		if( readlink("/proc/self/exe", path, 1024) > 0 ){
			std::string Directory(path);
			applicationPath = Directory.substr(0, Directory.find_last_of('/') + 1);
		}
		applicationPath = "./";
		return;
	}
#else
	std::string Filesystem::initApplicationPath()
	{
		applicationPath = "./";
		return;
	}
#endif

	const File* Filesystem::getFile(std::string filename)
	{
		//Note: only works for ascii strings:
		std::transform(filename.begin(), filename.end(), filename.begin(), tolower);

		//TODO:
		//First convert filename to some default format
		//so that when the same file gets requested but
		//with a different syntax, it will only be loaded once
		//Example: "./textures/tex.tga" should be converted to "textures/tex.tga"

		fileIterator loadedFile = loadedFiles.find(filename);
		if( loadedFile != loadedFiles.end() ){
			return loadedFile->second;
		}

		std::string path(applicationPath);
		path.append(filename);
		std::ifstream filestream;
		filestream.open( path.c_str(), std::ios::binary );
		if( filestream.is_open() == false ){
			return 0;
		}

		File* newFile = new File;

		//Get file length
		filestream.seekg(0, std::ios::end);
		newFile->size = (unsigned int)filestream.tellg();
		filestream.seekg(0, std::ios::beg);

		//allocate memory
		newFile->data = new char[newFile->size];

		filestream.read(newFile->data, newFile->size);

		//Add to loadedFiles
		loadedFiles.insert( _fileValueType(filename, newFile) );
		return newFile;
	}

	void Filesystem::unloadFile(const File* file)
	{
		for( fileIterator fileIter = loadedFiles.begin(); fileIter != loadedFiles.end(); ++fileIter ){
			if( file == fileIter->second ){
				loadedFiles.erase(fileIter);
				delete[] file->data;
				delete file;
				break;
			}
		}
	}

	void Filesystem::unloadAllFiles()
	{
		for( fileIterator file = loadedFiles.begin(); file != loadedFiles.end(); ++file ){
			delete[] file->second->data;
			delete file->second;
		}
		loadedFiles.clear();
	}

}
