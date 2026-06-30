#ifndef DANARC_ESP_H
#define DANARC_ESP_H
#include "Overlay.h"
#include "W2S.h"

inline double getProportionalRadius(double dist, double maxDist) {
    return (1-(dist/maxDist)) * 6 + 4;
}

inline void drawDistanceText(double dist, Vector2 spos) {
    char dBuf[32];
    sprintf(dBuf, "%.0fm", dist);
    DrawTextCentered(spos.x, spos.y, IM_COL32(255, 255, 255, 255), dBuf);
}

namespace COLOR {
    inline ImU32 red = IM_COL32(207, 56, 56, 255);
    inline ImU32 darkred = IM_COL32(147, 36, 36, 255);
    inline ImU32 blue = IM_COL32(58, 45, 237, 255);
    inline ImU32 orange = IM_COL32(240, 168, 43, 255);
    inline ImU32 darkorange = IM_COL32(120, 85, 21, 255);
    inline ImU32 green = IM_COL32(34, 189, 39, 255);
    inline ImU32 greenHealth = IM_COL32(34, 189, 39, 255); //because the green for items and for health CAN be different
    inline ImU32 cyan = IM_COL32(39, 219, 217, 255);
    inline ImU32 white = IM_COL32(255, 255, 255, 255);

    inline ImU32 withAlpha(ImU32 color, int alpha) {
        return (color & 0x00FFFFFF) | ((ImU32)(alpha & 0xFF) << 24);
    }
}

inline void ESPInit() {
    if (config::makeLootESPTranslucent) {
        COLOR::green = COLOR::withAlpha(COLOR::green, 128);
        COLOR::cyan = COLOR::withAlpha(COLOR::cyan, 128);
    }
    if (config::makeDeadBotESPTranslucent) COLOR::darkorange = COLOR::withAlpha(COLOR::darkorange, 128);
    if (config::makeDeadPlayerESPTranslucent) COLOR::darkred = COLOR::withAlpha(COLOR::darkred, 128);
}

inline void DrawESP(const std::vector<RenderEntity>& entities, FminimalViewInfo vm, char localPlayerTeam) {
    for (const auto& ent : entities) {
        Vector2 screenPos;
        ImU32 color{};

        screenPos = WorldToScreen(ent.pos, vm, config::SCREEN_W, config::SCREEN_H);

        //Check this because {-9999, -9999} is returned on failure
        if (screenPos.x < -1000 || screenPos.y < -1000) continue;

        if (isDebugMode) {
            double distM = ent.pos.Dist(vm.Location)/100.f;
            if (distM < 150) {
                char vtBuf[64];
                sprintf(vtBuf, "0x%lx [%.0fm]", ent.vt, distM);
                DrawTextImGui(screenPos.x, screenPos.y, COLOR::white, vtBuf);
            }
            continue;
        }


        //To consider. 1) Dist, 2) dead?, 3) visible, 4) team
        if (ent.type == Object::PLAYER) {
            if (ent.teamID == localPlayerTeam) continue; //on our team
            if (ent.isDead) {
                double rad = getProportionalRadius(ent.dist, config::maxPlayerDist);
                DrawCircleFilled(screenPos.x, screenPos.y, rad, COLOR::darkred);
                drawDistanceText(ent.dist, {screenPos.x, screenPos.y+rad+6});
                continue;
            }

            //should be rendered from this point on
            color = COLOR::red;
            if (!ent.isVisible) color = COLOR::blue;

            Vector3 headPos = ent.pos; headPos.z += 90;
            Vector3 feetPos = ent.pos; feetPos.z -= 90;

            Vector2 sHead = WorldToScreen(headPos, vm, config::SCREEN_W, config::SCREEN_H);
            Vector2 sFeet = WorldToScreen(feetPos, vm, config::SCREEN_W, config::SCREEN_H);

            if (sHead.x < -1000 || sHead.y < -1000 ||
                sFeet.x < -1000 || sFeet.y < -1000) continue;

            //ESP box
            float h = sFeet.y - sHead.y;
            float w = h / 2.0f;
            DrawBox(sHead.x - w/2, sHead.y, w, h, color);

            int lineX = sHead.x + w/2 + 1 + 5-int(5*(ent.dist/config::maxPlayerDist)); //scale gap between box and hp line
            int lineYHeight = (sFeet.y - sHead.y)*(ent.playerHealth/100); //calc remaining health
            DrawLine(lineX, sFeet.y, lineX, sFeet.y-lineYHeight, COLOR::greenHealth);
            if (!(100 - ent.playerHealth < 1)) { //not at full health
                DrawLine(lineX, sFeet.y-lineYHeight, lineX, sHead.y, COLOR::red);
            }


            drawDistanceText(ent.dist, {sHead.x - w/2, sHead.y - 8});
        } else {
            int radius = 5;
            if (ent.type == Object::ARC) {
                if (ent.dist > config::maxArcDist) continue;
                color = COLOR::orange;
                if (ent.isDead) color = COLOR::darkorange; //dead
                radius = getProportionalRadius(ent.dist, config::maxArcDist);
            } else if (ent.type == Object::SEARCH) {
                if (ent.dist > config::maxLootDist) continue;
                color = COLOR::green;
                radius = getProportionalRadius(ent.dist, config::maxLootDist);
            } else if (ent.type == Object::PICKUP) {
                if (ent.dist > config::maxLootDist) continue;
                color = COLOR::cyan;
                radius = getProportionalRadius(ent.dist, config::maxLootDist);
            }

            DrawCircleFilled(screenPos.x, screenPos.y, radius, color);
            drawDistanceText(ent.dist, {screenPos.x, screenPos.y+radius+6});
        }
    }
}

