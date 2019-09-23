#include "cell_autom.h"

bool cell_autom::Plane::operator[](Point<planeSize> p) const {
    return _plane[p.y][p.x];
}

void cell_autom::Plane::clear() {
    for(planeSize i = 0; i < h; ++i){
        for(planeSize j = 0; j < w; ++j){
            _plane[i][j] = DEAD_CELL;
            _planeBuffer[i][j] = DEAD_CELL;
        }
    }
}

void cell_autom::Plane::setState(Point<planeSize> p, bool state) {
    std::lock_guard<std::mutex> mtx(bufferOwner);
    if(checkRange(p))_plane[p.y][p.x] = state;
}

void cell_autom::Plane::nextStep() {
    std::lock_guard<std::mutex> lc(bufferOwner);

    for(planeSize h_pos = 0; h_pos < h; ++h_pos){
        for(planeSize w_pos = 0; w_pos < w; ++w_pos){
            auto live_cells_near = liveCellsNear({w_pos,h_pos});
            auto cell_state = _plane[h_pos][w_pos];

            if(cell_state == DEAD_CELL) {
                if (std::find(to_born.begin(), to_born.end(), live_cells_near) != to_born.end()) {
                    _planeBuffer[h_pos][w_pos] = LIVE_CELL;
                }else{
                    _planeBuffer[h_pos][w_pos] = DEAD_CELL;
                }
            }else{
                if (std::find(to_stay.begin(), to_stay.end(), live_cells_near) != to_stay.end()) {
                    _planeBuffer[h_pos][w_pos] = LIVE_CELL;
                }else{
                    _planeBuffer[h_pos][w_pos] = DEAD_CELL;
                }
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

uint8_t cell_autom::Plane::liveCellsNear(Point<planeSize> point) {
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

bool cell_autom::Plane::checkRange(Point<planeSize> point) {
    if( (point.x < 0)|| (point.x >= w)) return false;
    return !((point.y < 0) || (point.y >= h));
}

void cell_autom::Plane::fill(cell_autom::Point<planeSize> center, uint8_t size, bool state) {
    center = {center.x - size / 2, center.y - size / 2};
    for (uint8_t i = 0; i <= size; ++i) {
        for (uint8_t j = 0; j <= size; ++j) {
            setState({center.x + i, center.y + j}, state);
        }
    }
}