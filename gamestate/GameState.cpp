#include "GameState.h"

#include "imgui.h"

//Forward declaration for debug functions
void getViewMatrix(ptr actor);
void getPosPtr(ptr actor);
void DumpMemoryAround(uintptr_t targetAddress, size_t range);
void FindDeepHealthOffset(ptr actor, float targetHealth);

GameState::GameState(uintptr_t baseAddr) {
    this->baseAddr = baseAddr;
}

gsRet GameState::tick() {
    ptr uworld = getUworld();
    if (!isValidPtr(uworld)) {
        std::cout << "[-] Invalid Uworld" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return{};
    }

    DBG {std::cout << "Uworld: 0x" << std::hex << uworld << std::dec << std::endl;}

    RawInfo raw = getRawEnt(uworld);
    if (raw.rawEnt.empty() || std::abs(raw.VM.FOV - 70.0) > 60.0) {
        std::cout << "[-] Invalid rawEntities or no viewMatrix" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return{};
    }

    DBG {std::cout << "Raw Ent Size: " << raw.rawEnt.size() << std::endl;
        raw.VM.Print();}

    FilterRet filt{raw.rawEnt, 'l'};
    if (!isDebugMode) { //skip on debug
        filt = filterEnt(raw.rawEnt, raw.VM);
        if (filt.filtEnt.empty()) {
            std::cout << "[-] No ent found, may be in practice range or its just not working" << std::endl;
        }

        DBG {std::cout << "Filt Ent Size: " << filt.filtEnt.size() << std::endl;}
    }


    return {filt.filtEnt, raw.VM, filt.localPlayerTeam};
}

uintptr_t GameState::getUworld() {
    ptr uworldPtr = ReadMemory<ptr>(this->baseAddr + off::uworld);

    uworldPtr = ReadMemory<ptr>(uworldPtr);

    return uworldPtr;
}


GameState::FilterRet GameState::filterEnt(std::vector<RenderEntity> rawEnt, FminimalViewInfo vm) {
    FilterRet fr{};

    for (RenderEntity ent : rawEnt) {
        double dist = ent.pos.Dist(vm.Location);
        // if (dist < 5 * 100) { //very close to self
        //     if (ent.type == Object::PLAYER) {
        //         fr.localPlayerTeam = ent.teamID;
        //     }
        //     continue;
        // }

        ent.dist = dist/100;

        fr.filtEnt.push_back(ent);
    }
    return fr;
}

