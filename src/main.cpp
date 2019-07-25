#include <thread>
#include <future>
#include <chrono>
#include <iostream>

#ifdef ISUNIX
#include <SFML/Graphics.hpp>
#else
#include <SFML/graphics.hpp>
#endif

#include "window_processor.h"
#include "game_of_life.h"

using namespace std::chrono_literals;

void renderPlane(game_of_life::Plane& plane,sf::RenderTexture* texture, unsigned int windowWidth, unsigned int windowHeight);

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

int main() {
    Window_processor wp;

    constexpr unsigned int width = 800;
    constexpr unsigned int height = 600;

    std::promise<void> is_created;
    auto wind = std::async(std::launch::async, [&](){
        wp.set_renderWindow(std::make_unique<sf::RenderWindow>(sf::VideoMode(width,height), "Game of life"));
        is_created.set_value();
        wp.update_events();
    });



    is_created.get_future().wait(); /*   wait until window init    */

    sf::RenderTexture planeTexture;
    planeTexture.create(width,height);
/*
    sf::RectangleShape rec;
    rec.setPosition(0,0);
    rec.setFillColor(sf::Color::White);
    rec.setSize({40,40});

    planeTexture.draw(rec);
    planeTexture.display();
    wp.update(sf::Sprite(planeTexture.getTexture()));
*/
    ui::Surface surf(0,0,width,height);

    ui::Button b(4,10,50,50);
    ui::Button b2(40,70,50,50);
    b.setClickHandle([&](){
        std::cout<<"clicked!!!"<<std::endl;
    });


    b.element.setFillColor(sf::Color::Blue);
    b2.element.setFillColor(sf::Color::Cyan);
    surf.addButton(b2);
    surf.addButton(b);
    wp.setSurface(&surf);
    b.setCursorOnItemHandle([&](){
        b.element.setOutlineThickness(10);
    });
    b.setUpdateHandle([&](){
        auto tmp = wp.getCursorRelToWindow();
        if(!b.onPosiion(tmp.x, tmp.y))
            b.element.setOutlineThickness(0.f);
    });

    b2.element.setOutlineColor(sf::Color::Green);

    b2.setCursorOnItemHandle([&](){
        b2.element.setOutlineThickness(10);
    });
    b2.setUpdateHandle([&](){
        auto tmp = wp.getCursorRelToWindow();
        if(!b2.onPosiion(tmp.x, tmp.y))
            b2.element.setOutlineThickness(0.f);
    });


    surf.renderTexture.display();
    while(true) {
        wp.update(sf::Sprite(surf.renderTexture.getTexture()));
        std::this_thread::sleep_for(90ms);
    }
/*
    game_of_life::Plane plane(80*4,60*4);

   // for(game_of_life::planeSize i = 0; i < plane.getWidth(); ++i)
   //     plane.invert({i,0});

   // for(game_of_life::planeSize i = 0; i < plane.getHeight(); ++i)
    //    plane.invert({0,i});
plane.invert({0,0});
    while(wp.window_is_open()) {
        renderPlane(plane, &planeTexture, width, height);
        wp.update(sf::Sprite(planeTexture.getTexture()));
        planeTexture.clear();
        std::this_thread::sleep_for(400s);
        plane.nextStep();
    }
*/
    wind.wait();
    return 0;
}

void renderPlane(game_of_life::Plane& plane,sf::RenderTexture* texture, unsigned int windowWidth, unsigned int windowHeight){
    auto tmp1 = static_cast<float>(windowHeight)/plane.getHeight();
    auto tmp2 = static_cast<float>(windowWidth)/plane.getWidth();

    auto cellSize = tmp1>tmp2?tmp2:tmp1;
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