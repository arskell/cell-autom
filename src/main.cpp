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
#include "cell_autom.h"

using namespace std::chrono_literals;

struct Cursor_setup{
    enum{
        DRAW,
        ERASE
    }mode;
    unsigned int cursorRadius;
};

struct Render_settings{
    bool grid;
};


void renderPlane(const cell_autom::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight,
                 float& scale,
                 const Render_settings& render_settings);


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

    ////////////////
    //initializing data structures

    float scale = 1;
    //setup cursor mode
    Cursor_setup cursor_setup;
    cursor_setup.mode = Cursor_setup::DRAW;
    cursor_setup.cursorRadius = 2;


    //setup render settings
    Render_settings render_settings;
    render_settings.grid = true;
    ////////////////

    // setting up UI
    //GRID BUTTON
    sf::Texture switchGridTexture;
    switchGridTexture.loadFromFile(".\\res\\switchGrid.bmp");
    ui::Button GRIDButton(10,height-54-54,50,50);
    GRIDButton.element.setTexture(&switchGridTexture);
    GRIDButton.element.setFillColor(sf::Color::Cyan);
    GRIDButton.setClickHandle([&](){
        render_settings.grid=!render_settings.grid;
    });

    GRIDButton.element.setFillColor(sf::Color::Red);
    GRIDButton.setCursorOnItemHandle([&](){
        GRIDButton.element.setOutlineThickness(3);
    });
    GRIDButton.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&GRIDButton, cursor.x, cursor.y) ){
            GRIDButton.element.setOutlineThickness(0.f);
        }
    });

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
        if(update_speed != 0)update_speed-=50;
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
    ui::Button PAUSEButton(10,4+50+4+50+4,50,50);
    PAUSEButton.element.setTexture(&stopTexture);
    PAUSEButton.setClickHandle([&](){
        is_paused = !is_paused;
    });

    PAUSEButton.element.setFillColor(sf::Color::White);
    PAUSEButton.setCursorOnItemHandle([&](){
        PAUSEButton.element.setOutlineThickness(3);
    });
    PAUSEButton.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        if(!panel.relToPos(&PAUSEButton, cursor.x, cursor.y) ){
            PAUSEButton.element.setOutlineThickness(0.f);
        }
    });
    //SWITCH CURSOR MODE BUTTON

    sf::Texture switchButton;
    switchButton.loadFromFile(".\\res\\switchMode.bmp");
    ui::Button SWITCHMODEButton(10, height-54, 50,50);
    SWITCHMODEButton.element.setTexture(&switchButton);
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
    cell_autom::Plane plane(80*0.5,60*0.5);


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
            renderPlane(plane, &planeTexture, playGround.getWHSize().first ,
                        playGround.getWHSize().second,scale,render_settings);
            to_update = false;
        }else if(is_paused){
            renderPlane(plane, &planeTexture, playGround.getWHSize().first ,
                        playGround.getWHSize().second,scale,render_settings);
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
    panel.addButton((&GRIDButton));
    panel.addButton(&SWITCHMODEButton);
    panel.addButton(&PAUSEButton);
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

void renderPlane(const cell_autom::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight,
                 float& scale,
                 const Render_settings& render_settings){
    auto tmp1 = static_cast<float>(windowHeight)/plane.getHeight();
    auto tmp2 = static_cast<float>(windowWidth)/plane.getWidth();
    texture->clear();
    scale = tmp1>tmp2?tmp2:tmp1;
    auto recSize = scale;
    if(render_settings.grid)
        recSize-=1;
    sf::RectangleShape elem({recSize, recSize});
    elem.setFillColor(sf::Color::White);
    sf::RectangleShape vLine({1, scale*plane.getHeight()});
    sf::RectangleShape hLine({scale*plane.getWidth(), 1});

    vLine.setFillColor(sf::Color(128,128,128));
    hLine.setFillColor(sf::Color(128,128,128));

    if(render_settings.grid) {

         for (cell_autom::planeSize x = 0; x < plane.getWidth(); ++x) {
             vLine.setPosition(x * scale - 1, 0);
             texture->draw(vLine);
         }

         for (cell_autom::planeSize y = 0; y < plane.getHeight(); ++y) {
             hLine.setPosition(0, y * scale - 1);
             texture->draw(hLine);
         }
    }
    vLine.setPosition(scale*plane.getWidth() - 1,0);
    texture->draw(vLine);
    hLine.setPosition(0,scale*plane.getHeight());
    texture->draw(hLine);
    for(cell_autom::planeSize y = 0; y < plane.getHeight(); ++y){
        for(cell_autom::planeSize x = 0; x < plane.getWidth(); ++x){
            if(plane[{x,y}]){
                elem.setPosition(x*scale,y*scale);
                texture->draw(elem);
            }
        }
    }
    texture->display();
}