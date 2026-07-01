# danARC

## Setup
1) The program ONLY works on linux
2) Personally, I use Kubuntu, anything with KDE Plasma should have a similar setup and im sure this can work with other wayland displays (must be wayland, no xorg)
3) Steam MUST be installed under Flatpak or else the game can just see your process, using Flatpak gives PID isolation and hides the process
4) Do some desktop overlay rules (FOR KDE Plasma)
```
 Go to Window Management -> Window Rules.

    Click + Add New...

    Description: Discover Overlay Fix

    Window class: Choose "Exact Match" and type "Discover Overlay" (or whatever you named your window in config.h).

    Click + Add Property and search for/add these:

        Accept focus: Set to Force and No.

        Focus stealing prevention: Set to Force and Extreme.

        Ignore global shortcuts: Set to Force and Yes.

        No titlebar and frame: Set to Force and Yes.

    Apply.
```
5) Download clion from JetBrains (non-comertial version is free)
6) Install deps: sudo apt install cmake g++ libglfw3-dev libgl-de
7) Clone this repo: git clone https://github.com/DanielDenisov/danARC.git
8) Build:
```
Using cmake
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release
NOTE: this will likely fail due to some error about only cmake 3.2 being avilable, 
ask AI/google how to get the required version (or just change it in CMaleLists.txt but idk the consequence of that)

OR

google/ask AI how to setup the build utility inside of clion (Release build)
```
9) Run with: ```bash start.sh``` chance this does not work its probably because your output of danARC is not in the relative path 
"./cmake-build-release/danARC", find were your program build to in the last step and change the code in start.sh to match that path
10) Make sure the game is Load into a game is in windowed mode

## Known Issues
1) ESP gets distorted at the edges
2) Sometimes it freezes (you can solve this yourself by adding some debugging and then doing some multithreading)

## How to configure
All configuration options are located in the file config.h
It is expected that you have a system that has the environment set up to properly build this project
I suggest using clion by JetBrains for editing and building of the project (how I do it.)
Look at the top of config.h and edit the peramiters, if you are unsure about any specific peramiter, just look at the code were its used.
Rebuild

## Updating the source and project outline

This section is dedicated to going through the code and explaining how it works for the purpose of learning.
I will be describing this at a somewhat high level, so if you don't get something ask AI (ps. if it refuses to help you, use some Chinese AI or use Google AI overview and paste this readme in chunk by chunk)

### The process loop - main.cpp
Obviously, main.cpp is the entry point of the process. The main responsibility of main in this process is to 
1) Initialize the process, 
this includes initializing the overlay and SignalHandler (responsible for the overlay) as well as finding the PID (ID of the process) and starting the "GameState" class
2) Looping though and passing information form the "GameSate" to the "ESP (walls and radar)"
3) The loop catches when you try to close the window and goes through and actually destroyed the window

From this, the most important piece for the cheater is the loop (although getting the PID is also important, its not very hard and does not change update to update)

### Getting info from the game - gamestate/gamestate.cpp
This is the only file that reads memory from the game, the job of the program is to return a list of all players (and info associated with them), information your own camera, and what team the self is on
From this point I will refer to the "self player" as the <b>local player</b>

The main function of the gamestate class is the tick function witch actually preforms the aforementioned responsibilities. 

<b>UWorld</b>

it gets the 'uworld' through the function ```getUworld();``` while the uworld changes every update, the function does not need to be touched because 
the offset for uworld is pulled form the file config.h under the namespace "off". So basically on an update, you will get the error ```[-] Invalid Uworld``` from witch point you should look on unkowncheats at the ARC raiders reversal and offset thread for the new uworld

<b>GetRawEnt</b>

The next thing the tick() function does is call ```getRawEnt(uworld)```, this function is like 99% of the cheat. 
Like the previous one, this also uses offsets from config.h, (all the rest of them).
basically this function does this:
1) uses off::PERSISTENT_LEVEL to get the presistentLevel and checks if its a valid pointer
2) uses off::ACTORS_PTR to get the base of the list off all entities in the game, (and also gets the count of entries form this variable)
3) it then loops through every entity, that actor ptr list we got before has a bunch of pointer to the pawn of every entity

