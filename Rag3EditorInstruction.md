# RAG3EditorInstruction

![RAG3Editor](/graphics_dev/8.jpg?raw=true "RAG3Editor")

## Description

**Rag3Editor** is a tool which has been created to optimize (or even allow) map making for **RAG3** game.
It is strictly integrated with the game and is adjusted to read and write `*.j3x` format (which also has been created directly for RAG3 development purposes).
Every function which can be used within the editor was developed because of the demand of map makers / game editors. If you see any new potential features, don't hesitate to report an issue in a [Game Repository](https://github.com/jul3x/RAG3) or even consider implementing new functionality on your own (the code is open source).  

-----

## Editor interface

To create new map simply open **Rag3Editor**. You will see empty grid which is a good starting point for a new game map.
You should see three windows: *Menu*, *Tiles* and *Objects*. 

### Menu

This one contains buttons and widgets to control current map, game configuration and editor configuration.
+ *Load map* - opens pop-up window with list of available maps (this can be used to edit map). Maps are stored in `data/maps/` directory. 
+ *Save map* - opens pop-up window where you can provide new map name and save current progress. If map of given name already exists it will be overwritten.
+ *Edit graphics*, *Edit config*, *Edit characters*, *Edit sound* - open pop-ups which can be used to modify game parameters. Every file is in `*.j3x` format.
+ *Edit parameters* - opens pop-up with current map parameters. There you can edit map's background color, map's lighting color and used default shader.
+ *Controls* - shows controls which are needed to use when using desired tool (e.g. multi-grid matching).
+ *Visible z-index* - controls currently visible layers. Z-index is a mockup for third dimension in two dimensional game. Higher value of z-index for an object determines higher position. E.g. floor has z-index 1 when ceiling has z-index 2 and lava has -1. Value of visible z-index determines the highest value of z-index of displayed objects. It is useful when you want to implement e.g. bridges above water.
+ *Randomizing tiles* - this tool can be used to randomly pick tile when placing on the map. Mostly used for floor tiles. Set this to 8, take tile 10 (first tile of wooden floor tileset) and automatically create floor with tiles 10 to 18 (last tile of wooden floor tileset).
+ *Lighting* - checkbox switches lighting on or off.

### Tiles 

Window consists of two sections - `decorations_tiles` and `obstacles_tiles`. Those are base objects in the game and their placement determines map layout and size. Both of them are used when implementing floor, walls and ceiling. Every tile's size is 32px x 32px and only one can exists on the particular position at the time. Set of tiles is later combined into tile map. To place the tile use left mouse button on the grid. To remove - right mouse button.

### Objects

Window consists of five sections of objects which are not tiles and do not have their limitations. Every object from the window can be placed on the map using left mouse button. Removal can be done by pointing given object (till it is focused) and pressing right mouse button. Provided sections:
+ `characters` - list of every character in the game. When placed on the map, character is considered as an enemy NPC. More about `characters` in **Characters** section.
+ `specials` - objects with some assigned special function (e.g. switch, lever, special_event). Can be both visible or invisible but they do not have any collision response (do not have any physical resistance for players). There you can find very useful objects like `starting_position` and `ending_position` which are necessary for a valid game map. More about `specials` in **Specials** section.
+ `obstacles` - objects for collision system. Can be both destructible or indestructible. More about `obstacles` in **Obstacles** section.
+ `decorations` - objects which are only for visual purposes. Do not take part in collision detection system. More about `decorations` in **Decorations** section.
+ `weapons` - automatic weapons which can be placed on walls. Its rotation can be adjusted using mouse with left button pressed. More about `weapons` in **Weapons** section.

-----

## Tools

### Camera

Camera is developed in a standard way. To zoom in/out just use your `mouse scroll` in a desired position. To move camera use `left ctrl` and drag the view to the desired position.

### Placing and removing object

To place a tile or object just pick one from the list by clicking `left mouse button`.
Then place it on the desired position in the map by using `left mouse button`.
If you want to move object - drag it by pressing `mouse wheel` when it's focused, find another position and release `mouse wheel`.
To delete an object, focus it and press `right mouse button`. Deleting requires the appropriate category of object to be selected. I.e. if you want to delete a tile, you need to have some tile selected from the list.  

Sometimes more than one object is in the similar position. Than you may want to decrease `Visible z-index` parameter to focus only the desired one.

### Cursor alignment

Cursor shows information about current coordinates. Tiles are always aligned to the grid (32px x 32px). Objects can be placed without any constraints but sometimes you may want to align them anyway. To do it just press and hold `left alt` (cursor will be aligned to quarter size of a grid (8px x 8px)) or `left shift` (alignment to the grid).

