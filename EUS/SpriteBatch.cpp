#include "SpriteBatch.h"

SpriteBatch::SpriteBatch() : vertexArray(0),
							 vertexBuffer(0),
							 indexBuffer(0),
							 spritesCount(0),
							 isDrawing(false) {
	perspective = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
}

#pragma region Private members
void SpriteBatch::initializeShader() {
	ContentManager content("Content");
	shader = content.load<Effect>("basic");
}
void SpriteBatch::initializeBuffers() {
	for (size_t i = 0; i < INITIAL_BATCH_SIZE; i++) {
		spriteQueue.push_back(SpriteInfo());
	}

	batchSize = INITIAL_BATCH_SIZE;

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColorTexture), (void*)(offsetof(VertexPositionColorTexture, position)));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColorTexture), (void*)(offsetof(VertexPositionColorTexture, color)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColorTexture), (void*)(offsetof(VertexPositionColorTexture, uv)));
	
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
}
void SpriteBatch::createIndices() {
	size_t indicesCount = MAX_BATCH_SIZE * INDICES_PER_SPRITE;

	for (size_t i = 0; i < indicesCount; i += VERTICES_PER_SPRITE) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(i + 1);
		indices.push_back(i + 3);
		indices.push_back(i + 2);
	}
}
void SpriteBatch::renderBatch(Texture* const texture, const size_t& first, const size_t& last) {
	glBindTexture(GL_TEXTURE_2D, texture->getId());

	std::vector<VertexPositionColorTexture> vertices;

	for (size_t i = first; i < last; i++) {
		SpriteInfo& sprite = spriteQueue[i];

		// Top left.
		vertices.push_back(VertexPositionColorTexture(
			sprite.position.x - sprite.origin.x,
			sprite.position.y - sprite.origin.y,
			sprite.position.z,
			sprite.color.x,
			sprite.color.y,
			sprite.color.z,
			sprite.color.w,
			0.0f,
			1.0f));
		
		// Top right.
		vertices.push_back(VertexPositionColorTexture(
			sprite.position.x + sprite.texture->width * sprite.xScale - sprite.origin.x,
			sprite.position.y - sprite.origin.y,
			sprite.position.z,
			sprite.color.x,
			sprite.color.y,
			sprite.color.z,
			sprite.color.w,
			1.0f,
			1.0f));
		
		// Bottom left.
		vertices.push_back(VertexPositionColorTexture(
			sprite.position.x - sprite.origin.x,
			sprite.position.y + sprite.texture->height * sprite.yScale - sprite.origin.y,
			sprite.position.z,
			sprite.color.x,
			sprite.color.y,
			sprite.color.z,
			sprite.color.w,
			0.0f,
			0.0f));

		// Bottom right.
		vertices.push_back(VertexPositionColorTexture(
			sprite.position.x + sprite.texture->width * sprite.xScale - sprite.origin.x,
			sprite.position.y + sprite.texture->height * sprite.yScale - sprite.origin.y,
			sprite.position.z,
			sprite.color.x,
			sprite.color.y,
			sprite.color.z,
			sprite.color.w,
			1.0f,
			0.0f));
		
		// sizeof(unsigned short) != sizeof(unsigned int) you fucking moron, be more careful in the future..
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(VertexPositionColorTexture) * 4 * i, sizeof(VertexPositionColorTexture) * 4, vertices.data());
		
		glBufferSubData(
			GL_ARRAY_BUFFER,							
			sizeof(VertexPositionColorTexture) * 4 * i,	// Offset 
			sizeof(VertexPositionColorTexture) * 4,		// Size bytes
			(void*)vertices.data());					// Data

		glDrawElements(
			GL_TRIANGLES,
			INDICES_PER_SPRITE,
			GL_UNSIGNED_SHORT,
			(void*)(i * INDICES_PER_SPRITE * sizeof(unsigned short)));

		vertices.clear();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteBatch::sortBatch() {
	std::sort(spriteQueue.begin(), spriteQueue.begin() + spritesCount, [](SpriteInfo& a, SpriteInfo& b) {
		return a.texture->getId() < b.texture->getId();
	});
}
void SpriteBatch::flushBatch() {
	sortBatch();

	glActiveTexture(GL_TEXTURE0 + 0);

	// Bind vbo and alloc some memory for it.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColorTexture) * spritesCount * VERTICES_PER_SPRITE, nullptr, GL_DYNAMIC_DRAW);

	shader->bind();
	GLuint location = glGetUniformLocation(shader->getProgram(), "perspective");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(perspective));

	Texture* batchTexture = spriteQueue[0].texture;
	size_t first = 0;
	size_t last = 0;

	for (size_t i = 0; i < spritesCount; i++)	{
		Texture* texture = spriteQueue[i].texture;

		// Flush whenever texture changes.
		if (texture != batchTexture) {
			// Flush from first to last.
			renderBatch(batchTexture, first, last);
			
			// Next flush will start from this index.
			first = i;
		}
		
		batchTexture = texture;
		last = i;
	}

	renderBatch(batchTexture, first, last + 1);

	shader->unbind();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void SpriteBatch::growSpriteQueue() {
	assert(batchSize + INITIAL_BATCH_SIZE <= MAX_BATCH_SIZE);

	for (size_t i = 0; i < INITIAL_BATCH_SIZE; i++){
		spriteQueue.push_back(SpriteInfo());
	}

	batchSize = spriteQueue.size();
}
#pragma endregion

#pragma region Public members
void SpriteBatch::initialize() {
	createIndices();
	initializeShader();
	initializeBuffers();
}

void SpriteBatch::begin() {
	if (isDrawing) {
		throw std::logic_error("End must be called before begin.");
	}

	isDrawing = true;
}

void SpriteBatch::draw(Texture* texture, pmath::Vec3f& position, pmath::Vec2f& origin, pmath::Vec4f& color, float xScale, float yScale) {
	if (spritesCount >= batchSize) {
		growSpriteQueue();
	}

	SpriteInfo& sprite = spriteQueue[spritesCount];

	sprite.position = position;
	sprite.origin = origin;
	sprite.texture = texture;
	sprite.color = color;
	sprite.xScale = xScale;
	sprite.yScale = yScale;

	spritesCount++;
}
void SpriteBatch::draw(Texture* const texture, pmath::Vec3f& position, pmath::Vec4f& color, float scale) {
	draw(texture, position, pmath::Vec2f(0.0f, 0.0f), color, scale, scale);
}
void SpriteBatch::draw(Texture* const texture, pmath::Vec3f& position, pmath::Vec4f& color) {
	draw(texture, position, color, 1.0f);
}
void SpriteBatch::draw(Texture* const texture, pmath::Vec3f& position) {
	draw(texture, position, pmath::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
}
void SpriteBatch::draw(Texture* const texture, float x, float y) {
	draw(texture, pmath::Vec3f(x, y, 1.0f), pmath::Vec4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
}
void SpriteBatch::draw(Texture* const texture, pmath::Rectf& rect) {
	pmath::Vec3f position(rect.position.x, rect.position.y, 1.0f);

	float xScale = rect.size.x / texture->width;
	float yScale = rect.size.y / texture->height;

	draw(texture, position, pmath::Vec2f(0.0f, 0.0f), pmath::Vec4f(1.0f, 1.0f, 1.0f, 1.0f), xScale, yScale);
}

void SpriteBatch::end() {
	if (!isDrawing) {
		throw std::logic_error("Begin must be called before end.");
	}

	if (!spritesCount) {
		flushBatch();
		spritesCount = 0;
	}

	isDrawing = false;
}
#pragma endregion

SpriteBatch::~SpriteBatch() {
}