void DrawRadar(const std::vector<RenderEntity>& entities, FminimalViewInfo cameraInfo, int localPlayerTeam) {
    // Background
    DrawCircleFilled(config::radarCenterX, config::radarCenterY, config::radarRadius + 2, IM_COL32(0, 0, 0, 150));
    DrawCircleFilled(config::radarCenterX, config::radarCenterY, config::radarRadius, IM_COL32(0, 0, 0, 150));

    // Crosshairs
    DrawLine(config::radarCenterX - config::radarRadius, config::radarCenterY, config::radarCenterX + config::radarRadius, config::radarCenterY, IM_COL32(255, 255, 255, 50), 1);
    DrawLine(config::radarCenterX, config::radarCenterY - config::radarRadius, config::radarCenterX, config::radarCenterY + config::radarRadius, IM_COL32(255, 255, 255, 50), 1);

    // Convert yaw from degrees to radians
    float yawRad = (cameraInfo.Rotation.y + 90.0f) * 3.14159265358979323846f / 180.0f;
    float cosA = cos(yawRad);
    float sinA = sin(yawRad);

    for (const auto& ent : entities) {
        if (ent.type == Object::PICKUP || ent.type == Object::SEARCH) continue;
        if (ent.dist > config::radarRadius * config::scale) continue;
        if (ent.isDead) continue;

        float deltaX = ent.pos.x - cameraInfo.Location.x;
        float deltaY = ent.pos.y - cameraInfo.Location.y;

        // Rotate by camera yaw
        float rotatedX = deltaX * cosA + deltaY * sinA;
        float rotatedY = -deltaX * sinA + deltaY * cosA;

        float screenX = config::radarCenterX + rotatedX / config::scale;
        float screenY = config::radarCenterY + rotatedY / config::scale;

        // Clamp to radar circle
        float dx = screenX - config::radarCenterX;
        float dy = screenY - config::radarCenterY;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > config::radarRadius - 3) {
            float clampScale = (config::radarRadius - 3) / dist;
            screenX = config::radarCenterX + dx * clampScale;
            screenY = config::radarCenterY + dy * clampScale;
        }

        ImU32 color = COLOR::white;
        int radius = 3;
        if (ent.type == Object::PLAYER) {color = COLOR::red;}
        if (ent.type == Object::PLAYER && ent.teamID == localPlayerTeam) {color = COLOR::green;}
        else if (ent.type == Object::ARC) {color = COLOR::orange;}


        DrawCircleFilled(screenX, screenY, radius, color);
    }

    if (config::selfDot) DrawCircleFilled(config::radarCenterX, config::radarCenterY, 4, COLOR::green);
}

#endif //DANARC_ESP_H