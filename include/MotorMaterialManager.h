#pragma once
#include "MotorSingleton.h"
#include "MotorResourceManager.h"
#include "MotorMaterial.h"

namespace Motor {

	class MaterialManager : public Singleton<MaterialManager>, public ResourceManager<Material> {
	public:
		MaterialManager();
		~MaterialManager();

		int initialize();
		void cleanup();

		const Material* getMaterial( const char* filename ){ return getResource(filename); }

	private:
		const Material* loadResource( const char* filename );

		void loadDefaultMaterial(); //generates default material for when file can not be found
	};
}