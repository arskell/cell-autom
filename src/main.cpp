#include <cmath>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <iostream>
#include <string>
#include <cmath>

#include <SFML/Graphics.hpp>


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
    cell_autom::Point<cell_autom::planeSize> center;
    float zoomScale;
    bool zoom_UPD;
};

//void debugPrint(const std::string& msg){
//#ifdef DEBUG
//    std::cout<<msg<<std::endl;
//#endif
//}

void renderPlane(const cell_autom::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight,
                 float& scale,
                 Render_settings& render_settings);


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
    std::mutex plane_is_busy;
    float scale = 1;
    //setup cursor mode
    Cursor_setup cursor_setup;
    cursor_setup.mode = Cursor_setup::DRAW;
    cursor_setup.cursorRadius = 0;


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
    cell_autom::Plane plane(70,60);


    // setting up canvas
    ui::Button UIplane(0,0,700,height);
    UIplane.element.setSize({0,0});
    auto playGround_Handler = [&](){
        auto cursor = wp.getCursorRelToWindow();
        cell_autom::planeSize cursorXSc,cursorYSc;

       if(render_settings.zoomScale==1){
           cursorXSc = std::floor(cursor.x/scale);
           cursorYSc = std::floor(cursor.y/scale);
       }else{
           cursorXSc = (render_settings.center.x - floor((plane.getWidth()/2.0f)*render_settings.zoomScale)) + floor(cursor.x/scale);
           cursorYSc = (render_settings.center.y - floor((plane.getHeight()/2.0f)*render_settings.zoomScale)) + floor(cursor.y/scale);
       }
        switch (cursor_setup.mode){
            case Cursor_setup::DRAW:
                if(cursor_setup.cursorRadius > 0)
                    plane.fill({cursorXSc,cursorYSc}, cursor_setup.cursorRadius, LIVE_CELL);
                else
                    plane.setState({cursorXSc, cursorYSc},LIVE_CELL);
                break;
            case Cursor_setup::ERASE:
                if(cursor_setup.cursorRadius > 0)
                    plane.fill({cursorXSc, cursorYSc}, cursor_setup.cursorRadius, DEAD_CELL);
                else
                    plane.setState({cursorXSc, cursorYSc},DEAD_CELL);
                break;
            default:
                break;
        }
    };
    UIplane.setClickHandle(playGround_Handler);

    playGround.setDragHandle(playGround_Handler);

    UIplane.setUpdateHandle([&](){
        if(plane_is_busy.try_lock()){
            renderPlane(plane, &planeTexture, playGround.getWHSize().first ,
                        playGround.getWHSize().second,scale,render_settings);
            plane_is_busy.unlock();
        }
        playGround.renderTexture.draw(sf::Sprite(planeTexture.getTexture()));
        Title = "Game of life" + std::string((is_paused ? ", PAUSED" : ""));
        wp.setWindowTitleSync(Title);
    });

    playGround.setScrollDownHandle([&](){
        if(cursor_setup.cursorRadius != 0)
            cursor_setup.cursorRadius-=1;
    });

    playGround.setScrollUpHandle([&](){
            cursor_setup.cursorRadius+=1;
    });

    sf::Font fnt;
    fnt.loadFromFile(".\\res\\ArialRegular.ttf");
    //adding text


    ui::Surface info_panel(4,200,100,150);
    ui::Text speed_txt(0,0,0,11);
    speed_txt.text.setFont(fnt);
    speed_txt.text.setString("speed: ");
    speed_txt.setUpdateHandle([&](){
        static unsigned int  old_update_spd = update_speed + 1;
        if(old_update_spd !=update_speed) {
            speed_txt.text.setString("speed: " + std::to_string(update_speed) + "ms");
            old_update_spd = update_speed;
        }
    });

    ui::Text radius_txt(0,15,0,11);
    radius_txt.text.setFont(fnt);
    radius_txt.text.setString("radius: ");
    radius_txt.setUpdateHandle([&](){
        static decltype(cursor_setup.cursorRadius) old_cursor_rad = cursor_setup.cursorRadius + 1;
        if(old_cursor_rad != cursor_setup.cursorRadius) {
            radius_txt.text.setString(
                    "radius: " + (cursor_setup.cursorRadius == 0 ? "0.5" : std::to_string(cursor_setup.cursorRadius)));
            old_cursor_rad = cursor_setup.cursorRadius;
        }
    });

    ui::Text size_txt(0,30,0,11);
    size_txt.text.setFont(fnt);
    size_txt.text.setString("size: " + std::to_string(plane.getWidth()) + "X" + std::to_string(plane.getHeight()));
   // size_txt.setUpdateHandle([&](){
   //     size_txt.text.setString("size: " + std::to_string(plane.getWidth()) + "X" + std::to_string(plane.getHeight()));
   // });

    ui::Text mode_txt(0,45,0,11);
    mode_txt.text.setFont(fnt);
    mode_txt.text.setString("mode: ");
    mode_txt.setUpdateHandle([&](){
        static decltype(cursor_setup.mode) old_mode = (cursor_setup.mode==Cursor_setup::DRAW?Cursor_setup::ERASE:Cursor_setup::DRAW);
        if(old_mode !=cursor_setup.mode) {
            mode_txt.text.setString(
                    "mode: " + std::string((cursor_setup.mode == Cursor_setup::DRAW ? "DRAW" : "ERASE")));
            old_mode= cursor_setup.mode;
        }
    });

    ui::Text zooming_txt(0,60,0,11);
    zooming_txt.text.setFont(fnt);
    zooming_txt.text.setString("zoom: ");
    zooming_txt.setUpdateHandle([&](){
        static decltype(render_settings.zoomScale) old_zoom = render_settings.zoomScale +1;
        if(old_zoom != render_settings.zoomScale) {
            zooming_txt.text.setString("zoom: " + std::to_string(render_settings.zoomScale));
            old_zoom = render_settings.zoomScale;
        }
    });

    info_panel.addTextItem(&speed_txt);
    info_panel.addTextItem(&radius_txt);
    info_panel.addTextItem(&size_txt);
    info_panel.addTextItem(&mode_txt);
    info_panel.addTextItem(&zooming_txt);

    //adding elements on the surface
    panel.addButton((&GRIDButton));
    panel.addButton(&SWITCHMODEButton);
    panel.addButton(&PAUSEButton);
    panel.addButton(&DOWNbutton);
    panel.addButton(&UPbutton);
    panel.addSurf(&info_panel);
    playGround.addButton(&UIplane);
    surf.addSurf(&panel);
    surf.addSurf(&playGround);

    //setup render settings
    render_settings.center.x = plane.getWidth()/2;
    render_settings.center.y = plane.getHeight()/2;
    render_settings.zoomScale = 1;
    render_settings.zoom_UPD = true;
    //setup keys
    wp.setKeyEventHandler([&](sf::Event& ev){
        switch (ev.key.code){
            case sf::Keyboard::Key::Z:
                render_settings.zoomScale *= 0.6;
                render_settings.zoom_UPD = true;
                break;
            case sf::Keyboard::Key::X:
                if(render_settings.zoomScale < 1) {
                    render_settings.zoomScale /= 0.6;
                    render_settings.zoom_UPD = true;
                }
                break;
            case sf::Keyboard::Key::Up:
                if(render_settings.center.y - (plane.getHeight()/2.0)*render_settings.zoomScale > 0)
                    --render_settings.center.y;
                break;
            case sf::Keyboard::Key::Down:
                if(render_settings.center.y + (plane.getHeight()/2.0)*render_settings.zoomScale < plane.getHeight() - 1)
                    ++render_settings.center.y;
                break;
            case sf::Keyboard::Key::Right:
                if(render_settings.center.x + (plane.getWidth()/2.0)*render_settings.zoomScale < plane.getWidth() - 1)
                    ++render_settings.center.x;
                break;
            case sf::Keyboard::Key::Left:
                if(render_settings.center.x - (plane.getWidth()/2.0)*render_settings.zoomScale > 0)
                    --render_settings.center.x;
                break;
            default:
                break;
    }
    });
    /*   wait until window init    */
    is_created.get_future().wait();
    //setup surface on RenderWindow
    wp.setSurface(&surf);

   while(wp.window_is_open()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(update_speed) );
        if(!is_paused) {
            plane_is_busy.lock();
            plane.nextStep();
            plane_is_busy.unlock();
        }
    }

    wind.wait();
    return 0;
}

