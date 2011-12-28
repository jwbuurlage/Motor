#pragma once

namespace Motor {

	template <typename T> class Singleton{
	private:
		//Private copy constructor and assignment to prevent copies
		Singleton(const Singleton<T>&);
		Singleton& operator=(const Singleton<T>&);
	protected:
		static T* singleton;
	public:
		Singleton(){
			singleton = static_cast<T*>(this);
		}
		~Singleton(){
			singleton = 0;
		}

		static T& getSingleton(){ return (*singleton); }
	};

}
