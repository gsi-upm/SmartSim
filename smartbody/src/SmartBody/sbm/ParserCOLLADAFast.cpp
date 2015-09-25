#include "ParserCOLLADAFast.h"
#include "sr/sr_euler.h"
#include "sr/sr_timer.h"
#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <cctype>
#include <string>
#include <sbm/BMLDefs.h>
#include <sbm/GPU/SbmTexture.h>
#include <sb/SBScene.h>
#include <sbm/sbm_deformable_mesh.h>


class TextLineSplitterFast
{
public:

    TextLineSplitterFast( const size_t max_line_len );

    ~TextLineSplitterFast();

    void            SplitLine( const char *line,
                               const char sep_char = ','
                             );

    inline size_t   NumTokens( void ) const
    {
        return mNumTokens;
    }

    const char *    GetToken( const size_t token_idx ) const
    {
        assert( token_idx < mNumTokens );
        return mTokens[ token_idx ];
    }

private:
    const size_t    mStorageSize;

    char           *mBuff;
    char          **mTokens;
    size_t          mNumTokens;

    inline void     ResetContent( void )
    {
        memset( mBuff, 0, mStorageSize );
        // mark all items as empty:
        memset( mTokens, 0, mStorageSize * sizeof( char* ) );
        // reset counter for found items:
        mNumTokens = 0L;
    }
};

TextLineSplitterFast::TextLineSplitterFast( const size_t max_line_len ):
    mStorageSize ( max_line_len + 1L )
{
    // allocate memory
    mBuff   = new char  [ mStorageSize ];
    mTokens = new char* [ mStorageSize ];

    ResetContent();
}

TextLineSplitterFast::~TextLineSplitterFast()
{
    delete [] mBuff;
    delete [] mTokens;
}


void TextLineSplitterFast::SplitLine( const char *line,
                                  const char sep_char   /* = ',' */
                                )
{
    assert( sep_char != '\0' );

    ResetContent();
    strncpy( mBuff, line, mStorageSize );

    size_t idx       = 0L; // running index for characters

    do
    {
        assert( idx < mStorageSize );

        const char chr = line[ idx ]; // retrieve current character

        if( mTokens[ mNumTokens ] == NULL )
        {
            mTokens[ mNumTokens ] = &mBuff[ idx ];
        } // if

        if( chr == sep_char || chr == '\0' )
        { // item or line finished
            // overwrite separator with a 0-terminating character:
            mBuff[ idx ] = '\0';
            // count-up items:
            mNumTokens ++;
        } // if

    } while( line[ idx++ ] );
}



bool ParserCOLLADAFast::parse(SkSkeleton& skeleton, SkMotion& motion, std::string pathName, float scale, bool doParseSkeleton, bool doParseMotion)
{
	bool zaxis = false;
	rapidxml::file<char>* rapidFile = NULL;
	try 
	{
		int order;
		std::string filebasename = boost::filesystem::basename(pathName);
		std::string fileextension = boost::filesystem::extension(pathName);
		motion.setName(filebasename.c_str());
		std::stringstream strstr;
		if (fileextension.size() > 0 && fileextension[0] == '.')
			strstr << filebasename << fileextension;
		else
			strstr << filebasename << "." << fileextension;
		skeleton.setName(strstr.str().c_str());
		
		rapidFile = new rapidxml::file<char>(pathName.c_str());
		rapidxml::xml_document<> doc;
		doc.parse< rapidxml::parse_declaration_node>(rapidFile->data());
		rapidxml::xml_node<>* node = doc.first_node("COLLADA"); 
		rapidxml::xml_node<>* asset = getNode("asset", node, 0, 1);
		if (asset)
		{
			rapidxml::xml_node<>* upNode = getNode("up_axis", asset);
			if (upNode)
			{
				std::string upAxisName = upNode->value();
				if (upAxisName == "Z_UP" || upAxisName == "z_up")
				{
					// rotate the skeleton by -90 around the x-axis
					zaxis = true;
				}

			}
		}

		int depth = 0;
		rapidxml::xml_node<>* skNode = getNode("library_visual_scenes", node, depth, 2);
		if (!skNode)
		{
			LOG("ParserCOLLADAFast::parse ERR: no skeleton info contained in this file");
			if (rapidFile)
				delete rapidFile;
			return false;
		}
		std::map<std::string, std::string> materialId2Name;
		if (doParseSkeleton)
			parseLibraryVisualScenes(skNode, skeleton, motion, scale, order, materialId2Name);

		if (zaxis)
		{
			LOG("Prerot z-axis");
			// get the root node
			SkJoint* root = skeleton.root();
			if (root)
			{
				if (root->quat())
				{
					SrQuat prerot = root->quat()->prerot();
					SrVec xaxis(1, 0, 0);
					SrQuat adjust(xaxis, 3.14159f / -2.0f);
					SrQuat adjustY(SrVec(0,1,0), 3.14159f );
					SrQuat final = adjust * adjustY * prerot; 
					LOG("before = %f %f %f %f", prerot.w, prerot.x, prerot.y, prerot.z);
					LOG("after = %f %f %f %f", final.w, final.x, final.y, final.z);
					root->quat()->prerot(final);
					root->offset(root->offset()*adjust);
				}
			}
		}		
		skeleton.updateGlobalMatricesZero();
		depth = 0;
		rapidxml::xml_node<>* skmNode = getNode("library_animations", node, depth, 2);
		if (!skmNode)
		{
		//	LOG("ParserCOLLADAFast::parse WARNING: no motion info contained in this file");
			if (rapidFile)
				delete rapidFile;
			return true;
		}
		if (doParseMotion)
			parseLibraryAnimations(skmNode, skeleton, motion, scale, order, zaxis);
	//	animationPostProcess(skeleton, motion);
		
	}
	catch (const XMLException& toCatch) 
	{
		std::string message = "";
		xml_utils::xml_translate(&message, toCatch.getMessage());
		std::cout << "Exception message is: \n" << message << "\n";
		if (rapidFile)
			delete rapidFile;
		return false;
	}
	catch (const DOMException& toCatch) {
		std::string message = "";
		xml_utils::xml_translate(&message, toCatch.msg);
		std::cout << "Exception message is: \n" << message << "\n";
		if (rapidFile)
			delete rapidFile;
		return false;
	}
	catch (...) {
		LOG("Unexpected Exception in ParseOpenCollada::parse()");
		if (rapidFile)
			delete rapidFile;
		return false;
	}

	if (rapidFile)
		delete rapidFile;
	return true;
}

void ParserCOLLADAFast::getChildNodes(const std::string& nodeName, rapidxml::xml_node<>* node, std::vector<rapidxml::xml_node<>*>& children )
{
	std::string name = node->name();
	std::string value = node->value();
	if (name == nodeName)
		children.push_back(node);

	//if (!node->hasChildNodes()) // no child nodes
	//	return;
	rapidxml::xml_node<>* child = NULL;
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	//for (unsigned int c = 0; c < list->getLength(); c++)
	while (curNode)
	{
		getChildNodes(nodeName,curNode, children);
		curNode = curNode->next_sibling();
		//child = getNode(nodeName, list->item(c));
		//if (child)
		//	break;
	}
	//return child;

}

rapidxml::xml_node<>* ParserCOLLADAFast::getNode(const std::string& nodeName, rapidxml::xml_node<>* node)
{
	if (node && strcmp(node->name(), nodeName.c_str()) == 0)
		return node;

	rapidxml::xml_node<>* child = node->first_node();

	while (child)
	{
		rapidxml::xml_node<>* node = getNode(nodeName, child);
		if (node)
			return node;
		child = child->next_sibling();
	}

	return NULL;
}

rapidxml::xml_node<>* ParserCOLLADAFast::getNode(const std::string& nodeName, rapidxml::xml_node<>* node, int curDepth, int maximumDepth)
{
	if (node && strcmp(node->name(), nodeName.c_str()) == 0)
		return node;

	if (maximumDepth > -1 &&
		curDepth >= maximumDepth)
		return NULL;

	curDepth++;


	rapidxml::xml_node<>* child = NULL;
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		child = getNode(nodeName, curNode, curDepth, maximumDepth);
		if (child)
			break;
		curNode = curNode->next_sibling();
	}
	return child;
}


rapidxml::file<char>* ParserCOLLADAFast::getParserDocumentFile(std::string fileName, rapidxml::xml_document<>* doc)
{
	//std::string filebasename = boost::filesystem::basename(fileName);

	try {
		rapidxml::file<char>* rapidFile = new rapidxml::file<char>(fileName.c_str());
		doc->parse< rapidxml::parse_declaration_node>(rapidFile->data());
		return rapidFile;
	} catch (std::exception &e) {
		LOG(e.what());
	}
	return NULL;
}

/*
rapidxml::xml_node<>* ParserCOLLADAFast::getNode(const std::string& nodeName, std::string fileName, int maximumDepth)
{
	rapidxml::file<char>* rapidFile = new rapidxml::file<char>(fileName.c_str());
	rapidxml::xml_document<> doc;
	doc.parse< rapidxml::parse_declaration_node>(rapidFile->data());
	rapidxml::xml_node<>* node = doc.first_node("COLLADA");

	int depth = 0;
	return getNode(nodeName, node, depth, maximumDepth);
}



rapidxml::xml_node<>* ParserCOLLADAFast::getNode(const std::string& nodeName, std::string fileName)
{

	rapidxml::file<char>* rapidFile = new rapidxml::file<char>(fileName.c_str());
	rapidxml::xml_document<> doc;
	doc.parse< rapidxml::parse_declaration_node>(rapidFile->data());
	rapidxml::xml_node<>* node = doc.first_node("COLLADA");

	return getNode(nodeName, node);
}
*/
void ParserCOLLADAFast::nodeStr(const std::string s, std::string& out)
{
	out = s;
}

