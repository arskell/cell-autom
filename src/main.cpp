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
    planeTexture.create(width-100,height);

    ui::Surface surf(700,0,100,height);
    ui::Button b(10,4,50,50);
    b.element.setFillColor(sf::Color::Green);
    b.setCursorOnItemHandle([&](){
        b.element.setOutlineThickness(3);
    });
    b.setUpdateHandle([&](){
        auto cursor = wp.getCursorRelToWindow();
        auto sPos = surf.getXYposition();
        if(!surf.onPosition(cursor.x, cursor.y)){
            b.element.setOutlineThickness(0.f);
        }else if (!b.onPosition(cursor.x - sPos.first, cursor.y - sPos.second)) {
                b.element.setOutlineThickness(0.f);
        }
    });
    surf.addButton(&b);

    wp.setSurface(&surf);

    game_of_life::Plane plane(80*4,60*4);

    for(game_of_life::planeSize i = 0; i < plane.getWidth(); ++i)
        plane.invert({i,0});

    for(game_of_life::planeSize i = 0; i < plane.getHeight(); ++i)
        plane.invert({0,i});

   while(wp.window_is_open()) {
        renderPlane(plane, &planeTexture, width, height);
        wp.update(sf::Sprite(planeTexture.getTexture()));
        planeTexture.clear();
        std::this_thread::sleep_for(400ms);
        plane.nextStep();
    }

    wind.wait();
    return 0;
}

void renderPlane(game_of_life::Plane& plane,sf::RenderTexture* texture, unsigned int windowWidth, unsigned int windowHeight){
    auto tmp1 = static_cast<float>(windowHeight)/plane.getHeight();
    auto tmp2 = static_cast<float>(windowWidth)/plane.getWidth();
    texture->clear();
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