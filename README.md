# Cellular Automata

(at this time only conway's game of life is available)  
#### Required
sfml

###### Optional
Catch2(for testing)  

#### Compiling

`git clone https://github.com/arskell/cell-autom`  
`mkdir build`  
`cd ./build`  
`cmake .. -G"MinGW Makefiles"`  
`cd ./src`  
`make`  
  
#### Controls

<kbd>Z</kbd> - zoom in  
<kbd>X</kbd> - zoom out  
Arrows - moving around the canvas  

#### Config

After first execution the application should create the config file. Edit it to change cells amount(canvas width(W) and height(H)) and rule(R).

<img src="https://github.com/arskell/game-of-life/blob/master/screenshot.png" width="400" height="319">
