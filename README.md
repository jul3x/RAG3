![VERSION][version] [![Build Status](https://travis-ci.com/jul3x/RAG3.svg?branch=develop)](https://travis-ci.com/jul3x/RAG3)

![Logo](/graphics_dev/rag3-artistic-small.png?raw=true "RAG3 Logo")

# RAG3

Have you ever wondered what would you do if you could turn back time? **RAG3** makes impossible things possible. 
Join to the adventure and help main character in preventing the tragic events.

-----

## Description

**RAG3** is 2D top-down shooter game (currently in demo phase). The gameplay consists of skillful levelling,
 with elimination of all obstacles encountered (enemies and inanimate things as well) along the way. 
 The basic feature of the game is the significant **dynamics**, most of the time requiring the considerable concentration. 
 The game's goal is providing sensations and joy from battles with various creatures while wandering through many types of locations (from mazes to open worlds).  

 One of the most distinguishing features of the game is **real-time** time manipulation mechanics.
 Player is allowed to slow down the gameplay for a short period of time (**bullet time**) and is able to travel **back to the past**.
 This feature is strictly integrated with battles and can be used in a fully dynamic way.  

 The plot of the game can be discovered only directly through the gameplay.
 If you want to learn more about RAG3 world - download demo version [here](https://j3x.dev/rag3) or compile game by yourself using steps from **Build** section.
 You can play in **multiplayer mode** with your friends as well!  

 Beware! Travelling in time comes with consequences. Be careful when you meet your past self!

## Control

Steering can be changed by user. By default it is:  

- *WSAD* for movement. *LShift* changes moving to running.
- *Mouse* used for aiming, *LeftButton* uses current weapon, *MouseWheel* changes weapon, *RightButton* triggers bullet time.
- *Q* and *E* triggers dodging.
- *F* triggers object activation if player is somewhere near and *T* talks to the talkable NPC.
- *R* triggers time reversal. 

## Contribution
Every volunteer is strongly encouraged to take part in a development process.  

If you are not a software developer - piece of cake. You don't need to modify the code to change the game.  

Game has been created with potential extensions and modifications in mind. 
Core architecture is designed in a very flexible way. 
One can easily modify game objects, maps, characters and add new ones.
A lot of game's behaviors and actions is parametrized and is editable via configuration files.
Every object used in a game is described in `*.j3x` files in `data/` directory. Syntax of files is self-describing. RAG3 provides dedicated map/game editor **RAG3Editor** as an additional application for easy map making and environment edition.  

Usage of this tool is described in [RAG3Editor Instruction](/Rag3EditorInstruction.md).

## Screens

![Screen1](/graphics_dev/2.jpg?raw=true "Screen 1") ![Screen2](/graphics_dev/7.jpg?raw=true "Screen 2")
![Screen3](/graphics_dev/3.jpg?raw=true "Screen 3") ![Screen4](/graphics_dev/4.jpg?raw=true "Screen 4")

-----

## Build

Package uses **C++17** language, so you need a compatible compiler. Tested on **g++9** and **MinGW64 6.0**.

* **Windows**
    - Download **SFML @165b715d020e39264d35862aeb15e9139f059575** and **TGUI 0.8.9** from their homepages.
    - Change lines below in `CMakeLists.txt`  file to appropriate paths:
        ```
        set(SFML_ROOT "C:/SFML-2.5.1")
        set(TGUI_ROOT "C:/TGUI-0.8")
        ```
    - Copy `*.dll` files for **SFML** and **TGUI** to build directory
    - Build package using **CMake 3.8** or higher.
    - To build tests, there is another `test/CMakeLists.txt` file where you need change `SFML_ROOT` as well. 
    GTest from the latest commit on master branch required.
    
* **Linux**
    - Download **SFML @165b715d020e39264d35862aeb15e9139f059575** and **TGUI 0.8.9** using your package manager (instructions on their site).
    - Use `build_and_run.bash` script to build game and editor.
    - Use `build_and_run_tests.bash` script to build tests. GTest from the latest commit on master branch required.
  
-----

## Changelog
* 30-09-2019 Rag3 v0.0.0
    - Beginning of work

---

~jul3x

[VERSION]: https://img.shields.io/badge/version-0.0.0-blue.svg
