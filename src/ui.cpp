
#include "ui.h"


bool ui::UIElement::onPosition(posType cursorX, posType cursorY)const {
    return (cursorX >= xPos && cursorX <= xPos + width) &&
           (cursorY >= yPos && cursorY <= yPos + height);
}

void ui::Surface::updateContent(){
    renderTexture.clear();
    for(auto&e: child_surf){
        e->updateContent();
        renderTexture.draw(e->renderSprite);
    }
    for(auto e:items){
        e->updateContent();
        renderTexture.draw(e->element);
    }
    for(auto e:buttons){
        e->updateContent();
        renderTexture.draw(e->element);
    }

    for(auto e: text_items){
        e->updateContent();
        renderTexture.draw(e->text);
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

void ui::Surface::scrollUp(posType cursorX, posType cursorY) {
    if(relToPos(this, cursorX, cursorY)) {
        for (auto &e: child_surf) {
            e->scrollUp(cursorX - xPos, cursorY - yPos);
        }
        scrollUpHandle();
    }
}

void ui::Surface::scrollDown(posType cursorX, posType cursorY) {
    if(relToPos(this, cursorX, cursorY)) {
        for (auto &e: child_surf) {
            e->scrollDown(cursorX - xPos, cursorY - yPos);
        }
        scrollDownHandle();
    }
}

void ui::Surface::drag(posType cursorX, posType cursorY) {
    if(relToPos(this, cursorX, cursorY)) {
        for (auto &e: child_surf) {
            e->drag(cursorX - xPos, cursorY - yPos);
        }
        dragHandle();
    }
}
