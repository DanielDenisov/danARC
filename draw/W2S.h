#ifndef DANARC_W2S_H
#define DANARC_W2S_H
#include "../utils/gameUtil.h"

//IDK what the W2S for this game is, its kinda hell, this works good enough
inline Vector2 WorldToScreen(Vector3 WorldLocation, FminimalViewInfo CameraInfo,
                             int Width, int Height, bool FovIsHorizontal = false) {
    Vector3 Delta = WorldLocation - CameraInfo.Location;
    double Pi = 3.14159265358979;
    double Yaw   = CameraInfo.Rotation.y * Pi / 180.0;
    double Pitch = CameraInfo.Rotation.x * Pi / 180.0;

    Vector3 Forward(cos(Pitch)*cos(Yaw), cos(Pitch)*sin(Yaw), sin(Pitch));
    Vector3 Right(-sin(Yaw), cos(Yaw), 0.0);
    Vector3 Up(-sin(Pitch)*cos(Yaw), -sin(Pitch)*sin(Yaw), cos(Pitch));

    double CamX = Delta.Dot(Right);
    double CamY = Delta.Dot(Up);
    double CamZ = Delta.Dot(Forward);
    if (CamZ < 1.0) return Vector2(-9999.0, -9999.0);

    double TanHalfFov = tan((CameraInfo.FOV * Pi / 180.0) / 2.0);

    // One focal length for both axes (square pixels). Basis depends on which
    // axis the engine's FOV refers to.
    double Focal = FovIsHorizontal ? (Width  / 2.0) / TanHalfFov
                                   : (Height / 2.0) / TanHalfFov;

    double ScreenX = (Width  / 2.0) + (CamX / CamZ) * Focal;
    double ScreenY = (Height / 2.0) - (CamY / CamZ) * Focal;
    return Vector2(ScreenX, ScreenY);
}

#endif //DANARC_W2S_H