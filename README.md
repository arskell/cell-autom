# Cellular Automata

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


#### Rule

In the config file there is 'R' line. This Is it.  
The rule should be written in S/B format. S - stands for "stay"(condition for stay alive), B - "born"(condition to became alive in the next generation).  
For example, The game of life may be described like S23/B3 (order is not matter).  
  
<img src="https://github.com/arskell/game-of-life/blob/master/screenshot.png" width="400" height="319">
