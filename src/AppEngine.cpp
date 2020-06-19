#include "AppEngine.hpp"
#include "AppState.hpp"

#include <SFUI/Theme.hpp>

#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

// INFO: keep this bare minimums
// AppEngine is the core of the application,
// and should rely on as little external
// resources as reasonably possible.

void AppEngine::Init(AppSettings settings_)
{
	std::cout << "Initialising AppEngine." << std::endl;

	settings = settings_;

	for (int i = 0; i < settings.launchOptions.argc; i++)
	{
		std::string currentOption = settings.launchOptions.argv[i];

		if (currentOption == "-debug")
			settings.debug = true;

		if (currentOption == "-fullscreen")
			settings.fullscreen = true;

		if (currentOption == "-vsync")
			settings.vsync = true;

		if (currentOption == "-maxfps")
			// next arg should be an int (the maxfps)
			settings.maxfps = std::stoi(settings.launchOptions.argv[i += 1]);
	}

	// TODO: do this in startup state
	if (!fs::exists("./battlemapper_bin"))
		fs::create_directory("./battlemapper_bin");

	SFUI::Theme::loadFont("./battlemapper_bin/resources/interface/tahoma.ttf");
	SFUI::Theme::loadTexture("./battlemapper_bin/resources/interface/texture_square.png", settings.debug);
	SFUI::Theme::textCharacterSize = 12;
	SFUI::Theme::click.textColor = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::click.textColorHover = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::click.textColorFocus = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::input.textColor = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::input.textColorHover = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::input.textColorFocus = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::label.textColor = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::label.textColorHover = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::label.textColorFocus = SFUI::Theme::hexToRgb("#ffffff");
	SFUI::Theme::windowBgColor = SFUI::Theme::hexToRgb("#dddbde");
	SFUI::Theme::PADDING = 2.f;

	window = new sf::RenderWindow(sf::VideoMode(settings.width, settings.height), settings.title, sf::Style::Titlebar | sf::Style::Close);
	window->setVerticalSyncEnabled(true);

	running = true;

	std::cout << "AppEngine ready." << std::endl;
}

void AppEngine::Cleanup()
{
	std::cout << "Cleaning up AppEngine." << std::endl;
	
	// cleanup the all states
	while (!states.empty())
	{
		states.back()->Cleanup();

		delete states.back();
		states.pop_back();
	}

	window->close();
	delete window;
	window = nullptr;

	std::cout << "AppEngine cleaned up." << std::endl;
}

void AppEngine::ChangeState(AppState* state)
{
	if (!states.empty()) 
	{
		states.back()->Cleanup();

		delete states.back();
		states.pop_back();
	}

	states.push_back(state);
	states.back()->Init(this);
}

void AppEngine::PushState(AppState* state)
{
	// pause current state
	if (!states.empty())
		states.back()->Pause();

	// store and init the new state
	states.push_back(state);
	states.back()->Init(this);
}

void AppEngine::PopState()
{
	// cleanup the current state
	if (!states.empty())
	{
		states.back()->Cleanup();

		delete states.back();
		states.pop_back();
	}

	// resume previous state
	if (!states.empty())
		states.back()->Resume();
}

void AppEngine::HandleEvents()
{
	if (running)
		states.back()->HandleEvents();
}

void AppEngine::Update()
{
	// let the state update the game
	if (running)
		states.back()->Update();
}

void AppEngine::Draw()
{
	// let the state draw the screen
	if (running)
		states.back()->Draw();
}

void AppEngine::Quit()
{
	for (size_t i = 0; i < states.size(); i++)
	{
		states.back()->Cleanup();
		delete states.back();
		states.pop_back();
	}

	states.clear();

	running = false;
}
