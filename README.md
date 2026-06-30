# danARC

## How to update
if you have no programming experience, ask AI and point it to this document
1) all offsets (located in config.h) need to be updated
2) if you cannot find viewmatrix ptr and/or position, use the functions in GameState.cpp
3) enable debug mode (also in config.h) 
4) go into a real match, look at players, other ARC, loot checks and loot that you can pick up and write down the number (0x...) and put fill in the corresponding entity under vtabels in config.h


Things to consider:

If you want to see more that just players and some bots, go beyond the presistet level, ints all of the levels, bots sit at different levels than people



Always use wayland, if using KDE (what I test on, kubuntu):
    Open System Settings in KDE.

    Go to Window Management -> Window Rules.

    Click + Add New...

    Description: Better Discord Overlay Fix

    Window class: Choose "Exact Match" and type Better Discord Overlay (or whatever you named your window in glfwCreateWindow).

    Click + Add Property and search for/add these:

        Accept focus: Set to Force and No.

        Focus stealing prevention: Set to Force and Extreme.

        Ignore global shortcuts: Set to Force and Yes.

        No titlebar and frame: Set to Force and Yes.

    Apply.