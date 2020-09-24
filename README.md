![VERSION][version] [![Build Status](https://travis-ci.com/jul3x/RAG3.svg?branch=develop)](https://travis-ci.com/jul3x/RAG3)

![Logo](/graphics_dev/rag3-artistic-small.png?raw=true "RAG3 Logo")

# RAG3

Have you ever wondered what would you do if you could turn back time? **RAG3** makes impossible things possible. 
Join to adventure and help main character in preventing the tragic events.

-----

## Description

**RAG3** is 2D top-down shooter game (currently in early development phase). The gameplay consists of skillful levelling,
 with elimination of all obstacles encountered (enemies and inanimate things as well) along the way. 
 The basic feature of the game is the significant dynamics, most of the time requiring the considerable concentration. 
 The game's goal is providing sensations and joy from battles with various creatures while wandering through many types of locations (from mazes to open worlds).        

## Control

Due to an early phase of the game, steering can be changed in future. For now:  

- *WSAD* for movement. *LShift* changes moving to running.
- *Mouse* used for aiming, *LeftButton* uses current weapon, *MouseWheel* changes weapon, *RightButton* triggers bullet time.
- *Q* and *E* triggers dodging.
- *F* triggers object activation if player is somewhere near and *T* talks to the talkable NPC.
- *R* triggers time reversal. 

## Configuration
Game's architecture is created in a very flexible way. One can easily modify game objects, maps, characters and add new ones.
Every object used in a game is described in `*.j3x` file in `data/` directory. Syntax of files is self-describing. 
Repository provides **RAG3Editor** as an additional application for easy map making and environment edition. 
Usage of this tool will be later described in *RAG3Editor* section.

-----

## Build

Package uses **C++17** language, so you need a compatible compiler. Tested on **g++9** and **MinGW64 6.0**.

* **Windows**
    - Download **SFML master** and **TGUI 0.8** from their homepages.
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
    - Download **SFML 2.5** and **TGUI 0.8** using your package manager (instructions on their site).
    - Use `build_and_run.bash` script to build game and editor.
    - Use `build_and_run_tests.bash` script to build tests. GTest from the latest commit on master branch required.
  
-----

## RAG3Editor

TBA

-----

## Changelog
* 30-09-2019 Rag3 v0.0.0
    - Beginning of work

---

~jul3x

[VERSION]: https://img.shields.io/badge/version-0.0.0-blue.svg
