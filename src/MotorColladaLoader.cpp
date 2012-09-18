#pragma warning (disable : 4355)
#include <dae.h>
#include <dom/domCOLLADA.h>

#include "MotorMaterialManager.h"
#include "MotorColladaLoader.h"
#include "MotorLogger.h"
#include <map>
#include <queue>

namespace Motor{

	//The reason that this is not in the ColladaLoader class
	//is that we would have to include the dom header files in the header
	//which cause reeeeallly long compilation times
	void processMatrix(domFloat4x4& inMatrix, mat& outMatrix, int offset = 0){
		for(int i = 0; i < 4; ++i)
			for(int j = 0; j < 4; ++j)
				outMatrix.e[i][j] = (float)inMatrix[offset + j*4 + i];
	}

	int getJointIndexByName(ColladaModel* model, const char* name){
		for( std::vector<SkeletonJoint>::iterator joint = model->joints.begin(); joint != model->joints.end(); ++joint ){
			if( joint->nodeSid == name ) return joint->index;
		}
		return -1;
	}

	ColladaModel* ColladaLoader::loadModel(const char* filename, const char* fileData){
		LOG_DEBUG("Loading Collada file '" << filename << "'");

		DAE dae;
		//We have to supply a filename for when we would want to write changes back to disk
		domCOLLADA* root = dae.openFromMemory(filename, fileData);
		if( root == 0 ){
			LOG_WARNING(filename << " is not a valid Collada file.");
			return 0;
		}

		//
		// The correct approach would be to look in visual_scenes and check for all geometries
		// that are in the scene. We, however, just take the first mesh and disregard everything else
		//
		domLibrary_geometries_Array library_geometries = root->getLibrary_geometries_array();
		if( library_geometries.getCount() == 0 ){
			LOG_WARNING("No geometry data in " << filename);
			return 0;
		}
		domGeometry_Array geometries = library_geometries[0]->getGeometry_array();
		if( geometries.getCount() == 0 ){
			LOG_WARNING("No geometry data in " << filename);
			return 0;
		}
		if( geometries.getCount() > 1 ){
			LOG_WARNING("There are " << geometries.getCount()  << " meshes in " << filename << ". Only the first will be loaded.");
		}
		
		domGeometry* geometry = geometries[0]; //We will use this variable later for animation
		domMesh* mesh = geometry->getMesh();

		//Allocate the ColladaModel object
		model = new ColladaModel;

		//
		// Step 1. Check if there is a skeleton present
		//
		model->skeletonEnabled = false;
		//This data will be generated by step 1 and used at step 3 (geometry loading)
		struct VertexJointStruct{
			int indices[4];
			float weights[4];
		};
		std::vector<VertexJointStruct> vertexJointInfo;

		//Now find the skeleton
		domSkin* skeletonSkin = 0;
		domNode* skeletonRootJoint = 0;
		domLibrary_visual_scenes_Array library_visual_scenes = root->getLibrary_visual_scenes_array();
		if( library_visual_scenes.getCount() > 0 ){
			domVisual_scene_Array visual_scenes = library_visual_scenes[0]->getVisual_scene_array();
			if( visual_scenes.getCount() > 0 ){
				domNode_Array node_array = visual_scenes[0]->getNode_array();
				for( unsigned int i = 0; i < node_array.getCount(); ++i ){
					domNode* node = node_array[i];
					if( node->getType() != NODETYPE_NODE ) continue;
					if( node->getInstance_controller_array().getCount() == 0 ) continue;
					domInstance_controller* instance_controller = node->getInstance_controller_array()[0];
					daeElement* elem = instance_controller->getUrl().getElement();
					if( elem == 0 ) continue;
					domSkin* skin = ((domController*)elem)->getSkin();
					elem = skin->getSource().getElement();
					if( !elem ) continue;
					if( ((domGeometry*)elem) != geometry ) continue; //not the geometry that we loaded above
					domInstance_controller::domSkeleton_Array skeletons = instance_controller->getSkeleton_array();
					for( unsigned int j = 0; j < skeletons.getCount(); ++j ){
						daeElement* elem = skeletons[0]->getValue().getElement();
						if( elem ){
							domNode* skeletonNode = (domNode*)elem;
							if( skeletonNode->getType() == NODETYPE_JOINT ){
								skeletonRootJoint = skeletonNode;
								skeletonSkin = skin;
								break;
							}
						}
					}
					if( skeletonRootJoint != 0 ) break;
				}
			}
		}
		//Skeleton found, parse all matrices
		if( skeletonRootJoint && skeletonSkin ){
			//Get bind shape matrix
			if( skeletonSkin->getBind_shape_matrix() )
				processMatrix(skeletonSkin->getBind_shape_matrix()->getValue(), model->bindShapeMatrix);

			//Get the whole skeleton (recursive function)
			SkeletonJoint identityJoint;
			identityJoint.index = 0;
			identityJoint.parentIndex = -1;
			identityJoint.worldMatrix.setIdentity();
			identityJoint.inverseBindMatrix.setIdentity();
			identityJoint.bindMatrix.setIdentity();
			model->joints.push_back(identityJoint);
			processJoint(skeletonRootJoint, identityJoint.worldMatrix, 0);

			std::vector<int> jointIndexList;

			//Get the inverse bind matrices for each joint and also fill above array
			domSkin::domJoints *joints = skeletonSkin->getJoints();
			domSource* jointNamesSource = 0;
			domSource* jointMatricesSource = 0;
			for( size_t i = 0; i < joints->getInput_array().getCount(); ++i ){
				domInputLocal* input = joints->getInput_array()[i];
				daeElement* elem = input->getSource().getElement();
				if( !elem ){
					LOG_WARNING("Source URI of semantic '" << input->getSemantic() << "' is invalid");
				}else{
					if( strcmp(input->getSemantic(),"JOINT") == 0 ){
						jointNamesSource = (domSource*)elem;
					}else if( strcmp(input->getSemantic(),"INV_BIND_MATRIX") == 0 ){
						jointMatricesSource = (domSource*)elem;
					}else{
						LOG_INFO("Skeleton joint attributes contain unused source: " << input->getSemantic());
					}
				}
			}
			if( !jointNamesSource || !jointMatricesSource ){
				LOG_WARNING("Inverse bind matrices for joints not found");
			}else{
				domName_array* jointNameArray = jointNamesSource->getName_array();
				domFloat_array* jointMatrixArray = jointMatricesSource->getFloat_array();
				domUint nameCount = jointNamesSource->getTechnique_common()->getAccessor()->getCount();
				domUint matrixCount = jointMatricesSource->getTechnique_common()->getAccessor()->getCount();
				domUint nameStride = jointNamesSource->getTechnique_common()->getAccessor()->getStride();
				domUint matrixStride = jointMatricesSource->getTechnique_common()->getAccessor()->getStride();
				if( !jointNameArray || !jointMatrixArray || nameStride != 1 || matrixStride != 16 || nameCount != matrixCount ){
					LOG_WARNING("Inverse bind matrices have invalid sources");
				}else{
					for( size_t i = 0; i < nameCount; ++i ){
						int index = getJointIndexByName(model, jointNameArray->getValue()[i]);
						jointIndexList.push_back( index );
						if( index == -1 ){
							LOG_WARNING("Bone '" << jointNameArray->getValue()[i] << "' not found. Bone is probably not in root skeleton");
						}else{
							processMatrix(jointMatrixArray->getValue(), model->joints[index].inverseBindMatrix, i*16);
						}
					}
				}

			}

			//Now for each vertex get the attached joints
			domSkin::domVertex_weights* vertexWeights = skeletonSkin->getVertex_weights();
			domSource* weightsSource = 0;
			domSource* jointsSource = 0;
			int skeletonVstride = 0;
			int skeletonJointOffset;
			int skeletonWeightOffset;

			for( size_t i = 0; i < vertexWeights->getInput_array().getCount(); ++i ){
				domInputLocalOffset* input = vertexWeights->getInput_array()[i];
				daeElement* elem = input->getSource().getElement();

				if( !elem ){
					LOG_WARNING("Source URI of semantic '" << input->getSemantic() << "' is invalid");
				}else{
					if( strcmp(input->getSemantic(),"JOINT") == 0 ){
						jointsSource = (domSource*)elem;
						skeletonJointOffset = (unsigned int)input->getOffset();
					}else if( strcmp(input->getSemantic(),"WEIGHT") == 0 ){
						weightsSource = (domSource*)elem;
						skeletonWeightOffset = (unsigned int)input->getOffset();
					}else{
						LOG_INFO("Skeleton vertex weights contain unused source: " << input->getSemantic());
					}
				}
				if( (input->getOffset() + 1) > skeletonVstride )
					skeletonVstride = (unsigned int)input->getOffset() + 1;
			}

			if( !jointsSource || !weightsSource ){
				LOG_WARNING("Joint or weight sources for vertex weights not found");
			}else{
				domSkin::domVertex_weights::domVcount *skeletonVcounts = vertexWeights->getVcount();
				domSkin::domVertex_weights::domV *skeletonV = vertexWeights->getV();
				domUint skeletonVertexCount = vertexWeights->getCount();
				domName_array *jointNames = jointsSource->getName_array();

				int currentVoffset = 0;
				VertexJointStruct vertJoint = {0};
				for( int i = 0; i < skeletonVertexCount; ++i ){
					int boneCount = (int)skeletonVcounts->getValue()[i];
					float totalWeight = 0.0f; //normalisation
					for( int j = 0; j < boneCount && j < 4; j++ ){
						int jointOffset = (int)skeletonV->getValue()[currentVoffset + skeletonJointOffset];
						int weightOffset = (int)skeletonV->getValue()[currentVoffset + skeletonWeightOffset];
						vertJoint.indices[j] = jointIndexList[jointOffset];
						vertJoint.weights[j] = (float)weightsSource->getFloat_array()->getValue()[weightOffset];
						totalWeight += vertJoint.weights[j];
					}
					totalWeight = 1.0f/totalWeight;
					for( int j = 0; j < boneCount && j < 4; j++ ){
						vertJoint.weights[j] *= totalWeight;
					}
					//If bonecount is less than 4 fill the remaining indices with -1
					for( int j = boneCount; j < 4; j++ ){
						vertJoint.indices[j] = 0;
						vertJoint.weights[j] = 0.0f;
					}

					vertexJointInfo.push_back(vertJoint);

					currentVoffset += boneCount * skeletonVstride;
				}

				model->skeletonEnabled = true;
			}
		}
		//If skeleton parsing failed somewhere, delete all the joints that we may have made
		if( !model->skeletonEnabled ){
			model->joints.clear();
		}else{
			LOG_DEBUG("Model contains " << model->joints.size() << " joints.");
		}

		//
		// Step 2: Load animatins
		//
		model->animated = false;
		if( model->skeletonEnabled ){
			domLibrary_animations_Array library_animations_array = root->getLibrary_animations_array();
			if( library_animations_array.getCount() > 0 ){
				model->animated = true;
				domAnimation_Array animations_array = library_animations_array[0]->getAnimation_array();
				for( size_t i = 0; i < animations_array.getCount(); ++i ){
					domAnimation* animation = animations_array[i];
					for( size_t j = 0; j < animation->getChannel_array().getCount(); ++j ){
						domElement* elem = animation->getChannel_array()[j]->getSource().getElement();
						if( elem == 0 ) continue;
						domSampler* sampler = (domSampler*)elem;
						std::string animationTarget = animation->getChannel_array()[j]->getTarget();
						size_t slash = animationTarget.find_first_of('/');
						size_t seperator = animationTarget.find_first_of("(");
						if( slash != std::string::npos && seperator != std::string::npos && seperator > slash ){
							//size_t dotPos = animationTarget.find_first_of('.');
							//size_t parenthesisPos = animationTarget.find_first_of('(');
							std::string jointID = animationTarget.substr(0,slash);
							std::string matrixSID = animationTarget.substr(slash+1, seperator - slash - 1);
							std::string matrixElement = animationTarget.substr(seperator);
							if( matrixElement.length() == 6 ){ //matrixElement should be "(c)(r)"
								int col, row;
								col = matrixElement[1] - '0';
								row = matrixElement[4] - '0';
								if( col >= 0 && col <= 3 && row >= 0 && row <= 3 ){
									//Find the joint
									for( std::vector<SkeletonJoint>::iterator joint = model->joints.begin(); joint != model->joints.end(); ++joint ){
										if( joint->nodeID != jointID ) continue;
										if( joint->matrixSid != matrixSID ) continue;
										processAnimation(&(*joint), sampler, col*4+row);
										break;
									}
								}
							}else{
								LOG_WARNING("Animation target is not of form 'transform(c)(r)' but '" << matrixElement << "'.");
							}
						}else{
							LOG_WARNING("Animation target is of unkown form: " << animationTarget);
						}
					}
				}
				//for(size_t i = 0; i < model->joints.size(); ++i){
				//	if( !model->joints[i].keyFrames.empty() ){
				//		size_t count = model->joints[i].keyFrames.size();
				//		Logger::getSingleton() << Logger::DEBUG << "Joint " << i << " has " << count << " keyframes. First and last keyframe:\n";
				//		Logger::getSingleton() << model->joints[i].keyFrames[0].bindMatrix << '\n';
				//		if( count > 1 ) Logger::getSingleton() << model->joints[i].keyFrames[count].bindMatrix;
				//		Logger::getSingleton() << endLog;
				//	}
				//}
			}
		}

		//
		// Step 3: Load the geometry
		//

		//This is a mesh with several submeshes.
		//The submeshes can be of several forms: triangles, triangle fans, polygons, polylists etc
		//Currently we only support triangle submeshes.

		domTriangles_Array triangles_array = mesh->getTriangles_array();
		if( triangles_array.getCount() == 0 ){
			LOG_DEBUG("File does not contain geometry stored as triangles. Please export the geometry as triangles.");
			return 0;
		}

		//Now since several submeshes will usually share the same source data (but not always)
		//we will first record a list of sources that are used
		//so later we can put all of those in big vertex buffer

		struct submeshInfo{
			domSource* position_source;
			domSource* normal_source;
			domSource* texcoord_source;
			//Offsets into the index buffers
			unsigned int position_offset;
			unsigned int normal_offset;
			unsigned int texcoord_offset;
			domP* indexBuffer;
			unsigned int indexBufferStride;
			unsigned int indexCount; //indexCount * indexBufferStride = amount of values
			//Material pointer
		};

		std::vector<submeshInfo> subMeshList;
		
		//Loop through all submeshes stored in triangles format

		Logger& log = Logger::getSingleton();
		log << Logger::DEBUG;

		for( size_t i = 0; i < triangles_array.getCount(); ++i ){
			domTriangles* triangles = triangles_array[i];
			log << "- domTriangles: triangles=" << triangles->getCount() << " \t material = " << triangles->getMaterial() << "\n";

			struct submeshInfo subMesh = {0};
			//Loop through the input data that is supplied with this modelpart
			for( size_t j = 0; j < triangles->getInput_array().getCount(); ++j ){
				domInputLocalOffset* input = triangles->getInput_array()[j];
				daeElement* elem = input->getSource().getElement();
				if( !elem ){
					log << "- + WARNING: Source URI of semantic '" << input->getSemantic() << "' is invalid!\n";
				}else{
					if( strcmp(input->getSemantic(),"VERTEX") == 0 ){
						subMesh.position_offset = (unsigned int)input->getOffset();
						//Collada files have a very very weird construction here
						//Look it up online for more info
						domVertices* vertices = (domVertices*)elem;
						if( vertices->getInput_array().getCount() > 0 ){
							domInputLocal* input2 = vertices->getInput_array()[0];
							daeElement* elem2 = input2->getSource().getElement();
							if( elem2 ){
								subMesh.position_source = (domSource*)elem2;
							}
						}
					}else if( strcmp(input->getSemantic(),"NORMAL") == 0 ){
						subMesh.normal_source = (domSource*)elem;
						subMesh.normal_offset = (unsigned int)input->getOffset();
					}else if( strcmp(input->getSemantic(),"TEXCOORD") == 0 ){
						subMesh.texcoord_source = (domSource*)elem;
						subMesh.texcoord_offset = (unsigned int)input->getOffset();
					}else{
						log << "- + contains unused source: " << input->getSemantic() << "\n";
					}
				}
				if( (input->getOffset() + 1) > subMesh.indexBufferStride )
					subMesh.indexBufferStride = (unsigned int)input->getOffset() + 1;
			}
			subMesh.indexBuffer = triangles->getP();
			subMesh.indexCount = subMesh.indexBuffer->getValue().getCount() / subMesh.indexBufferStride;

			if( subMesh.position_source ){
				subMeshList.push_back(subMesh);
			}else{
				log << "- + Not all required data found for this submesh.\n";
			}
		}
				
		log << endLog;
		
		//All submeshes were processed, meaning we have a list of all sources (the float data)
		//Now construct a vertex buffer that holds all these submeshes

		//If the index buffer was interleaved (different indices for position/normals/texcoords)
		//then we need to convert this to a 'normal' index buffer
		//For this conversion we will not know how big the needed buffer is
		//So we will assume that not a single index is shared and allocate
		//a buffer big enough for that

		//In the final buffer we want 8 floats per vertex: positions, normals and texture coordinates
		//If skeletons are present however, we want more
		unsigned int VERTEX_SIZE = 8;
		//When there is a skeleton, we also need
		//float4 - 4  floats to specify 4 bone indices. Yes you would need integers but vertex attibutes MUST be floats
		//float4 - 4 floats to specify 4 bone weights
		//So that makes 8 'floats' since an integer is just as big as a float
		if( model->skeletonEnabled ) VERTEX_SIZE += 8;

		unsigned int indexBufferSize = 0;
		unsigned int vertexBufferMaxSizeNeeded = 0;

		for( std::vector<submeshInfo>::iterator subMesh = subMeshList.begin(); subMesh != subMeshList.end(); ++subMesh ){
			indexBufferSize += subMesh->indexCount;
		}
		vertexBufferMaxSizeNeeded += indexBufferSize * VERTEX_SIZE;

		GLfloat* vertexBuffer = new GLfloat[vertexBufferMaxSizeNeeded];
		GLushort* indexBuffer = new GLushort[indexBufferSize];

		struct triplet{
			unsigned short pos, norm, tex;
			bool operator< (const triplet& right) const{
				if( pos < right.pos ) return true;
				else if( pos > right.pos ) return false;
				else{
					if( norm < right.norm ) return true;
					else if( norm > right.norm ) return false;
					else{
						if( tex < right.tex ) return true;
						else return false;
					}
				}
			}
		};

		std::map<triplet,GLushort> tripletList;

		unsigned int currentIndexOffset = 0;
		unsigned int currentVertexOffset = 0;
		//Now fill the buffers and save ColladaSubmesh
		for( std::vector<submeshInfo>::iterator subMesh = subMeshList.begin(); subMesh != subMeshList.end(); ++subMesh ){
			ColladaSubmesh outputInfo;
			outputInfo.vertexOffset = currentVertexOffset * sizeof(GLfloat);
			outputInfo.indexOffset = currentIndexOffset * sizeof(GLushort);
			outputInfo.indexCount = subMesh->indexCount;
			model->subMeshes.push_back(outputInfo);

			unsigned int positionStride = (unsigned int)subMesh->position_source->getTechnique_common()->getAccessor()->getStride();
			unsigned int normalStride = 0, texcoordStride = 0;
			if( subMesh->normal_source ) normalStride = (unsigned int)subMesh->normal_source->getTechnique_common()->getAccessor()->getStride();
			if( subMesh->texcoord_source) texcoordStride = (unsigned int)subMesh->texcoord_source->getTechnique_common()->getAccessor()->getStride();

			GLushort* indexPtr = &indexBuffer[currentIndexOffset];
			GLfloat* vertexPtr = &vertexBuffer[currentVertexOffset];
			currentIndexOffset += outputInfo.indexCount;

			//Now walk through the index buffer in the file
			//If we encounter a new index triplet, then save a new vertex in the vertex buffer
			//If we encounter a triplet that we have seen already, then do not write a new
			//vertex to the vertex buffer, but save the old index
			tripletList.clear();
			GLushort newIndex = 0;
			for( unsigned int i = 0; i < subMesh->indexBuffer->getValue().getCount(); i += subMesh->indexBufferStride ){
				triplet offsets;
				unsigned short positionIndex = (unsigned short)subMesh->indexBuffer->getValue()[i+subMesh->position_offset];
				offsets.pos = positionStride*positionIndex;
				offsets.norm = normalStride*(unsigned short)subMesh->indexBuffer->getValue()[i+subMesh->normal_offset];
				offsets.tex = texcoordStride*(unsigned short)subMesh->indexBuffer->getValue()[i+subMesh->texcoord_offset];
				
				bool tripletExisted = false;
				unsigned short oldIndex;
				std::map<triplet,GLushort>::iterator existingTriplet = tripletList.find(offsets);
				if( existingTriplet != tripletList.end() ){
					oldIndex = existingTriplet->second;
					tripletExisted = true;
				}

				if( tripletExisted == false ){
					//Index did not exist yet. Create new vertex in vertex buffer and create new index
					*vertexPtr++ = (GLfloat)subMesh->position_source->getFloat_array()->getValue()[offsets.pos+0]; //by default
					*vertexPtr++ = (GLfloat)subMesh->position_source->getFloat_array()->getValue()[offsets.pos+1]; //Z-axis
					*vertexPtr++ = (GLfloat)subMesh->position_source->getFloat_array()->getValue()[offsets.pos+2]; //was up
					if( subMesh->normal_source ){
						*vertexPtr++ = (GLfloat)subMesh->normal_source->getFloat_array()->getValue()[offsets.norm+0];
						*vertexPtr++ = (GLfloat)subMesh->normal_source->getFloat_array()->getValue()[offsets.norm+1];
						*vertexPtr++ = (GLfloat)subMesh->normal_source->getFloat_array()->getValue()[offsets.norm+2];
					}else{ //TODO: Calculate normals if they are not in given
						*vertexPtr++ = 0.0f;
						*vertexPtr++ = 1.0f;
						*vertexPtr++ = 0.0f;
					}
					if( subMesh->texcoord_source ){
						*vertexPtr++ = (GLfloat)subMesh->texcoord_source->getFloat_array()->getValue()[offsets.tex+0];
						*vertexPtr++ = 1.0f-(GLfloat)subMesh->texcoord_source->getFloat_array()->getValue()[offsets.tex+1];
					}else{
						*vertexPtr++ = 0.0f;
						*vertexPtr++ = 0.0f;
					}
					if( model->skeletonEnabled ){
						//add indices and weights.
						for(int j = 0; j < 4; ++j) *vertexPtr++ = (float)vertexJointInfo[positionIndex].indices[j];
						for(int j = 0; j < 4; ++j) *vertexPtr++ = vertexJointInfo[positionIndex].weights[j];
					}
					currentVertexOffset += VERTEX_SIZE;
					tripletList.insert(std::pair<triplet,GLushort>(offsets, newIndex));
					*indexPtr = newIndex;
					++newIndex;
				}else{
					*indexPtr = oldIndex;
				}
				++indexPtr;
			}
			
		}

		//The final size of the vertex buffer will probably be less then the size we allocated
		//So we only pass the first part to OpenGL
		//The part that has been filled is currentVertexOffset floats.

		//TODO: Add to meshmanager!!!!! it is untracked now! memory leak
		Mesh* modelMesh = new Mesh;
		model->setMesh(modelMesh);

		glGenBuffers(1, &(modelMesh->vertexBuffer));
		glGenBuffers(1, &(modelMesh->indexBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, modelMesh->vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelMesh->indexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*currentVertexOffset, vertexBuffer, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexBufferSize, indexBuffer, GL_STATIC_DRAW);

		delete[] vertexBuffer;
		delete[] indexBuffer;

		modelMesh->hasColor = false;
		modelMesh->hasNormal = true;
		//modelMesh->vertexCount = ??;
		modelMesh->vertexBufferSize = sizeof(GLfloat)*currentVertexOffset;
		modelMesh->primitiveType = GL_TRIANGLES;
		modelMesh->dimension = 3;
		modelMesh->stride = model->getVBOstride();
		modelMesh->hasIndexBuffer = true;
		modelMesh->indexBufferSize = sizeof(GLushort)*indexBufferSize;
		//modelMesh->indexCount = ??;


		//TODO: for every submesh, look up the material
		//then set all material values, and find the seperate textures
		Motor::Material* mat = MaterialManager::getSingleton().getMaterial("default");
		domLibrary_images_Array library_images = root->getLibrary_images_array();
		if( library_images.getCount() > 0 ){
			domImage_Array images = library_images[0]->getImage_array();
			if( images.getCount() > 0 ){
				domImage::domInit_from* init_from = images[0]->getInit_from();
				if( init_from ){ //If it points to a file
					std::string imageFilename;
					imageFilename = "textures/";
					imageFilename.append(init_from->getValue().originalStr());
					mat = MaterialManager::getSingleton().getMaterial(imageFilename.c_str());
				}
			}
		}

		model->setMaterial( mat );

		return model;
	}

	void ColladaLoader::processJoint(class domNode* joint, const class mat& parentMatrix, short parentIndex){
		if( joint->getMatrix_array().getCount() != 1 ){
			LOG_WARNING("- Joint " << joint->getName() << " has " << joint->getMatrix_array().getCount() << " matrices insteas of 1. Can not load skeleton!");
		}else{
			if( joint->getType() == NODETYPE_JOINT ){
				domMatrix* nodeMatrix = joint->getMatrix_array()[0];
				SkeletonJoint newJoint;
				newJoint.parentIndex = parentIndex;
				newJoint.index = model->joints.size();
				if( joint->getSid() != 0 ) newJoint.nodeSid = joint->getSid();
				if( joint->getID() != 0 ) newJoint.nodeID = joint->getId();
				if( nodeMatrix->getSid() != 0 ) newJoint.matrixSid = nodeMatrix->getSid();
				processMatrix(nodeMatrix->getValue(), newJoint.bindMatrix);
				newJoint.worldMatrix = parentMatrix;
				newJoint.worldMatrix *= newJoint.bindMatrix;
				model->joints.push_back(newJoint);
				for( unsigned int i = 0; i < joint->getNode_array().getCount(); ++i ){
					processJoint(joint->getNode_array()[i], newJoint.worldMatrix, newJoint.index);
				}
			}else{
				mat nodeMatrix;
				processMatrix(joint->getMatrix_array()[0]->getValue(), nodeMatrix);
				nodeMatrix = parentMatrix * nodeMatrix;
				for( unsigned int i = 0; i < joint->getNode_array().getCount(); ++i ){
					processJoint(joint->getNode_array()[i], nodeMatrix, parentIndex);
				}
			}
		}
	}

	void ColladaLoader::processAnimation(struct SkeletonJoint* targetJoint, class domSampler* sampler, int elementIndex){
		//Get the sources
		domSource* timeSource = 0;
		domSource* outputSource = 0;
		for(size_t i = 0; i < sampler->getInput_array().getCount(); ++i){
			domInputLocal* input = sampler->getInput_array()[i];
			daeElement* elem = input->getSource().getElement();
			if( !elem ){
				LOG_WARNING("Source URI of semantic '" << input->getSemantic() << "' is invalid");
			}else{
				if( strcmp(input->getSemantic(), "INPUT") == 0 ){
					timeSource = (domSource*)elem;
				}else if( strcmp(input->getSemantic(), "OUTPUT") == 0 ){
					outputSource = (domSource*)elem;
				}else if( strcmp(input->getSemantic(), "INTERPOLATION") == 0 ){
					//Not used. We assume it is always linear
				}else{
					LOG_DEBUG("Animation sampler contains unused source: " << input->getSemantic());
				}
			}
		}
		if( timeSource->getTechnique_common()->getAccessor()->getCount() != outputSource->getTechnique_common()->getAccessor()->getCount() ){
			LOG_WARNING("Animation sampler input and output have unequal amount of values");
		}else{
			if( timeSource->getTechnique_common()->getAccessor()->getStride() != 1 || outputSource->getTechnique_common()->getAccessor()->getStride() != 1 ){
				LOG_WARNING("Animation sampler input and output have not got stride 1");
			}else{
				//Loop through all the time values
				domFloat_array* time_values = timeSource->getFloat_array();
				domFloat_array* output_values = outputSource->getFloat_array();
				for( size_t i = 0; i < time_values->getCount(); ++i ){
					float keyframeTime = (float)time_values->getValue()[i];
					float keyframeValue = (float)output_values->getValue()[i];
					//Now try to find this keyframe or create a new one
					bool existed = false;
					for( size_t j = 0; j < targetJoint->keyFrames.size(); ++j ){
						if( targetJoint->keyFrames[j].time == keyframeTime ){
							targetJoint->keyFrames[j].bindMatrix.value[elementIndex] = keyframeValue;
							existed = true;
							break;
						}
					}
					if( !existed ){
						KeyFrame newKeyFrame;
						newKeyFrame.time = keyframeTime;
						newKeyFrame.bindMatrix = targetJoint->bindMatrix;
						newKeyFrame.bindMatrix.value[elementIndex] = keyframeValue;
						targetJoint->keyFrames.push_back(newKeyFrame);
					}
				}
			}
		}
	}
}