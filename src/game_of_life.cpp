#include "game_of_life.h"

bool game_of_life::Plane::operator[](Point<planeSize> p) const {
    return _plane[p.y][p.x];
}

void game_of_life::Plane::invert(Point<planeSize> p) {
    _plane[p.y][p.x] = !_plane[p.y][p.x];
}

void game_of_life::Plane::clear() {
    for(planeSize i = 0; i < h; ++i){
        for(planeSize j = 0; j < w; ++j){
            _plane[i][j] = DEAD_CELL;
            _planeBuffer[i][j] = DEAD_CELL;
        }
    }
}

void game_of_life::Plane::setState(Point<planeSize> p, bool state) {
    std::lock_guard<std::mutex> mtx(bufferOwner);
    if(checkRange(p))_plane[p.y][p.x] = state;
}

void game_of_life::Plane::nextStep() {
    std::lock_guard<std::mutex> lc(bufferOwner);

    for(planeSize h_pos = 0; h_pos < h; ++h_pos){
        for(planeSize w_pos = 0; w_pos < w; ++w_pos){
            auto live_cells_near = liveCellsNear({w_pos,h_pos});
            auto cell_state = _plane[h_pos][w_pos];

            if(live_cells_near < 2 && (cell_state == LIVE_CELL)){
                _planeBuffer[h_pos][w_pos] = DEAD_CELL;
            }else if(live_cells_near > 3 && (cell_state==LIVE_CELL)){
                _planeBuffer[h_pos][w_pos] = DEAD_CELL;
            }else if((live_cells_near == 3) && (cell_state == DEAD_CELL)){
                _planeBuffer[h_pos][w_pos] = LIVE_CELL;
            }else{
                _planeBuffer[h_pos][w_pos] = cell_state;
            }
        }
    }

    for(planeSize i = 0; i < h; ++i){
        for(planeSize j = 0; j < w; ++j){
            _plane[i][j] = _planeBuffer[i][j];
            _planeBuffer[i][j] = DEAD_CELL;
        }
    }
}

uint8_t game_of_life::Plane::liveCellsNear(Point<planeSize> point) {
    uint8_t result = 0;
    if(checkRange({point.x+1,point.y})){
        result+= static_cast<bool>(_plane[point.y][point.x+1]);
    }
    if(checkRange({point.x+1,point.y+1})){
        result+= static_cast<bool>(_plane[point.y+1][point.x+1]);
    }
    if(checkRange({point.x+1,point.y-1})){
        result+= static_cast<bool>(_plane[point.y-1][point.x+1]);
    }
    if(checkRange({point.x,point.y+1})){
        result+= static_cast<bool>(_plane[point.y+1][point.x]);
    }
    if(checkRange({point.x-1,point.y+1})){
        result+= static_cast<bool>(_plane[point.y+1][point.x-1]);
    }
    if(checkRange({point.x-1,point.y})){
        result+= static_cast<bool>(_plane[point.y][point.x-1]);
    }
    if(checkRange({point.x-1,point.y-1})){
        result+= static_cast<bool>(_plane[point.y-1][point.x-1]);
    }
    if(checkRange({point.x,point.y-1})){
        result+= static_cast<bool>(_plane[point.y-1][point.x]);
    }
    return result;
}

bool game_of_life::Plane::checkRange(Point<planeSize> point) {
    if( (point.x < 0)|| (point.x >= w)) return false;
    if( (point.y < 0)|| (point.y >= h)) return false;
    return true;
}