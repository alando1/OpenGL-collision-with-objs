#include "Mesh.h"

/*
	Parses a Wavefront .obj file into vertex arrays and materials (if present)
	suitable for rendering.  The multi-indexed face data that the .obj file format
	uses is converted into non-indexed straight vertex arrays, at a cost of higher
	memory requirements.
*/
void Mesh::loadOBJ(const char* filePath, const char* Name)
{
	cout << endl << "Object: " << "__________" << Name << "____________" << endl;
	if (filePath != NULL)
	{
		ifstream fileIn;

		fileIn.open(filePath);

		if (!fileIn.fail())
		{
			cout << endl << ">Mesh::loadOBJ: loading '" << filePath << "'..." << endl;
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

			// current chunk being tracked
			Chunk chunk;

			// starting properties have a blank material and start at 0
			chunk.materialName = "";
			chunk.startingIndex = 0;

			// this later gets built into a real GL texture only for chunks that
			// actually use the material
			chunk.texture = 0;

			// holds the 3 separate indices per vertex.
			// this is later rebuilt into GL-compatible vertex arrays
			vector<glm::ivec3> indices;

			fileIn >> token;

			// track the range of the UV coords
			float minU = 10000.0f, maxU = -10000.0f, minV = 10000.0f, maxV = -10000.0f;

			// loop until the end or something goes wrong
			while (!fileIn.eof() && !fileIn.fail())
			{
				// ignore comments
				if (token == "#")
					getline(fileIn, dump);
				// ignore user-defined object names
				else if (token == "o")
					getline(fileIn, dump);
				// ignore groups for now
				else if (token == "g")
					getline(fileIn, dump);
				// ignore smoothing groups
				else if (token == "s")
					getline(fileIn, dump);

				// read the .mtl file
				else if (token == "mtllib")
				{
					// if there's already a material map made, ignore the extra token
					if (_materials.size() > 0)
					{
						cout << "Warning: Mesh::loadOBJ: extra mtllib token encountered, ignoring." << endl;
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
				// with the appropriate texture bound. encountering the toke
				else if (token == "usemtl")
				{
					// if there were faces already read, save them into a chunk along with the
					// current material.  if material is blank, they are rendered without a texture
					if (indices.size() > 0)
					{
						// compute the element count for this chunk
						chunk.elementCount = indices.size() - chunk.startingIndex;
						chunk.materialName = currentMaterialName;
						_chunks.push_back(chunk);

						// start new chunk
						chunk.startingIndex = indices.size();						
					}

					// get the new material name
					fileIn >> currentMaterialName;
					chunk.materialName = currentMaterialName;
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
					cout << "Warning: Mesh::loadOBJ: unknown token: '" << token << "'" << endl;
					getline(fileIn, dump);
				}

				fileIn >> token;
			}
			
			// save the last chunk to the chunk list
			chunk.elementCount = indices.size() - chunk.startingIndex;
			_chunks.push_back(chunk);

			cout  << "\t# of chunk(s): " << _chunks.size() <<  endl;

 			cout << "done" << endl;
			cout << "\tu range: (" << minU << ", " << maxU << "), v range: (" << minV << ", " << maxV << ")" << endl;
			 
			// compute a rough measure of the memory required to hold all of the vertex data
			float vertSize = static_cast<float>(verts.size() * sizeof(float) * 3) / 1024.0f;
			float texCoordSize = static_cast<float>(texCoords.size() * sizeof(float) * 2) / 1024.0f;
			float normalSize = static_cast<float>(normals.size() * sizeof(float) * 3) / 1024.0f;
			float indicesSize = static_cast<float>(indices.size() * sizeof(int) * 3) / 1024.0f;

			cout << setprecision(1) << fixed << setw(10);
			cout << "\tvertex data:    ~" << vertSize << " KB" << endl;
			cout << "\ttex coord data: ~" << texCoordSize << " KB" << endl;
			cout << "\tnormal data:    ~" << normalSize << " KB" << endl;
			cout << "\tindex data:     ~" << indicesSize << " KB" << endl;
			cout << "\ttotal:          ~" << (vertSize + texCoordSize + normalSize + indicesSize) << " KB" << endl;
			cout << endl;

			// build the actual float arrays used for rendering
			_buildArrays(verts, texCoords, normals, indices);
			// and lastly, load the textures used by the chunks
			_loadTextures();
		}
		else
			cout << "ERROR: Mesh::loadOBJ: failed to open file '" << filePath << "'" << endl;
	}
	else
		cout << "ERROR: Mesh::loadOBJ: null file path" << endl;
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
			cout << ">building vertex arrays..." << endl;
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
			_vertCount = indices.size();
			cout << "done" << endl;

			// compute a rough measure of the memory required to hold all of the vertex data
			float vertSize = static_cast<float>(_vertCount * sizeof(float) * 3) / 1024.0f;
			float texCoordSize = static_cast<float>(_vertCount * sizeof(float) * 2) / 1024.0f;
			float normalSize = static_cast<float>(_vertCount * sizeof(float) * 3) / 1024.0f;

			cout << setprecision(1) << fixed << setw(10);
			cout << "\tvertex data:    ~" << vertSize << " KB" << endl;
			cout << "\ttex coord data: ~" << texCoordSize << " KB" << endl;
			cout << "\tnormal data:    ~" << normalSize << " KB" << endl;
			cout << "\ttotal:          ~" << (vertSize + texCoordSize + normalSize) << " KB" << endl;
			cout << endl;
		}
		catch (bad_alloc& ba)
		{
			cout << "ERROR: Mesh::_buildArrays: failed to allocate arrays (" << ba.what() << ")" << endl;
			destroy();
		}
	}
	else
		cout << "ERROR: Mesh::_buildArrays: one or more empty vectors given" << endl;
}