void ParserCOLLADAFast::parseLibraryControllers(rapidxml::xml_node<>* node, DeformableMesh* mesh, float scaleFactor, std::string jointPrefix)
{
	boost::char_separator<char> sep(" \n");

	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
		//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		//rapidxml::xml_node<>* node = list->item(c);
		rapidxml::xml_node<>* node = curNode;
		std::string name = node->name();
		std::string value = node->value();

		boost::algorithm::to_lower(name);
		if (name == "controller")
		{
			rapidxml::xml_attribute<>* idAttr = node->first_attribute("id");
			if (!idAttr)	continue;
			std::string skinId = idAttr->value();

			if (rapidxml::count_children(node) > 0)
			{
				//const DOMNodeList* childrenList = node->getChildNodes();
				rapidxml::xml_node<>* childrenCurNode = node->first_node();
				while (childrenCurNode)
					//for (unsigned int cc = 0; cc < childrenList->getLength(); cc++)
				{
					//rapidxml::xml_node<>* childNode = childrenList->item(cc);
					rapidxml::xml_node<>* childNode = childrenCurNode;
					std::string childName = childNode->name();
					if (childName == "skin")	// parsing skinning weights
					{

						rapidxml::xml_attribute<>* skinAttr = childNode->first_attribute("source");
						std::string skinSource = skinAttr->value();
						skinSource = skinSource.substr(1, skinSource.size() - 1);
						SkinWeight* skinWeight = new SkinWeight();
						
						skinWeight->sourceMesh = skinSource;

						// further for children
						//const DOMNodeList* childListOfSkin = childNode->getChildNodes();
						rapidxml::xml_node<>* childOfSkinCurNode = childNode->first_node();
						while (childOfSkinCurNode)
							//for (unsigned int cSkin = 0; cSkin < childListOfSkin->getLength(); cSkin++)
						{
							//rapidxml::xml_node<>* childNodeOfSkin = childListOfSkin->item(cSkin);
							rapidxml::xml_node<>* childNodeOfSkin = childOfSkinCurNode;
							std::string childNameOfSkin = childNodeOfSkin->name();
							std::string bindJointName = skinSource + "-skin-joints";
							std::string bindWeightName = skinSource + "-skin-weights";
							std::string bindPoseMatName = skinSource + "-skin-bind_poses";

							if (childNameOfSkin == "bind_shape_matrix")
							{
								std::string tokenBlock = childNodeOfSkin->value();
								float bindShapeMat[16];

								boost::tokenizer<boost::char_separator<char> > tokens(tokenBlock, sep);
								int i = 0;
								for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
									it != tokens.end();
									++it)
								{
									bindShapeMat[i] = (float)atof((*it).c_str());
									i++;
									if (i >= 16)
										break;
								}
								//for (int i = 0; i < 16; i++)
								//	bindShapeMat[i] = (float)atof(tokenize(tokenBlock).c_str());

								skinWeight->bindShapeMat.set(bindShapeMat);
								skinWeight->bindShapeMat.transpose();
								skinWeight->bindShapeMat.setl4(skinWeight->bindShapeMat.get_translation()*scaleFactor);
							}
							if (childNameOfSkin == "source")
							{
								std::string sourceId = childNodeOfSkin->first_attribute("id")->value();
								boost::algorithm::to_lower(sourceId);
								bool isBindJointName = (sourceId.find("joints") != std::string::npos);
								bool isBindWeights = (sourceId.find("weights") != std::string::npos);
								bool isBindPoseMatrices = (sourceId.find("bind_poses") != std::string::npos || sourceId.find("matrices") != std::string::npos || sourceId.find("poses") != std::string::npos);

								//DOMNodeList* realContentNodeList = childNodeOfSkin->getChildNodes();
								rapidxml::xml_node<>* realContentCurNode = childNodeOfSkin->first_node();
								while (realContentCurNode)
									//for (unsigned int cSource = 0; cSource < realContentNodeList->getLength(); cSource++)
								{
									//rapidxml::xml_node<>* realContentNode = realContentNodeList->item(cSource);
									rapidxml::xml_node<>* realContentNode = realContentCurNode;
									std::string realNodeName = realContentNode->name();

									std::string tokenBlock = realContentNode->value();

									boost::tokenizer<boost::char_separator<char> > tokens(tokenBlock, sep);
									int matCounter = 0;
									float bindPosMat[16];
									SrMat newMat;									
									for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
										it != tokens.end();
										++it)
									{
										//if ( sourceId == bindJointName && realNodeName == "Name_array") // joint name
										if ( isBindJointName && (realNodeName == "Name_array" || realNodeName == "IDREF_array") ) // joint name
										{
											std::string jointName = (*it);
											// check if the joint name start with the pre-fix and remove the prefix
											if ((*it).compare(0, jointPrefix.size(), jointPrefix) == 0)
											{
												jointName.erase(0, jointPrefix.size());
											}
											//cout << "joint name = " << jointName << endl;	
											skinWeight->infJointName.push_back(jointName);

										}
										//if ( sourceId == bindWeightName && realNodeName == "float_array") // joint weights
										if ( isBindWeights && realNodeName == "float_array") // joint weights
											skinWeight->bindWeight.push_back((float)atof((*it).c_str()));
										//if ( sourceId == bindPoseMatName && realNodeName == "float_array") // bind pose matrices
										if ( isBindPoseMatrices && realNodeName == "float_array") // bind pose matrices
										{
											bindPosMat[matCounter] = (float)atof((*it).c_str());
											matCounter ++;
											if (matCounter == 16)
											{
												matCounter = 0;
												newMat.set(bindPosMat);													
												newMat.transpose();
												//newMat = skinWeight->bindShapeMat.inverse()*newMat;
												SrVec newTran = newMat.get_translation()*scaleFactor;
												newMat.setl4(newTran);
												skinWeight->bindPoseMat.push_back(newMat);
											}
										}
									}
									realContentCurNode = realContentCurNode->next_sibling();
								}								
							} // end of if (childNameOfSkin == "source")
							if (childNameOfSkin == "vertex_weights")
							{
								//DOMNodeList* indexNodeList = childNodeOfSkin->getChildNodes();
								rapidxml::xml_node<>* indexCurNode = childNodeOfSkin->first_node();
								while (indexCurNode)
									//for (unsigned int cVertexWeights = 0; cVertexWeights < indexNodeList->getLength(); cVertexWeights++)
								{
									//rapidxml::xml_node<>* indexNode = indexNodeList->item(cVertexWeights);
									rapidxml::xml_node<>* indexNode = indexCurNode;
									std::string indexNodeName = indexNode->name();
									std::string tokenBlock = indexNode->value();

									boost::tokenizer<boost::char_separator<char> > tokens(tokenBlock, sep);

									if (indexNodeName == "vcount")
									{
										for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
											it != tokens.end();
											++it)
										{
											skinWeight->numInfJoints.push_back(atoi((*it).c_str()));
										}
									}
									else if (indexNodeName == "v")
									{
										for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
											it != tokens.end();
											++it)
										{
											skinWeight->jointNameIndex.push_back(atoi((*it).c_str()));
											it++;
											skinWeight->weightIndex.push_back(atoi((*it).c_str()));
										}
									}
									else
									{
										indexCurNode = indexCurNode->next_sibling();
										continue;
									}
									indexCurNode = indexCurNode->next_sibling();
								}
							}
							childOfSkinCurNode = childOfSkinCurNode->next_sibling();
						}
						mesh->skinWeights.push_back(skinWeight);
					} // end of if (childName == "skin")
					if (childName == "morph")	// parsing morph targets
					{
						rapidxml::xml_attribute<>* morphAttr = childNode->first_attribute("source");
						std::string morphName = morphAttr->value();
						morphName = morphName.substr(1, morphName.size() - 1);
						std::string morphFullName = morphName + "-morph";
						// futhur for children
						//const DOMNodeList* childListOfMorph = childNode->getChildNodes();
						rapidxml::xml_node<>* childOfMorphCurNode = childNode->first_node();
						while (childOfMorphCurNode)
							//for (unsigned int cMorph = 0; cMorph < childListOfMorph->getLength(); cMorph++)
						{
							//rapidxml::xml_node<>* childNodeOfMorph = childListOfMorph->item(cMorph);
							rapidxml::xml_node<>* childNodeOfMorph = childOfMorphCurNode;
							std::string childNameOfMorph = childNodeOfMorph->name();
							if (childNameOfMorph == "source")
							{
								//const DOMNodeList* childListOfSource = childNodeOfMorph->getChildNodes();
								rapidxml::xml_node<>* childOfSourceCurNode = childNodeOfMorph->first_node();
								while (childOfSourceCurNode)
									//for (size_t cMorphSource = 0; cMorphSource < childListOfSource->getLength(); cMorphSource++)
								{
									//rapidxml::xml_node<>* childNodeOfSource = childListOfSource->item(cMorphSource);
									rapidxml::xml_node<>* childNodeOfSource = childOfSourceCurNode;
									std::string childNameOfSource = childNodeOfSource->name();
									if (childNameOfSource == "IDREF_array" || childNameOfSource == "Name_array")
									{
										std::vector<std::string> refMesh;
										refMesh.push_back(morphName);	// first one is the base shape
										std::string tokenBlock = childNodeOfSource->value();
										boost::char_separator<char> sep2(" \n");
										boost::tokenizer<boost::char_separator<char> > tokens(tokenBlock, sep2);
										for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
											it != tokens.end();
											++it)
										{
											refMesh.push_back((*it));
										}
										mesh->morphTargets.insert(make_pair(morphFullName, refMesh));
									}
									childOfSourceCurNode = childOfSourceCurNode->next_sibling();
								}
							}
							childOfMorphCurNode = childOfMorphCurNode->next_sibling();
						}
					} // end of if (childName == "morph")
					childrenCurNode = childrenCurNode->next_sibling();
				}
			}
		}
		curNode = curNode->next_sibling();
	}

	
	/*
	// cache the joint names for each skin weight
	if (char_p && char_p->dMesh_p)
	{
		for (size_t x = 0; x < char_p->dMesh_p->skinWeights.size(); x++)
		{
			SkinWeight* skinWeight = char_p->dMesh_p->skinWeights[x];
			for (size_t j = 0; j < skinWeight->infJointName.size(); j++)
			{
				std::string& jointName = skinWeight->infJointName[j];
				SkJoint* curJoint = char_p->getSkeleton()->search_joint(jointName.c_str());
				skinWeight->infJoint.push_back(curJoint); // NOTE: If joints are added/removed during runtime, this list will contain stale data
			}
		}
	}
	*/
}

void ParserCOLLADAFast::parseLibraryVisualScenes(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name)
{
	//const DOMNodeList* list1 = node->getChildNodes();
	rapidxml::xml_node<>* curNode1 = node->first_node();
	while (curNode1)
	//for (unsigned int c = 0; c < list1->getLength(); c++)
	{
		//rapidxml::xml_node<>* node1 = list1->item(c);
		rapidxml::xml_node<>* node1 = curNode1;
		std::string nodeName = node1->name();
		if (nodeName == "visual_scene")
			parseJoints(node1, skeleton, motion, scale, order, materialId2Name, NULL);
		curNode1 = curNode1->next_sibling();
	}	
}

