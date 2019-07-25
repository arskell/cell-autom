
#include "ui.h"


bool ui::UIElement::onPosition(posType cursorX, posType cursorY)const {
    if((cursorX >= xPos && cursorX <= xPos+width)&&
       (cursorY >= yPos && cursorY <= yPos+height)){
        return true;
    }
    return false;
}

void ui::Surface::updateContent(){
    renderTexture.clear();
    for(auto& e:items){
        e->updateContent();
        renderTexture.draw(e->element);
    }
    for(auto& e:buttons){
        e->updateContent();
        renderTexture.draw(e->element);
    }
    renderTexture.display();
    renderSprite = sf::Sprite(renderTexture.getTexture());
    renderSprite.setPosition(xPos,yPos);
}

void ui::Surface::clicked(posType cursorX, posType cursorY) {
    auto relCursorX = cursorX - xPos;
    auto relCursorY = cursorY - yPos;
    if(onPosition(cursorX,cursorY)){
        for(auto& e:buttons){
            if(e->onPosition(relCursorX,relCursorY)){
                e->click();
            }
        }
    }
}

void ui::Surface::cursorOn(posType cursorX, posType cursorY) {
    auto relCursorX = cursorX - xPos;
    auto relCursorY = cursorY - yPos;
    if(onPosition(cursorX,cursorY)){
        for(auto& e:buttons){
            if(e->onPosition(relCursorX,relCursorY)){
                e->cursorOnItem();
            }
        }
    }
}