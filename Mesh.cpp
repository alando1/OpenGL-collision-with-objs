#include "Mesh.h"

map<string, Mesh::TextureRef> Mesh::_textureFilePathMap;

/*
	Parses a Wavefront .obj file into vertex arrays and materials (if present)
	suitable for rendering.  The multi-indexed face data that the .obj file format
	uses is converted into non-indexed straight vertex arrays, at a cost of higher
	memory requirements.
*/
void Mesh::loadOBJ(const char* filePath)
{
	if (filePath != NULL)
	{
		ifstream fileIn;

		fileIn.open(filePath);

		if (!fileIn.fail())
		{
			_sourceFilePath = filePath;

			cout << endl << string(80, '=') << endl;
			cout << "Mesh::loadOBJ: loading '" << filePath << "'...";
			// clean up old data if it exists
			destroy();

			string token, dump;
			// temporary vectors to hold the data as it's parsed
			vector<glm::vec3> verts;
			vector<glm::vec2> texCoords;
			vector<glm::vec3> normals;

			// maintain a current material name to apply to a given chunk.
			// this gets changed when a 'usemtl' token is encountered
			string currentMaterialName;

			// total number of chunks read
			int chunkCount = 0;

			// current object being tracked
			Object object;
			object.name = "";

			// current chunk being tracked
			Chunk chunk;
			// starting properties have a blank material and start at 0
			chunk.materialName = "";
			chunk.startingIndex = 0;
			chunk.elementCount = 0;
			chunk.isAnimated = false;
			// this later gets built into a real GL texture only for chunks that
			// actually use the material
			chunk.texture = 0;

			// holds the 3 separate indices per vertex.
			// this is later rebuilt into GL-compatible vertex arrays
			vector<glm::ivec3> indices;

			// grab the first token
			fileIn >> token;

			// track the range of the UV coords
			float minU = 10000.0f, maxU = -10000.0f, minV = 10000.0f, maxV = -10000.0f;

			// loop until the end or something goes wrong
			while (!fileIn.eof() && !fileIn.fail())
			{
				// ignore comments
				if (token == "#")
					getline(fileIn, dump);
				// ignore groups for now
				else if (token == "g")
					getline(fileIn, dump);
				// ignore smoothing groups
				else if (token == "s")
					getline(fileIn, dump);

				// create objects
				else if (token == "o")
				{
					fileIn >> token;
					//cout << "starting object " << token << "..." << endl;

					// if we had a previous object already being built, save it
					if (object.name != "")
					{
						// save the current chunk to the current object
						//cout << "done with object '" << object.name << "', saving chunk using material " << currentMaterialName << endl;
						// compute the element count for this chunk
						chunk.elementCount = static_cast<unsigned int>(indices.size()) - chunk.startingIndex;
						chunk.materialName = currentMaterialName;
						object.chunks.push_back(chunk);
						chunkCount++;
						_objects.push_back(object);						
					}

					//cout << "starting object '" << token << "'..." << endl;
					object.name = token;
					object.chunks.clear();
					chunk.startingIndex = static_cast<unsigned int>(indices.size());
					currentMaterialName = "";
				}
				// read the .mtl file
				else if (token == "mtllib")
				{
					// if there's already a material map made, ignore the extra token
					if (_materials.size() > 0)
					{
						cout << endl << "\tWarning: Mesh::loadOBJ: extra mtllib token encountered, ignoring." << endl;
						getline(fileIn, dump);
					}
					else
					{
						// construct the full path to the .mtl file from the .obj file
						string objFilePath = string(filePath);
						string mtlFileName, mtlFilePath;

						fileIn >> mtlFileName;
						mtlFilePath = objFilePath.substr(0, objFilePath.find_last_of("/\\")) + "/" + mtlFileName;

						// load it
						_loadMaterials(mtlFilePath);
					}
				}

				// the 'usemtl' token is used to delineate collections (chunks) of faces so they render
				// with the appropriate texture bound. 
				else if (token == "usemtl")
				{
					// if there were faces already read, save them into a chunk along with the
					// current material.  if material is blank, they are rendered without a texture
					if (currentMaterialName != "")
					{
						//cout << "done with chunk using material " << currentMaterialName << ", saving to current object" << endl;
						// compute the element count for this chunk
						chunk.elementCount = static_cast<unsigned int>(indices.size()) - chunk.startingIndex;
						chunk.materialName = currentMaterialName;
						object.chunks.push_back(chunk);
						chunkCount++;

						// start new chunk
						chunk.startingIndex = static_cast<unsigned int>(indices.size());
						chunk.elementCount = 0;
					}

					// get the new material name
					fileIn >> currentMaterialName;
					chunk.materialName = currentMaterialName;
					//cout << "setting material to " << currentMaterialName << endl;
				}

				// vertex data
				else if (token == "v")
				{
					glm::vec3 v;
					fileIn >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
				// texture UV data
				else if (token == "vt")
				{
					glm::vec2 tc;
					fileIn >> tc.x >> tc.y;

					// the direction of the V axis (or Y axis) in the OBJ format is flipped
					// compared to GL, so invert it.
					tc.y = 1.0f - tc.y;

					texCoords.push_back(tc);

					if (tc.x > maxU)
						maxU = tc.x;
					if (tc.x < minU)
						minU = tc.x;

					if (tc.y > maxV)
						maxV = tc.y;
					if (tc.y < minV)
						minV = tc.y;
				}
				// normal data
				else if (token == "vn")
				{
					glm::vec3 n;
					fileIn >> n.x >> n.y >> n.z;
					normals.push_back(n);
				}
				// face data, assumed to be triangles only
				else if (token == "f")
				{
					glm::ivec3 i;
					char c;

					// read 3 sets of indices (vertex/texcoord/normal)
					for (int j = 0; j < 3; ++j)
					{
						fileIn >> i.x >> c >> i.y >> c >> i.z;
						indices.push_back(i);
					}
				}
				else
				{
					cout << endl << "\tWarning: Mesh::loadOBJ: unknown token: '" << token << "'" << endl;
					getline(fileIn, dump);
				}

				fileIn >> token;
			}
			
			// save the last chunk to the chunk list of the last object
			chunk.elementCount = static_cast<unsigned int>(indices.size()) - chunk.startingIndex;
			//cout << "saving last chunk to object" << endl;
			object.chunks.push_back(chunk);
			chunkCount++;
			// save the last object to the object list
			//cout << "saving last object" << endl;
			_objects.push_back(object);
			cout << "done." << endl;
			cout << "\t" << _objects.size() << " objects, " << chunkCount << " chunks (";

			// print the grouping of chunks for each object as an ordered list of integers
			auto i = _objects.begin();
			cout << i->chunks.size();
			i++;
			// uh?
			for (; i != _objects.end(); ++i)
				cout << ", " << i->chunks.size();
			cout << ")" << endl;

 			//cout << "\tu range: (" << minU << ", " << maxU << "), v range: (" << minV << ", " << maxV << ")" << endl;
			 
			// compute a rough measure of the memory required to hold all of the vertex data
			float vertSize = static_cast<float>(verts.size() * sizeof(float) * 3) / 1024.0f;
			float texCoordSize = static_cast<float>(texCoords.size() * sizeof(float) * 2) / 1024.0f;
			float normalSize = static_cast<float>(normals.size() * sizeof(float) * 3) / 1024.0f;
			float indicesSize = static_cast<float>(indices.size() * sizeof(int) * 3) / 1024.0f;

			cout << setprecision(1) << fixed << setw(10);
			//cout << "vertex data:    ~" << vertSize << " KB" << endl;
			//cout << "tex coord data: ~" << texCoordSize << " KB" << endl;
			//cout << "normal data:    ~" << normalSize << " KB" << endl;
			//cout << "index data:     ~" << indicesSize << " KB" << endl;
			cout << "\tdata size: ~" << (vertSize + texCoordSize + normalSize + indicesSize) << " KB, rebuilt: ";

			// build the actual float arrays used for rendering
			_buildArrays(verts, texCoords, normals, indices);
			// load the textures used by the chunks
			_loadTextures();
			// lastly, build (or load) the ordered list of chunks to render with
			_buildRenderList();
			cout << "done." << endl << endl;

			// great success
			_isBuilt = true;
		}
		else
			cout << "ERROR: Mesh::loadOBJ: failed to open file '" << filePath << "'" << endl;
	}
	else
		cout << "ERROR: Mesh::loadOBJ: null file path" << endl;
}

// part of the random blob of shit that I later stuffed into Mesh::summarize
class VertexAttribs
{
public:
	int uvIndex, normalIndex;

	VertexAttribs() :
		uvIndex(0), normalIndex(0)
	{
	}

	VertexAttribs(int a, int b) :
		uvIndex(a), normalIndex(b)
	{
	}

	VertexAttribs(const VertexAttribs& in)
	{
		uvIndex = in.uvIndex;
		normalIndex = in.normalIndex;
	}

	VertexAttribs& operator=(const VertexAttribs& in)
	{
		uvIndex = in.uvIndex;
		normalIndex = in.normalIndex;
		return *this;
	}
};

void Mesh::summarize(const char* filePath)
{
	ifstream fileIn;
	string token, lastToken = "", dump;
	bool countingElements = false;
	int count = 0;

	//fileIn.open("./data/obj/stormtrooper/Stormtrooper.obj");
	fileIn.open(filePath);

	map<int, vector<VertexAttribs> > indexMap;

	if (!fileIn.fail())
	{
		cout << endl << "[ SUMMARY for '" << filePath << "' ]" << endl;
		cout << string(80, '-') << endl;

		fileIn >> token;

		while (!(fileIn.eof() || fileIn.fail()))
		{
			if (countingElements && (token == lastToken))
			{
				getline(fileIn, dump);
				count++;
			}
			else
			{
				if (countingElements)
				{
					cout << count << endl;
					countingElements = false;
					count = 0;
				}


				if (token == "#")
				{
					getline(fileIn, dump);
					cout << "#" << dump << endl;
				}
				else if (token == "mtllib")
				{
					cout << token;
					getline(fileIn, dump);
					cout << dump << endl;
				}
				else if (token == "o")
				{
					getline(fileIn, dump);
					cout << "o " << dump << endl;
				}
				else if (token == "v")
				{
					getline(fileIn, dump);
					cout << "\tv ";
					countingElements = true;
					count = 1;
				}
				else if (token == "vt")
				{
					getline(fileIn, dump);
					cout << "\tvt ";
					countingElements = true;
					count = 1;
				}
				else if (token == "vn")
				{
					getline(fileIn, dump);
					cout << "\tvn ";
					countingElements = true;
					count = 1;
				}
				else if (token == "f")
				{
					//getline(fileIn, dump);
					int vi, uvi, ni;
					char delim;
					VertexAttribs va;
					vector<VertexAttribs> v;

					for (int i = 0; i < 3; ++i)
					{
						fileIn >> vi >> delim >> uvi >> delim >> ni;
						va.uvIndex = uvi;
						va.normalIndex = ni;
						if (indexMap.find(vi) != indexMap.end())
						{
							indexMap[vi].push_back(va);
						}
						else
						{
							v.clear();
							v.push_back(va);
							indexMap[vi] = v;
						}
					}

					cout << "\t\tf ";
					countingElements = true;
					count = 1;
				}
				else
				{
					getline(fileIn, dump);
					cout << token << " " << dump << endl;
				}
			}

			lastToken = token;
			fileIn >> token;
		}

		if (countingElements)
		{
			cout << count << endl;
		}

		fileIn.close();
	}
	else
		cout << "error opening OBJ" << endl;

	cout << endl;

	cout << "done." << endl;
}

/*
	This function takes in the multi-indexed face data of the OBJ format and converts it to non-indexed
	straight arrays suitable for glDrawArrays().
*/
void Mesh::_buildArrays(vector<glm::vec3>& verts, vector<glm::vec2>& UVs, vector<glm::vec3>& normals, vector<glm::ivec3>& indices)
{
	// make sure all vectors have data
	if (verts.size() > 0 && UVs.size() > 0 && normals.size() > 0 && indices.size() > 0)
	{
		// try/catch block to catch any memory allocation exceptions
		try
		{
			//cout << "\tbuilding vertex arrays..." << endl;
			// the indices vector determines the size of the float arrays
			_verts = new float[indices.size() * 3];
			_texCoords = new float[indices.size() * 2];
			_normals = new float[indices.size() * 3];

			int vp = 0, tcp = 0, np = 0;

			// loop through each index and retrieve the corresponding vertex attributes.
			// this converts 3 arrays of index values into 3 arrays of floats, one for
			// position, one for tex coords, and one for normals.
			for (auto i = indices.begin(); i != indices.end(); ++i)
			{
				// i->x is the vertex index
				_verts[vp++] = verts[i->x - 1].x;
				_verts[vp++] = verts[i->x - 1].y;
				_verts[vp++] = verts[i->x - 1].z;

				// i->y is the UV index
				_texCoords[tcp++] = UVs[i->y - 1].x;
				_texCoords[tcp++] = UVs[i->y - 1].y;

				// i->z is the normal index
				_normals[np++] = normals[i->z - 1].x;
				_normals[np++] = normals[i->z - 1].y;
				_normals[np++] = normals[i->z - 1].z;
			}

			// lastly, save the total vertex count
			_vertCount = static_cast<unsigned int>(indices.size());
			//cout << "\tdone." << endl;

			// compute a rough measure of the memory required to hold all of the vertex data
			float vertSize = static_cast<float>(_vertCount * sizeof(float) * 3) / 1024.0f;
			float texCoordSize = static_cast<float>(_vertCount * sizeof(float) * 2) / 1024.0f;
			float normalSize = static_cast<float>(_vertCount * sizeof(float) * 3) / 1024.0f;

			//cout << setprecision(1) << fixed << setw(10);
			//cout << "vertex data:    ~" << vertSize << " KB" << endl;
			//cout << "tex coord data: ~" << texCoordSize << " KB" << endl;
			//cout << "normal data:    ~" << normalSize << " KB" << endl;
			cout << (vertSize + texCoordSize + normalSize) << " KB" << endl;
			//cout << endl;

			verts.clear();
			UVs.clear();
			normals.clear();
			indices.clear();
		}
		catch (bad_alloc& ba)
		{
			cout << endl << "ERROR: Mesh::_buildArrays: failed to allocate arrays (" << ba.what() << ")" << endl;
			destroy();
		}
	}
	else
	{
		cout << endl << "ERROR: Mesh::_buildArrays: one or more empty vectors given" << endl;
		destroy();
	}
}

/*
	Loads the .mtl file from the .obj file being read in loadOBJ and populates a
	map with the material name as the key and a Material struct as the value.
*/
void Mesh::_loadMaterials(const string& filePath)
{
	//cout << "\tloading material file '" << filePath << "'... " << endl;
	
	ifstream fileIn;
	fileIn.open(filePath);

	if (!fileIn.fail())
	{
		Material mtl;
		mtl.name = "";
		string token, dump;

		// parse materials
		fileIn >> token;
		while (fileIn.good())
		{
			// ignore comments
			if (token == "#")
				getline(fileIn, dump);
			// start new material
			else if (token == "newmtl")
			{
				// if there was a previous material, save it to the map before reading the new one
				if (mtl.name.length() != 0)
					_materials[mtl.name] = mtl;
					
				// get the new material name
				fileIn >> mtl.name;
			}
			// else, only parse additional tokens if there's a current material
			else if (mtl.name.size() > 0)
			{
				// read all the material lighting/shading parameters
				if (token == "Ka")
					fileIn >> mtl.ambientColor.r >> mtl.ambientColor.g >> mtl.ambientColor.b;
				else if (token == "Kd")
					fileIn >> mtl.diffuseColor.r >> mtl.diffuseColor.g >> mtl.diffuseColor.b;
				else if (token == "Ks")
					fileIn >> mtl.specularColor.r >> mtl.specularColor.g >> mtl.specularColor.b;
				else if (token == "Ns")
					fileIn >> mtl.specularExp;
				else if (token == "d")
					fileIn >> mtl.alpha;
				else if (token == "illum")
					fileIn >> mtl.illumModel;
				// read the diffuse map file name
				else if (token == "map_Kd")
				{
					fileIn >> mtl.diffuseMapPath;
					mtl.diffuseMapPath = filePath.substr(0, filePath.find_last_of("/\\")) + "/" + mtl.diffuseMapPath;
				}
			}
			// otherwise ignore until a 'newmtl' token
			else
				getline(fileIn, dump);

			fileIn >> token;
		}

		// save the last material being read
		_materials[mtl.name] = mtl;

		//cout << "\tdone. " << _materials.size() << " materials read." << endl;

		// print the map
		/*for (auto i = _materials.begin(); i != _materials.end(); ++i)
		{
			cout << "\t\t[" << i->first << "] diffuse map:" << endl << i->second.diffuseMapPath << endl;
		}*/
	}
	else
		cout << endl << "ERROR: Mesh::_loadMaterials: failed to open mtllib '" << filePath << "'" << endl;
}

/*
	Iterates through the loaded chunks and loads the corresponding textures for each
	using the material map.
*/
void Mesh::_loadTextures()
{
	cout << "\tloading textures... " << endl;
	int textureCount = 0;

	// loop through the objects
	for (auto i = _objects.begin(); i != _objects.end(); ++i)
	{
		// loop through the chunks
		for (auto j = i->chunks.begin(); j != i->chunks.end(); ++j)
		{
			// find the material it wants
			auto k = _materials.find(j->materialName);

			// if found...
			if (k != _materials.end())
			{
				// first, assign the alpha of the material to the chunk for later use in sorting for blending
				j->alpha = k->second.alpha;

				// find the texture path in the map
				auto m = _textureFilePathMap.find(k->second.diffuseMapPath);
				// if this texture has already been created before, don't load another copy
				if (m != _textureFilePathMap.end())
				{
					// copy the existing GL texture name and increment the ref count
					j->texture = m->second.texture;
					m->second.refCount++;
				}
				else
				{
					// else call SOIL and load it up
					j->texture = _textureFilePathMap[k->second.diffuseMapPath].texture = SOIL_load_OGL_texture(k->second.diffuseMapPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);
					cout << k->second.diffuseMapPath.c_str() << endl;
					_textureFilePathMap[k->second.diffuseMapPath].refCount++;

					// keep track of the number of loaded textures for this mesh only
					textureCount++;

					// if the texture loaded successfully, turn on repeat/tiling at the edges! this is important.
					// many OBJs rely on this since their uv's span way beyond 0.0 to 1.0
					if (j->texture != 0)
					{
						glBindTexture(GL_TEXTURE_2D, j->texture);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					}
					else
						cout << "ERROR: Mesh::_loadTextures: failed to load texture '" << k->second.diffuseMapPath << "'" << endl;
				}
			}
			else
				cout << "\tWarning: Mesh::_loadTextures: material '" << j->materialName << "' not found" << endl;
		}
	}

	//cout << "\tdone." << endl;
	cout << "\t" << textureCount << " textures loaded." << endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
	Comparison function used by std::sort to sort chunks by alpha
*/
bool Mesh::_compareChunkAlphas(Chunk* a, Chunk* b)
{
	if (a != NULL && b != NULL)
	{
		// a greater than comparison produces a descending order sorted list
		if (a->alpha > b->alpha)
			return true;
		else
			return false;
	}
	else
	{
		cout << "ERROR: Mesh::_compareChunkAlphas(): bad chunk pointers" << endl;
		return false;  // eh? maybe throw an exception?
	}
}

// todo!
bool Mesh::_compareChunkTextures(Chunk* a, Chunk* b)
{
	if (a != NULL && b != NULL)
	{
		//Material m = _materials[a->materialName];
		//if (a->materialNa
	}

	return true;
}

void Mesh::_printRenderList()
{
	//
}

/*
	Enables the continuous rotation of a particular chunk
*/
void Mesh::enableChunkSpin(string objectName, unsigned int chunkIndex, const glm::vec3& axis, float rotationSpeed)
{
	// loop through objects
	for (auto i = _objects.begin(); i != _objects.end(); ++i)
	{
		// find the one we need
		if (i->name == objectName)
		{
			// make sure the chunk index is valid
			if (chunkIndex < i->chunks.size())
			{
				// get the first chunk in the list
				auto j = i->chunks.begin();
				// loop until the target index
				for (unsigned int k = 0; k < chunkIndex; ++k)
					j++;
				// set the animation stuff
				j->isAnimated = true;
				j->axis = axis;
				j->rotationSpeed = rotationSpeed;
				j->rotationAngle = 0.0f;

				// add to the animation list used by update
				_animateList.push_back(&(*j));
			}
			else
				cout << "Warning: Mesh::enableChunkSpin(): invalid chunk index given" << endl;

			return;
		}
	}

	cout << "Warning: Mesh::enableChunkSpin(): unable to find object name '" << objectName << "'" << endl;
}


/*
	Builds a vector of Chunk* specifying the render order for the chunks
	Chunks rendered with blending are sorted and moved to the end
*/
void Mesh::_buildRenderList()
{
	// search 
	/*
	string renderDataFileName = _sourceFilePath.substr(0, _sourceFilePath.size() - 4) + ".rd";
	ifstream fileIn;

	fileIn.open(renderDataFileName);
	if (!fileIn.fail())
	{
		string token;
		fileIn >> token;
		cout << "token from .rd file: " << token << endl;

		fileIn.close();
	}*/

	//cout << "\tbuilding render list..." << endl;

	// iterate the objects and chunks and aggregate them into one list
	for (auto i = _objects.begin(); i != _objects.end(); ++i)
		for (auto j = i->chunks.begin(); j != i->chunks.end(); ++j)
			_renderList.push_back(&(*j)); // strange

	// sort the list of chunks by alpha, descending
	sort(_renderList.begin(), _renderList.end(), _compareChunkAlphas);

	// this member variable holds the index at which the alpha of the chunk
	// switches from 1.0 to less than that.
	// if it's equal to the size of the rendelist, it means no blending is needed.
	// this is the default state unless found differently below
	_blendingStartChunk = static_cast<int>(_renderList.size());

	// find the point where alpha changes from 1.0 to something else
	for (unsigned int i = 0; i < _renderList.size(); ++i)
	{
		if (_renderList[i]->alpha != 1.0f)
		{
			_blendingStartChunk = i;
			break;
		}
	}
}

/*
	Renders the list of objects, each having one or more chunks that render with a specific texture and/or blending
*/
void Mesh::render()
{
	if (_isBuilt)
	{
		// enable all the appropriate arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, _verts);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, _texCoords);

		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, _normals);

		// set color and turn on texturing
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
		// render opaque objects first
		for (int i = 0; i < _blendingStartChunk; ++i)
		{
			Chunk* c = _renderList[i];

			glBindTexture(GL_TEXTURE_2D, c->texture);
			if (c->isAnimated)
			{
				glPushMatrix();
					glRotatef(c->rotationAngle, c->axis.x, c->axis.y, c->axis.z);
					glDrawArrays(GL_TRIANGLES, c->startingIndex, c->elementCount);
				glPopMatrix();
			}
			else
				glDrawArrays(GL_TRIANGLES, c->startingIndex, c->elementCount);			
		}

		glEnable(GL_BLEND);
		// render transparent objects last.
		// if no transparent chunks exist, this for loop will terminate immediately
		for (int i = _blendingStartChunk; i < static_cast<int>(_renderList.size()); ++i)
		{
			Chunk* c = _renderList[i];

			glBindTexture(GL_TEXTURE_2D, c->texture);
			if (c->isAnimated)
			{
				glPushMatrix();
				glRotatef(c->rotationAngle, c->axis.x, c->axis.y, c->axis.z);
				glDrawArrays(GL_TRIANGLES, c->startingIndex, c->elementCount);
				glPopMatrix();
			}
			else
				glDrawArrays(GL_TRIANGLES, c->startingIndex, c->elementCount);
		}

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
}

/*
	//
*/
void Mesh::update(float dt)
{
	// loop through all chunks with animations and update the appropriate values
	for (auto i = _animateList.begin(); i != _animateList.end(); ++i)
	{
		// there's only rotation for now
		(*i)->rotationAngle += (dt / 1000.0f * (*i)->rotationSpeed);
	}
}

/*
	Decrements the reference count to a particular texture in the map.
	If the count reaches 0, the texture is deleted
*/
void Mesh::_releaseTexture(GLuint texture)
{
	//cout << "releasing texture " << texture << endl;
	for (auto i = _textureFilePathMap.begin(); i != _textureFilePathMap.end(); ++i)
	{
		if (i->second.texture == texture)
		{
			i->second.refCount--;
			//cout << "decrementing ref count. new value: " << i->second.refCount << endl;

			if (i->second.refCount == 0)
			{
				//cout << "deleting GL texture " << i->second.texture << endl;
				glDeleteTextures(1, &(i->second.texture));
				_textureFilePathMap.erase(i);
				return;
			}
		}
	}
}

/*
	Clean up the mess
*/
void Mesh::destroy()
{
	if (_verts)
	{
		delete[] _verts;
		_verts = NULL;
	}

	if (_texCoords)
	{
		delete[] _texCoords;
		_texCoords = NULL;
	}

	if (_normals)
	{
		delete[] _normals;
		_normals = NULL;
	}

	_vertCount = 0;
	_materials.clear();

	// free the GL textures
	for (auto i = _objects.begin(); i != _objects.end(); ++i)
	{
		for (auto j = i->chunks.begin(); j != i->chunks.end(); ++j)
		{
			if (j->texture != 0)
				_releaseTexture(j->texture);
				//glDeleteTextures(1, &(j->texture));
		}

		i->chunks.clear();
	}

	_objects.clear();
}