void ParserCOLLADAFast::parseJoints(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name, SkJoint* parent, bool hasRootJoint)
{
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int i = 0; i < list->getLength(); i++)
	{
		//rapidxml::xml_node<>* childNode = list->item(i);
		rapidxml::xml_node<>* childNode = curNode;
		std::string nodeName = childNode->name();
		if (nodeName == "node")
		{
			rapidxml::xml_attribute<>* idAt = childNode->first_attribute("id");
			std::string idAttr = "";
			if (idAt)
				idAttr = idAt->value();

			rapidxml::xml_attribute<>* nameAt = childNode->first_attribute("name");
			std::string nameAttr = "";
			if (nameAt)
				nameAttr = nameAt->value();

			rapidxml::xml_attribute<>* sidAt = childNode->first_attribute("sid");
			std::string sidAttr = "";
			if (sidAt)
				sidAttr = sidAt->value();

			rapidxml::xml_attribute<>* typeAt = childNode->first_attribute("type");
			std::string typeAttr = "";
			if (typeAt)
				typeAttr = typeAt->value();
			
			rapidxml::xml_node<>* tempMaterialNode = ParserCOLLADAFast::getNode("bind_material", childNode, 0, 2);
			
			// process this node as a joint if it's a non-geometry node
			bool treatAsJoint = false;
			if (typeAttr == "NODE" && hasRootJoint)
			{
				treatAsJoint = true;
				rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("instance_geometry", childNode, 0, 1);
				if (geometryNode)
					treatAsJoint = false;
			}
			
			if (treatAsJoint || typeAttr == "JOINT" || (nameAttr.find("Bip")!= std::string::npos && skeleton.root() == NULL) )
			{
				hasRootJoint = true;

				int index = -1;
				if (parent != NULL)	
					index = parent->index();

				SkJoint* joint = skeleton.add_joint(SkJoint::TypeQuat, index);
				joint->quat()->activate();
				joint->name(nameAttr);
				joint->extName(nameAttr);
				joint->extID(idAttr);
				joint->extSID(sidAttr);


				bool hasTranslate = false;

				skeleton.channels().add(joint->jointName(), SkChannel::XPos);
				skeleton.channels().add(joint->jointName(), SkChannel::YPos);
				skeleton.channels().add(joint->jointName(), SkChannel::ZPos);
				joint->pos()->limits(SkVecLimits::X, false);
				joint->pos()->limits(SkVecLimits::Y, false);
				joint->pos()->limits(SkVecLimits::Z, false);
				skeleton.channels().add(joint->jointName(), SkChannel::Quat);
				joint->quat()->activate();
				float rotx = 0.0f;
				float roty = 0.0f;
				float rotz = 0.0f;
				float jorientx = 0.0f;
				float jorienty = 0.0f;
				float jorientz = 0.0f;
				SrVec offset;

				std::vector<std::string> orderVec;

				if (parent == NULL && !skeleton.root())
					skeleton.root(joint);

				//const DOMNodeList* infoList = childNode->getChildNodes();
				rapidxml::xml_node<>* infoCurNode = childNode->first_node();
				while (infoCurNode)
				//for (unsigned int j = 0; j < infoList->getLength(); j++)
				{
					//rapidxml::xml_node<>* infoNode = infoList->item(j);
					rapidxml::xml_node<>* infoNode = infoCurNode;
					std::string infoNodeName = infoNode->name();
					if (infoNodeName == "matrix")
					{
						std::string matrixString = infoNode->value();
						std::vector<std::string> tokens;
						vhcl::Tokenize(matrixString, tokens, " \n");
						SrMat matrix;
						for (int m = 0; m < 16; m++)
							matrix[m] = (float)atof(tokens[m].c_str());
						matrix.transpose();
						offset.x = matrix[12];
						offset.y = matrix[13];
						offset.z = matrix[14];
						SrQuat quat(matrix);
						SrVec euler = quat.getEuler();
						rotx = euler[0];
						roty = euler[1];
						rotz = euler[2];					
						orderVec.push_back("Y");
						orderVec.push_back("X");
						orderVec.push_back("Z");
					}
					if (infoNodeName == "translate")
					{
						if (!hasTranslate) // need to distinguish between "origin" and "translate" nodes. For now, assume that the first one is the only valid one 
						{
							std::string offsetString = infoNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(offsetString, tokens, " \n");
							offset.x = (float)atof(tokens[0].c_str()) * scale;
							offset.y = (float)atof(tokens[1].c_str()) * scale;
							offset.z = (float)atof(tokens[2].c_str()) * scale;
							hasTranslate = true;
						}

						
					}
					if (infoNodeName == "rotate")
					{
						rapidxml::xml_attribute<>* sidAt= infoNode->first_attribute("sid");
						std::string sidAttr = sidAt->value();

						if (sidAttr.substr(0, 11) == "jointOrient")
						{
							std::string jointOrientationString = infoNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(jointOrientationString, tokens, " \n");
							float finalValue;
							for (int tokenizeC = 0; tokenizeC < 4; tokenizeC++)
								finalValue = (float)atof(tokens[tokenizeC].c_str());
							if (sidAttr == "jointOrientX") jorientx = finalValue;
							if (sidAttr == "jointOrientY") jorienty = finalValue;
							if (sidAttr == "jointOrientZ") jorientz = finalValue;
							if (orderVec.size() != 3)
								orderVec.push_back(sidAttr.substr(11, 1));
						}
						if (sidAttr.substr(0, 6) == "rotate")
						{
							std::string rotationString = infoNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(rotationString, tokens, " \n");
							float finalValue;
							for (int tokenizeC = 0; tokenizeC < 4; tokenizeC++)
								finalValue = (float)atof(tokens[tokenizeC].c_str());
							if (sidAttr == "rotateX") rotx = finalValue;
							if (sidAttr == "rotateY") roty = finalValue;
							if (sidAttr == "rotateZ") rotz = finalValue;
							if (orderVec.size() != 3)
								orderVec.push_back(sidAttr.substr(6, 1));
						}
						if (sidAttr.substr(0, 8) == "rotation")
						{
							std::string rotationString = infoNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(rotationString, tokens, " \n");
							float finalValue;
							for (int tokenizeC = 0; tokenizeC < 4; tokenizeC++)
								finalValue = (float)atof(tokens[tokenizeC].c_str());
							if (sidAttr == "rotationX") rotx = finalValue;
							if (sidAttr == "rotationY") roty = finalValue;
							if (sidAttr == "rotationZ") rotz = finalValue;
							if (orderVec.size() != 3)
								orderVec.push_back(sidAttr.substr(8, 1));
						}
					}
					infoCurNode = infoCurNode->next_sibling();
				}
				order = getRotationOrder(orderVec);
				if (order == -1)
				{
					if (orderVec.size() == 0)
					{
						order = 321;
					}
					else
					{
						LOG("COLLADA Parser: skeleton joint has invalid rotations.");
					}
				}

				SrMat rotMat;
				rotx *= float(M_PI) / 180.0f;
				roty *= float(M_PI) / 180.0f;
				rotz *= float(M_PI) / 180.0f;
				sr_euler_mat(order, rotMat, rotx, roty, rotz);
				SrMat jorientMat;
				jorientx *= float(M_PI) / 180.0f;
				jorienty *= float(M_PI) / 180.0f;
				jorientz *= float(M_PI) / 180.0f;
				sr_euler_mat(order, jorientMat, jorientx, jorienty, jorientz);
				joint->offset(offset);
				SrMat finalRotMat = rotMat;
				SrQuat quat = SrQuat(rotMat);
				SkJointQuat* jointQuat = joint->quat();
				jointQuat->prerot(quat);
				SrQuat jorientQ = SrQuat(jorientMat);
				jointQuat->orientation(jorientQ);


				rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("instance_geometry", childNode, 0, 1);
				if (geometryNode)	// might need to add support for rotation as well later when the case showed up
				{
					rapidxml::xml_attribute<>* geometryNodeAt= geometryNode->first_attribute("url");
					std::string sidAttr = geometryNodeAt->value();
					sidAttr = sidAttr.substr(1);
					//LOG("translate: %f, %f, %f", offset.x, offset.y, offset.z);
					//LOG("instance_geometry: %s", sidAttr.c_str());
					SrModel* newModel = new SrModel();
					newModel->name = SrString(sidAttr.c_str());
					newModel->translate(offset);
					newModel->translate(offset);
					if (!parent)
					{
						LOG("No parent for geometry '%s', geometry will be ignored...", (const char*) newModel->name);
						delete newModel;
					}
					else
					{
						parent->visgeo(newModel);
					}
				}

				rapidxml::xml_node<>* materialNode = ParserCOLLADAFast::getNode("bind_material", childNode);
				if (materialNode)
				{
					rapidxml::xml_node<>* techniqueCommonNode = ParserCOLLADAFast::getNode("technique_common", materialNode);
					if (techniqueCommonNode)
					{
						//const DOMNodeList* materialList = techniqueCommonNode->getChildNodes();
						rapidxml::xml_node<>* materialCurNode = techniqueCommonNode->first_node();
						while (materialCurNode)
						//for (unsigned int ml = 0; ml < materialList->getLength(); ml++)
						{
							//rapidxml::xml_node<>* childNode = materialList->item(ml);
							rapidxml::xml_node<>* childNode = materialCurNode;
							std::string nodeName = childNode->name();
							if (nodeName == "instance_material")
							{
								rapidxml::xml_attribute<>* symbolAt = childNode->first_attribute("symbol");
								std::string materialName = symbolAt->value();

								rapidxml::xml_attribute<>* targetAt= childNode->first_attribute("target");
								std::string targetNameString = targetAt->value();
								std::string targetName = "";
								if (targetNameString.length() > 0)
									targetName = targetNameString.substr(1);
// 								if (materialId2Name.find(targetName) == materialId2Name.end() && targetName != "")
// 									materialId2Name.insert(std::make_pair(targetName, materialName));
								if (materialId2Name.find(materialName) == materialId2Name.end() && materialName != "")
									materialId2Name.insert(std::make_pair(materialName, targetName));

							}
							materialCurNode = materialCurNode->next_sibling();
						}
					}
				}


				parseJoints(curNode, skeleton, motion, scale, order, materialId2Name, joint, hasRootJoint);
			}
			else if (typeAttr == "NODE" || tempMaterialNode)
			{
				rapidxml::xml_node<>* translateNode = ParserCOLLADAFast::getNode("translate", childNode, 0, 1);
				SrVec offset;
				if (translateNode)
				{
					std::string offsetString = translateNode->value();
					std::vector<std::string> tokens;
					vhcl::Tokenize(offsetString, tokens, " \n");
					offset.x = (float)atof(tokens[0].c_str()) * scale;
					offset.y = (float)atof(tokens[1].c_str()) * scale;
					offset.z = (float)atof(tokens[2].c_str()) * scale;
				}
				rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("instance_geometry", childNode, 0, 1);
				if (geometryNode)	// might need to add support for rotation as well later when the case showed up
				{
					rapidxml::xml_attribute<>* geometryNodeAt= geometryNode->first_attribute("url");
					std::string sidAttr = geometryNodeAt->value();
					sidAttr = sidAttr.substr(1);
					//LOG("translate: %f, %f, %f", offset.x, offset.y, offset.z);
					//LOG("instance_geometry: %s", sidAttr.c_str());
					SrModel* newModel = new SrModel();
					newModel->name = SrString(sidAttr.c_str());
					newModel->translate(offset);
					newModel->translate(offset);
					if (!parent)
					{
						LOG("No parent for geometry '%s', geometry will be ignored...", (const char*) newModel->name);
						delete newModel;
					}
					else
					{
						parent->visgeo(newModel);
					}
				}

				rapidxml::xml_node<>* materialNode = ParserCOLLADAFast::getNode("bind_material", childNode, 0, 2);
				if (materialNode)
				{
					rapidxml::xml_node<>* techniqueCommonNode = ParserCOLLADAFast::getNode("technique_common", materialNode);
					if (techniqueCommonNode)
					{
						//const DOMNodeList* materialList = techniqueCommonNode->getChildNodes();
						rapidxml::xml_node<>* materialCurNode = techniqueCommonNode->first_node();
						while (materialCurNode)
						//for (unsigned int ml = 0; ml < materialList->getLength(); ml++)
						{
							//rapidxml::xml_node<>* childNode = materialList->item(ml);
							rapidxml::xml_node<>* childNode = materialCurNode;
							std::string nodeName = childNode->name();
							if (nodeName == "instance_material")
							{
								rapidxml::xml_attribute<>* symbolAt = childNode->first_attribute("symbol");
								std::string materialName = symbolAt->value();

								rapidxml::xml_attribute<>* targetAt= childNode->first_attribute("target");
								std::string targetNameString = targetAt->value();
								std::string targetName = "";
								if (targetNameString.length() > 0)
									targetName = targetNameString.substr(1);
// 								if (materialId2Name.find(targetName) == materialId2Name.end() && targetName != "")
// 									materialId2Name.insert(std::make_pair(targetName, materialName));
								if (materialId2Name.find(materialName) == materialId2Name.end() && materialName != "")
									materialId2Name.insert(std::make_pair(materialName, targetName));

							}
							materialCurNode = materialCurNode->next_sibling();
						}
					}
				}
				parseJoints(curNode, skeleton, motion, scale, order, materialId2Name, parent, hasRootJoint);
			}
			else
				parseJoints(curNode, skeleton, motion, scale, order, materialId2Name, parent, hasRootJoint);
		}

		curNode = curNode->next_sibling();
	}
}

