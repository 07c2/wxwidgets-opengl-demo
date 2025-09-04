// src/render/RenderState.h
#pragma once

/**
 * RenderState
 * A simple POD-like struct holding the adjustable parameters of the scene.
 *
 * This acts as the "contract" between the wxWidgets UI (SidePanel) and
 * the rendering backend (Renderer/Scene).
 *
 * - rotation_deg: rotation angle in degrees around the Z axis (2D scene).
 * - scale:        uniform scale factor (>0).
 * - object_visible: whether the main scene object is drawn.
 */
struct RenderState
{
    float rotation_deg {0.0f};
    float scale        {1.0f};
    bool  object_visible {true};
};