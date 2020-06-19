#ifndef MAP_DESIGNER_STATE_HPP
#define MAP_DESIGNER_STATE_HPP

#include "AppEngine.hpp"
#include "AppState.hpp"
#include "Tilemap.hpp"

#include "Map.hpp"

#include <SFUI/SFUI.hpp>
#include <SFML/Graphics.hpp>

#include <map>
#include <vector>

class Ribbon;
class RibbonTab;

class RibbonEntry : public sf::Drawable
{
public:
    RibbonEntry() {}
    ~RibbonEntry() {}

    int callbackID = -1;
    sf::RectangleShape shape;

    virtual void setPosition(const sf::Vector2f& position) {}

    virtual int onMouseMove(const sf::Vector2i& position) {}
    virtual bool onMousePressed(const sf::Vector2i& position) {}

    virtual float getLength() const {}

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {}
};

class RibbonSpacerEntry : public RibbonEntry
{

};

class RibbonTextEntry : public RibbonEntry
{
public:
    RibbonTextEntry()
    {

    }

    RibbonTextEntry(const sf::String& title, const int callbackID = -1, const sf::String hotkey = "")
    {
        std::cout << "constructing ribbonentry. callback: " << callbackID << std::endl;

        text.setFont(SFUI::Theme::getFont());
        text.setCharacterSize(SFUI::Theme::textCharacterSize);

        this->hotkey.setFont(SFUI::Theme::getFont());
        this->hotkey.setCharacterSize(SFUI::Theme::textCharacterSize);

        shape.setFillColor(sf::Color(80, 80, 80));

        setText(title);
        setHotkey(hotkey);

        this->callbackID = callbackID;
    }

    RibbonTextEntry(const RibbonEntry&) = delete;

    void setText(const sf::String& string)
    {
        text.setString(string);
        resize();
    }

    void setHotkey(const sf::String& string)
    {
        hotkey.setString(string);
        resize();
    }

    void resize()
    {
        shape.setSize(sf::Vector2f(text.getGlobalBounds().width + hotkey.getGlobalBounds().width + divider.getSize().x + (SFUI::Theme::PADDING * 4), SFUI::Theme::getBoxHeight()));

        text.setPosition(shape.getPosition());
        text.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));

        hotkey.setPosition(shape.getPosition());
        hotkey.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));
    }

    void setPosition(const sf::Vector2f& position)
    {
        shape.setPosition(position);
        text.setPosition(position);
        text.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));
        hotkey.setPosition(position);
        hotkey.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));
    }

    float getLength() const
    {
        return shape.getGlobalBounds().width;
    }

    int onMouseMove(const sf::Vector2i& position)
    {
        if (shape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
            shape.setFillColor(sf::Color(120, 120, 120));
        else
            shape.setFillColor(sf::Color(80, 80, 80));

        return -1;
    }

    bool onMousePressed(const sf::Vector2i& position)
    {
        if (shape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
        {
            std::cout << "i been click" << std::endl;
            return true;
        }

        return false;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape, states);
        target.draw(divider, states);
        target.draw(text, states);
        target.draw(hotkey, states);
    }

    sf::RectangleShape shape;

private:
    sf::Text text;
    sf::Text hotkey;
    sf::RectangleShape divider;
};