void ParserCOLLADAFast::parseLibraryAnimations( rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, bool zaxis )
{
	SkChannelArray& skChannels = skeleton.channels();
	motion.init(skChannels);
	SkChannelArray& motionChannels = motion.channels();
	SkChannelArray channelsForAdjusting;

	std::map<std::string, ColladaFloatArrayFast > floatArrayMap;
	std::map<std::string, ColladaSamplerFast  > samplerMap;
	std::vector<ColladChannelFast > channelSamplerNameMap;

	std::map<std::string,std::vector<std::string> > jointRotationOrderMap;

	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	// load all array of floats with corresponding channel names and sample rates
	while (curNode)
	//for (unsigned int i = 0; i < list->getLength(); i++)
	{
		//rapidxml::xml_node<>* node1 = list->item(i);	
		rapidxml::xml_node<>* node1 = curNode;
		std::string node1Name = node1->name();
		if (node1Name == "animation")
		{			
			parseNodeAnimation(node1, floatArrayMap, scale, samplerMap, channelSamplerNameMap, skeleton);
		}
		curNode = curNode->next_sibling();
	}

	std::vector<ColladChannelFast>::iterator mi;
	for ( mi  = channelSamplerNameMap.begin();
		  mi != channelSamplerNameMap.end();
		  mi++)
	{
		ColladChannelFast& colChannel = *mi;
		SkJoint* joint = skeleton.search_joint(colChannel.targetJointName.c_str());
		if (!joint) continue; // joint does not exist in the skeleton
		if (samplerMap.find(colChannel.sourceName) == samplerMap.end()) continue; // sampler does not exist
		ColladaSamplerFast& sampler = samplerMap[colChannel.sourceName];
		if (floatArrayMap.find(sampler.inputName) == floatArrayMap.end() || floatArrayMap.find(sampler.outputName) == floatArrayMap.end()) 
			continue; // no float array

		ColladaFloatArrayFast& inFloatArray = floatArrayMap[sampler.inputName];
		ColladaFloatArrayFast& outFloatArray = floatArrayMap[sampler.outputName];

		// insert frames for the motion
		if (motion.frames() == 0)
		{
			for (unsigned int frameCt = 0; frameCt < inFloatArray.floatArray.size(); frameCt++)
			{
				motion.insert_frame(frameCt, inFloatArray.floatArray[frameCt]);
				for (int postureCt = 0; postureCt < motion.posture_size(); postureCt++)
					motion.posture(frameCt)[postureCt] = 0.0f;										
			}								
		}		
		if (colChannel.targetType == "matrix")
		{
			int stride = 16;
			SrMat tran;
			std::string jName = colChannel.targetJointName;	
// 			SrQuat rotateOffset;
// 			if (zaxis && jName == skeleton.root()->name())
// 			{
// 				// rotate by 90 degree				
// 				SrVec xaxis(1, 0, 0);
// 				SrQuat adjust(xaxis, 3.14159f / -2.0f);
// 				rotateOffset = adjust;	
// 			}
			for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
			{
				SrMat tran;				
				for (int m = 0; m < stride; m++)
				{
					int idx = frameCt*stride+m;
					if (idx >= (int) outFloatArray.floatArray.size())
						break;
					tran[m] = outFloatArray.floatArray[idx];
				}
				tran.transpose();				
				SrVec offset;												
				offset.x = tran[12];
				offset.y = tran[13];
				offset.z = tran[14];
 				//if (joint)
 				//	offset -= joint->offset();
				//offset = offset*joint->quat()->prerot();
				SrQuat quat(tran);	
// 				quat = rotateOffset*quat;
// 				offset = offset*rotateOffset;
				//quat = joint->quat()->prerot().inverse()*quat;
				int channelId = -1;
				// put in translation
				std::string strTranslate = "translate";
				channelId = getMotionChannelId(motionChannels,jName,strTranslate);
				if (channelId >= 0)
				{
					for (int k=0;k<3;k++)
						motion.posture(frameCt)[channelId+k] = offset[k];
				}
				// put in rotation
				std::string strRotation = "rotateX";
				channelId = getMotionChannelId(motionChannels,jName,strRotation);
				if (channelId >= 0)
				{
					for (int k=0;k<4;k++)
						motion.posture(frameCt)[channelId+k] = quat.getData(k);
				}										
			}
		}
		else
		{
			int channelId = getMotionChannelId(motionChannels, colChannel.targetJointName,colChannel.targetType);									
			if (channelId >= 0)
			{
				int stride = outFloatArray.stride;
				for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
				{
					for (int strideCt = 0; strideCt < stride; strideCt++)
					{
						int idx = frameCt*stride + strideCt;
						if (idx >= (int)outFloatArray.floatArray.size()) continue;
						motion.posture(frameCt)[channelId + strideCt] = outFloatArray.floatArray[idx];
					}
				}
			}	

			std::string jointName = colChannel.targetJointName;
			std::string channelType = colChannel.targetType;
			if (channelType == "rotateX" || channelType == "rotateY" || channelType == "rotateZ")
			{
				if (channelsForAdjusting.search(jointName.c_str(), SkChannel::Quat) < 0)
					channelsForAdjusting.add(jointName.c_str(), SkChannel::Quat);
				if (jointRotationOrderMap.find(jointName) == jointRotationOrderMap.end())
				{
					std::vector<std::string> emptyString;
					jointRotationOrderMap[jointName] = emptyString;
				}
				std::string rotationOrder = channelType.substr(channelType.size()-1,1);
				jointRotationOrderMap[jointName].push_back(rotationOrder);
			}
			if (channelType == "translate")
			{
				channelsForAdjusting.add(jointName.c_str(), SkChannel::XPos);
				channelsForAdjusting.add(jointName.c_str(), SkChannel::YPos);
				channelsForAdjusting.add(jointName.c_str(), SkChannel::ZPos);
			}
			if (channelType == "translateX")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::XPos);
			if (channelType == "translateY")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::YPos);
			if (channelType == "translateZ")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::ZPos);
		}

	}	

	// remap the euler angles to quaternion if necessary
	std::vector<int> quatIndices;	
	int rootIdx = -1;
	std::map<std::string,std::vector<std::string> >::iterator vi;
	for (vi  = jointRotationOrderMap.begin();
		vi != jointRotationOrderMap.end();
		vi++)
	{
		std::string jointName = (*vi).first;
		//LOG("joint name = %s",jointName.c_str());
		int channelID = motionChannels.search(jointName,SkChannel::Quat);
		if (channelID != -1)
		{
			int quatIdx = motionChannels.float_position(channelID);
			if (zaxis && jointName == skeleton.root()->jointName())
				rootIdx = quatIdx;
			quatIndices.push_back(quatIdx);
		}		
	}

	for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
	{
		for (size_t i = 0; i < quatIndices.size(); i++)
		{
			int quatId = quatIndices[i];
			float rotx = motion.posture(frameCt)[quatId + 0] / scale;
			float roty = motion.posture(frameCt)[quatId + 1] / scale;
			float rotz = motion.posture(frameCt)[quatId + 2] / scale;
			//LOG("rotx = %f, roty = %f, rotz = %f",rotx,roty,rotz);
			rotx *= float(M_PI) / 180.0f;
			roty *= float(M_PI) / 180.0f;
			rotz *= float(M_PI) / 180.0f;
			SrMat mat;
			sr_euler_mat(order, mat, rotx, roty, rotz);			
			SrQuat quat = SrQuat(mat);			
			motion.posture(frameCt)[quatId + 0] = quat.w;
			motion.posture(frameCt)[quatId + 1] = quat.x;
			motion.posture(frameCt)[quatId + 2] = quat.y;
			motion.posture(frameCt)[quatId + 3] = quat.z;
		}

		if (zaxis) // rotate the root joints depending on up axis
		{
			std::string rootName = skeleton.root()->jointName();
			SrVec newPos;
			SrQuat newQuatRot;
			SrVec xaxis(1, 0, 0);
			SrQuat adjust(xaxis, 3.14159f / -2.0f);
			int channelID = motionChannels.search(rootName,SkChannel::Quat);
			if (channelID != -1)
			{
				int quatId = motionChannels.float_position(channelID);
				newQuatRot.w = motion.posture(frameCt)[quatId + 0];
				newQuatRot.x = motion.posture(frameCt)[quatId + 1];
				newQuatRot.y = motion.posture(frameCt)[quatId + 2];
				newQuatRot.z = motion.posture(frameCt)[quatId + 3];
				newQuatRot = adjust*newQuatRot;
				motion.posture(frameCt)[quatId + 0] = newQuatRot.w;
				motion.posture(frameCt)[quatId + 1] = newQuatRot.x;
				motion.posture(frameCt)[quatId + 2] = newQuatRot.y;
				motion.posture(frameCt)[quatId + 3] = newQuatRot.z;
			}

			for (int k=0;k<3;k++)
			{
				int chanType = SkChannel::XPos + k; 				
				int xID = motionChannels.search(rootName,(SkChannel::Type)chanType);
				if (xID)
					newPos[k] = motion.posture(frameCt)[motionChannels.float_position(xID)];
			}			
			newPos = newPos*adjust;
			for (int k=0;k<3;k++)
			{
				int chanType = SkChannel::XPos + k; 	
				int xID = motionChannels.search(rootName,(SkChannel::Type)chanType);
				if (xID)
					motion.posture(frameCt)[motionChannels.float_position(xID)] = newPos[k];
			}
		}
	}

	

	double duration = double(motion.duration());
	motion.synch_points.set_time(0.0, duration / 3.0, duration / 2.0, duration / 2.0, duration / 2.0, duration * 2.0/3.0, duration);
	motion.compress();
	// now there's adjust for the channels by default
	//animationPostProcessByChannels(skeleton, motion, channelsForAdjusting);
	animationPostProcess(skeleton,motion);
}

void ParserCOLLADAFast::parseLibraryAnimations2(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order)
{
	SkChannelArray& skChannels = skeleton.channels();
	motion.init(skChannels);
	SkChannelArray& motionChannels = motion.channels();
	SkChannelArray channelsForAdjusting;
	
	std::map<std::string,std::vector<SrMat> > jointTransformMap;
	std::map<std::string,std::vector<std::string> > jointRotationOrderMap;

	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int i = 0; i < list->getLength(); i++)
	{
		//rapidxml::xml_node<>* node1 = list->item(i);
		rapidxml::xml_node<>* node1 = curNode;
		std::string node1Name = node1->name();
		if (node1Name == "animation")
		{
			rapidxml::xml_attribute<>* idNode = node1->first_attribute("id");
			std::string idAttr;
			if (idNode)
				idAttr = idNode->value();
			std::string jointName = tokenize(idAttr, ".-");	
			std::string channelType = tokenize(idAttr, "_");
			//LOG("joint name = %s, channel type = %s",jointName.c_str(),channelType.c_str());
			int numTimeInput = -1;
			if (channelType == "rotateX" || channelType == "rotateY" || channelType == "rotateZ")
			{
				if (channelsForAdjusting.search(jointName.c_str(), SkChannel::Quat) < 0)
					channelsForAdjusting.add(jointName.c_str(), SkChannel::Quat);
				if (jointRotationOrderMap.find(jointName) == jointRotationOrderMap.end())
				{
					std::vector<std::string> emptyString;
					jointRotationOrderMap[jointName] = emptyString;
				}
				std::string rotationOrder = channelType.substr(channelType.size()-1,1);
				jointRotationOrderMap[jointName].push_back(rotationOrder);
			}
			if (channelType == "translate")
			{
				channelsForAdjusting.add(jointName.c_str(), SkChannel::XPos);
				channelsForAdjusting.add(jointName.c_str(), SkChannel::YPos);
				channelsForAdjusting.add(jointName.c_str(), SkChannel::ZPos);
			}
			if (channelType == "translateX")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::XPos);
			if (channelType == "translateY")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::YPos);
			if (channelType == "translateZ")
				channelsForAdjusting.add(jointName.c_str(), SkChannel::ZPos);
			
			//const DOMNodeList* list1 = node1->getChildNodes();
			rapidxml::xml_node<>* curNode1 = node1->first_node();
			while (curNode1)
			//for (unsigned int j = 0; j < list1->getLength(); j++)
			{
				//rapidxml::xml_node<>* node2 = list1->item(j);
				rapidxml::xml_node<>* node2 = curNode1;
				std::string node2Name = node2->name();
				if (node2Name == "source")
				{
					rapidxml::xml_attribute<>* sourceIdNode = node2->first_attribute("id");	
					std::string sourceIdAttr = sourceIdNode->value();
					size_t pos = sourceIdAttr.find_last_of("-");
					std::string op = sourceIdAttr.substr(pos + 1);
					if (sourceIdAttr.find("input") != std::string::npos) op = "input";
					else if (sourceIdAttr.find("output") != std::string::npos) op = "output";

					//const DOMNodeList* list2 = node2->getChildNodes();
					rapidxml::xml_node<>* curNode2 = node2->first_node();
					while (curNode2)
					//for (unsigned int k = 0; k < list2->getLength(); k++)
					{
						//rapidxml::xml_node<>* node3 = list2->item(k);
						rapidxml::xml_node<>* node3 = curNode2;
						std::string node3Name = node3->name();
						if (node3Name == "float_array")
						{
							rapidxml::xml_attribute<>* arrayCountNode = node3->first_attribute("count");
							std::string temp = arrayCountNode->value();
							int counter = atoi(temp.c_str());
							std::string arrayString = node3->value();
							//std::vector<std::string> tokens;
							//vhcl::Tokenize(arrayString, tokens, " \n");
							TextLineSplitterFast spl(counter);
							spl.SplitLine(arrayString.c_str(), ' ');
						
							if (op == "input")
							{
								numTimeInput = counter;
								std::string jName = tokenize(sourceIdAttr, ".-");
								if (motion.frames() == 0)
								{
									for (int frameCt = 0; frameCt < counter; frameCt++)
									{
										//motion.insert_frame(frameCt, (float)atof(tokens[frameCt].c_str()));
										motion.insert_frame(frameCt, (float)atof(spl.GetToken(frameCt)));
										for (int postureCt = 0; postureCt < motion.posture_size(); postureCt++)
											motion.posture(frameCt)[postureCt] = 0.0f;										
									}								
								}
/*								if (motion.frames() < counter)
								{
									for (int frameCt = 0; frameCt < counter; frameCt++)
									{
										float k = (float)atof(tokenize(arrayString).c_str());
										float keyTime = motion.keytime(frameCt);
										if (keyTime > k)
										{
											bool flag = motion.insert_frame(frameCt, k);
											for (int postureCt = 0; postureCt < motion.posture_size(); postureCt++)
												motion.posture(frameCt)[postureCt] = 0.0f;
										}
									}
								}
*/
							}

							if (op == "output")
							{
								if (sourceIdAttr.find("transform") != std::string::npos)
								{
									// load a transform matrix
									int stride = 16;
									SrMat tran;
									std::string jName = tokenize(sourceIdAttr, ".-");
									SkJoint* joint = skeleton.search_joint(jName.c_str());
									jointTransformMap[jName] = std::vector<SrMat>();
									jointTransformMap[jName].resize(motion.frames());
									for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
									{
										for (int m = 0; m < stride; m++)
										{
											//tran[m] = (float)atof(tokens[frameCt*stride+m].c_str());
											tran[m] = (float)atof(spl.GetToken(frameCt*stride+m));
										}
 										tran.transpose();
										jointTransformMap[jName][frameCt] = tran;
										
										SrVec offset;												
										offset.x = tran[12];
										offset.y = tran[13];
										offset.z = tran[14];
										if (joint)
											offset -= joint->offset();
										SrQuat quat(tran);
										SrVec euler = quat.getEuler();		
										int channelId = -1;
										// put in translation
										channelId = getMotionChannelId(motionChannels,jName+"-translate");
										if (channelId >= 0)
										{
											for (int k=0;k<3;k++)
												motion.posture(frameCt)[channelId+k] = offset[k];
										}
										// put in rotation
										channelId = getMotionChannelId(motionChannels,jName+"-rotateX");
										if (channelId >= 0)
										{
											for (int k=0;k<4;k++)
												motion.posture(frameCt)[channelId+k] = quat.getData(k);
										}										
									}
								}								
								else
								{
									int channelId = getMotionChannelId(motionChannels, sourceIdAttr);									
									if (channelId >= 0)
									{
										int stride = counter / numTimeInput;
										for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
										{
											for (int strideCt = 0; strideCt < stride; strideCt++)
											{
												//if (tokens.size() <= (unsigned int)frameCt) continue;
												if (spl.NumTokens() <= (unsigned int)frameCt) continue;

												//float v = (float)atof(tokens[frameCt*stride+strideCt].c_str());
												float v = (float)atof(spl.GetToken(frameCt*stride+strideCt));
												motion.posture(frameCt)[channelId + strideCt] = v * scale;
											}
										}
									}
								}								
							}
						}
						curNode2 = curNode2->next_sibling();
					}
				}
				curNode1 = curNode1->next_sibling();
			}
		}
		curNode = curNode->next_sibling();
	}

