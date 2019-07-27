
#ifndef PROJECT_UI_H
#define PROJECT_UI_H

#include <utility>
#include <list>
#include <functional>

#ifdef ISUNIX
#include <SFML/Graphics.hpp>
#else
#include <SFML/graphics.hpp>
#endif

namespace ui {
    using posType  = unsigned int;
    using sizeType = unsigned int;

    class UIElement{
    public:
        explicit UIElement(posType x = 0, posType y = 0, sizeType _width = 0, sizeType _height = 0):
                xPos(x),yPos(y),width(_width),height(_height){}
        bool onPosition(posType cursorX, posType cursorY)const;
        virtual void updateContent() = 0;
        std::pair<posType,posType> getXYposition()const{return {xPos, yPos};};
        std::pair<sizeType,sizeType> getWHSize()const{return {width, height};};
        void setXposition(posType new_x){xPos = new_x;}
        void setYposition(posType new_y){yPos = new_y;}
        void setWidth(sizeType w){width = w;}
        void setHeight(sizeType h){height = h;}
    protected:
        posType xPos, yPos;
        sizeType width, height;
    };

    class Item:     public UIElement{
    public:
        template<typename ...T>
        Item(T... UIsettings):UIElement(UIsettings...){
            element.setPosition(xPos, yPos);
            element.setSize({width,height});
            cursorOnHandle = [](){};
            updateHandle = [](){};
        }
        virtual void updateContent() override{updateHandle();}
        void cursorOnItem(){cursorOnHandle();}
        void setUpdateHandle( std::function<void()> hndl){updateHandle = std::move(hndl);}
        void setCursorOnItemHandle( std::function<void()> hndl){cursorOnHandle = std::move(hndl);}
        sf::RectangleShape element;
    protected:
        std::function<void()> cursorOnHandle;
        std::function<void()> updateHandle;
    };


    class Button:   public Item{
    public:
        template<typename ...T>
        Button(T... UIsettings):Item(UIsettings...){
            clickHandle = [](){};
        }
        void click(){clickHandle();}
        void setClickHandle(  std::function<void()> hndl){clickHandle = std::move(hndl);}
    protected:
        std::function<void()> clickHandle;
    };
    class Surface:   public UIElement{
    public:
        template<typename ...T>
        Surface(T... UIsettings):UIElement(UIsettings...){
            renderTexture.create(width,height);
            scrollDownHandle = [](){};
            scrollUpHandle = [](){};
            dragHandle = [](){};
            zoom = 1;
        }
        virtual void updateContent() override;
        void clicked(posType cursorX, posType cursorY);
        void cursorOn(posType cursorX, posType cursorY);
        void scrollUp(posType cursorX, posType cursorY);
        void scrollDown(posType cursorX, posType cursorY);
        void drag(posType cursorX, posType cursorY);
        void setScrollUpHandle(std::function<void()>&& hndl ){
            scrollUpHandle = std::forward<decltype(hndl)>(hndl);
        }
        void setScrollDownHandle(std::function<void()>&& hndl ){
            scrollDownHandle = std::forward<decltype(hndl)>(hndl);
        }
        void dragUpHandle(std::function<void()>&& hndl ){
            dragHandle = std::forward<decltype(hndl)>(hndl);
        }
        void addButton(ui::Button* button){
            buttons.push_back(button);
             }
        void addItem(ui::Item* item){
            items.push_back(item);
        }

        void addSurf(ui::Surface* srf){
            child_surf.push_back(srf);
        }

        void zoomChange(float d){
            zoom+=d;
        }

        void setZoom(float z){
            zoom = z;
        }

        bool relToPos(const UIElement* elem, ui::posType x, ui::posType y);

        sf::RenderTexture renderTexture;
        sf::Sprite renderSprite;
        sf::View view;
    private:
        std::function<void()> scrollUpHandle;
        std::function<void()> scrollDownHandle;
        std::function<void()> dragHandle;
        std::list<ui::Button*> buttons;
        std::list<ui::Item*> items;
        std::list<ui::Surface*> child_surf;
        float zoom;
    };

}

#endif //PROJECT_UI_H