So for every loop, we have the pawn of the entity, weather that be a player, arc AI, or loot (or even our camera).

This next part is somewhat complicated. We now read "vt", by reading the pawn of the current entity, what this does is point us to the "vtable" of the entity. Basically, image you have a "player class", 
all players have the same class, the same variables, the same function, when you compile this into a program, that structure gets saved to the .text section (i think) and by looking at this structure, we can know if two entities
are of the same type. This is what we are doing, we know what the player class looks like (or rather were it points to in the .text section) so we know what type of entity we have. You will also see ```isDebugMode```, 
I will get into this later but this is needed because when the game updates, it breaks vtables (and other stuff), and they have to be rediscovered.

The next thing it does is get the rootComp, witch is just needed to get player position (kinda simple)

Then, we read the "VM" (View Matrix aka. camera info). s you can see, for this we also use vtables, except this vtable is not skipped when in debug mode because its essential for rediscovering the vtable values (will talk about this later).
Basically, by using the vtables method described above, we know what the structure for a player camera looks like, so when we come across one, we just assume it's our own (ARC servers don't give you other players camera info in the actor array so this works)

we then go into player specific checks (except when in debug mode), witch covers stuff like health, if the player is dead, and what team they are on.

there are also bot specific checks, that determine if its killed or not.

then we save the current entity into a long list

at the end of the function, both the list of entities and the camera info is returned

<b>filterEnt</b>
 
all that this does is go through all the entities and use the camera info to take out the local player as well as calculate the distance of every enemy form us, and lastly, give back what team the local player is on

### Drawing the info - draw/ESP.h
the actual drawing is done by the Overlay.h file, but its stand alone, and you probably won't have to touch it.

The ESP logic is kinda simple. when the tick() function from gamestate returns all that info, it gets passed to the ESP function witch does some math and transposes the 3d coord into a 2d overlay.

tbh, ask AI about this cus its kinda run-of-the-mill.

The ONLY thing I would like to draw attention to is again, the Debug mode stuff, normally, only players, loot, and arc get rendered in ESP, but with debug mode, every object gets rendered along with its vtable value.

## isDebugMode - updating the program
With the technical overview out of the way, this is how the program actually gets updated.

First, switch the flag in config.h to true. This will give you some debug messages in the console so you can see were your program is failing. 

You should first goto: https://www.unknowncheats.me/forum/arc-raiders/590414-arc-raiders-structs-offsets.html and go to the last page, and basically just hope people have published a list of a "dump" of ofssets.
Inside of config.h, in the off namespace, I provide some info about how these offsets can be found in the dump, but a lot of them are still hard to find and your better off looking for people who have already published a list.

I am lazy and hate decrypting anything, in fact, this whole cheat uses no decryption even though they try. I have had to come up with some creative solutions (mostly the vtable stuff and the finding of the ViewMatrix from the actor array)
Because of this, I have included some functions to help me (and ig now you) update it. 

Before using them, you need to first get it to a place were you can loop through the actor array. Your very first priority will be getting the view matrix, in the function getRawEnt, uncomment the line ```// getViewMatrix(actor);```
Make sure your FOV is at 70.0 and your not scoped in or anything. It should dump a bunch of memory, you might want to use AI, but basically your looking for a pattern of 3 numbers (~20,000) two spaces, another 3 numbers two spaces, and then that FOV number, 
then look back on the first number in that sequence and look at the hex code to the left (ie 0xC88), that's your new off::VIEW_MATRIX 

The same can be done for finding the root_component and pos_ptr, but more commonly than not, those are posted to the thread. 

Likewise, there is a cheat engine like software, memsed, if you can find you exact health value, you can scan for it using the function ```void FindDeepHealthOffset(ptr actor, float targetHealth)```
because sometimes, the offsets/pointer chain for finding a players health is also not posted. 

Once you find your viewmatrix, you should then in the ESP see a BUNCH of tech all over the screen that look something like 0x14c767570. These are the aforementioned vtables. 
If you look at yourself, you will see a number, that is the vtable for player, look at a loot chest thing, it will also have a number, that is the vtable for searchable items.
The same will go for ARC AI and items that you can pick up off the ground. Once you put these hex codes into the namespace vtables in config.h, your esp should basically just start working.

That's basically it,