void renderPlane(const cell_autom::Plane& plane,sf::RenderTexture* texture,
                 unsigned int windowWidth,
                 unsigned int windowHeight,
                 float& scale,
                 Render_settings& render_settings){

    auto planeRenderWidthStart = ceil(render_settings.center.x - (plane.getWidth()/2.0)*render_settings.zoomScale);
    auto planeRenderHeightStart = ceil(render_settings.center.y - (plane.getHeight()/2.0)*render_settings.zoomScale);
    auto planeRenderWidthEnd = ceil(render_settings.center.x + (plane.getWidth()/2.0)*render_settings.zoomScale);
    auto planeRenderHeightEnd = ceil(render_settings.center.y + (plane.getHeight()/2.0)*render_settings.zoomScale);

    if(render_settings.zoom_UPD) {
        render_settings.zoom_UPD = false;
        bool in_bounds[4] = {false, false, false, false};
        while (!(in_bounds[0] && in_bounds[1] && in_bounds[2] && in_bounds[3])) {
            if (planeRenderWidthStart >= 0) {
                in_bounds[0] = true;
            }
            if (planeRenderHeightStart >= 0) {
                in_bounds[1] = true;
            }
            if (planeRenderWidthEnd <= plane.getWidth()) {
                in_bounds[2] = true;
            }
            if (planeRenderHeightEnd <= plane.getHeight()) {
                in_bounds[3] = true;
            }

            if (!(in_bounds[0] && in_bounds[1] && in_bounds[2] && in_bounds[3])) {
                if (!in_bounds[0]) {
                    ++render_settings.center.x;
                }
                if (!in_bounds[1]) {
                    ++render_settings.center.y;
                }
                if (!in_bounds[2]) {
                    --render_settings.center.x;
                }
                if (!in_bounds[3]) {
                    --render_settings.center.y;
                }
                planeRenderWidthStart = ceil(render_settings.center.x - (plane.getWidth() / 2.0) * render_settings.zoomScale);
                planeRenderHeightStart =
                        ceil(render_settings.center.y - (plane.getHeight() / 2.0) * render_settings.zoomScale);
                planeRenderWidthEnd = ceil(render_settings.center.x + (plane.getWidth() / 2.0) * render_settings.zoomScale);
                planeRenderHeightEnd = ceil(render_settings.center.y + (plane.getHeight() / 2.0) * render_settings.zoomScale);
            }
        }
//        debugPrint();
    }
    auto tmp1 = static_cast<float>(windowHeight)/(planeRenderHeightEnd-planeRenderHeightStart);
    auto tmp2 = static_cast<float>(windowWidth)/(planeRenderWidthEnd-planeRenderWidthStart);
    texture->clear();
    scale = tmp1>tmp2?tmp2:tmp1;
    auto recSize = scale;
    if(render_settings.grid)
        recSize-=1;
    sf::RectangleShape elem({recSize, recSize});
    elem.setFillColor(sf::Color::White);
    sf::RectangleShape vLine({1, scale*(planeRenderHeightEnd-planeRenderHeightStart)});
    sf::RectangleShape hLine({scale*(planeRenderWidthEnd-planeRenderWidthStart), 1});

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
    for(cell_autom::planeSize y(planeRenderHeightStart),_y(0); y < planeRenderHeightEnd; ++y, ++_y){
        for(cell_autom::planeSize x(planeRenderWidthStart), _x(0); x < planeRenderWidthEnd; ++x, ++_x){
            if(plane[{x,y}]){
                elem.setPosition((_x)*scale,(_y)*scale);
                texture->draw(elem);
            }
        }
    }
    texture->display();
}