/*
	Loads the .mtl file from the .obj file being read in loadOBJ and populates a
	map with the material name as the key and a Material struct as the value.
*/
void Mesh::_loadMaterials(const string& filePath)
{
	cout << ">loading material file '" << filePath << "'... " << endl;
	
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
				// read the diffuse map file name
				if (token == "map_Kd")
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

		cout << "\t# of material(s): " << _materials.size() << endl;

		// print the map
		for (auto i = _materials.begin(); i != _materials.end(); ++i)
		{
			cout << "\t[" << i->first << "]\t\t" << "diffuse map: " << i->second.diffuseMapPath << endl;
		}
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
	cout << ">loading textures... " << endl;

	// loop through the chunks
	for (auto i = _chunks.begin(); i != _chunks.end(); ++i)
	{
		// find the material it wants
		auto j = _materials.find(i->materialName);

		// if found...
		if (j != _materials.end())
		{
			// load it.
			i->texture = SOIL_load_OGL_texture(j->second.diffuseMapPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
			// if the texture loaded successfully, turn on repeat/tiling at the edges! this is important.
			if (i->texture != 0)
			{
				cout << "\t" << j->second.diffuseMapPath.c_str() << endl;
				glBindTexture(GL_TEXTURE_2D, i->texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			else
				cout << "ERROR: Mesh::_loadTextures: failed to load texture '" << j->second.diffuseMapPath << "'" << endl;
		}
		else
			cout << "Warning: Mesh::_loadTextures: material '" << i->materialName << "' not found" << endl;
	}

	cout << "done" << endl;

	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
	Renders the list of chunks, each having a separate texture
*/
void Mesh::render()
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

	// loop through chunks
	for (auto i = _chunks.begin(); i != _chunks.end(); ++i)
	{
		// bind whatever texture it uses
		if (i->texture != 0)
			glBindTexture(GL_TEXTURE_2D, i->texture);
		else
			glBindTexture(GL_TEXTURE_2D, 0);

		// draw its portion of the whole vertex arrays
		glDrawArrays(GL_TRIANGLES, i->startingIndex, i->elementCount);
	}	

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

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
	for (auto i = _chunks.begin(); i != _chunks.end(); ++i)
	{
		if (i->texture != 0)
			glDeleteTextures(1, &(i->texture));
	}
	_chunks.clear();
}
