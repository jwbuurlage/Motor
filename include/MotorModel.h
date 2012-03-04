//TODO: one model needs multiple materials or maybe even multiple meshes

#pragma once
#include "MotorMesh.h"
#include "MotorMaterial.h"

namespace Motor {
    struct AnimState;

	class Model{
	public:
		Model(){ mesh = 0; material = 0; animated = false; modelType = MODELTYPE_DEFAULT; }
		virtual ~Model(){
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
        
        void update(float timeElapsed) { return; }

		void setMaterial(Material* newMaterial){
			//TODO: release/addRef
			material = newMaterial;
		}
		Material* getMaterial() const { return material; }
        
        virtual void updateAnimationState(struct AnimState* state, float timeElapsed) const { };
        virtual void setAnimation(struct AnimState* state, int _type) const { };
        
        virtual int verticesPerFrame() const { return 0; }
        
        bool isAnimated() const { return animated; }
        bool animated;

		enum {MODELTYPE_DEFAULT, MODELTYPE_MD2, MODELTYPE_COLLADA} modelType;

	protected:
		Mesh* mesh;
		Material* material;
	};

}