### Marking

When working with large spaces you may need some tool to move or delete multiple objects. To mark multiple objects on a grid use `m` with pressed `left mouse button`. Then use given selector to mark desired objects. Marked objects will be focused with green color. To move them to other position use arrows. To delete them use `delete`.

### Multi placement

If you want to place more than one object at once, use `n` with pressed `left mouse button`. It will start a selector which can be used to set a rectangle inside which objects should be placed. After `left mouse button` release there will be created instance of a previously selected object on every grid tile within the rectangle. 

### Parameters edition

Every object in the `Tiles` and `Objects` windows has its own predefined configuration with parameters. It can be edited by clicking `right mouse button` on a position on the list. Those parameters will be applied to all objects which are placed on the map and will be in the future.  

Most of the objects have also set of parameters which are applied only to the particular instance. To edit parameters for picked instances, focus it and press `E`.

### Data helper

Some objects can be dependent on others. In those cases `uid` (unique identifier) is provided for every object instance which is in the game. It is applied automatically. If you want to quickly link two objects with special function - open `Parameters edition` for desired object and place function in `Function data`. Focus object to link and press `F2`. It will automatically read linked object's `uid` and type it into editbox. If you want to link special function with some position - place cursor on the desired position and press `F1`. It will read current coordinates and type it into editbox. More on `special function` in the **Special functions** section.

-----

## Game objects

Every object in the game has belongs to the `category` and has `id`. For example `Giorgio` - one of the NPCs in the *RAG3* belongs to the category `characters` and has `id: giorgio`. Every instance of an object has its own unique identifier which is unique not only in range of a `id` or `category` but all objects within a game map.  

Objects have its default parameters which are described inside `data/{category}/{id}.j3x` files. Instances of some categories may have their own parameters (e.g. special objects). Every possible parameter will be described in sections below.

### Characters

Characters' structure is kinda tricky (like any other part of this codebase ;)). Every character should have its own `data/characters/{id}.j3x` file, `data/sound/{id}_dead.ogg` file and `data/textures/characters/{id}/{b, f, face, sb, sf}.png` textures. If character is using melee attack it needs additionaly: `data/weapons/melee_{id}.j3x`, `data/sound/melee_{id}.ogg`, `data/textures/weapons/melee_{id}.png`, `data/textures/weapons/melee/{id}_melee_{id}.png`, `data/textures/weapons/{id}_melee_{id}_back.png` files.  

Default characters parameters are described below:

* `max_speed: float`
* `standby_speed: float`
* `max_hit_distance: float` - distance above which NPC will not attack the player
* `max_health: float`
* `min_attack_distance: float` - distance for melee NPCs above which NPC will not try attacking
* `size: vector` - texture size
* `collision_size: vector` - collision box size
* `collision_offset: vector` - offset between collision and visual texture
* `map_offset: vector` - map offset between real position and texture
* `frames_number: int` - number of animation frames
* `frame_duration: float`
* `gun_offset: list` - list of `frames_number` length of vector values which correspond to offset between weapon position and base NPC position. It is a list because offset may differ between frames.
* `default_activation: string` - default activation of special functions assigned to the NPC. In most cases it will be set to `OnKill`.
* `default_functions: list` - default list of assigned special functions
* `default_datas: list` - default list of arguments for special functions - sizes of those two lists should be equal
* `ai_type: string` - there are three implemented AI types: `None`, `MeleeAttack` and `Standard`
* `z_index: int` 
* `weapons: list` - ids of carried weapons
* `is_talkable: bool` - if NPC is talkable, its talk scenario is editable via RAG3Editor
* `light_point: string` - when set to empty string, NPC remain without any light point. Other options are `const`, `sine`, `flicker`.
* `light_point_data` - if `light_point` is set to `sine` of `flicker` the data corresponds to the frequency of light changes
* `show_health_bar: bool`
* `enemy_kill_exp: int` - number of exp points which are given to player after NPC is killed
* `can_avoid_fluid: bool` - when set to true, NPC path finding system will avoid every "wet" object like `lava` or `water` to avoid drowning

### Specials

Special objects are described in `data/specials/{id}.j3x` files and needs its texture in `data/textures/specials/{id}.png`. Those objects are used if any scripting is needed. They emit signals which can be connected to special functions described in the latter section. Signals available `OnUse`, `OnEnter`, `OnCollect`. If player is near any special with `OnUse` signal connected he can press `Use` key (`F` by default) and it invokes connected functions. `OnEnter` signal causes invocation when collision detection detects player inside special object zone. `OnCollect` signal means that this particular special object can be stored inside player's backpack and used later.  