// 	for (std::map<std::string,std::vector<SrMat> >::iterator mi  = jointTransformMap.begin();
// 		 mi != jointTransformMap.end();
// 		 mi++)
// 	{
// 		std::string jName = mi->first;
// 		std::vector<SrMat>& childMatList = mi->second;
// 		SkJoint* joint = skeleton.search_joint(jName.c_str());
// 		if (!joint) continue;
// 		SkJoint* parent = joint->parent();
// 		if (parent)
// 		{
// 			std::vector<SrMat>& parentMatList = jointTransformMap[parent->name()];
// 			for (unsigned int i=0;i<childMatList.size();i++)
// 			{
// 				SrMat tran = childMatList[i];//*parentMatList[i].inverse();
// 				SrVec offset;												
// 				offset.x = 0;//tran[12];
// 				offset.y = 0;//tran[13];
// 				offset.z = 0;//tran[14];
// 				SrQuat quat(tran);
// 				SrVec euler = quat.getEuler();
// 				int channelId = -1;
// 				// put in translation
// 				channelId = getMotionChannelId(motionChannels,jName+"-translate");
// 				if (channelId >= 0)
// 				{
// 					for (int k=0;k<3;k++)
// 						motion.posture(i)[channelId+k] = offset[k];
// 				}
// 				// put in rotation
// 				channelId = getMotionChannelId(motionChannels,jName+"-rotateX");
// 				if (channelId >= 0)
// 				{
// 					for (int k=0;k<3;k++)
// 						motion.posture(i)[channelId+k] = euler[k];
// 				}	
// 			}
// 		}
// 	}



	// now transfer the motion euler data to quaternion data
	
	std::vector<int> quatIndices;
	/*
	for (int i = 0; i < motionChannels.size(); i++)
	{
		SkChannel& chan = motionChannels[i];
		
		if (chan.type == SkChannel::Quat)
		{
			int id = motionChannels.float_position(i);
			quatIndices.push_back(id);
		}
	}
	*/
	std::map<std::string,std::vector<std::string> >::iterator vi;
	for (vi  = jointRotationOrderMap.begin();
		 vi != jointRotationOrderMap.end();
		 vi++)
	{
		std::string jointName = (*vi).first;
		//LOG("joint name = %s",jointName.c_str());
		int channelID = motionChannels.search(jointName,SkChannel::Quat);
		if (channelID != -1)
		{
			quatIndices.push_back(motionChannels.float_position(channelID));
		}		
	}


	
	for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
		for (size_t i = 0; i < quatIndices.size(); i++)
		{
			int quatId = quatIndices[i];
			float rotx = motion.posture(frameCt)[quatId + 0] / scale;
			float roty = motion.posture(frameCt)[quatId + 1] / scale;
			float rotz = motion.posture(frameCt)[quatId + 2] / scale;
			//LOG("rotx = %f, roty = %f, rotz = %f",rotx,roty,rotz);
 			rotx *= float(M_PI) / 180.0f;
 			roty *= float(M_PI) / 180.0f;
 			rotz *= float(M_PI) / 180.0f;
			SrMat mat;
			sr_euler_mat(order, mat, rotx, roty, rotz);
			SrQuat quat = SrQuat(mat);
			motion.posture(frameCt)[quatId + 0] = quat.w;
			motion.posture(frameCt)[quatId + 1] = quat.x;
			motion.posture(frameCt)[quatId + 2] = quat.y;
			motion.posture(frameCt)[quatId + 3] = quat.z;
		}

	double duration = double(motion.duration());
	motion.synch_points.set_time(0.0, duration / 3.0, duration / 2.0, duration / 2.0, duration / 2.0, duration * 2.0/3.0, duration);
	motion.compress();	
}

void ParserCOLLADAFast::animationPostProcess(SkSkeleton& skeleton, SkMotion& motion)
{
	ParserCOLLADAFast::animationPostProcessByChannels(skeleton, motion, motion.channels());
}

void ParserCOLLADAFast::animationPostProcessByChannels(SkSkeleton& skeleton, SkMotion& motion, SkChannelArray& motionChannels)
{
	int numChannel = motionChannels.size(); 
	std::vector<std::pair<SkJoint*, int> > jointIndexMap;
	for (int j = 0; j < numChannel; j++)
	{
		SkChannel& chan = motionChannels[j];
		const std::string chanName = motionChannels.name(j);
		SkChannel::Type chanType = chan.type;
		SkJoint* joint = skeleton.search_joint(chanName.c_str());
		if (!joint)
		{
			jointIndexMap.push_back(std::pair<SkJoint*, int>());
			continue;
		}

		int id = motion.channels().search(chanName.c_str(), chanType);
		int dataId = motion.channels().float_position(id);
		if (dataId < 0)
		{
			jointIndexMap.push_back(std::pair<SkJoint*, int>());
			continue;
		}
		jointIndexMap.push_back(std::pair<SkJoint*, int>(joint, dataId));
	}

	
	for (int i = 0; i < motion.frames(); i++)
	{
		for (int j = 0; j < numChannel; j++)
		{
			SkChannel& chan = motionChannels[j];
		/*	const std::string chanName = motionChannels.name(j);
			SkChannel::Type chanType = chan.type;
			SkJoint* joint = skeleton.search_joint(chanName.c_str());
			if (!joint)
				continue;

			int id = motion.channels().search(chanName.c_str(), chanType);
			int dataId = motion.channels().float_position(id);
			if (dataId < 0)
				continue;
				*/
			SkJoint* joint = jointIndexMap[j].first;
			int dataId = jointIndexMap[j].second;
			if (!joint || dataId < 0)
				continue;
			SkChannel::Type chanType = chan.type;
			SrVec offset = joint->offset();					
			if (chanType == SkChannel::XPos)
			{
				float v = motion.posture(i)[dataId];
				motion.posture(i)[dataId] = v - offset[0];
			}
			if (chanType == SkChannel::YPos)
			{
				float v = motion.posture(i)[dataId];
				motion.posture(i)[dataId] = v - offset[1];
			}
			if (chanType == SkChannel::ZPos)
			{
				float v = motion.posture(i)[dataId];
				motion.posture(i)[dataId] = v - offset[2];
			}


			if (chanType == SkChannel::Quat)
			{
				SrQuat globalQuat = SrQuat(motion.posture(i)[dataId], motion.posture(i)[dataId + 1], motion.posture(i)[dataId + 2], motion.posture(i)[dataId + 3]);
				SrQuat preQuat = joint->quat()->prerot();
				//LOG("prerot = %f",preQuat.angle());
				SrQuat localQuat = preQuat.inverse() * globalQuat;
				motion.posture(i)[dataId] = localQuat.w;
				motion.posture(i)[dataId + 1] = localQuat.x;
				motion.posture(i)[dataId + 2] = localQuat.y;
				motion.posture(i)[dataId + 3] = localQuat.z;
			}

		}
	}
}

int ParserCOLLADAFast::getMotionChannelId( SkChannelArray& channels, std::string& jointName, std::string& targetType )
{
	int id = -1;
	int dataId = -1;	
	SkChannel::Type chanType;
	if (targetType == "translate")
	{
		chanType = SkChannel::XPos;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
	}
	else if (targetType == "translateX")
	{
		chanType = SkChannel::XPos;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
	}
	else if (targetType == "translateY")
	{
		chanType = SkChannel::YPos;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
	}
	else if (targetType == "translateZ")
	{
		chanType = SkChannel::ZPos;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
	}
	else if (targetType == "rotateX")
	{
		chanType = SkChannel::Quat;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
	}
	else if (targetType == "rotateY")
	{
		chanType = SkChannel::Quat;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
		if (id >= 0)
			dataId += 1;
	}
	else if (targetType == "rotateZ")
	{
		chanType = SkChannel::Quat;
		id = channels.search(jointName, chanType);
		dataId = channels.float_position(id);
		if (id >= 0)
			dataId += 2;
	}
	return dataId;	
}

int ParserCOLLADAFast::getMotionChannelId(SkChannelArray& mChannels, const std::string& sourceName)
{
	int id = -1;
	int dataId = -1;
	std::string sourceNameCopy = sourceName;
	std::string jName = tokenize(sourceNameCopy, ".-");
	SkChannel::Type chanType;
	
	if (sourceName.find("translate") != std::string::npos)
	{
		chanType = SkChannel::XPos;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
	}
	else if (sourceName.find("translateX") != std::string::npos)
	{
		chanType = SkChannel::XPos;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
	}
	else if (sourceName.find("translateY") != std::string::npos)
	{
		chanType = SkChannel::YPos;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
	}
	else if (sourceName.find("translateZ") != std::string::npos)
	{
		chanType = SkChannel::ZPos;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
	}
	else if (sourceName.find("rotateX") != std::string::npos)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
	}
	else if (sourceName.find("rotateY") != std::string::npos)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
		if (id >= 0)
			dataId += 1;
	}
	else if (sourceName.find("rotateZ") != std::string::npos)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName, chanType);
		dataId = mChannels.float_position(id);
		if (id >= 0)
			dataId += 2;
	}
	return dataId;
}

std::string ParserCOLLADAFast::tokenize(std::string& str, const std::string& delimiters, int mode)
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	if (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		std::string return_string = str.substr(lastPos, pos - lastPos);
		std::string::size_type lastPos = str.find_first_not_of(delimiters, pos);
		if (mode == 1)
		{
			if (std::string::npos == lastPos)	str = "";
			else								str = str.substr(lastPos, str.size() - lastPos);
		}
		return return_string;
	}
	else
		return "";
}
int ParserCOLLADAFast::getRotationOrder(std::vector<std::string> orderVec)
{
	if (orderVec.size() == 3)
	{
		if (orderVec[0] == "X" && orderVec[1] == "Y" && orderVec[2] == "Z")
			return 321;
		if (orderVec[0] == "X" && orderVec[1] == "Z" && orderVec[2] == "Y")
			return 231;
		if (orderVec[0] == "Y" && orderVec[1] == "X" && orderVec[2] == "Z")
			return 312;
		if (orderVec[0] == "Y" && orderVec[1] == "Z" && orderVec[2] == "X")
			return 132;
		if (orderVec[0] == "Z" && orderVec[1] == "X" && orderVec[2] == "Y")
			return 213;
		if (orderVec[0] == "Z" && orderVec[1] == "Y" && orderVec[2] == "X")
			return 123;
	}
	else if (orderVec.size() == 2)
	{
		if (orderVec[0] == "X" && orderVec[1] == "Y")
			return 321;
		if (orderVec[0] == "X" && orderVec[1] == "Z")
			return 231;
		if (orderVec[0] == "Y" && orderVec[1] == "X")
			return 312;
		if (orderVec[0] == "Y" && orderVec[1] == "Z")
			return 132;
		if (orderVec[0] == "Z" && orderVec[1] == "X")
			return 213;
		if (orderVec[0] == "Z" && orderVec[1] == "Y")
			return 123;
	}
	else if (orderVec.size() == 1)
	{
		if (orderVec[0] == "X")
			return 321;
		if (orderVec[0] == "Y")
			return 312;
		if (orderVec[0] == "Z")
			return 213;
	}
	return -1;
}

std::string ParserCOLLADAFast::getGeometryType(std::string idString)
{
	boost::algorithm::to_lower(idString);
	size_t found = idString.find("position");
	if (found != std::string::npos)
		return "positions";

	found = idString.find("binormal");
	if (found != std::string::npos)
		return "";

	found = idString.find("normal");
	if (found != std::string::npos)
		return "normals";

	found = idString.find("uv");
	if (found != std::string::npos)
		return "texcoords";

	found = idString.find("map");
	if (found != std::string::npos)
		return "texcoords";

//	LOG("ParserCOLLADAFast::getGeometryType WARNING: type %s not supported!", idString.c_str());	
	return "";
}

