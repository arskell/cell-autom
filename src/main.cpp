#include <thread>
#include <future>
#include <chrono>
#include <iostream>
#include <atomic>
#include <string>

#ifdef ISUNIX
#include <SFML/Graphics.hpp>
#else
#include <SFML/graphics.hpp>
#endif

#include "window_processor.h"
#include "game_of_life.h"

using namespace std::chrono_literals;

void renderPlane(const game_of_life::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight);


struct Cursor_setup{
    enum{
        DRAW,
        ERASE
    }mode;
    unsigned int cursorRadius;
};

float scale;

int main() {
    Window_processor wp;

    constexpr unsigned int width = 800;
    constexpr unsigned int height = 600;

    //setup window manager
    std::promise<void> is_created;
    auto wind = std::async(std::launch::async, [&](){
        wp.set_renderWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(width,height,8), "Game of life",
                sf::Style::Close | sf::Style::Titlebar));
        is_created.set_value();
        wp.update_events();
    });

    is_created.get_future().wait(); /*   wait until window init    */

    sf::RenderTexture planeTexture;

    planeTexture.create(width-100,height);
    ui::Surface surf(0,0,width,height);

    std::atomic<unsigned int> update_speed(400);
    std::string Title = "Game of life, UPDATE SPEED: " + std::to_string(update_speed) + "ms";


    ui::Surface playGround(0,0,700, height);
    ui::Surface panel(700,0,100,height);

    bool is_paused(false);
    // setting up UI


    //UP BUTTON
    sf::Texture speedDownTexture;
    speedDownTexture.loadFromFile(".\\res\\speedDown.bmp");
    ui::Button UPbutton(10,4,50,50);
    UPbutton.element.setTexture(&speedDownTexture);
    UPbutton.setClickHandle([&](){
       update_speed+=50;
    });

    UPbutton.element.setFillColor(sf::Color::Red);
    UPbutton.setCursorOnItemHandle([&](){
        UPbutton.element.setOutlineThickness(3);
    });
    UPbutton.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&UPbutton, cursor.x, cursor.y) ){
            UPbutton.element.setOutlineThickness(0.f);
        }
    });

    //DOWN BUTTON
    sf::Texture speedUpTexture;
    speedUpTexture.loadFromFile(".\\res\\speedUp.bmp");
    ui::Button DOWNbutton(10,4+50+4,50,50);
    DOWNbutton.element.setTexture(&speedUpTexture);
    DOWNbutton.setClickHandle([&](){
        update_speed-=50;
    });

    DOWNbutton.element.setFillColor(sf::Color::Green);
    DOWNbutton.setCursorOnItemHandle([&](){
        DOWNbutton.element.setOutlineThickness(3);
    });
    DOWNbutton.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&DOWNbutton, cursor.x, cursor.y) ){
            DOWNbutton.element.setOutlineThickness(0.f);
        }
    });

    //PAUSE BUTTON
    sf::Texture stopTexture;
    stopTexture.loadFromFile(".\\res\\stop.bmp");
    ui::Button PAUSEButtom(10,4+50+4+50+4,50,50);
    PAUSEButtom.element.setTexture(&stopTexture);
    PAUSEButtom.setClickHandle([&](){
        is_paused = !is_paused;
    });

    PAUSEButtom.element.setFillColor(sf::Color::White);
    PAUSEButtom.setCursorOnItemHandle([&](){
        PAUSEButtom.element.setOutlineThickness(3);
    });
    PAUSEButtom.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&PAUSEButtom, cursor.x, cursor.y) ){
            PAUSEButtom.element.setOutlineThickness(0.f);
        }
    });

    sf::Texture switchButton;
    switchButton.loadFromFile(".\\res\\switchMode.bmp");
    ui::Button SWITCHMODEButton(10, height-54, 50,50);
    SWITCHMODEButton.element.setTexture(&switchButton);


    //setup cursor mode
    Cursor_setup cursor_setup;
    cursor_setup.mode = Cursor_setup::DRAW;
    cursor_setup.cursorRadius = 2;


    SWITCHMODEButton.setClickHandle([&](){
       if(cursor_setup.mode == Cursor_setup::DRAW){
           cursor_setup.mode = Cursor_setup::ERASE;
       } else{
           cursor_setup.mode = Cursor_setup::DRAW;
       }
    });

    SWITCHMODEButton.element.setFillColor(sf::Color::White);
    SWITCHMODEButton.setCursorOnItemHandle([&](){
        SWITCHMODEButton.element.setOutlineThickness(3);
    });
    SWITCHMODEButton.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&SWITCHMODEButton, cursor.x, cursor.y) ){
            SWITCHMODEButton.element.setOutlineThickness(0.f);
        }
    });


    //setup game of life plane
    game_of_life::Plane plane(80*0.5,60*0.5);


    // setting up canvas
    ui::Button UIplane(0,0,700,height);
    UIplane.element.setSize({0,0});
    auto playGround_Handler = [&](){
        auto cursor = wp.getCursorRelToWindow();
        switch (cursor_setup.mode){
            case Cursor_setup::DRAW:
                if(cursor_setup.cursorRadius > 0)
                    plane.fill({cursor.x/scale, cursor.y/scale}, cursor_setup.cursorRadius, LIVE_CELL);
                else
                    plane.setState({cursor.x/scale, cursor.y/scale},LIVE_CELL);
                break;
            case Cursor_setup::ERASE:
                if(cursor_setup.cursorRadius > 0)
                    plane.fill({cursor.x/scale, cursor.y/scale}, cursor_setup.cursorRadius, DEAD_CELL);
                else
                    plane.setState({cursor.x/scale, cursor.y/scale},DEAD_CELL);
                break;
            default:
                break;
        }
    };
    UIplane.setClickHandle(playGround_Handler);

    playGround.setDragHandle(playGround_Handler);

    std::atomic<bool> to_update(false);
    UIplane.setUpdateHandle([&](){
        if(to_update){
            renderPlane(plane, &planeTexture, width, height);
            to_update = false;
        }else if(is_paused){
            renderPlane(plane, &planeTexture, width, height);
        }
        playGround.renderTexture.draw(sf::Sprite(planeTexture.getTexture()));
        Title = "Game of life, UPDATE SPEED: " +
                std::to_string(update_speed) +
                "ms" + (is_paused ? ", PAUSED" : "") +
                ", CURSOR RADIUS: " +
                (cursor_setup.cursorRadius==0?"0.5":std::to_string(cursor_setup.cursorRadius))+
                ", MODE: "+ (cursor_setup.mode==Cursor_setup::DRAW?"DRAW":"ERASE");
        wp.setWindowTitleSync(Title);
    });

    playGround.setScrollDownHandle([&](){
       cursor_setup.cursorRadius+=1;
    });

    playGround.setScrollUpHandle([&](){
        cursor_setup.cursorRadius-=1;
    });

    //adding elements on the surface
    panel.addButton(&SWITCHMODEButton);
    panel.addButton(&PAUSEButtom);
    panel.addButton(&DOWNbutton);
    panel.addButton(&UPbutton);
    playGround.addButton(&UIplane);
    surf.addSurf(&panel);
    surf.addSurf(&playGround);
    wp.setSurface(&surf);


   while(wp.window_is_open()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(update_speed) );
        if(!is_paused) {
            plane.nextStep();
            to_update = true;
        }
    }

    wind.wait();
    return 0;
}

void renderPlane(const game_of_life::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight){
    auto tmp1 = static_cast<float>(windowHeight)/plane.getHeight();
    auto tmp2 = static_cast<float>(windowWidth)/plane.getWidth();
    texture->clear();
    scale = tmp1>tmp2?tmp2:tmp1;
    sf::RectangleShape elem({scale, scale});
    elem.setFillColor(sf::Color::White);
    for(game_of_life::planeSize y = 0; y < plane.getHeight(); ++y){
        for(game_of_life::planeSize x = 0; x < plane.getWidth(); ++x){
            if(plane[{x,y}]){
                elem.setPosition(x*scale,y*scale);
                texture->draw(elem);
            }
        }
    }
    texture->display();
}