GameState::RawInfo GameState::getRawEnt(ptr uworld) {
    RawInfo ri{};
    ptr persistentLevel = ReadMemory<ptr>(uworld + off::PERSISTENT_LEVEL);
    DBG{std::cout << "PersistentLvl: 0x" << std::hex <<  persistentLevel << std::dec << std::endl;}
    if (!isValidPtr(persistentLevel)) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return {};
    }

    ptr actors = ReadMemory<ptr>(persistentLevel + off::ACTORS_PTR);
    DBG{std::cout << "Actors: 0x" << std::hex << actors << std::dec << std::endl;}
    int actorsCount = ReadMemory<int>(persistentLevel + off::ACTORS_PTR + 0x8);
    DBG{std::cout << "actorsCount: " << actorsCount << std::endl;}
    if (!(isValidPtr(actors) && actorsCount > 0 && actorsCount < 5000)) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return {};
    }

    for (int a = 0; a < actorsCount; a++) {
        ptr actor = ReadMemory<ptr>(actors + (a * 0x8));
        if (!isValidPtr(actor)) continue;

        RenderEntity ent;

        ptr vt = ReadMemory<ptr>(actor);
        if (!isDebugMode) { //allows everything through when debugging
            if (vt == vtabels::PLAYER) ent.type = Object::PLAYER;
            else if (vt == vtabels::ARC) ent.type = Object::ARC;
            else if (vt == vtabels::PICKUP) ent.type = Object::PICKUP;
            else if (vt == vtabels::SEARCH) ent.type = Object::SEARCH;
            else if (vt == vtabels::CAMMANAGER) ent.type = Object::NONE;
            else continue;
        }
        ent.actor = actor;
        ent.vt = vt;

        ptr rootComp = ReadMemory<ptr>(actor + off::ROOT_COMPONENT_PTR);
        if (!isValidPtr(rootComp)) continue;

        Vector3 pos = ReadMemory<Vector3>(rootComp + off::POS_PTR);
        if (std::abs(pos.x) < 100) continue;
        ent.pos = pos;

        //Used for finding ViewMatrix
        //vtabels::CAMMANAGER is like a #1 priority to find for everything else, use getViewMatrix
        if (vt == vtabels::CAMMANAGER) ri.VM = ReadMemory<FminimalViewInfo>(actor + off::VIEW_MATRIX);
\

        //Only if player or debugging
        if (ent.type == Object::PLAYER || isDebugMode) {
            //Not adding this to offsets because they cannot typically be found in SDK dumps
            //can also use method below to find another pointer chain to memory
            ptr healthComponent = ReadMemory<ptr>(actor + 0xD78); //HealthComponent
            if (isValidPtr(healthComponent)) {
                ent.playerHealth= ReadMemory<double>(healthComponent + 0x690); //CachedHealth
                if (ent.playerHealth < 1) ent.isDead = true;
            }
            // // FindDeepHealthOffset(actor, 75.1292); //find second value from memsed
            // auto dep1 = ReadMemory<ptr>(actor + 0xdc8);
            // auto dep2 = ReadMemory<ptr>(dep1 + 0x1b0);
            // float health = ReadMemory<float>(dep2 + 0xb8);
            if (ent.playerHealth < 1) ent.isDead = true;

            ptr mesh = ReadMemory<ptr>(actor + off::MESH);
            if (isValidPtr(mesh)) {
                float lastSubmitTime = ReadMemory<float>(mesh + off::LAST_SUBMIT_TIME);
                float lastRenderTime = ReadMemory<float>(mesh + off::LAST_SUBMIT_TIME + sizeof(float));
                float lastRenderTimeOnScreen = ReadMemory<float>(mesh + off::LAST_SUBMIT_TIME + sizeof(float)*2);
                bool isOnScreen = (lastRenderTime == lastRenderTimeOnScreen);
                bool isRecent = (lastSubmitTime - lastRenderTime) <= 0.06f;
                ent.isVisible = isOnScreen && isRecent;
            }

            ent.teamID = ReadMemory<char>(actor + off::TEAM_ID);
        } else if (ent.type == Object::ARC) {
            ent.isDead = ReadMemory<bool>(actor + 0x1210);
        }


        //TODO: only hav eit check for destroyed if is bot
        //TODO: Implement viz check
        //TODO: Implement team check



        // getViewMatrix(actor); //uncomment to dump memort and find
        // getPosPtr(actor); //can find both Root Component AND position, print out is Off: rootComp posPtr

        ri.rawEnt.push_back(ent);
    }
    return ri;
}

//------------------------------------------------------------------------
//          HELPER FUNCTION INTENDED TO HELP WITH DEBUGGING             //
//------------------------------------------------------------------------
// These functions only sometimes have to be used and only for the purpose of getting offsets when they are
// not found on the reversal and offsets thread, the only one that really needs to be used every update
// is the getViewMatrix one because I have not seen anybody else use this type of method to get the ViewMatrix


