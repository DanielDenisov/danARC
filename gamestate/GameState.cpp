#include "GameState.h"

#include <thread>
#include "../memory/memory.h"

GameState::GameState(uintptr_t baseAddr) {
    this->BaseAddr = baseAddr;
}

InfoReturn GameState::GetState() {
    ptr uworld = getUworld();
    if (!isValidPtr(uworld)) {
        std::cout << "[-] Invalid Uworld" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return{};
    }

    // Used on new update
    DBG {std::cout << "Uworld: 0x" << std::hex << uworld << std::dec << std::endl;}

    // std::vector<RenderEntity> rawEntities = getEntities(uworld);
    std::vector<RenderEntity> rawEntities;
    FminimalViewInfo viewMatrix;
    if (!getEntities(uworld, rawEntities, viewMatrix)) {
        fh.setFirstScan({}); //reset because new map
        std::cout << "[-] Invalid rawEntities" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return{};
    }

    DBG {std::cout << "Raw Ent Size: " << rawEntities.size() << std::endl;
         viewMatrix.Print();}



    //filter
    std::vector<RenderEntity> filteredEntities = filterEntities(rawEntities, viewMatrix);
    if (isDebugMode) filteredEntities = rawEntities; //dont filter on debug

    DBG {std::cout << "Fil Ent Size: " << filteredEntities.size() << std::endl;}

    return {viewMatrix, filteredEntities};
}

/************* Private ************/

ptr GameState::getUworld() {
    ptr uworldPtr = ReadMemory<uintptr_t>(this->BaseAddr + 0xDDC4ED8);

    uworldPtr = ReadMemory<uintptr_t>(uworldPtr);

    return uworldPtr;
}

// bool indicates success
bool GameState::getEntities(uintptr_t uworld, std::vector<RenderEntity> &retEntitiesRaw, FminimalViewInfo &retVM) {
    std::vector<RenderEntity> entities;
    ptr persistentLevel = ReadMemory<ptr>(uworld + off::PERSISTENT_LEVEL);
    DBG{std::cout << "PersistentLvl: 0x" << std::hex <<  persistentLevel << std::dec << std::endl;}
    if (!isValidPtr(persistentLevel)) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return 0;
    }

    ptr actors = ReadMemory<ptr>(persistentLevel + off::ACTORS_PTR);
    DBG{std::cout << "Actors: 0x" << std::hex << actors << std::dec << std::endl;}
    int actorsCount = ReadMemory<int>(persistentLevel + off::ACTORS_PTR + 0x8);
    DBG{std::cout << "actorsCount: " << actorsCount << std::endl;}
    if (!(isValidPtr(actors) && actorsCount > 0 && actorsCount < 5000)) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return 0;
    }

    for (int a = 0; a < actorsCount; a++) {
        ptr actor = ReadMemory<ptr>(actors + (a * 0x8));
        if (!isValidPtr(actor)) continue;

        ptr rootComp = ReadMemory<ptr>(actor + off::ROOT_COMPONENT_PTR);
        if (!isValidPtr(rootComp)) continue;

        Vector3 pos = ReadMemory<Vector3>(rootComp + off::POS_PTR);
        if (std::abs(pos.x) < 100) continue;

        FminimalViewInfo vm = ReadMemory<FminimalViewInfo>(actor + off::VIEW_MATRIX);
        if (30 < vm.FOV && vm.FOV < 120) {
            retVM = vm;
        }

        ptr hc = ReadMemory<ptr>(actor + off::HEALTH_COMPONENT);
        double health = ReadMemory<double>(hc + off::CACHED_HEALTH);

        ptr vt = ReadMemory<ptr>(actor);

        RenderEntity ent;
        ent.actor = actor;
        ent.pos = pos;
        ent.health = health;
        ent.vt = vt;
        if (ent.health < 0.01) {ent.isDead = true;} else {ent.isDead = false;}


        if (!isDebugMode) {
            if (vt == vtabels::PLAYER) ent.type = Object::PLAYER;
            else if (vt == vtabels::ARC) ent.type = Object::ARC;
            else if (vt == vtabels::PICKUP) ent.type = Object::PICKUP;
            else if (vt == vtabels::SEARCH) ent.type = Object::SEARCH;
            else continue;
        }

        entities.push_back(ent);
    }
    retEntitiesRaw = entities;
    return 1;
}