Default specials parameters are described below:

* `size: vector` - texture size
* `collision_size_x: float` - collision box width
* `collision_size_y: float` - collision box height
* `collision_radius: float` - if special should have circular detection area - this parameter should be used instead
* `collision_offset: vector` - offset between collision and visual texture
* `map_offset: vector` - map offset between real position and texture
* `frames_number: int` - number of animation frames
* `frame_duration: float`
* `default_activation: string` - default activation of special functions
* `default_functions: list` - default list of assigned special functions
* `default_datas: list` - default list of arguments for special functions - sizes of those two lists should be equal
* `is_drawable: bool` - determines wether special should be visible
* `z_index: int` 
* `light_point: string` - when set to empty string, object remain without any light point. Other options are `const`, `sine`, `flicker`.
* `light_point_data` - if `light_point` is set to `sine` of `flicker` the data corresponds to the frequency of light changes
* `shadow: bool` - determines wether special should drop shadow
* `default_active: bool`
* `tooltip_header: string` - if special can be stored in the backpack this determines how it should be described on hover
* `tooltip: string`

### Obstacles

Obstacle objects are described in `data/obstacles/{id}.j3x` files and needs its texture in `data/textures/obstacles/{id}.png`. Those objects are mostly used for collisions. They may be destructible - in this case they send `OnKill` event which invokes all special functions assigned to the obstacle.  

Default obstacles parameters are described below:

* `size: vector` - texture size
* `collision_size: vector` - collision box size
* `collision_offset: vector` - offset between collision and visual texture
* `map_offset: vector` - map offset between real position and texture
* `frames_number: int` - number of animation frames
* `frame_duration: float`
* `default_activation: string` - default activation of special functions (`OnKill`)
* `default_functions: list` - default list of assigned special functions
* `default_datas: list` - default list of arguments for special functions - sizes of those two lists should be equal
* `z_index: int` 
* `light_point: string` - when set to empty string, object remain without any light point. Other options are `const`, `sine`, `flicker`.
* `light_point_data` - if `light_point` is set to `sine` of `flicker` the data corresponds to the frequency of light changes
* `shadow: bool` - determines wether special should drop shadow

### Decorations

Decoration objects are described in `data/decorations/{id}.j3x` files and needs its texture in `data/textures/decorations/{id}.png`. Those objects are used only for visual purposes.  

Default decorations parameters are described below:

* `size: vector` - texture size
* `map_offset: vector` - map offset between real position and texture
* `frames_number: int` - number of animation frames
* `frame_duration: float`
* `z_index: int` 
* `light_point: string` - when set to empty string, object remain without any light point. Other options are `const`, `sine`, `flicker`.
* `light_point_data` - if `light_point` is set to `sine` of `flicker` the data corresponds to the frequency of light changes

### Weapons

Weapon objects are described in `data/weapons/{id}.j3x` files and needs its texture in `data/textures/weapons/{id}.png` and `data/textures/weapons/{id}_back.png` files. Also - they should be pickable from the map so it is needed to make corresponding `special` object of the same id. If weapon is a melee you need to add appropriate animations as well.  

Weapons act in a two ways: as a placed weapon on a map or a weapon which belongs to user.  

Default weapons parameters are described below:

* `recoil: float` - force affecting player's velocity/position after weapon usage
* `deadly_factor: float` - factor which determines weapon's strength
* `spawn_func: string` - `bullet` - for standard gun, `fire` - for flamethrower, `null` - for melee
* `use_time: float` - melee weapon usage time.
* `spawn_timeout: float` - standard/fire weapon timeout between new spawn function invocations
* `spawn_type: string` - identifier of a bullet / object to spawn by `spawn_func`
* `spawn_quantity: int` - number of instances of `spawn_type` object
* `spawn_angular_diff: float` - used by shotguns, radius in degrees
* `spawn_offset: float` - distance between weapon position and spawning position
* `size: vector` - texture size
* `offset: vector` - offset between weapon position and its origin
* `map_offset: vector` - map offset (mostly zero vector)
* `frames_number: int` - number of animation frames
* `frame_duration: float`
* `use_size: vector` - melee weapon usage texture size
* `use_size_back: vector` - melee weapon usage texture size when standing back
* `use_offset: vector` - melee weapon usage texture offset
* `use_offset_back: vector` - melee weapon usage texture offset when standing back
* `shadow_offset: vector` - shadow offset when using weapon
* `shadow_offset_back: vector` - shadow offset when using weapon when standing back
* `use_offset_flipped: vector` - same as `use_offset` but when player is rotated
* `use_offset_back_flipped: vector`
* `shadow_offset_flipped: vector`
* `shadow_offset_back_flipped: vector`
* `area_offset: vector` - melee weapon collision area position offset from base position
* `range: float` - melee weapon range (collision radius)
* `activate_time_elapsed: float` - determines how long weapon should be blocked
* `z_index: int`
* `default_usage: string` - placed weapon parameter. Determines how it should fire. Options: `Const`, `Interrupted`, `Single`, `NearConst`, `NearInterrupted`, `NearSingle`.
* `default_data: float` - placed weapon parameter. When `default_usage` is set, it determines the frequency of firing.
* `activation_area_radius: float` - determines the distance between player and weapon below which weapon should be activated. It only applies when `usage` of weapon is set to `NearConst`, `NearInterrupted` or `NearSingle`.