void DumpMemoryAround(uintptr_t targetAddress, size_t range) {
    // Align start address to a clean 16-byte boundary
    uintptr_t startAddress = (targetAddress - range) & ~0xF;
    uintptr_t endAddress = (targetAddress + range + 0xF) & ~0xF;

    std::cout << std::hex << std::uppercase;
    std::cout << "--- Memory Dump Around: 0x" << targetAddress << " ---\n";

    for (uintptr_t current = startAddress; current < endAddress; current += 16) {
        // Print row memory address
        std::cout << "0x" << std::setw(16) << std::setfill('0') << current << ": ";

        std::vector<uint8_t> rowBytes(16);

        // Read 16 bytes for the current row
        for (size_t i = 0; i < 16; ++i) {
            rowBytes[i] = ReadMemory<uint8_t>(current + i);
        }

        // Hexadecimal representation
        for (size_t i = 0; i < 16; ++i) {
            // Highlight the exact target address if it falls in this row
            if (current + i == targetAddress) {
                std::cout << "[" << std::setw(2) << (int)rowBytes[i] << "]";
            } else {
                std::cout << " " << std::setw(2) << (int)rowBytes[i] << " ";
            }
        }

        std::cout << " | ";

        // ASCII representation
        for (size_t i = 0; i < 16; ++i) {
            char c = rowBytes[i];
            // Check for printable characters
            if (c >= 32 && c <= 126) {
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        std::cout << "\n";
    }
}


void getViewMatrix(ptr actor) {
    // Uncomment to find offset of ViewMatrix
    // Currently take this output and do -0x40
    for (int j = 0x000; j < 0xFFFF; j += 1) {
        if (std::abs(ReadMemory<float>(actor + j)-70.0f/*Your FOV*/) < 0.0001) {
            std::cout << "FOV at offset: " << ReadMemory<float>(actor + j) << ' ' << std::hex << j << std::endl;
            for (int k = -0x100; k < 0x0; k += sizeof(double)) {
                std::cout << std::hex << j+k << std::dec << " " << ReadMemory<double>(actor+j+k) << std::endl;
            }
            std::cout << std::hex << j << std::dec << " FOV here" << std::endl;
            ptr vt = ReadMemory<ptr>(actor);
            std::cout << "VT Entry: 0x" << std::hex << vt << std::endl;
        }
    }
}

void getPosPtr(ptr actor) {
    //Only works when you close
    for (int i = 0; i < 0x400; i++) {
        auto rootComp = ReadMemory<ptr>(actor + i);
        for (int j = 0; j < 0x400; j++) {
            auto pos = ReadMemory<Vector3>(rootComp + j);
            if (pos.Dist({198076.59, -199831.00, 150590.18}) < 10000) {
                std::cout << "Off: " << std::hex << i << " " << j << std::dec << std::endl;
                pos.Print();
            }
        }
    }
}

void FindDeepHealthOffset(ptr actor, float targetHealth) {
    const int MAX_OFFSET = 0x1000; // 4KB scan range
    const float EPSILON = 0.0001f;

    // Layer 1: Scan for pointers on the Actor
    for (int off1 = 0; off1 < MAX_OFFSET; off1 += 8) {
        ptr layer1 = ReadMemory<ptr>(actor + off1);
        if (!isValidPtr(layer1)) continue;

        // Layer 2: Scan for pointers inside Layer 1
        for (int off2 = 0; off2 < 0x800; off2 += 8) {
            ptr layer2 = ReadMemory<ptr>(layer1 + off2);

            // CHECK A: Is the health here? (2-Layer check again just in case)
            float checkValA = ReadMemory<float>(layer1 + off2);
            if (std::abs(checkValA - targetHealth) < EPSILON) {
                std::cout << "FOUND (2-Layer): Actor + " << std::hex << off1
                          << " -> " << off2 << std::dec << std::endl;
            }

            if (!isValidPtr(layer2)) continue;

            // Layer 3: Scan for the float inside Layer 2
            for (int off3 = 0; off3 < 0x500; off3 += 4) {
                float val = ReadMemory<float>(layer2 + off3);
                if (std::abs(val - targetHealth) < EPSILON) {
                    std::cout << "FOUND (3-Layer): Actor + 0x" << std::hex << off1
                              << " -> + 0x" << off2
                              << " -> + 0x" << off3 << " = " << val << std::dec << std::endl;
                }
            }
        }
    }
}