std::vector<RenderEntity> GameState::filterEntities(std::vector<RenderEntity> entities, FminimalViewInfo vm) {
    std::vector<RenderEntity> filteredEntities; //should not be dupicated or anything
    for (int i{}; i < entities.size(); i++) {
        entities[i].dist = entities[i].pos.Dist(vm.Location);
        if (entities[i].type == Object::PLAYER || entities[i].type == Object::ARC) {
            if (entities[i].dist < 500 && entities[i].type == Object::PLAYER) continue; //LP
        }
        filteredEntities.push_back(entities[i]);
    }
    return filteredEntities;
}

// std::vector<RenderEntity> GameState::getEntities(uintptr_t uworld) {
//     std::vector<RenderEntity> entities;
//     ptr persistentLevel = ReadMemory<ptr>(uworld + off::PERSISTENT_LEVEL);
//     DBG{std::cout << "PersistentLvl: 0x" << std::hex <<  persistentLevel << std::dec << std::endl;}
//     if (!isValidPtr(persistentLevel)) {
//         std::this_thread::sleep_for(std::chrono::seconds(10));
//         return{};
//     }
//
//     ptr actors = ReadMemory<ptr>(persistentLevel + off::ACTORS_PTR);
//     DBG{std::cout << "Actors: 0x" << std::hex << actors << std::dec << std::endl;}
//     int actorsCount = ReadMemory<int>(persistentLevel + off::ACTORS_PTR + 0x8);
//     DBG{std::cout << "actorsCount: " << actorsCount << std::endl;}
//     if (!(isValidPtr(actors) && actorsCount > 0 && actorsCount < 5000)) {
//         std::this_thread::sleep_for(std::chrono::seconds(10));
//         return{};
//     }
//
//     for (int a = 0; a < actorsCount; a++) {
//         ptr actor = ReadMemory<ptr>(actors + (a * 0x8));
//         if (!isValidPtr(actor)) continue;
//
//         ptr rootComp = ReadMemory<ptr>(actor + off::ROOT_COMPONENT_PTR);
//         if (!isValidPtr(rootComp)) continue;
//
//         Vector3 pos = ReadMemory<Vector3>(rootComp + off::POS_PTR);
//         if (std::abs(pos.x) < 100) continue;
//
//         // std::cout << "is LP: " << ReadMemory<bool>(actor + 0xd28) << std::endl;
//         // if (ReadMemory<bool>(actor + 0xd28) == 1) {
//         //     pos.Print();
//         // }
//
//         FminimalViewInfo vm = ReadMemory<FminimalViewInfo>(actor + 0xca8);
//
//
//         //found FOV = ce8
//         //real start = ca8
//         //diff = 0x40
//         for (int i = 0; i < 0xFFF; i += sizeof(float)) {
//             if (std::abs(ReadMemory<float>(actor + i) - 70.0f) < 0.01) {
//                 std::cout << "FOV Location: " << std::hex << i << std::dec << std::endl;
//                 std::cout << "Debug: " << ReadMemory<float>(actor + i) << std::endl;
//                 pos.Print();
//                 vm.Print();
//                 for (int j = 0; j < 0x90; j += sizeof(double)) {
//                     std::cout <<std::hex<<i + j - 0x90<<std::dec<< " | "; ReadMemory<Vector3>(actor + i + j - 0x90).Print();
//                 }
//             }
//         }
//
//         ptr vt = ReadMemory<ptr>(actor);
//         std::cout << "VT: " << std::hex << vt << std::dec << std::endl;
//
//
//
//
//
//         RenderEntity ent;
//         ent.actor = actor;
//         ent.pos = pos;
//         ent.vt = vt;
//         ent.vm = vm;
//
//         if (!isDebugMode) {
//             if (vt == vtabels::PLAYER) ent.type = Object::PLAYER;
//             else if (vt == vtabels::ARC) ent.type = Object::ARC;
//             else if (vt == vtabels::PICKUP) ent.type = Object::PICKUP;
//             else if (vt == vtabels::SEARCH) ent.type = Object::SEARCH;
//             else if (vm.FOV > 30 && vm.FOV < 120) ent.type = Object::PLAYERCAM;
//             else continue;
//         }
//
//         entities.push_back(ent);
//     }
//     return entities;
// }

