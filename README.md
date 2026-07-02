# danARC
https://youtu.be/tiK0ihPw6m8

## Setup
1) The program ONLY works on Linux.
2) Personally, I use Kubuntu; anything with KDE Plasma should have a similar setup, and I'm sure this can work with other Wayland displays (must be Wayland, no Xorg).
3) Steam MUST be installed under Flatpak, or else the game can see your process. Using Flatpak provides PID isolation and hides the process.
4) Desktop overlay rules (FOR KDE Plasma):
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
5) Download CLion from JetBrains (the non-commercial version is free).
6) Install dependencies: `sudo apt install cmake g++ libglfw3-dev libgl-dev`
7) Download the code (which you have presumably done already) and unzip it.
8) Build:
```
Using cmake:
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release

NOTE: This will likely fail due to an error about only cmake 3.2 being available. 
Ask AI/Google how to get the required version (or just change it in CMakeLists.txt, though I am unsure of the consequences).

OR

Google/ask AI how to set up the build utility inside of CLion (Release build).
```
9) Run with: `bash start.sh`. If this does not work, it is probably because your output of danARC is not in the relative path `./cmake-build-release/danARC`. Find where your program built to in the last step and change the code in `start.sh` to match that path.
10) Make sure the game is loaded in windowed mode.
11) When you start the process, right-click the overlay -> More Actions -> Keep Above Others.

## Known Issues
1) ESP gets distorted at the edges.
2) some annoying stuff with stuttering and for like a split second you teammates are labeled as enemies.

## How to Configure
All configuration options are located in the file `config.h`. It is expected that you have a system with the environment set up to properly build this project. I suggest using CLion by JetBrains for editing and building the project (that is how I do it). Look at the top of `config.h` and edit the parameters; if you are unsure about any specific parameter, just look at the code where it's used. Rebuild afterward.

## Updating the Source and Project Outline
This section is dedicated to going through the code and explaining how it works for learning purposes. I will describe this at a high level; if you don't understand something, ask an AI (P.S. if it refuses to help, use a Chinese AI or Google AI Overview and paste this README in chunk by chunk).

### The Process Loop - main.cpp
`main.cpp` is the entry point of the process. The main responsibilities of `main` are:
1) **Initializing the process:** This includes initializing the overlay and `SignalHandler` (responsible for the overlay), as well as finding the PID and starting the `GameState` class.
2) **Looping:** Passing information from `GameState` to the ESP (walls and radar).
3) **Shutdown:** The loop catches when you try to close the window and destroys the window properly.

The most important piece for the user is the loop (though getting the PID is also important, it is not very difficult and does not change from update to update).

### Getting Info from the Game - gamestate/gamestate.cpp
This is the only file that reads memory from the game. The job of the program is to return a list of all players (and associated info), information about your own camera, and what team you are on. From this point, I will refer to the "self player" as the **local player**.

The main function of the `GameState` class is the `tick()` function, which actually performs the aforementioned responsibilities.

**UWorld**
It gets the 'UWorld' through the function `getUworld();`. While the UWorld changes every update, the function does not need to be touched because the offset for UWorld is pulled from the file `config.h` under the namespace `off`. Basically, after an update, you will get the error `[-] Invalid Uworld`, at which point you should look on UnknownCheats in the ARC Raiders reversal and offset thread for the new UWorld.

**GetRawEnt**
The next thing the `tick()` function does is call `getRawEnt(uworld)`. This function is 99% of the cheat. Like the previous one, this also uses offsets from `config.h`. Basically, this function:
1) Uses `off::PERSISTENT_LEVEL` to get the `persistentLevel` and checks if it's a valid pointer.
2) Uses `off::ACTORS_PTR` to get the base of the list of all entities in the game (and the entry count).
3) Loops through every entity. The actor pointer list contains pointers to the pawn of every entity.

For every loop, we have the pawn of the entity, whether that be a player, ARC AI, or loot.

This next part is somewhat complicated. We read the "VT" (vtable) by reading the pawn of the current entity. This points us to the "vtable" of the entity. Imagine you have a "player class"—all players have the same class, variables, and functions. When compiled, that structure is saved to the `.text` section. By looking at this structure, we can know if two entities are of the same type. We know what the player class looks like, so we can identify the entity. You will also see `isDebugMode`; this is needed because when the game updates, it breaks vtables, and they have to be rediscovered.

Next, it gets the `rootComp`, which is needed to get the player position. Then, we read the "VM" (View Matrix/camera info). As you can see, we also use vtables for this, except this vtable is not skipped in debug mode because it is essential for rediscovering vtable values. We assume the camera structure we find is our own (ARC servers don't give you other players' camera info in the actor array).

We then perform player-specific checks (except in debug mode), covering health, death status, and team. There are also bot-specific checks to determine if they are killed. Finally, we save the entity into a list and return it with the camera info.

### Drawing the Info - draw/ESP.h
The actual drawing is done by the `Overlay.h` file, which is standalone. The ESP logic is simple: when `tick()` returns the info, it is passed to the ESP function, which performs the math to transpose 3D coordinates onto a 2D overlay.

The only thing to note is the **Debug Mode**. Normally, only players, loot, and ARC AI are rendered. In debug mode, every object is rendered along with its vtable value.

## isDebugMode - Updating the Program
First, switch the flag in `config.h` to `true`. This will provide debug messages in the console.

Go to the ARC Raiders thread on UnknownCheats and check the last page for a "dump" of offsets. Inside `config.h`, I provide info on how to find these in a dump, but you are often better off looking for people who have already published a list.

This cheat uses no decryption. I have used creative solutions like vtable identification and finding the `ViewMatrix` from the actor array.

To update the `ViewMatrix`: in `getRawEnt`, uncomment the line `// getViewMatrix(actor);`. Ensure your FOV is at 70.0 and you are not scoped in. It will dump memory; look for a pattern of three numbers (~20,000), two spaces, another three numbers, two spaces, and then your FOV number. Look at the hex code to the left (e.g., `0xC88`); that is your new `off::VIEW_MATRIX`.

You can also use a tool like `memsed`. If you find your exact health value, you can scan for it using `void FindDeepHealthOffset(ptr actor, float targetHealth)` if the pointer chain is not posted on UC.

Once you find your `ViewMatrix`, you will see a BUNCH of text on the screen (hex codes like `0x14c767570`). These are the vtables. If you look at yourself, you will see the vtable for the player; a loot chest will have a different number. Once you put these hex codes into the `vtables` namespace in `config.h`, your ESP should start working.