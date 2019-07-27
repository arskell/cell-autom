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

void renderPlane(const game_of_life::Plane& plane,sf::RenderTexture* texture, unsigned int windowWidth, unsigned int windowHeight);

enum Mode{
    NONE,
    DRAW,
    ZOOM
};

enum Material{
    DEFAULT
};

struct draw_setup{
    Material material;
    unsigned int cursorSize;
};

struct zoom_setup{
    float k;
};

float scale;

int main() {
    Window_processor wp;

    constexpr unsigned int width = 800;
    constexpr unsigned int height = 600;

    //setup window manager
    std::promise<void> is_created;
    auto wind = std::async(std::launch::async, [&](){
        wp.set_renderWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(width,height), "Game of life"));
        is_created.set_value();
        wp.update_events();
    });

    is_created.get_future().wait(); /*   wait until window init    */

    sf::RenderTexture planeTexture;

    planeTexture.create(width-100,height);


   ui::Surface surf(0,0,width,height);

   std::atomic<unsigned int> update_rate(400);
    std::string Title = "Game of life, RATE: " + std::to_string(update_rate) + "ms";


    ui::Surface playGround(0,0,700, height);
    ui::Surface panel(700,0,100,height);

    bool is_paused(false);
    // setting up UI
    ui::Button UPbutton(10,4,50,50);
    UPbutton.setClickHandle([&](){
       update_rate+=50;
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

    ui::Button DOWNbutton(10,4+50+4,50,50);

    DOWNbutton.setClickHandle([&](){
        update_rate-=50;
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
    ui::Button PAUSEButtom(10,4+50+4+50+4,50,50);

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

    //setup cursor mode
    Mode mode = DRAW;

    //setup game of life plane
    game_of_life::Plane plane(80*4,60*4);

    for(game_of_life::planeSize i = 1; i < plane.getWidth(); ++i)
        plane.invert({i,0});

    for(game_of_life::planeSize i = 0; i < plane.getHeight(); ++i)
        plane.invert({0,i});


    // setting up canvas
    ui::Button UIplane(0,0,700,height);
    UIplane.element.setSize({0,0});
    UIplane.setClickHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        switch (mode){
            case DRAW:
                plane.setState({cursor.x/scale, cursor.y/scale},LIVE_CELL);
                break;
            default:
                break;
        }
    });

    std::atomic<bool> to_update(false);
    UIplane.setUpdateHandle([&](){
        if(to_update){
            renderPlane(plane, &planeTexture, width, height);
            to_update = false;
        }else if(is_paused){
            renderPlane(plane, &planeTexture, width, height);
        }
        playGround.renderTexture.draw(sf::Sprite(planeTexture.getTexture()));
        Title = "Game of life, RATE: " + std::to_string(update_rate) + "ms" + (is_paused ? ", PAUSED" : "");
        wp.setWindowTitleSync(Title);
    });

    playGround.setScrollDownHandle([&](){
       playGround.zoomChange(0.01);
    });

    playGround.setScrollUpHandle([&](){
        playGround.zoomChange(-0.01);
    });

    //adding elements on the surface
    panel.addButton(&PAUSEButtom);
    panel.addButton(&DOWNbutton);
    panel.addButton(&UPbutton);
    playGround.addButton(&UIplane);
    surf.addSurf(&panel);
    surf.addSurf(&playGround);
    wp.setSurface(&surf);


   while(wp.window_is_open()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(update_rate) );
        if(!is_paused) {
            plane.nextStep();
            to_update = true;
        }
    }

    wind.wait();
    return 0;
}

void renderPlane(const game_of_life::Plane& plane,sf::RenderTexture* texture, unsigned int windowWidth, unsigned int windowHeight){
    auto tmp1 = static_cast<float>(windowHeight)/plane.getHeight();
    auto tmp2 = static_cast<float>(windowWidth)/plane.getWidth();
    texture->clear();
    auto cellSize = tmp1>tmp2?tmp2:tmp1;
    scale = cellSize;
    sf::RectangleShape elem({cellSize, cellSize});
    elem.setFillColor(sf::Color::White);
    for(game_of_life::planeSize y = 0; y < plane.getHeight(); ++y){
        for(game_of_life::planeSize x = 0; x < plane.getWidth(); ++x){
            if(plane[{x,y}]){
                elem.setPosition(x*cellSize,y*cellSize);
                texture->draw(elem);
            }
        }
    }
    texture->display();
}