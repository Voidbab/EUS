#pragma once
#include <cassert>
#include <string>
#include <iostream>
#include <cassert>
#include "GL\glew.h"
#include "SDL\SDL.h"
#include "SpriteBatch.h"
#include "SceneManager.h"
#include "Input.h"
#include "View.h"

// Base class for all games.
class Game {
private:
	SDL_GLContext context;
	SDL_Window* m_window;

	ContentManager m_content;
	SpriteBatch m_spriteBatch;
	SceneManager m_sceneManager;
	InputManager m_inputManager;
	View m_view;

	std::string windowTitle;
	SDL_Event e;

	float deltaTime;
	float lastTime;
	float currentTime;

	bool running;
	bool exitCalled;

	int windowWidth;
	int windowHeight;

	void internalInitialize();
	void internalUpdate();
	void internalDraw();
protected:
	virtual void onExit();

	virtual void onEvent(const SDL_Event& e);

	virtual void initialize();
	virtual void update(const float delta);
	virtual void draw(const float delta);
public:
	Game(const int windowWidth = 1280, const int windowHeight = 720, const char* windowTitlte = "OpenGL");

	ContentManager& content();
	SpriteBatch& spriteBatch();
	SceneManager& sceneManager();
	InputManager& inputManager();
	SDL_Window& window();
	View& view();

	const int getWindowWidth() const;
	const int getWindowHeight() const;

	void setWindowTitle(const std::string& value);
	std::string getWindowTitle() const;

	void run();
	void exit();
	const bool isRunning();

	virtual ~Game();
};

