
#include "ui.h"


bool ui::UIElement::onPosiion(posType cursorX, posType cursorY)const {
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
}

void ui::Surface::clicked(posType cursorX, posType cursorY) {
    if(onPosiion(cursorX,cursorY)){
        for(auto& e:buttons){
            if(e->onPosiion(cursorX,cursorY)){
                e->click();
            }
        }
    }
}

void ui::Surface::cursorOn(posType cursorX, posType cursorY) {
    if(onPosiion(cursorX,cursorY)){
        for(auto& e:buttons){
            if(e->onPosiion(cursorX,cursorY)){
                e->cursorOnItem();
            }
        }
    }
}