
#include "window_processor.h"

void Window_processor::update_events(){
    std::lock_guard<std::mutex> eventer(eventHandler_owner);
    mtx.lock();
    while(renderWindow->isOpen()) {
        renderWindow->setActive(false);
        mtx.unlock(),std::this_thread::sleep_for(UPDATE_DELAY),mtx.lock();
        renderWindow->setActive(true);
        sf::Event ev;
        if (renderWindow->pollEvent(ev)) {
            switch (ev.type) {
                case sf::Event::Closed:
                    renderWindow->close();
                    break;
                case sf::Event::KeyPressed:
                    keyEventHandler(ev);
                    break;
                case sf::Event::MouseButtonPressed:
                    if(surface!= nullptr){
                        auto tmp = sf::Mouse::getPosition(*renderWindow);
                        surface->clicked(tmp.x, tmp.y);
                    }
                    break;
                case sf::Event::MouseMoved:
                    if(surface!= nullptr){
                        auto tmp = sf::Mouse::getPosition(*renderWindow);
                        surface->cursorOn(tmp.x, tmp.y);
                    }
                    break;
                default:
                    break;
            }
        }
        if(surface != nullptr){
            surface->updateContent();
        }
    }
    renderWindow->setActive(false);
    mtx.unlock();
}

void Window_processor::setKeyEventHandler(std::function<void(sf::Event &)>&& _fnc) {
    keyEventHandler = std::forward<decltype(_fnc)>(_fnc);
}

/* unstable
 *
 * TO REMOVE*/
void Window_processor::closeWindow() {
    //std::lock_guard<std::mutex> lc(mtx);
    //renderWindow->setActive(true);
    renderWindow->close();
    //renderWindow->setActive(false);
}

void Window_processor::update(const sf::Sprite& sprt){
    std::lock_guard<std::mutex> lc(mtx);
    renderWindow->setActive(true);
    renderWindow->clear();
    renderWindow->draw(sprt);
    renderWindow->display();
    renderWindow->setActive(false);
}