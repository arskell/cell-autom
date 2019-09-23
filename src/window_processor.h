

#ifndef CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H
#define CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H

#include <memory>
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <functional>

#include <SFML/Graphics.hpp>

#include "ui.h"

using namespace std::chrono_literals;

#define UPDATE_DELAY 5ms

class Window_processor {
public:

    Window_processor(){
        surface = nullptr;
        keyEventHandler = [](sf::Event&){};
    }

    void set_renderWindow(std::unique_ptr<sf::RenderWindow>&& wind){
        renderWindow = std::move(wind);
        renderbuffer.create(renderWindow->getSize().x, renderWindow->getSize().y);
    }

    bool window_is_open() const{
        return renderWindow->isOpen();
    }

    void setSurface(ui::Surface* srfc){
        mtx.lock(),surface = srfc,mtx.unlock();}
    ui::Surface* getSurface()const{return surface;}
    void update(const sf::Sprite& sprt);
    void update_events();
    void setKeyEventHandler(std::function<void(sf::Event&)>&& _fnc);
    auto getCursorRelToWindow() const{
        return sf::Mouse::getPosition(*renderWindow);
    }
    void setWindowTitleSync(const std::string& name){
        renderWindow->setTitle(name);
    }
private:
    ui::Surface* surface;
    std::function<void(sf::Event&)> keyEventHandler;
    sf::RenderTexture renderbuffer;
    std::unique_ptr<sf::RenderWindow> renderWindow;
    std::mutex eventHandler_owner;
    std::mutex mtx;
};


#endif //CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H