class RibbonTab : public sf::Drawable
{
public:
    RibbonTab()
    {
        text.setFont(SFUI::Theme::getFont());
        text.setCharacterSize(SFUI::Theme::textCharacterSize);
        text.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));
        shape.setFillColor(sf::Color(60, 60, 60));
        dropDownShape.setFillColor(sf::Color(80, 80, 80));
    }

    void setText(const sf::String& text)
    {
        this->text.setString(text);
        shape.setSize(sf::Vector2f(this->text.getGlobalBounds().width + (SFUI::Theme::PADDING * 2), SFUI::Theme::getBoxHeight()));
    }

    void setPosition(const sf::Vector2f& newPosition)
    {
        shape.setPosition(newPosition);
        text.setPosition(newPosition);
        text.move(sf::Vector2f(SFUI::Theme::PADDING, SFUI::Theme::PADDING));
        std::cout << "setting position" << std::endl;
    }

    float getLength() const
    {
        return shape.getGlobalBounds().width;
    }

    void addEntry(RibbonEntry* entry)
    {
        if (entry->shape.getSize().x > currentLargestTextLength)
            currentLargestTextLength = entry->shape.getSize().x;

        sf::Vector2f size = dropDownShape.getSize();
        size.x = currentLargestTextLength;
        size.y += SFUI::Theme::getBoxHeight();
        dropDownShape.setSize(size);

        resposition();

        /*
        float length = 0;
        for (size_t i = 0; i < entries.size(); i++)
            length += entries[i].getLength() /*+ SFUI::Theme::MARGIN*\/;

        entry.setPosition(sf::Vector2f(length, SFUI::Theme::getBoxHeight()));
        */

        entries.push_back(entry);
    }

    void resposition()
    {
        dropDownShape.setPosition(sf::Vector2f(shape.getPosition().x, SFUI::Theme::getBoxHeight()));

        for (size_t i = 0; i < entries.size(); i++)
            entries[i]->setPosition(sf::Vector2f(dropDownShape.getPosition().x, SFUI::Theme::getBoxHeight() * (i + 1)));
    }

    int onMouseMove(const sf::Vector2i& position)
    {
        int code = -1;

        if (!isopen)
        {
            if (shape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
                shape.setFillColor(sf::Color(80, 80, 80));
            else
                shape.setFillColor(sf::Color(60, 60, 60));
        }
        else
        {
            if (dropDownShape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
            {
                for (RibbonEntry* entry : entries)
                    code = entry->onMouseMove(position);
            }
        }

        return code;
    }

    int onMousePressed(const sf::Vector2i& position)
    {
        // mouse is over shape
        if (shape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
        {
            if (!isopen)
                open();
        }
        else
        {
            // mouse is over dropdown
            if (dropDownShape.getGlobalBounds().contains(sf::Vector2f(position.x, position.y)))
            {
                for (RibbonEntry* entry : entries)
                {
                    if (entry->onMousePressed(position))
                    {
                        std::cout << "returning: " << entry->callbackID << std::endl;
                        close();
                        return entry->callbackID;
                    }
                }
            }
            else if (isopen)
                close();
        }

        return -1;
    }

    std::string getText() const
    {
        return text.getString().toAnsiString();
    }

    void open()
    {
        isopen = true;
    }

    void close()
    {
        isopen = false;
        shape.setFillColor(sf::Color(60, 60, 80));
    }

    bool isOpen() const
    {
        return isopen;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape, states);
        target.draw(text, states);

        if (isopen)
        {
            target.draw(dropDownShape, states);

            for (RibbonEntry* entry : entries)
                target.draw(*entry, states);
        }
    }

private:
    std::vector<RibbonEntry*> entries;

    sf::RectangleShape shape;
    sf::RectangleShape dropDownShape;

    sf::Text text;

    bool isopen = false;

    int currentLargestTextLength = -1;
};

class Ribbon : public sf::Drawable
{
public:
    Ribbon()
    {
        shape.setFillColor(sf::Color(50, 50, 50));
    }

    void setLength(const float& length)
    {
    	shape.setSize(sf::Vector2f(length, SFUI::Theme::getBoxHeight()));
    }

    void addTab(RibbonTab& tab)
    {
        float length = 0;
        for (size_t i = 0; i < tabs.size(); i++)
            length += tabs[i].getLength() /*+ SFUI::Theme::MARGIN*/;

        tab.setPosition(sf::Vector2f(length, 0));
        tab.resposition();
        tabs.push_back(tab);
    }

    int onEvent(const sf::Event& event, sf::RenderWindow* window)
    {
        currentFrameCallback = -1;

        switch (event.type)
        {
        case sf::Event::EventType::MouseMoved:
            for (RibbonTab& tab : tabs)
                tab.onMouseMove(sf::Mouse::getPosition(*window));
            break;
        case sf::Event::EventType::MouseButtonPressed:
            for (RibbonTab& tab : tabs)
            {
                int newCode = -1;
                newCode = tab.onMousePressed(sf::Mouse::getPosition(*window));
                if (newCode != -1)
                {
                    std::cout << "sending code " << newCode << " to application" << std::endl;
                    return newCode;
                }
            }
            break;
        default:
            return -1;
        }

        return currentFrameCallback;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape, states);

        for (size_t i = 0; i < tabs.size(); i++)
            target.draw(tabs[i], states);
    }

    int currentFrameCallback = -1;

private:
    sf::RectangleShape shape;

    std::vector<RibbonTab> tabs;
};

class MapDesignerState : public AppState
{
public:
	void Init(AppEngine* app_);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents();
	void Update();
	void Draw();

private:
	bool ready = false;

    Tileset tileset;
    Map map;

    struct Tile
    {
        int idtype = -1;
        std::string name;
    };

    // information about types of tiles
    // first: id
    // second: reference
    std::map<int, Tile> tiles;

    SFUI::Menu* menu;
    sf::RectangleShape menuBackground;
    sf::RectangleShape layerSelectBackground;

    Ribbon ribbon;

    RibbonTab fileTab;
    RibbonTab editTab;
    RibbonTab viewTab;
    RibbonTab settingsTab;
    RibbonTab helpTab;

    sf::RectangleShape hideTileSelectionButton;

    bool mouseIsOver(const sf::Shape &object);

    int placingTileType = 4;

    bool draggingMap = false;
	sf::Vector2i lastMousePosition;
};

#endif // !MAP_DESIGNER_STATE_HPP
