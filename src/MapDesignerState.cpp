#include "MapDesignerState.hpp"

#include <SFML/Graphics.hpp>

#include <fstream>

// format for save files
// 1name
// 2author
// 3revision
// 4application revision
// width
// height
// map information

enum RibbonCallbacks
{
	FileOpenToggled,
	FileSave,
	FileSaveAs,
	FileExportAs,
	FileLoad,

	ViewOpenToggled,
	ViewCenter,

	SettingsOpenToggled,
	SettingsAutoSaveOnClose,

	HelpOpenToggled,
	HelpManual,
	HelpHuntress,
	HelpAbout
};

void MapDesignerState::Init(AppEngine* app_)
{
	std::cout << "Initialising MapDesignerState" << std::endl;

	app = app_;

	ribbon.setLength(app->window->getSize().x);

	fileTab.setText("File");
	fileTab.addEntry(new RibbonEntry("Save", RibbonCallbacks::FileSave));
	fileTab.addEntry(new RibbonEntry("Save As", RibbonCallbacks::FileSaveAs));
	fileTab.addEntry(new RibbonEntry("Export As", RibbonCallbacks::FileExportAs));
	fileTab.addEntry(new RibbonEntry("Load", RibbonCallbacks::FileLoad));
	ribbon.addTab(fileTab);

	viewTab.setText("View");
	viewTab.addEntry(new RibbonEntry("Center View", RibbonCallbacks::ViewCenter));
	ribbon.addTab(viewTab);

/*
	settingsTab.setText("Settings");
	settingsTab.addEntry(new RibbonEntry("Auto Save when Closed", RibbonCallbacks::SettingsAutoSaveOnClose));
	ribbon.addTab(settingsTab);
*/

	helpTab.setText("Help");
	helpTab.addEntry(new RibbonEntry("User Manual", RibbonCallbacks::HelpManual));
	helpTab.addEntry(new RibbonEntry("Huntress", RibbonCallbacks::HelpHuntress));
	helpTab.addEntry(new RibbonEntry("About", RibbonCallbacks::HelpAbout));
	ribbon.addTab(helpTab);

	menuBackground.setSize(sf::Vector2f(85, app->window->getSize().y - SFUI::Theme::getBoxHeight()));

	tileset.loadFromFile("./battlemapper_bin/resources/tileset.png");

	std::ifstream definitionsParser("./battlemapper_bin/resources/tiledefinition.dat", std::ios::in | std::ios::binary);

	std::string line;
	while (std::getline(definitionsParser, line))
	{
		std::string id_s = line;
		id_s.erase(id_s.find_first_of(':'), id_s.length());
		int id = std::stoi(id_s);

		std::string name = line;
		name.erase(0, name.find_first_of(':') + 1);

		std::cout << id << ": " << name << std::endl;

		Tile t;
		t.name = name;
		t.idtype = id;
		tiles.emplace(id, t);
	}

	map.load("dock");

	sf::Vector2f center;
	center.x = (app->window->getSize().x / 2) - ((map.size.x * 32) / 2);
	center.y = (app->window->getSize().y / 2) - ((map.size.y * 32) / 2);
	map.visual.setPosition(center);

	ready = true;

	std::cout << "MapDesignerState ready." << std::endl;
}

void MapDesignerState::Cleanup()
{
	std::cout << "Cleaning up MapDesignerState." << std::endl;

	std::cout << "Cleaned up MapDesignerState." << std::endl;
}

void MapDesignerState::Pause()
{
	std::cout << "StartupState paused." << std::endl;
}

void MapDesignerState::Resume()
{
	std::cout << "StartupState resumed." << std::endl;
}

void MapDesignerState::HandleEvents()
{
	sf::Event event;
	while (app->window->pollEvent(event))
	{
		if (event.type == sf::Event::EventType::Closed)
			app->Quit();
		else if (event.type == sf::Event::EventType::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Key::C)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
				{
					std::cout << "need to copy an image of the map" << std::endl;
				}
			}
			else if (event.key.code == sf::Keyboard::Key::S)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
				{
					sf::Texture texture;
					texture.create(app->window->getSize().x, app->window->getSize().y);
					texture.update(*app->window);

					sf::Image fullImage = texture.copyToImage();

					sf::Image snippet;
					snippet.create(map.size.x * 32, map.size.y * 32);

					for (size_t i = 0; i < (map.size.y * 32); i++)
						for (size_t j = 0; j < (map.size.x * 32); j++)
							snippet.setPixel(j, i, fullImage.getPixel(map.visual.getPosition().x + j, map.visual.getPosition().y + i));

					if (snippet.saveToFile("./battlemapper_bin/" + map.name + ".png"))
					{
						std::cout << "screenshot saved to " << map.name << ".dat" << std::endl;
					}
 
					map.save();
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseButtonPressed)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
				{
					draggingMap = true;
					lastMousePosition = sf::Mouse::getPosition(*app->window);
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseButtonReleased)
		{
			if (draggingMap)
			{
				draggingMap = false;
			}
			else
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(*app->window);

				if (mouseIsOver(menuBackground))
				{
					float rowsDown = mousePos.y / 75;
					//float xPos = rowsDown + (10 * rowsDown);
					float xPos = rowsDown;

					placingTileType = xPos;
				}
				else
				{			
					// TODO: if mouse inside map area
					mousePos -= sf::Vector2i(map.visual.getPosition().x, map.visual.getPosition().y);
					map.setTile(sf::Vector2i(mousePos.x / 32, mousePos.y / 32), placingTileType);
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseMoved)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*app->window);
			
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if (draggingMap)
				{
					sf::Vector2i difference = mousePos - lastMousePosition;
					map.visual.move(sf::Vector2f(difference.x, difference.y));
					lastMousePosition = mousePos;
				}
				else
				{
					// TODO: if mouse inside map area
					mousePos -= sf::Vector2i(map.visual.getPosition().x, map.visual.getPosition().y);
					map.setTile(sf::Vector2i(mousePos.x / 32, mousePos.y / 32), placingTileType);
				}
			}
		}

		int ribbonCallback = ribbon.onEvent(event, app->window);

		if (ribbonCallback > 0)
			std::cout << "ribbon: " << ribbonCallback << std::endl;
		
		switch (ribbonCallback)
		{
		case RibbonCallbacks::ViewCenter:
		{
			sf::Vector2f center;
			center.x = (app->window->getSize().x / 2) - ((map.size.x * 32) / 2);
			center.y = (app->window->getSize().y / 2) - ((map.size.y * 32) / 2);
			map.visual.setPosition(center);
			break;
		}
		default:
			break;
		}
	}
}

void MapDesignerState::Update()
{
}

void MapDesignerState::Draw()
{
	app->window->clear(sf::Color(30, 30, 30));

	app->window->draw(map.visual);

	app->window->draw(menuBackground);

	for (size_t i = 0; i < tiles.size(); i++)
	{
		sf::RectangleShape shape;
		shape.setSize(sf::Vector2f(75, 75));
		shape.setPosition(sf::Vector2f(0, 75 * i));
		shape.setTexture(&tileset);
		shape.setTextureRect(sf::IntRect(32 * tiles[i].idtype, 0, 32, 32));

		app->window->draw(shape);
	}

	app->window->draw(ribbon);

	app->window->draw(hideTileSelectionButton);

	app->window->display();
}

bool MapDesignerState::mouseIsOver(const sf::Shape &object)
{
	if (object.getGlobalBounds().contains(app->window->mapPixelToCoords(sf::Mouse::getPosition(*app->window))))
		return true;

	return false;
}