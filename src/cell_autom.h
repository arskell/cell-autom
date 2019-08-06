
#ifndef CONWAY_S_GAME_OF_LIFE_GAME_OF_LIFE_H
#define CONWAY_S_GAME_OF_LIFE_GAME_OF_LIFE_H

#include <cstdint>
#include <vector>
#include <mutex>
#define LIVE_CELL true
#define DEAD_CELL false


namespace cell_autom {

    using planeSize = unsigned int;

    template<typename T>
    struct Point{
        T x, y;
    };

    struct Plane{

        Plane(planeSize width, planeSize height):w(width),h(height){
            _plane = new bool*[h];
            for(planeSize i = 0; i < h; ++i){
                _plane[i] = new bool[w];
            }
            _planeBuffer = new bool*[h];
            for(planeSize i = 0; i < h; ++i){
                _planeBuffer[i] = new bool[w];
            }

            clear();
        }

        ~Plane(){
            for(planeSize i = h -1; i > 0; --i)
                delete[] _plane[i];
            delete[] _plane;
            for(planeSize i = h -1; i > 0; --i)
                delete[] _planeBuffer[i];
            delete[] _planeBuffer;
        }

        void nextStep();
        void invert(Point<planeSize> p);
        void setState(Point<planeSize> p, bool state);
        void clear();
        bool operator[](Point<planeSize> p) const;
        void fill(cell_autom::Point<planeSize> center, uint8_t size, bool state);

        planeSize getWidth()const noexcept { return w;}
        planeSize getHeight()const noexcept { return h;}

    private:
        bool **_plane;
        bool **_planeBuffer;
        planeSize w;
        planeSize h;
        uint8_t liveCellsNear(Point<planeSize> point);
        bool checkRange(Point<planeSize> point);
        std::mutex bufferOwner;
    };

}


#endif //CONWAY_S_GAME_OF_LIFE_GAME_OF_LIFE_H
