#include <atomic>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cmath>
#include <csignal>
#include <thread>
#include <variant>

#include "gamestate/GameState.h"
#include "draw/ESP.h"
#include "memory/memory.h"

inline std::atomic<bool> g_Running(true);
void SignalHandler(int signum) {g_Running = false;}

int main() {
    std::signal(SIGINT, SignalHandler);
    if (!InitOverlay()) {
        std::cerr << "[-] Failed to init Overlay (GLFW/Wayland)" << std::endl;
        return 1;
    }

    ProcessId = FindGamePID();
    GameState gs = GameState();
    ESPInit();
    while (g_Running && !glfwWindowShouldClose(window)) {
        gsRet gsr = gs.tick();

        RenderBegin();

        char buf[64];
        sprintf(buf, "Entities: %lu", gsr.entities.size());
        DrawTextImGui(10, 10, IM_COL32(255, 0, 0, 255), buf);


        DrawESP(gsr.entities, gsr.vm, gsr.localPlayerTeam);
        if (config::enableRadar) DrawRadar(gsr.entities, gsr.vm, gsr.localPlayerTeam);

        RenderEnd();


    }

    std::cout << "[+] Destructing Window" << std::endl;

    if (ImGui::GetCurrentContext()) {
        if (ImGui::GetIO().BackendRendererUserData)
            ImGui_ImplOpenGL3_Shutdown();

        if (ImGui::GetIO().BackendPlatformUserData)
            ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    if (window) {
        glfwHideWindow(window);
        glfwDestroyWindow(window);
    }

    // causes segmentation fault
    //glfwTerminate();
    return 0;
}
