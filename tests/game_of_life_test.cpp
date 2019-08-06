#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <array>

#define private public
#include "../src/cell_autom.h"
#include "../src/ui.h"

TEST_CASE("plane test", "[plane]"){
    constexpr cell_autom::planeSize testW = 40;
    constexpr cell_autom::planeSize testH = 60;

    cell_autom::Plane testPlane(testW, testH);

    SECTION("Check plane size"){
        REQUIRE(testPlane.getHeight() == testH);
        REQUIRE(testPlane.getWidth()  == testW);
    }

    SECTION("Access to dead cell"){
        REQUIRE( (testPlane[{10,25}]  == DEAD_CELL));
        REQUIRE( (testPlane[{testW-1,testH-1}]  == DEAD_CELL) );
    }

    SECTION("Invert cell state"){
        testPlane.invert({5,10});
        REQUIRE( testPlane[{5,10}]  == LIVE_CELL);
        testPlane.invert({5,10});
        REQUIRE( testPlane[{5,10}]  == DEAD_CELL);
    }
    SECTION("Set a state"){
        testPlane.setState({7,11}, LIVE_CELL);
        REQUIRE( testPlane[{7,11}]  == LIVE_CELL);
        testPlane.setState({7,11}, DEAD_CELL);
        REQUIRE( testPlane[{7,11}]  == DEAD_CELL);
    }
    SECTION("Clear a plane"){
        testPlane.setState({3,4}, LIVE_CELL);
        testPlane.setState({20,6}, LIVE_CELL);
        testPlane.setState({testW-1,testH-1}, LIVE_CELL);
        testPlane.clear();
        REQUIRE( testPlane[{3,4}]  == DEAD_CELL );
        REQUIRE( testPlane[{20,6}]  == DEAD_CELL );
        REQUIRE( testPlane[{testW-1,testH-1}]  == DEAD_CELL );
    }
    SECTION("Check range"){
        REQUIRE(!testPlane.checkRange({10,150}));
        REQUIRE(!testPlane.checkRange({400, 5}));
        REQUIRE(!testPlane.checkRange({testW,testH}));
        REQUIRE(testPlane.checkRange({testW-1,testH-1}));
        REQUIRE(testPlane.checkRange({10,10}));
        REQUIRE(testPlane.checkRange({0,0}));
    }
    std::array<cell_autom::Point<cell_autom::planeSize>, 8> p;
    SECTION("Detect 8 points near"){
        p[0] = {0,0}; p[7] = {1,0}; p[6] = {2,0};
        p[1] = {0,1};               p[5] = {2,1};
        p[2] = {0,2}; p[3] = {1,2}; p[4] = {2,2};
        for(auto& e:p){
            testPlane.invert(e);
        }
        REQUIRE(testPlane.liveCellsNear({1,1}) == 8);
    }
    SECTION("Detect 2 points near"){
        p[0] = {4,4};
        p[1] = {4,5};
        p[2] = {10,20};
        for(int i = 0; i < 3; ++i){
            testPlane.invert(p[i]);
        }
        REQUIRE(testPlane.liveCellsNear({5,5}) == 2);
    }
    SECTION("Detect 3 points near"){
        p[0] = {0,13};
        p[1] = {1,14};
        p[2] = {1,15};
        p[3] = {0,0};
        for(int i = 0; i < 4; ++i){
            testPlane.invert(p[i]);
        }
        REQUIRE(testPlane.liveCellsNear({0,14}) == 3);
    }
    SECTION("Detect zero points"){
        p[0] = {4,4};
        testPlane.invert(p[0]);
        REQUIRE(testPlane.liveCellsNear({24,30}) == 0);
    }

    SECTION("Check the rule 1"){
        p[0] = {4,4};
        p[1] = {5,5};
        for(int i = 0; i < 2; ++i){
            testPlane.invert(p[i]);
        }
        testPlane.nextStep();
        REQUIRE(testPlane[p[0]] == DEAD_CELL);
        REQUIRE(testPlane[p[1]] == DEAD_CELL);
        REQUIRE(testPlane[{5,4}] == DEAD_CELL);
        REQUIRE(testPlane[{4,5}] == DEAD_CELL);
    }
    SECTION("Check the rule 2"){
        p[0] = {10,10};
        p[1] = {10,11};
        p[2] = {11,11};
        for(int i = 0; i < 3; ++i){
            testPlane.invert(p[i]);
        }
        testPlane.nextStep();
        REQUIRE(testPlane[p[0]] == LIVE_CELL);
        REQUIRE(testPlane[p[1]] == LIVE_CELL);
        REQUIRE(testPlane[p[2]] == LIVE_CELL);
        REQUIRE(testPlane[{10,9}] == DEAD_CELL);
    }
    SECTION("Check the rule 3"){
        p[0] = {10,10};
        p[1] = {10,11};
        p[2] = {11,11};
        p[3] = {11,10};
        p[4] = {11, 9};
        for(int i = 0; i < 5; ++i){
            testPlane.invert(p[i]);
        }
        testPlane.nextStep();
        REQUIRE(testPlane[p[0]] == DEAD_CELL);
        REQUIRE(testPlane[p[1]] == LIVE_CELL);
        REQUIRE(testPlane[p[2]] == LIVE_CELL);
        REQUIRE(testPlane[p[3]] == DEAD_CELL);
        REQUIRE(testPlane[p[4]] == LIVE_CELL);
    }
    SECTION("Check the rule 4"){
        p[1] = {13,10};
        p[2] = {11,11};
        p[3] = {11, 9};
        for(int i = 0; i < 4; ++i){
            testPlane.invert(p[i]);
        }
        testPlane.nextStep();
        REQUIRE(testPlane[{12, 10}] == LIVE_CELL);
    }

}


TEST_CASE("UI elements", "[ui]"){

    ui::Button testElement((ui::posType)3,(ui::posType)10,
                         (ui::sizeType)10,(ui::sizeType) 5);
    SECTION("Cursor position detecting"){
        REQUIRE_FALSE(testElement.onPosition(0,0));
        REQUIRE_FALSE(testElement.onPosition(4,33));
        REQUIRE(testElement.onPosition(8, 12));
        REQUIRE(testElement.onPosition(3,10));
    }
    SECTION("Item update handler"){
        bool testval = false;
        testElement.setUpdateHandle([&](){
           testval = true;
        });
        testElement.updateContent();
        REQUIRE(testval);
    }

    SECTION("Button click handle"){
        bool testval = false;
        testElement.setClickHandle([&](){
            testval = true;
        });
        testElement.click();
        REQUIRE(testval);
    }
    
    SECTION("Cursor on item"){
        bool testval = false;
        testElement.setCursorOnItemHandle([&](){
            testval = true;
        });
        testElement.cursorOnItem();
        REQUIRE(testval);
    }
    
    SECTION("Surface update"){
        ui::Surface testSurface(0,0,20,20);
        bool testvalUpdateItem = false;
        ui::Item testItem(1,1,3,3);
        testItem.setUpdateHandle([&](){
            testvalUpdateItem = true;
        });
        int testButton = 0;
        testElement.setUpdateHandle([&](){
            ++testButton;
        });
        testElement.setClickHandle([&](){
            ++testButton;
        });
        testElement.setCursorOnItemHandle([&](){
            ++testButton;
        });
        testSurface.addButton(&testElement);
        testSurface.addItem(&testItem);
        testSurface.updateContent();
        testSurface.clicked(1,1);
        testSurface.clicked(8,12);
        testSurface.cursorOn(8,12);
        REQUIRE(testvalUpdateItem);
        REQUIRE(testButton == 3);
    }


}