void ParserCOLLADAFast::parseLibraryGeometries( rapidxml::xml_node<>* node, const char* file, SrArray<SrMaterial>& M, SrStringArray& mnames,std::map<std::string, std::string>& materialId2Name, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap,std::vector<SrModel*>& meshModelVec, float scale )
{
	SrTimer timer;
	timer.start();

	std::map<std::string,bool> vertexSemantics;
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		//LOG("parseLibraryGeometries, iter %d", c);
		//rapidxml::xml_node<>* node = list->item(c);
		rapidxml::xml_node<>* node = curNode;
		std::string nodeName = node->name();	
		if (nodeName == "geometry")
		{
			//LOG("nodeName = geometry");
			std::map<std::string, std::string> verticesArrayMap;
			std::map<std::string, std::vector<SrVec> > floatArrayMap;	

			SrModel* newModel = new SrModel();
			rapidxml::xml_attribute<>* nameAt = node->first_attribute("name");
			std::string nameAttr = "";
			rapidxml::xml_attribute<>* idAt = node->first_attribute("id");
			std::string idString = idAt->value();

			if (nameAt)
				nameAttr = nameAt->value();

			newModel->name = SrString(idString.c_str());
			rapidxml::xml_node<>* meshNode = ParserCOLLADAFast::getNode("mesh", node);
			if (!meshNode)	continue;
			rapidxml::xml_node<>* meshCurNode = meshNode->first_node();
			while (meshCurNode)
			//for (unsigned int c1 = 0; c1 < meshNode->getChildNodes()->getLength(); c1++)
			{
				//rapidxml::xml_node<>* node1 = meshNode->getChildNodes()->item(c1);
				rapidxml::xml_node<>* node1 = meshCurNode;
				std::string nodeName1 = node1->name();
				
				if (nodeName1 == "source")
				{
					//LOG("nodeName1 = source");
					rapidxml::xml_attribute<>* idNode = node1->first_attribute("id");
					std::string idString = idNode->value();
					std::string sourceID = idString;
					size_t pos = idString.find_last_of("-");
					std::string tempString = idString.substr(pos + 1);
					idString = tempString;
					std::transform(idString.begin(), idString.end(), idString.begin(), ::tolower);
					std::string idType = getGeometryType(idString);
					// below is a faster way to parse all the data, have potential bug
					rapidxml::xml_node<>* floatNode = ParserCOLLADAFast::getNode("float_array", node1);			
					rapidxml::xml_attribute<>* countNode = floatNode->first_attribute("count");				
					rapidxml::xml_node<>* accessorNode = ParserCOLLADAFast::getNode("accessor", node1);
					rapidxml::xml_attribute<>* strideNode = accessorNode->first_attribute("stride");
					int count = atoi(countNode->value());
					int stride = atoi(strideNode->value());				
					//int strde = atoi()	
					const char* allfloats = floatNode->value();

					//TextLineSplitterFast splitter(strlen(allfloats));
					//splitter.SplitLine(allfloats, ' ');
					//size_t numTokens = splitter.NumTokens();
					
					std::string floatString = floatNode->value();
					boost::char_separator<char> sep(" \n");
					boost::tokenizer<boost::char_separator<char> > tokens(floatString, sep);
					
					floatArrayMap[sourceID] = std::vector<SrVec>();			
					//for (size_t c = 0; c < numTokens; c++)
					for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
							it != tokens.end();
							)
					{
						int nstep = stride > 3 ? 3 : stride;
						SrVec tempV;
						for (int k=0;k<nstep;k++)
						{
							tempV[k] = (float)atof((*it).c_str());
							//tempV[k] = (float)atof(splitter.GetToken(c));

							it++;
							//c++;
							//if (c >= numTokens)
								//break;
						}
						floatArrayMap[sourceID].push_back(tempV);
					}
					/*
					std::vector<std::string> tokens;
					vhcl::Tokenize(floatString, tokens, " \n");
					
					floatArrayMap[sourceID] = std::vector<SrVec>();					
					for (int i=0; i< count ; i+= stride)
					{
						int nstep = stride > 3 ? 3 : stride;		
						SrVec tempV;
						for (int k=0;k<nstep;k++)
						{
							tempV[k] = (float)atof(tokens[i+k].c_str());
						}
						floatArrayMap[sourceID].push_back(tempV);
					}
					*/


#if 0
					if (idType == "positions")
						count /= 3;
					if (idType == "normals")
						count /= 3;
					if (idType == "texcoords")
						count /= 2;
					std::string floatString;
					xml_utils::xml_translate(&floatString, floatNode->getTextContent());
					std::vector<std::string> tokens;
					vhcl::Tokenize(floatString, tokens, " \n");
					int index = 0;
					for (int i = 0; i < count; i++)
					{
						if (idType == "positions")
						{
							newModel->V.push();
							newModel->V.top().x = (float)atof(tokens[index++].c_str());
							newModel->V.top().y = (float)atof(tokens[index++].c_str());
							newModel->V.top().z = (float)atof(tokens[index++].c_str());
						}
						if (idType == "normals")
						{
							newModel->N.push();
							newModel->N.top().x = (float)atof(tokens[index++].c_str());
							newModel->N.top().y = (float)atof(tokens[index++].c_str());
							newModel->N.top().z = (float)atof(tokens[index++].c_str());
						}
						if (idType == "texcoords")
						{
							newModel->T.push();
							newModel->T.top().x = (float)atof(tokens[index++].c_str());
							newModel->T.top().y = (float)atof(tokens[index++].c_str());
						}
					}
#endif
					// duplication???
					//meshCurNode = meshCurNode->next_sibling();
				}			
				if (nodeName1 == "vertices")
				{
					//LOG("nodeName1 = vertices");
					vertexSemantics.clear();
					rapidxml::xml_node<>* verticeCurNode = node1->first_node();
					while (verticeCurNode)
					//for (unsigned int c2 = 0; c2 < node1->getChildNodes()->getLength(); c2++)
					{
						//rapidxml::xml_node<>* inputNode = node1->getChildNodes()->item(c2);
						rapidxml::xml_node<>* inputNode = verticeCurNode;

						if (strcmp(inputNode->name(), "input") == 0)
						{
							rapidxml::xml_attribute<>* semanticNode = inputNode->first_attribute("semantic");
							std::string inputSemantic = semanticNode->value();
							vertexSemantics[inputSemantic] = true;

							rapidxml::xml_attribute<>* sourceNameNode = inputNode->first_attribute("source");
							std::string sourceName = sourceNameNode->value();
							setModelVertexSource(sourceName,inputSemantic,newModel,floatArrayMap);
						}
						verticeCurNode = verticeCurNode->next_sibling();
					}										
				}

				if (nodeName1 == "triangles" || nodeName1 == "polylist" || nodeName1 == "polygons")
				{
					int curmtl = -1;
					rapidxml::xml_attribute<>* countNode = node1->first_attribute("count");
					std::string temp = countNode->value();
					int count = atoi(temp.c_str());
					rapidxml::xml_attribute<>* materialNode = node1->first_attribute("material");
					std::string materialName;
					if (materialNode)
						materialName = materialNode->value();
					std::string materialID = materialName;
					if (materialId2Name.find(materialName) != materialId2Name.end())
						materialID = materialId2Name[materialName];
					curmtl = mnames.lsearch(materialID.c_str());
					//curmtl = mnames.lsearch(materialName.c_str());
					//LOG("Material name is %s", materialName.c_str());
					std::map<int, std::string> inputMap;
					int pStride = 0;
					std::vector<int> vcountList;
					rapidxml::xml_node<>* triangleCurNode = node1->first_node();
					while (triangleCurNode)
					//for (unsigned int c2 = 0; c2 < node1->getChildNodes()->getLength(); c2++)
					{
						//rapidxml::xml_node<>* inputNode = node1->getChildNodes()->item(c2);
						rapidxml::xml_node<>* inputNode = triangleCurNode;
						if (strcmp(inputNode->name(), "input") == 0)
						{
							rapidxml::xml_attribute<>* semanticNode = inputNode->first_attribute("semantic");
							std::string inputSemantic = semanticNode->value();
							rapidxml::xml_attribute<>* offsetNode = inputNode->first_attribute("offset");
							std::string temp = offsetNode->value();
							rapidxml::xml_attribute<>* sourceNameNode = inputNode->first_attribute("source");
							std::string sourceName = sourceNameNode->value();
							int offset = atoi(temp.c_str());
							if (pStride <= offset)	pStride = offset;
							if (inputMap.find(offset) != inputMap.end())	// same offset is wrong
							{
								if (inputSemantic == "VERTEX" || inputSemantic == "NORMAL" || inputSemantic == "TEXCOORD")
									LOG("ParserCOLLADAFast::parseLibraryGeometries ERR: file not correct.");
							}
							else
							{
								setModelVertexSource(sourceName,inputSemantic,newModel,floatArrayMap);
								// should not allow same input semantic
								bool hasDuplicate = false;
								std::map<int, std::string>::iterator iter = inputMap.begin();
								for (; iter != inputMap.end(); iter++)
								{
									if (iter->second == inputSemantic)
									{
										hasDuplicate = true;
										break;
									}
								}
								if (!hasDuplicate)
									inputMap.insert(std::make_pair(offset, inputSemantic));
								else
									inputMap.insert(std::make_pair(offset, "null"));
							}
						}
						if (strcmp(inputNode->name(), "vcount") == 0)
						{
							std::string vcountString = inputNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(vcountString, tokens, " \n");
							for (int i = 0; i < count; i++)
								vcountList.push_back(atoi(tokens[i].c_str()));
						}

						triangleCurNode = triangleCurNode->next_sibling();
					}
					int totalVertex = 0;
					for (size_t i = 0; i < vcountList.size(); i++)
						totalVertex += vcountList[i];

					if (vcountList.size() == 0)
					{
						for (int i = 0; i < count; i++)
							vcountList.push_back(3);
					}

					// iterating to get all the <p> node

					std::string pString = "";
					rapidxml::xml_node<>* pNode = node1->first_node();
					while (pNode)
					{
						if (strcmp(pNode->name(), "p") == 0)
						{
							pString += std::string(pNode->value());
							pString += " ";
						}
						pNode = pNode->next_sibling();
					}

					//std::string pString = pNode->value();
					//const char* pString = pNode->value();

					/*
					std::vector<std::string> tokens;
					vhcl::Tokenize(pString, tokens, " \n");
					int index = 0;
					for (int i = 0; i < count; i++)
					{
					*/

					pStride += 1;
					boost::char_separator<char> sep(" \n");
					boost::tokenizer<boost::char_separator<char> > tokens(pString, sep);
					boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
					//TextLineSplitterFast splitter(strlen(pString));
					//splitter.SplitLine(pString, ' ');
					int index = 0;
					for (int i = 0; i < count; i++)
					{
						std::vector<int> fVec;
						std::vector<int> ftVec;
						std::vector<int> fnVec;
						if (i >= count)
							break;
						for (int j = 0; j < vcountList[i]; j++)
						{
							//if (i >= count)
								//break;
							for (int k = 0; k < pStride; k++)
							{
								std::string& semantic = inputMap[k];
								if (semantic == "VERTEX")
								{
									if (vertexSemantics.find("POSITION") != vertexSemantics.end())																								
										fVec.push_back(atoi((*it).c_str()));
										//fVec.push_back(atoi(splitter.GetToken(i)));

									if (vertexSemantics.find("NORMAL") != vertexSemantics.end())
										fnVec.push_back(atoi((*it).c_str()));									
										//fnVec.push_back(atoi(splitter.GetToken(i)));									
								}
								if (semantic == "TEXCOORD")
									ftVec.push_back(atoi((*it).c_str()));
									//ftVec.push_back(atoi(splitter.GetToken(i)));

								if (semantic == "NORMAL" && vertexSemantics.find("NORMAL") == vertexSemantics.end())
									fnVec.push_back(atoi((*it).c_str()));
									//fnVec.push_back(atoi(splitter.GetToken(i)));
								it++;
								//i++;
								//if (i >= count)
									//break;
								index++;
							}
							//if (i >= count)
								//break;
						}

						// process each polylist
							
						for (size_t x = 2; x < fVec.size(); x++)
						{
							newModel->F.push().set(fVec[0], fVec[x - 1], fVec[x]);
							newModel->Fm.push() = curmtl;
							if (ftVec.size() > x)
								newModel->Ft.push().set(ftVec[0], ftVec[x - 1], ftVec[x]);
							else if (ftVec.size() > 2)
								newModel->Ft.push().set(ftVec[0], ftVec[1], ftVec[2]);
							else
								newModel->Ft.push().set(0, 0, 0);
							
							if (fnVec.size() > x)
								newModel->Fn.push().set(fnVec[0], fnVec[x - 1], fnVec[x]);
							else
								newModel->Fn.push().set(fnVec[0], fnVec[1], fnVec[2]);
						}
					}
					/*
					if (tokens.size() != index)
						LOG("ParserCOLLADAFast::parseLibraryGeometries ERR: parsing <p> list uncorrectly (%s)!", nameAttr.c_str());
						*/
				}
				meshCurNode = meshCurNode->next_sibling();
			}
			

			newModel->mtlTextureNameMap = mtlTexMap;
			newModel->mtlNormalTexNameMap = mtlTexBumpMap;
			newModel->mtlSpecularTexNameMap = mtlTexSpecularMap;
			newModel->M = M;
			newModel->mtlnames = mnames;

			newModel->validate();

			newModel->remove_redundant_materials();

//			newModel->remove_redundant_normals();
			newModel->compress();
			
			meshModelVec.push_back(newModel);
			//LOG("Added model %s", (const char*) newModel->name);

			SrString path = file;
			SrString filename;
			path.extract_file_name(filename);
			SrStringArray paths;
			paths.push ( path );
			for (int i = 0; i < newModel->M.size(); i++)
			{
			   std::string matName = newModel->mtlnames[i];
			   if (newModel->mtlTextureNameMap.find(matName) != newModel->mtlTextureNameMap.end())
			   {
				   ParserCOLLADAFast::load_texture(SbmTextureManager::TEXTURE_DIFFUSE, newModel->mtlTextureNameMap[matName].c_str(), paths);	   
			   }	
			   if (newModel->mtlNormalTexNameMap.find(matName) != newModel->mtlNormalTexNameMap.end())
			   {
				   ParserCOLLADAFast::load_texture(SbmTextureManager::TEXTURE_NORMALMAP, newModel->mtlNormalTexNameMap[matName].c_str(), paths);	   
			   }
			   if (newModel->mtlSpecularTexNameMap.find(matName) != newModel->mtlSpecularTexNameMap.end())
			   {
				   //LOG("Load specular map = %s",newModel->mtlSpecularTexNameMap[matName].c_str());
				   ParserCOLLADAFast::load_texture(SbmTextureManager::TEXTURE_SPECULARMAP, newModel->mtlSpecularTexNameMap[matName].c_str(), paths);	   
			   }
			}
		}
		curNode = curNode->next_sibling();
	}	
}

