#include "resources.h"

#pragma region Resource class
Resource::Resource() {
}

Resource::~Resource() {
}
#pragma endregion

#pragma region Texture class
Texture::Texture() : Resource(), width(0), height(0) {
}

bool Texture::readFromFile(const std::string& path) {
	// we only accept pngs
	std::string fullpath(path + ".png");
	std::vector<unsigned char> png;

	lodepng::load_file(png, fullpath);
	std::vector<unsigned char> pixels;
	size_t error = lodepng::decode(pixels, width, height, png.data(), png.size());

	if (error) {
		std::fprintf(stderr, "Error loading texture file %s\n", lodepng_error_text(error));
		return false;
	}

	// create new name for texture
	glGenTextures(1, &id);
	// bind it so we can modify it
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	// set GL_NEAREST filter on
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}
const GLuint& Texture::getId() const {
	return id;
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}
#pragma endregion

#pragma region Effect class
Effect::Effect() : program(0) {
}

#pragma region Private members
std::string Effect::readFile(const std::string& filepath) {
	std::ifstream fileStream(filepath, std::ios::binary);
	assert(fileStream.good());

	fileStream.seekg(0u, std::ios::end);
	std::string source;
	source.reserve(static_cast<unsigned int>(fileStream.tellg()));
	fileStream.seekg(0u, std::ios::beg);
	source.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
	assert(fileStream.good());

	return source;
}
GLuint Effect::compileShaders(const char* vertexSrc, const char* fragmentSrc) {
	GLint result = GL_FALSE;
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// create vertexshader
	glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	assert(result == GL_TRUE);

	// create fragment shader
	glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	assert(result == GL_TRUE);

	GLuint program = glCreateProgram();
	assert(program != 0);
	// attach shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);


	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	assert(result == GL_TRUE);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}
#pragma endregion

#pragma region Public members
bool Effect::readFromFile(const std::string& path) {
	std::string vertex(path + ".vert");
	std::string fragment(path + ".frag");
	program = compileShaders(readFile(vertex).c_str(), readFile(fragment).c_str());
	return true;
}

void Effect::bind() const {
	glUseProgram(program);
}
void Effect::unbind() const {
	glUseProgram(0);
}

GLuint Effect::getProgram() const {
	return program;
}
#pragma endregion

Effect::~Effect() {
	glDeleteProgram(program);
}
#pragma endregion

#pragma region Model class
Model::Model() : Resource() {
}

#pragma region Public members
bool Model::readFromFile(const std::string& path) {
	std::ifstream inStream(path + ".obj");

	assert(inStream.is_open());

	int meshCount = -1;

	StringHelper strHelper;
	std::stringstream ss;
	std::string line;

	while (std::getline(inStream, line)) {
		// Skip empty lines and comments.
		if (line.empty()) {
			continue;
		}
		else if (line[0] == COMMENT) {
			continue;
		}

		// Split line into tokens.
		std::vector<std::string> tokens;
		strHelper.split(line, SPLIT, tokens, true);

		const std::string& first = tokens[0];
		ModelMesh* const mesh = meshCount >= 0 ? &meshes[meshCount] : nullptr;

		// Get material lib.
		if (first == MTLLIB) {
			materialLib = tokens[1];
		}
		// Get model name.
		else if (first == MTNAME) {
			mesh->materialName = tokens[1];
		}
		// Get smoothing group.
		else if (first == SMOOTHGRP) {
			mesh->smoothingGroup = tokens[1];
		}
		// Parse indices.
		else if (first == FACING) {
			for (size_t i = 1; i < tokens.size(); i++) {
				ss.clear();
				ss.str(tokens[i]);

				unsigned int j = 0;
				ss >> j;

				mesh->indices.push_back(j);
			}
		}
		// Parse texture vertices.
		else if (first == TEXTURE_VERTICES) {
			for (size_t i = 1; i < tokens.size(); i++) {
				ss.clear();
				ss.str(tokens[i]);

				float f = 0.0f;
				ss >> f;

				mesh->textureVertices.push_back(f);
			}
		}
		else if (first == VERTEX_NORMALS) {
			// Parse normals.
			for (size_t i = 1; i < tokens.size(); i++) {
				ss.clear();
				ss.str(tokens[i]);

				float f = 0.0f;
				ss >> f;

				mesh->textureVertices.push_back(f);
			}
		}
		else if (first == VERTICE) {
			// Parse all vertices.
			for (size_t i = 1; i < tokens.size(); i++) {
				ss.clear();
				ss.str(tokens[i]);

				float f = 0.0f;
				ss >> f;

				mesh->vertices.push_back(f);
			}
		}
		// New object token. Push new model mesh to meshes 
		// and start reading data to it.
		else if (first == OBJECTNAME) {
			meshCount++;

			meshes.push_back(ModelMesh());

			meshes[meshCount].name = tokens[1];
		}
	}

	return true;
}

bool Model::isEmpty() const {
	return meshes.size() == 0;
}

const std::vector<ModelMesh>& Model::getMeshes() const {
	return meshes;
}
#pragma endregion

Model::~Model() {
}
#pragma endregion
