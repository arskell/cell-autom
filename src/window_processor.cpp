
#include "window_processor.h"

void Window_processor::update_events(){
    std::lock_guard<std::mutex> eventer(eventHandler_owner);
    mtx.lock();
    while(renderWindow->isOpen()) {
        mtx.unlock();
        std::this_thread::sleep_for(UPDATE_DELAY);
        mtx.lock();
        renderWindow->clear();
        if(surface != nullptr) {
            renderbuffer.draw(surface->renderSprite);
        }
        renderbuffer.display();
        renderWindow->draw(sf::Sprite(renderbuffer.getTexture()));
        renderWindow->display();
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
                        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                            surface->drag(tmp.x,tmp.y);
                        }else {
                            surface->cursorOn(tmp.x, tmp.y);
                        }
                    }
                    break;
                case sf::Event::MouseWheelScrolled:
                    if(surface!= nullptr){
                        auto tmp = this->getCursorRelToWindow();
                        if(ev.mouseWheelScroll.delta>0){
                            surface->scrollUp(tmp.x, tmp.y);
                        }else if(ev.mouseWheelScroll.delta<0){
                            surface->scrollDown(tmp.x, tmp.y);
                        }
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
    mtx.unlock();
}

void Window_processor::setKeyEventHandler(std::function<void(sf::Event &)>&& _fnc) {
    keyEventHandler = std::forward<decltype(_fnc)>(_fnc);
}

void Window_processor::update(const sf::Sprite& sprt){
    std::lock_guard<std::mutex> lc(mtx);
    renderbuffer.draw(sprt);
}