void ParserCOLLADAFast::setModelVertexSource( std::string& sourceName, std::string& semanticName, SrModel* model, VecListMap& vecMap )
{
	std::string srcName = sourceName;
	std::vector<SrVec>* sourceArray = NULL;
	if (srcName[0] == '#') 
	{
		srcName.erase(0,1);		
	}

	if (vecMap.find(srcName) != vecMap.end())
		sourceArray = &vecMap[srcName];

	if ( semanticName == "POSITION" && sourceArray && model->V.size() == 0)
	{
		bool printOut = false;
// 		if (sourceName.find("LEyeLashShape-positions") != std::string::npos || sourceName.find("REyelashShape-positions") != std::string::npos)
// 		{
// 			LOG("sourname = %s",sourceName.c_str());
// 			printOut = true;
// 		}
		for (unsigned int i=0;i<sourceArray->size();i++)
		{
// 			if (printOut)
// 			{
// 				SrVec pos = (*sourceArray)[i];
// 				LOG("pos = %f %f %f",pos[0],pos[1],pos[2]);
// 			}
			model->V.push((*sourceArray)[i]);										
		}
	}
	else if (semanticName == "NORMAL" && sourceArray && model->N.size() == 0)
	{
		for (unsigned int i=0;i<sourceArray->size();i++)
		{
			model->N.push((*sourceArray)[i]);										
		}
	}
	else if (semanticName == "TEXCOORD" && sourceArray && model->T.size() == 0)
	{
		for (unsigned int i=0;i<sourceArray->size();i++)
		{
			SrVec ts = (*sourceArray)[i];
			model->T.push(SrVec2(ts[0],ts[1]));										
		}
	}
	else if (semanticName == "COLOR" && sourceArray && model->Vc.size() == 0)
	{
		for (unsigned int i=0;i<sourceArray->size();i++)
		{			
			model->Vc.push((*sourceArray)[i]);										
		}
	}
}

void ParserCOLLADAFast::load_texture(int type, const char* file, const SrStringArray& paths)
{
	SrString s;
	SrInput in;
	std::string imageFile = file;
	in.init( fopen(file,"r"));
	int i = 0;
	while ( !in.valid() && i < paths.size())
	{
		s = paths[i++];
		s << file;
		imageFile = s;
		in.init ( fopen(s,"r") );
	}
	if (!in.valid()) return;		
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	texManager.loadTexture(type,file,s);	
}


void ParserCOLLADAFast::parseLibraryMaterials(rapidxml::xml_node<>* node, std::map<std::string, std::string>& effectId2MaterialId)
{
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		//rapidxml::xml_node<>* node = list->item(c);
		rapidxml::xml_node<>* node = curNode;
		std::string nodeName = node->name();
		if (nodeName == "material")
		{
			rapidxml::xml_attribute<>* idNode = node->first_attribute("id");
			std::string materialId = idNode->value();
			rapidxml::xml_node<>* meshNode = ParserCOLLADAFast::getNode("instance_effect", node);
			if (!meshNode)	continue;
			rapidxml::xml_attribute<>* urlAt = meshNode->first_attribute("url");
			if (!urlAt)	continue;
			std::string urlString = urlAt->value();
			// get ride of the "#" in front, potential bug here if other file has different format
			if (urlString != "")
			{
				std::string effectId = urlString.substr(1);
				if (effectId2MaterialId.find(effectId) == effectId2MaterialId.end())
					effectId2MaterialId.insert(std::make_pair(effectId, materialId));
				else
					LOG("ParserCOLLADAFast::parseLibraryMaterials ERR: two effects mapped to material %s", materialId.c_str());
			}
		}
		curNode = curNode->next_sibling();
	}
}

void ParserCOLLADAFast::parseLibraryImages(rapidxml::xml_node<>* node, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name)
{
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		//rapidxml::xml_node<>* node = list->item(c);
		rapidxml::xml_node<>* node = curNode;
		std::string nodeName = node->name();
		if (nodeName == "image")
		{
			rapidxml::xml_attribute<>* idAt = node->first_attribute("id");
			std::string imageId = idAt->value();
			rapidxml::xml_node<>* initFromNode = ParserCOLLADAFast::getNode("init_from", node);
			std::string imageFile = initFromNode->value();

			rapidxml::xml_attribute<>* nameAt = node->first_attribute("name");
			std::string imageName = imageId;
			if (nameAt)
				imageName = nameAt->value();
			if (pictureId2File.find(imageId) == pictureId2File.end())
			{
				pictureId2File.insert(std::make_pair(imageId, imageFile));
				pictureId2Name.insert(std::make_pair(imageId, imageName));
			}
			else
				LOG("ParserCOLLADAFast::parseLibraryImages ERR: two image files mapped to same image id %s", imageId.c_str());
		}
		curNode = curNode->next_sibling();
	}
}

void ParserCOLLADAFast::parseLibraryEffects( rapidxml::xml_node<>* node, std::map<std::string, std::string>&effectId2MaterialId, std::map<std::string, std::string>& materialId2Name, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name, SrArray<SrMaterial>& M, SrStringArray& mnames, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap )
{
	//const DOMNodeList* list = node->getChildNodes();
	rapidxml::xml_node<>* curNode = node->first_node();
	while (curNode)
	//for (unsigned int c = 0; c < list->getLength(); c++)
	{
		//rapidxml::xml_node<>* node = list->item(c);
		rapidxml::xml_node<>* node = curNode;
		std::string nodeName = node->name();
		if (nodeName == "effect")
		{
			rapidxml::xml_attribute<>* idNode = node->first_attribute("id");
			std::string effectId = idNode->value();
			std::string materialId = effectId2MaterialId[effectId];
			//std::string materialName = materialId2Name[materialId];
			SrMaterial material;
			material.init();
			M.push(material);
			SrString matName(materialId.c_str());
			mnames.push(matName);

			std::vector<rapidxml::xml_node<>*> initNodes;
			ParserCOLLADAFast::getChildNodes("init_from", node, initNodes);
			std::string diffuseTexture, normalTexture, specularTexture;
			diffuseTexture = "";
			normalTexture = "";
			specularTexture = "";
			rapidxml::xml_node<>* diffuseNode = ParserCOLLADAFast::getNode("diffuse",node);
			rapidxml::xml_node<>* bumpNode = ParserCOLLADAFast::getNode("bump",node);
			rapidxml::xml_node<>* specularNode = ParserCOLLADAFast::getNode("specularLevel",node);
			if (diffuseNode)
			{
				rapidxml::xml_node<>* texNode = ParserCOLLADAFast::getNode("texture",diffuseNode);
				if (texNode)
				{
					rapidxml::xml_attribute<>* texAttrNode = texNode->first_attribute("texture");			
					std::string texID = texAttrNode->value();
					diffuseTexture = texID;

					std::string imageId = diffuseTexture;
					std::string imageFile = pictureId2File[imageId];
					std::string mtlName = mnames.top();
#if (BOOST_VERSION > 104400)
					std::string fileExt = boost::filesystem::extension(imageFile);
#else
					std::string fileExt = boost::filesystem2::extension(imageFile);
#endif
					std::string fileName = boost::filesystem::basename(imageFile);
					if (diffuseTexture.find(imageId) != std::string::npos)
						mtlTexMap[mtlName] = fileName + fileExt;		

				}
				rapidxml::xml_node<>* colorNode = ParserCOLLADAFast::getNode("color", diffuseNode);
				if (colorNode)
				{
					std::string color = colorNode->value();
					std::vector<std::string> tokens;
					vhcl::Tokenize(color, tokens, " \n");
					float w = 1;
					if (tokens.size() == 4)
						w = (float)atof(tokens[3].c_str());
					M.top().diffuse = SrColor((float)atof(tokens[0].c_str()), (float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), w);
				}
			}

			if (bumpNode)
			{
				rapidxml::xml_node<>* texNode = ParserCOLLADAFast::getNode("texture",bumpNode);
				if (texNode)
				{
					rapidxml::xml_attribute<>* texAttrNode = texNode->first_attribute("texture");			
					std::string texID = texAttrNode->value();
					normalTexture = texID;

					std::string imageId = diffuseTexture;
					std::string imageFile = pictureId2File[imageId];
					std::string mtlName = mnames.top();
#if (BOOST_VERSION > 104400)
					std::string fileExt = boost::filesystem::extension(imageFile);
#else
					std::string fileExt = boost::filesystem2::extension(imageFile);
#endif
					std::string fileName = boost::filesystem::basename(imageFile);
					if (diffuseTexture.find(imageId) != std::string::npos)
						mtlTexBumpMap[mtlName] = fileName + fileExt;		

					M.top().specular = SrColor(0.1f,0.1f,0.1f,1.f);
					M.top().shininess = 20;
				}			
			}

			if (specularNode)
			{
				rapidxml::xml_node<>* texNode = ParserCOLLADAFast::getNode("texture",specularNode);
				if (texNode)
				{
					rapidxml::xml_attribute<>* texAttrNode = texNode->first_attribute("texture");			
					std::string texID = texAttrNode->value();
					specularTexture = texID;

					std::string imageId = diffuseTexture;
					std::string imageFile = pictureId2File[imageId];
					std::string mtlName = mnames.top();
#if (BOOST_VERSION > 104400)
					std::string fileExt = boost::filesystem::extension(imageFile);
#else
					std::string fileExt = boost::filesystem2::extension(imageFile);
#endif
					std::string fileName = boost::filesystem::basename(imageFile);
					if (diffuseTexture.find(imageId) != std::string::npos)
						mtlTexSpecularMap[mtlName] = fileName + fileExt;	

					M.top().specular = SrColor(0.1f,0.1f,0.1f,1.f);
					M.top().shininess = 20;
				}			
			}
			//rapidxml::xml_node<>* initFromNode = ParserCOLLADAFast::getNode("init_from", node);
			//if (initFromNode)
			// get all textures
			for (unsigned int i=0;i<initNodes.size();i++)			// IF there's init_from node inside library_effect 
			{
				rapidxml::xml_node<>* initFromNode = initNodes[i];
				std::string imageId = initFromNode->value();
				std::string imageName = imageId;
				if (pictureId2Name.find(imageId) != pictureId2Name.end())
					imageName = pictureId2Name[imageId];

				std::string imageFile = pictureId2File[imageId];
				std::string mapKaName = imageFile;
				std::string texFile = mapKaName;
				std::string mtlName = mnames.top();
#if (BOOST_VERSION > 104400)
				std::string fileExt = boost::filesystem::extension(texFile);
#else
				std::string fileExt = boost::filesystem2::extension(texFile);
#endif
				std::string fileName = boost::filesystem::basename(texFile);
				if (diffuseTexture.find(imageName) != std::string::npos)
					mtlTexMap[mtlName] = fileName + fileExt;	
				else if (normalTexture.find(imageName) != std::string::npos)
					mtlTexBumpMap[mtlName] = fileName + fileExt;
				else if (specularTexture.find(imageName) != std::string::npos)
					mtlTexSpecularMap[mtlName] = fileName + fileExt;				
			}


			rapidxml::xml_node<>* emissionNode = ParserCOLLADAFast::getNode("emission", node);
			if (emissionNode)
			{
				rapidxml::xml_node<>* colorNode = ParserCOLLADAFast::getNode("color", emissionNode);
				std::string color = colorNode->value();
				std::vector<std::string> tokens;
				vhcl::Tokenize(color, tokens, " \n");
				float w = 1;
				if (tokens.size() == 4)
					w = (float)atof(tokens[3].c_str());
				M.top().emission = SrColor((float)atof(tokens[0].c_str()), (float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), w);
			}

			rapidxml::xml_node<>* ambientNode = ParserCOLLADAFast::getNode("ambient", node);
			if (ambientNode)
			{
				rapidxml::xml_node<>* colorNode = ParserCOLLADAFast::getNode("color", ambientNode);
				if (colorNode)
				{
					std::string color = colorNode->value();
					std::vector<std::string> tokens;
					vhcl::Tokenize(color, tokens, " \n");
					float w = 1;
					if (tokens.size() == 4)
						w = (float)atof(tokens[3].c_str());
					M.top().ambient = SrColor((float)atof(tokens[0].c_str()), (float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), w);
				}
			}
			rapidxml::xml_node<>* transparentNode = ParserCOLLADAFast::getNode("transparent", node);
			if (transparentNode)
			{
				float alpha = 1.f;	
				rapidxml::xml_attribute<>* opaqueNode = transparentNode->first_attribute("opaque");			
				std::string opaqueMode;
				if (opaqueNode)
				{
					opaqueMode = opaqueNode->value();
						
					if (opaqueMode == "RGB_ZERO")
					{
						rapidxml::xml_node<>* colorNode = ParserCOLLADAFast::getNode("color", transparentNode);		
						std::string color;
						if (colorNode)
						{
							color = colorNode->value();
							std::vector<std::string> tokens;
							vhcl::Tokenize(color, tokens, " \n");
							SrVec colorVec;
							if (tokens.size() >= 3)
							{
								for (int i=0;i<3;i++)
								{
									colorVec[i] = (float)atof(tokens[i].c_str());
								}
							}
							alpha = 1.f - (colorVec[0]+colorVec[1]+colorVec[2])/3;
							//alpha = 1.f - colorVec.norm();		
							if (alpha >= 1.f)
							{
								M.top().useAlphaBlend = false;
							}
						}														
					}				
				}
				else // by default it should be RGB_ZERO ?
				{
					rapidxml::xml_node<>* colorNode = ParserCOLLADAFast::getNode("color", transparentNode);		
					std::string color;
					if (colorNode)
					{
						color = colorNode->value();
						std::vector<std::string> tokens;
						vhcl::Tokenize(color, tokens, " \n");
						SrVec colorVec;
						if (tokens.size() >= 3)
						{
							for (int i=0;i<3;i++)
							{
								colorVec[i] = (float)atof(tokens[i].c_str());
							}
						}
						alpha = 1.f - (colorVec[0]+colorVec[1]+colorVec[2])/3;
						//alpha = 1.f - colorVec.norm();		
						if (alpha >= 1.f)
						{
							M.top().useAlphaBlend = false;
						}
					}														
				}				
				//float alpha = float(1.0 - xml_utils::xml_translate_float(colorNode->getTextContent()));				
				M.top().diffuse.a = (srbyte) ( alpha*255.0f );
			}
		}
		curNode = curNode->next_sibling();
	}
}

