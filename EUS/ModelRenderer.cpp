#include "ModelRenderer.h"

ModelRenderer::ModelRenderer(Game& game, Entity& owner, Model* const model, const int updateOrder, const int drawOrder) : DrawableComponent(owner, updateOrder, drawOrder),
																														  model(model),
																														  vertexArray(0),
																														  vertexBuffer(0),
																														  indexBuffer(0) {
	shader = game.content().load<Effect>("basic3d");

	initializeBuffers();
}
ModelRenderer::ModelRenderer(Game& game, Entity& owner, const int updateOrder, const int drawOrder) : ModelRenderer(game, owner, nullptr, updateOrder, drawOrder) {
}

#pragma region Private members
void ModelRenderer::releaseBuffers() {
	glDeleteBuffers(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}
void ModelRenderer::initializeBuffers() {
	if (model == nullptr) {
		// No model, just release buffers and return.
		releaseBuffers();

		return;
	}

	// Initialize buffers.
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}
#pragma endregion

#pragma region Protected members
void ModelRenderer::onDraw() {
	if (model != nullptr && !model->isEmpty()) {
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, model->getVertices().size() * sizeof(float), model->getVertices().data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->getIndices().size() * sizeof(unsigned short), model->getIndices().data(), GL_DYNAMIC_DRAW);

		GLuint error = glGetError();
		assert(error == 0);

		shader->bind();

		glDrawArrays(GL_TRIANGLES, 0, model->getVertices().size());
		/*glDrawElements(
			GL_TRIANGLES,
			0,
			GL_UNSIGNED_SHORT,
			(void*)0);*/

		shader->unbind();

		error = glGetError();
		assert(error == 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		bool errors = false;
		error = 0;

		while ((error = glGetError()) != 0) {
			std::cout << "GL ERROR: " + error << std::endl;
			
			errors = true;
		}

		if (errors) {
			std::abort();
		}
	}
}
#pragma endregion


#pragma region Public members
Model& ModelRenderer::getModel() {
	return *model;
}
void ModelRenderer::setModel(Model* const model) {
	this->model = model;

	initializeBuffers();
}
#pragma endregion

ModelRenderer::~ModelRenderer() {
	releaseBuffers();
}
