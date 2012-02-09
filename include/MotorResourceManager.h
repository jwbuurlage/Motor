#pragma once
#include <string>
#include <map>

namespace Motor {

	template <typename T> class ResourceManager {
	public:
		ResourceManager(){};
		virtual ~ResourceManager(){ unloadAll(); }

		//Will load the resource if not already loaded
		T* getResource( const char* filename )
		{
			ResourceContainer::iterator iter = resources.find(filename);
			if( iter != resources.end() )
				return (T*)(iter->second);
			return loadResource(filename);
		}

		void unloadAll()
		{
			for( ResourceContainer::iterator iter = resources.begin(); iter != resources.end(); ++iter ){
				T* resource = (T*)iter->second;
				delete resource; //This will call the deconstructor
			}
			resources.clear();
		}

		bool resourceLoaded( const char* name ){
			ResourceContainer::iterator iter = resources.find(name);
			return (iter != resources.end());
		}

	private:
		typedef std::multimap<std::string,void*> ResourceContainer;
		ResourceContainer resources;
		
	protected:
		//Must be implemented by subclass and use addResource to add the resource
		virtual T* loadResource( const char* filename )=0;

		void addResource( const char* name, T* res ){
			resources.insert( ResourceContainer::value_type( name, res ) );
		}
	};
}