-----

## Special functions

*RAG3* provides a set of special functions which can be connected to events in the game. Handled events are - `OnKill`, `OnUse`, `OnEnter`, `OnCollect`. First one is sent by characters, obstacles and shot bullets. Other ones are sent only by specials.

List of available functions with their parameters:
* **MapStart** *(args: `{}`, e.g.: `0`)*  
  Mockup function for special object `starting_position`. Does nothing.

* **ChangeMapParams** *(args: `{shader: string, bg_color: int, lighting_color: int}`, e.g.: `["normal.frag"; 0; 0]`)*  
  When applied map parameters are immediately reloaded to given parameters. This can be used to change visuals when player is entering some special room.

* **MapEnd** *(args: `{}`, e.g.: `0`)*  
  Ends map.

* **Checkpoint** *(args: `{}`, e.g.: `0`)*  
  Sets checkpoint of a game (quick save).

* **TurnLight** *(args: `{uid: int}`, e.g.: `1337`)*  
  Turns light object of given `uid` on or off.

* **PourWater** *(args: `{}`, e.g.: `0`)*  
  Sets main player's state to normal. Used mostly to allow player on fire "heal" himself.

* **OpenDoor** *(args: `{uid: int}`, e.g.: `1337`)*  
  Changes door of given `uid` state to open/closed.

* **ReadNote** *(args: `{text: string}`, e.g.: `"Note to read"`)*  
  Opens a note pop-up with provided text.

* **AddWeapon** *(args: `{weapon_id: string}`, e.g.: `"desert_eagle"`)*  
  Adds weapon of given `id` to inventory. If player already has this weapon, the state of the weapon is set to 100%.

* **AddAmmo** *(args: `{weapon_id: string}`, e.g.: `"chaingun"`)*  
  Adds ammo portion to the weapon of given `id`. Ammo quantity is described in a weapon default parameters. If user does not have weapon, it has no effect.

* **AddHealth** *(args: `{health: int}`, e.g.: `50`)*  
  Adds health of given amount to the player.

* **AddSpeed** *(args: `{speed_with_time: vector}`, e.g.: `(1.5, 1000.0)`)*  
  Temporarily adds speed factor to the player. Due to the technical debt, this method takes one argument which corresponds two parameters - speed factor and duration.

* **TakeRag3** *(args: `{}`, e.g.: `0`)*  
  Initiates player Rag3 dosage. It causes visual effects, temporal speed factor, spawning thought object and affects player attack/defence.

* **PickCrystal** *(args: `{}`, e.g.: `0`)*  
  Adds 1 to picked crystal stats.

* **PayRespect** *(args: `{}`, e.g.: `0`)*  
  Easter egg function to pay respect. When used spawns thought object with "Respect BRO!" text.

* **SpawnThought** *(args: `{text: string}`, e.g.: `"Text to speak"`)*  
  Spawns thought object for a character which initiated the function with given text. Used for speaking and informing player about current situation.

* **SpawnPlayerThought** *(args: `{text: string}`, e.g.: `"Text to speak"`)*  
  Same as *SpawnThought* but is always applied to the main player.

* **ChangeOpenState** *(args: `{}`, e.g.: `0`)*  
  Changes open/closed state of used object. Used mostly for levers to change its state from unused to used.

* **Teleport** *(args: `{pos: vector}`, e.g.: `(100.0, 100.0)`)*  
  Teleports player to given position. Spawns fade in/out graphics effect if main player is the subject. Used in stairs and holes. 

