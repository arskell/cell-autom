
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
    for(auto&e: child_surf){
        e->updateContent();
        renderTexture.draw(e->renderSprite);
    }
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
        for(auto&e: child_surf){
            e->clicked(relCursorX,relCursorY);
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
        for(auto&e: child_surf){
            e->cursorOn(relCursorX,relCursorY);
        }
    }
}

bool ui::Surface::relToPos(const ui::UIElement *elem, ui::posType x, ui::posType y) {
    if(!onPosition(x, y))return false;
    return elem->onPosition(x - xPos, y - yPos);
}