#ifndef APP_ENGINE_HPP
#define APP_ENGINE_HPP

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>

struct AppSettings
{
	std::string version = "1-20w23a";
	std::string title = "battlemapper " + version;

	// TODO: make these const
	int clientProtocol = 1;
	int serverProtocol = 1;

	bool fullscreen = false;
	bool vsync = true;
	bool debug = false;
	bool console = false;
	int maxfps = 60;
	int width = 1080;
	int height = 720;
	float mouseMoveBorderWidth = 20.0f;
	bool mouseMoveEnabled = true;

	struct CommandLineArguments
	{
		int argc;
		char** argv;
	} launchOptions;
};

class AppState;

class AppEngine
{
public:
	void Init(AppSettings settings_);
	void Cleanup();

	// Note: code after these functions is still executed.
	void ChangeState(AppState* state);
	void PushState(AppState* state);
	void PopState();

	void HandleEvents();
	void Update();
	void Draw();

	bool isRunning() { return running; }
	void Quit();

	std::vector<AppState*> states;
	std::vector<sf::Event> events;

	sf::RenderWindow* window;
	AppSettings settings;

	bool dedicatedServer = false;

private:
	bool running;
};

#endif // !APP_ENGINE_HPP
