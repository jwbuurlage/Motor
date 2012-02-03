//TODO: one model needs multiple materials or maybe even multiple meshes

#pragma once
#include "MotorMesh.h"
#include "MotorMaterial.h"

namespace Motor {

	class Model{
	public:
		Model(){ mesh = 0; material = 0; }
		~Model(){
			if( mesh ){
				mesh->release();
			}
			if( material ){
				//material->release();
			}
		};

		void setMesh(Mesh* newMesh){
			if( mesh != newMesh ){
				if( mesh ) mesh->release();
				mesh = newMesh;
				if( mesh ) mesh->addRef();
			}
		}
		Mesh* getMesh() const { return mesh; }

		void setMaterial(Material* newMaterial){
			//TODO: release/addRef
			material = newMaterial;
		}
		Material* getMaterial() const { return material; }

	private:
		Mesh* mesh;
		Material* material;
	};

}