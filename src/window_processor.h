

#ifndef CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H
#define CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>

#ifdef ISUNIX
#include <SFML/Graphics.hpp>
#else
#include <SFML/graphics.hpp>
#endif

#include "ui.h"

using namespace std::chrono_literals;

#define UPDATE_DELAY 40ms

class Window_processor {
public:

    Window_processor(){
        surface = nullptr;
    }

    void set_renderWindow(std::unique_ptr<sf::RenderWindow>&& wind){
        renderWindow = std::move(wind);
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
    void closeWindow();
    auto getCursorRelToWindow() const{
        return sf::Mouse::getPosition(*renderWindow);
    }
private:
    ui::Surface* surface;
    std::function<void(sf::Event&)> keyEventHandler;
    std::unique_ptr<sf::RenderWindow> renderWindow;
    std::mutex eventHandler_owner;
    std::mutex mtx;
};


#endif //CONWAY_S_GAME_OF_LIFE_WINDOW_PROCESSOR_H
