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
	FileViewImage,

	EditOpenToggled,
	EditResizeMap,

	ViewOpenToggled,
	ViewCenter,

	SettingsOpenToggled,
	SettingsEnableExperimentalFeatures,
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
	fileTab.addEntry(new RibbonTextEntry("Save", RibbonCallbacks::FileSave));
	//fileTab.addEntry(new RibbonTextEntry("Save As", RibbonCallbacks::FileSaveAs));
	//fileTab.addEntry(new RibbonTextEntry("Export As", RibbonCallbacks::FileExportAs));
	fileTab.addEntry(new RibbonTextEntry("Load", RibbonCallbacks::FileLoad));
	//fileTab.addEntry(new RibbonSpacerEntry);
	fileTab.addEntry(new RibbonTextEntry("View Image", RibbonCallbacks::FileViewImage));
	ribbon.addTab(fileTab);

/*
	editTab.setText("Edit");
	editTab.addEntry(new RibbonTextEntry("Resize Map", RibbonCallbacks::EditResizeMap));
	ribbon.addTab(editTab);
*/

	viewTab.setText("View");
	viewTab.addEntry(new RibbonTextEntry("Center View", RibbonCallbacks::ViewCenter));
	ribbon.addTab(viewTab);

/*
	settingsTab.setText("Settings");
	settingsTab.addEntry(new RibbonTextEntry("Toggle Experimental Features", RibbonCallbacks::SettingsEnableExperimentalFeatures));
	settingsTab.addEntry(new RibbonTextEntry("Auto Save when Closed", RibbonCallbacks::SettingsAutoSaveOnClose));
	ribbon.addTab(settingsTab);
*/

	helpTab.setText("Help");
	helpTab.addEntry(new RibbonTextEntry("User Manual", RibbonCallbacks::HelpManual));
	helpTab.addEntry(new RibbonTextEntry("Huntress", RibbonCallbacks::HelpHuntress));
	helpTab.addEntry(new RibbonTextEntry("About", RibbonCallbacks::HelpAbout));
	ribbon.addTab(helpTab);

	menuBackground.setSize(sf::Vector2f(85, app->window->getSize().y));
	menuBackground.setFillColor(sf::Color(70, 70, 70));

	layerSelectBackground.setSize(sf::Vector2f(85, app->window->getSize().y));
	layerSelectBackground.setPosition(sf::Vector2f(app->window->getSize().x - 85, 0));
	layerSelectBackground.setFillColor(sf::Color(70, 70, 70));

	std::ifstream definitionsParser("./battlemapper_bin/resources/tiles/tiledefinition.dat", std::ios::in | std::ios::binary);

	std::string line;
	while (std::getline(definitionsParser, line))
	{
		std::string id_s = line;
		id_s.erase(id_s.find_first_of(':'), id_s.length());

		std::string name = line;
		name.erase(0, name.find_first_of(':') + 1);

		std::cout << id_s << ": " << name << std::endl;

		Tile t;
		t.name = name;
		t.idtype = tiles.size();
		tiles.emplace(t.idtype, t);
	}

	tileset.loadFromDirectory("./battlemapper_bin/resources/tiles", sf::Vector2u(32, 32));

	map.load("dock", tileset);

	sf::Vector2f center;
	center.x = (app->window->getSize().x / 2) - ((map.size.x * 32) / 2);
	center.y = (app->window->getSize().y / 2) - ((map.size.y * 32) / 2);
	map.setPosition(center);

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
			if (event.key.control && event.key.code == sf::Keyboard::Key::C)
			{
				std::cout << "need to copy an image of the map" << std::endl;
			}
			else if (event.key.control && event.key.code == sf::Keyboard::Key::S)
			{
				sf::Texture texture;
				texture.create(app->window->getSize().x, app->window->getSize().y);
				texture.update(*app->window);

				sf::Image fullImage = texture.copyToImage();

				sf::Image snippet;
				snippet.create(map.size.x * 32, map.size.y * 32);

				for (size_t i = 0; i < (map.size.y * 32); i++)
					for (size_t j = 0; j < (map.size.x * 32); j++)
						snippet.setPixel(j, i, fullImage.getPixel(map.getPosition().x + j, map.getPosition().y + i));

				if (snippet.saveToFile("./battlemapper_bin/" + map.name + ".png"))
					std::cout << "screenshot saved to " << map.name << ".dat" << std::endl;

				map.save();
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
				sf::Vector2f mousePosf = sf::Vector2f(mousePos.x, mousePos.y);

				if (mouseIsOver(menuBackground))
				{
					float rowsDown = mousePos.y / 75;
					//float xPos = rowsDown + (10 * rowsDown);
					float xPos = rowsDown;

					placingTileType = xPos;
				}
				else
				{		
					sf::FloatRect rect(map.getPosition().x, map.getPosition().y, map.size.x * tileset.tileSize.x, map.size.y * tileset.tileSize.y);

					if (rect.contains(mousePosf))
					{
						mousePos -= sf::Vector2i(map.getPosition().x, map.getPosition().y);
						map.setTile(sf::Vector2i(mousePos.x / 32, mousePos.y / 32), placingTileType, tileset);
					}
					else
					{
						/*
						if (map.layers[map.layers.begin() + map.currentLayer].up.getGlobalBounds().contains(mousePosf))
							map.increaseSizeUp(1, tileset);
						else if (map.layers[map.layers.begin() + map.currentLayer].down.getGlobalBounds().contains(mousePosf))
						{
							map.layers[map.layers.begin() + map.currentLayer].move(sf::Vector2f(0, -1 * (int)tileset.tileSize.y));
							map.increaseSizeDown(1, tileset);
						}
						else if (map.layers[map.layers.begin() + map.currentLayer].left.getGlobalBounds().contains(mousePosf))
							map.increaseSizeLeft(1, tileset);
						else if (map.layers[map.layers.begin() + map.currentLayer].visual.right.getGlobalBounds().contains(mousePosf))
						{
							map.move(sf::Vector2f(-1 * (int)tileset.tileSize.x, 0));
							map.increaseSizeRight(1, tileset);
						}
						*/
					}
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseMoved)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*app->window);
			sf::Vector2f mousePosf = sf::Vector2f(mousePos.x, mousePos.y);
			
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if (draggingMap)
				{
					sf::Vector2i difference = mousePos - lastMousePosition;
					map.move(sf::Vector2f(difference.x, difference.y));
					lastMousePosition = mousePos;
				}
				else
				{
					sf::FloatRect rect(map.getPosition().x, map.getPosition().y, map.size.x * tileset.tileSize.x, map.size.y * tileset.tileSize.y);

					if (rect.contains(mousePosf))
					{
						mousePos -= sf::Vector2i(map.getPosition().x, map.getPosition().y);
						map.setTile(sf::Vector2i(mousePos.x / 32, mousePos.y / 32), placingTileType, tileset);
					}
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
			map.setPosition(center);
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

	app->window->draw(map);

	app->window->draw(menuBackground);

	for (size_t i = 0; i < tiles.size(); i++)
	{
		sf::RectangleShape shape;
		shape.setSize(sf::Vector2f(75, 75));
		shape.setPosition(sf::Vector2f(0, (75 * i) + SFUI::Theme::getBoxHeight()));
		shape.setTexture(&tileset);
		shape.setTextureRect(sf::IntRect(tileset.tileSize.x * tiles[i].idtype, 0, 32, 32));

		app->window->draw(shape);
	}

	app->window->draw(layerSelectBackground);

	for (int i = 0; i < map.layers.size(); i++)
	{
		sf::RectangleShape shape;
		shape.setSize(sf::Vector2f(75, 75));
		shape.setPosition(sf::Vector2f(app->window->getSize().x - layerSelectBackground.getSize().x + 10, (75 * i) + SFUI::Theme::getBoxHeight()));
		shape.setFillColor(sf::Color(80, 80, 80));

		if (i == map.currentLayer)
		{
			shape.setOutlineColor(sf::Color(50, 50, 50));
			shape.setOutlineThickness(-2);
		}
		else
		{
			shape.setOutlineColor(sf::Color(70, 70, 70));
			shape.setOutlineThickness(-1);
		}

		app->window->draw(shape);
		
		sf::Text layerNumber;
		layerNumber.setCharacterSize(32);
		layerNumber.setFont(SFUI::Theme::getFont());
		layerNumber.setString(std::to_string(i));
		layerNumber.setPosition(shape.getPosition() + sf::Vector2f(28, 16));

		app->window->draw(layerNumber);
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