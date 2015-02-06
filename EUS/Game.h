#pragma once
#include <cassert>
#include <string>
#include <iostream>
#include <cassert>
#include "GL\glew.h"
#include "SDL\SDL.h"
#include "SpriteBatch.h"

// Base class for all games.
class Game {
private:
	SDL_Window* window;
	SDL_GLContext context;

	ContentManager content;
	SpriteBatch spriteBatch;

	std::string windowTitle;
	SDL_Event e;

	bool running;
	bool exitCalled;

	int windowWidth;
	int windowHeight;

	void internalInitialize();
	void internalUpdate();
	void internalDraw();
protected:
	virtual void onExit();

	virtual void onEvent(SDL_Event& e);

	virtual void initialize();
	virtual void update();
	virtual void draw();
public:
	Game(int windowWidth = 1280, int windowHeight = 720);

	int getWindowWidth() const;
	int getWindowHeight() const;

	void setWindowTitle(const std::string& value);
	std::string getWindowTitle() const;

	void run();
	void exit();
	bool isRunning();

	virtual ~Game();
};