std::string ParserCOLLADAFast::getNodeAttributeString( rapidxml::xml_node<>* node, std::string attrName )
{
	std::string sourceIdAttr = "";
	rapidxml::xml_attribute<>* sourceIdNode = node->first_attribute(attrName.c_str());					
	if (sourceIdNode)
		sourceIdAttr = sourceIdNode->value();
	return sourceIdAttr;
}

int ParserCOLLADAFast::getNodeAttributeInt( rapidxml::xml_node<>* node, const std::string attrName )
{
	rapidxml::xml_attribute<>* attribute = node->first_attribute(attrName.c_str());
	if (attribute)
		return atoi(attribute->value());
	else
		return 0;
}

void ParserCOLLADAFast::parseNodeAnimation(rapidxml::xml_node<>* node1, std::map<std::string, ColladaFloatArrayFast > &floatArrayMap, float scale, std::map<std::string, ColladaSamplerFast > &samplerMap, std::vector<ColladChannelFast> &channelSamplerNameMap, SkSkeleton &skeleton )
{
	rapidxml::xml_attribute<>* attribute = node1->first_attribute("id");
	std::string idAttr = "";
	if (attribute)
		idAttr = attribute->value();

	//const DOMNodeList* list1 = node1->getChildNodes();	
	rapidxml::xml_node<>* curNode1 = node1->first_node();	
	while (curNode1)
	//for (unsigned int j = 0; j < list1->getLength(); j++)
	{
		//rapidxml::xml_node<>* node2 = list1->item(j);
		rapidxml::xml_node<>* node2 = curNode1;
		std::string node2Name = node2->name();
		if (node2Name == "source")
		{							
			std::string sourceIdAttr = getNodeAttributeString(node2, "id");
			//const DOMNodeList* list2 = node2->getChildNodes();
			rapidxml::xml_node<>* curNode2 = node2->first_node();
			while (curNode2)
			//for (unsigned int k = 0; k < list2->getLength(); k++)
			{
				//rapidxml::xml_node<>* node3 = list2->item(k);
				rapidxml::xml_node<>* node3 = curNode2;
				std::string node3Name = node3->name();
				// parse float array
				if (node3Name == "float_array")
				{							
					int counter = getNodeAttributeInt(node3, "count"); 
					std::string nodeID = getNodeAttributeString(node3, "id");
					std::string arrayString = node3->value();
					std::vector<std::string> tokens;
					vhcl::Tokenize(arrayString, tokens, " \n");		
					if (floatArrayMap.find(sourceIdAttr) == floatArrayMap.end())
					{
						floatArrayMap[sourceIdAttr] = ColladaFloatArrayFast();
					}
					ColladaFloatArrayFast& colFloatArray = floatArrayMap[sourceIdAttr];
					std::vector<float>& floatArray = colFloatArray.floatArray;
					floatArray.resize(counter);						
					for (int m=0;m<counter;m++)
					{
						float v = (float)atof(tokens[m].c_str());
						floatArray[m] = v * scale;
					}												
					rapidxml::xml_node<>* accessorNode = getNode("accessor",node2);
					if (accessorNode)
					{
						colFloatArray.stride = getNodeAttributeInt(accessorNode, "stride");
						rapidxml::xml_node<>* paramNode = getNode("param", accessorNode);
						if (paramNode)
						{
							colFloatArray.accessorParam = getNodeAttributeString(paramNode, "name");
						}
					}
				}
				curNode2 = curNode2->next_sibling();
			}
		}
		else if (node2Name == "sampler")
		{
			std::string samplerID = getNodeAttributeString(node2, "id");
			if (samplerMap.find(samplerID) == samplerMap.end())
			{
				samplerMap[samplerID] = ColladaSamplerFast();
			}
			ColladaSamplerFast& sampler = samplerMap[samplerID];
			//const DOMNodeList* list2 = node2->getChildNodes();
			rapidxml::xml_node<>* curNode2 = node2->first_node();
			while (curNode2)
			//for (unsigned int k = 0; k < list2->getLength(); k++)
			{
				//rapidxml::xml_node<>* node3 = list2->item(k);
				rapidxml::xml_node<>* node3 = curNode2;
				std::string node3Name = node3->name();						
				if (node3Name == "input")
				{
					std::string attrSemantic = getNodeAttributeString(node3, "semantic");
					if (attrSemantic == "INPUT")
					{
						sampler.inputName = getNodeAttributeString(node3, "source").substr(1);	
						//LOG("sampelr input name = %s",sampler.inputName.c_str());
					}
					else if (attrSemantic == "OUTPUT")
					{
						sampler.outputName = getNodeAttributeString(node3, "source").substr(1);
						//LOG("sampelr input name = %s",sampler.outputName.c_str());
					}
				}
				curNode2 = curNode2->next_sibling();
			}
		} 
		else if (node2Name == "channel")
		{
			std::string target = getNodeAttributeString(node2, "target");
			std::string source = getNodeAttributeString(node2, "source");
			channelSamplerNameMap.push_back(ColladChannelFast());
			ColladChannelFast& colChannel = channelSamplerNameMap.back();
			colChannel.sourceName = source.substr(1);
			//LOG("colChannel input name = %s",colChannel.sourceName.c_str());
			std::vector<std::string> tokens;
			vhcl::Tokenize(target, tokens, "/.");
			//LOG("token1 = %s, token2 = %s",tokens[0].c_str(),tokens[1].c_str());
			std::string jname = tokens[0];
			SkJoint* joint = skeleton.search_joint(jname.c_str());
			if (joint) jname = joint->jointName();
			colChannel.targetJointName = jname;
			colChannel.targetType = tokens[1];					
		}
		else if (node2Name == "animation") // for some reasons this kind of recursion does happen in some OpenCollada files
		{
			parseNodeAnimation(node2,floatArrayMap,scale,samplerMap,channelSamplerNameMap,skeleton);
		}
		curNode1 = curNode1->next_sibling();
	}
}

bool ParserCOLLADAFast::parseStaticMesh( std::vector<SrModel*>& meshModelVecs, std::string fileName )
{
	rapidxml::xml_document<> doc;
	rapidxml::file<char>* rapidFile = ParserCOLLADAFast::getParserDocumentFile(fileName, &doc);
	if (!rapidFile)
	{
		LOG("Problem parsing file '%s", fileName.c_str());
		return false;
	}
	rapidxml::xml_node<>* node = doc.first_node("COLLADA");
	
	//rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("library_geometries", fileName, 2);	
	int depth = 0;
	rapidxml::xml_node<>* geometryNode = getNode("library_geometries", node, depth, 2);		 
	if (geometryNode)
	{
		// first from library visual scene retrieve the material id to name mapping (TODO: needs reorganizing the assets management)
		std::map<std::string, std::string> materialId2Name;
		//rapidxml::xml_node<>* visualSceneNode = ParserCOLLADAFast::getNode("library_visual_scenes", fileName, 2);
		depth = 0;
		rapidxml::xml_node<>* visualSceneNode = getNode("library_visual_scenes", node, depth, 2);	
		if (!visualSceneNode)
			LOG("mcu_character_load_mesh ERR: .dae file doesn't contain correct geometry information.");
		SkSkeleton skeleton;
		SkMotion motion;
		int order;
		ParserCOLLADAFast::parseLibraryVisualScenes(visualSceneNode, skeleton, motion, 1.0, order, materialId2Name);

		// get picture id to file mapping
		std::map<std::string, std::string> pictureId2File;
		std::map<std::string, std::string> pictureId2Name;
		//rapidxml::xml_node<>* imageNode = ParserCOLLADAFast::getNode("library_images", fileName, 2);
		depth = 0;
		rapidxml::xml_node<>* imageNode = getNode("library_images", node, depth, 2);	
		if (imageNode)
			ParserCOLLADAFast::parseLibraryImages(imageNode, pictureId2File,pictureId2Name);

		// start parsing mateiral
		std::map<std::string, std::string> effectId2MaterialId;
		//rapidxml::xml_node<>* materialNode = ParserCOLLADAFast::getNode("library_materials", fileName, 2);
		depth = 0;
		rapidxml::xml_node<>* materialNode = getNode("library_materials", node, depth, 2);	
		if (materialNode)
			ParserCOLLADAFast::parseLibraryMaterials(materialNode, effectId2MaterialId);

		// start parsing effect
		SrArray<SrMaterial> M;
		SrStringArray mnames;
		std::map<std::string,std::string> mtlTextMap;
		std::map<std::string,std::string> mtlTextBumpMap;
		std::map<std::string,std::string> mtlTextSpecularMap;
		//rapidxml::xml_node<>* effectNode = ParserCOLLADAFast::getNode("library_effects", fileName, 2);
		depth = 0;
		rapidxml::xml_node<>* effectNode = getNode("library_effects", node, depth, 2);	
		if (effectNode)
		{
			ParserCOLLADAFast::parseLibraryEffects(effectNode, effectId2MaterialId, materialId2Name, pictureId2File, pictureId2Name, M, mnames, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap);
		}
		// parsing geometry
		ParserCOLLADAFast::parseLibraryGeometries(geometryNode, fileName.c_str(), M, mnames, materialId2Name, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap, meshModelVecs, 1.0f);
	}
	else
	{
		LOG( "Could not load mesh from file '%s'", fileName.c_str());
		if (rapidFile)
			delete rapidFile;
		return false;
	}

	if (rapidFile)
		delete rapidFile;
	return true;
}