* **Kill** *(args: `{cause: string}`, e.g.: `lava`)*  
  Sets player health to 0 and sets death cause basing on given `cause` string. 

* **KillObj** *(args: `{uid: int}`, e.g.: `1337`)*  
  Sets obstacle health to 0 for a given `uid` of an obstacle.

* **ZoomTo** *(args: `{uid: int}`, e.g.: `1337`)*  
  Zooms camera for a short period of time to the NPC of given `uid`. Fot this short moment, time is slowed down and player's control is disabled.

* **ActivateWeapon** *(args: `{uid: int}`, e.g.: `1337`)*  
  Activates placed weapon of a given `uid` on a map. This causes weapon to start firing in a preset way.

* **SetOnFire** *(args: `{}`, e.g.: `0`)*  
  Sets character state to OnFire. Causes health decreasing.

* **Explode** *(args: `{force: float}`, e.g.: `500.0`)*  
  Spawns explosion force in a position of special object of a `force` set in the parameter. 

* **SpawnDecorationTile** *(args: `{id: string, pos: vector}`, e.g.: `["3"; (32.0, 32.0)]`)*  
  Spawns `decoration_tile` object of given `id` in a given `pos`.

* **SpawnObstacleTile** *(args: `{id: string, pos: vector}`, e.g.: `["3"; (32.0, 32.0)]`)*  
  Spawns `obstacle_tile` object of given `id` in a given `pos`.

* **RemoveDecorationTile** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Removes `decoration_tile` from a given position.

* **RemoveObstacleTile** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Removes `obstacle_tile` from a given position.

* **RemoveDecoration** *(args: `{uid: int}`, e.g.: `1337`)*  
  Removes `decoration` object of a given `uid`.

* **RemoveSpecial** *(args: `{uid: int}`, e.g.: `1337`)*  
  Removes `special` object of a given `uid`.

* **RemoveObstacle** *(args: `{uid: int}`, e.g.: `1337`)*  
  Removes `obstacle` object of a given `uid`.

* **SpawnLava** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Spawns `special` object of `id: lava` in given position. Used mostly when bridges/walls are destructible.

* **SpawnExplosionEvent** *(args: `{}`, e.g.: `0`)*  
  Spawns `explosion` animation event in a position of functional object. Only for visual purposes.

* **SpawnMiniLava** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Spawns `special` object of `id: mini_lava` in given position. Used mostly when bridges/walls are destructible.

* **SpawnFlame** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Spawns `special` object of `id: flame` in given position. Used mostly after explosions.

* **SpawnAmmo** *(args: `{id: string}`, e.g.: `"light"`)*  
  Spawns `special` object which represents an collectable ammunition for a given type of bullets. Ammunition is spawned in a globally configurable quantity in a position of functional (mostly NPC).

* **SpawnDestruction** *(args: `{}`, e.g.: `0`)*  
  Spawns `decoration` object of `id: destroyed_wall_{1,2,3}` with last number randomness in a position of functional object.

* **SpawnCrystal** *(args: `{pos: vector}`, e.g.: `(32.0, 32.0)`)*  
  Spawns `special` object of `id: crystal` in a given position.

* **SpawnAnimationEvent** *(args: `{id: string, pos: vector}`, e.g.: `["teleportation"; (300.0, 300.0)]`)*  
  Spawns animation event of given `id` in a given `pos`. Only for visual purposes.

* **SpawnSound** *(args: `{id: string, pos: vector}`, e.g.: `["collect"; (300.0, 300.0)]`)*  
  Spawns sound event of given `id` in a given `pos`. Only for audio purposes.

* **SpawnNPC** *(args: `{id: string, pos: vector}`, e.g.: `["giorgio"; (300.0, 300.0)]`)*  
  Spawns `character` object of given `id` in a given `pos`. Character automatically becomes enemy NPC to the player.

* **SpawnDebris** *(args: `{pos: vector}`, e.g.: `(300.0, 300.0)`)*  
  Similar to *SpawnDestruction* but spawns object in a given position.

* **null** *(args: `{}`, e.g.: `0`)*  
  Mock function without any effect.

* **Deactivate** *(args: `{}`, e.g.: `0`)*  
  Deactivates functional object so it cannot be used in the future. Opposite to the *ActivateSpecial* function.

* **Destroy** *(args: `{}`, e.g.: `0`)*  
  Destroys current functional object.

* **ActivateSpecial** *(args: `{uid: int}`, e.g.: `1337`)*  
  Activates `special` object of given `uid` which has been deactivated (by default or by other special object). Opposite to the *Deactivate*.

---

~jul3x
