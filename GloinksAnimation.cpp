#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include "GloinksAnimation.hpp"

using namespace ProjectGloinks;

/**
 * Constructor that clears the collection of active Gloinks.
 */
GloinksAnimation::GloinksAnimation()
{
    activeGloinks.clear();
}

/**
 * Spawns the initial set of 6 Gloinks spaced out along the X axis.
 */
void GloinksAnimation::initGloinks()
{
    activeGloinks.clear();

    const float groundLevel = -18.7f;
    const float spacing = 15.0f;

    for (int i = 0; i < 6; ++i)
    {
        Gloink gloink;
        
        // Place them in a queue along the X-axis, centered at X = 0
        gloink.posX = (i - 2.5f) * spacing;
        gloink.posZ = 0.0f;
        gloink.posY = groundLevel; // Bottom of the shape sits at the ground level
        gloink.jumpTimer = static_cast<float>(std::rand() % 100);
        gloink.shapeType = i;
        gloink.isHurt = false;
        gloink.hurtTimer = 0.0f;
        gloink.health = 3; // Initial health/lives
        gloink.isDead = false;
        gloink.deathTimer = 0.0f;
        activeGloinks.push_back(gloink);
    }
}

/**
 * Updates positions and timers for all active Gloinks.
 * deltaTime The elapsed frame time in seconds.
 */
void GloinksAnimation::updateGloinks(float deltaTime)
{
    const float bounceSpeed = 3.0f;
    const float bounceHeight = 5.0f;
    const float groundLevel = -18.7f;
    const float hurtDuration = 0.5f;
    const float deathDuration = 1.0f; // 1 second death animation

    for (auto& gloink : activeGloinks)
    {
        if (gloink.isDead)
        {
            gloink.deathTimer += deltaTime;
            gloink.posY = groundLevel; // Keep it on the ground when dead
            
            // After 1 second, respawn the Gloink for testing
            if (gloink.deathTimer >= deathDuration)
            {
                gloink.isDead = false;
                gloink.deathTimer = 0.0f;
                gloink.health = 3; // Reset health
                gloink.isHurt = false;
                gloink.hurtTimer = 0.0f;
            }
        }
        else
        {
            gloink.jumpTimer += deltaTime;
            gloink.posY = groundLevel + std::abs(std::sin(gloink.jumpTimer * bounceSpeed)) * bounceHeight;

            if (gloink.isHurt)
            {
                gloink.hurtTimer += deltaTime;
                if (gloink.hurtTimer >= hurtDuration)
                {
                    gloink.isHurt = false;
                    gloink.hurtTimer = 0.0f;
                }
            }
        }
    }
}

/**
 * Damages a specific Gloink by index, triggering a hurt state or death.
 * index The index of the Gloink to damage.
 */
void GloinksAnimation::hurtGloink(int index)
{
    if (index >= 0 && index < (int)activeGloinks.size())
    {
        Gloink& gloink = activeGloinks[index];
        if (gloink.isDead)
            return; // Already dead

        gloink.health -= 1;
        if (gloink.health <= 0)
        {
            gloink.health = 0;
            gloink.isDead = true;
            gloink.deathTimer = 0.0f;
        }
        else
        {
            gloink.isHurt = true;
            gloink.hurtTimer = 0.0f;
        }
    }
}

/**
 * Applies rotation, scaling, and fall translation animations to the active OpenGL matrix.
 * center The bounding box visual center of the shape model.
 * isDead Whether the Gloink is dead.
 * deathTimer The current progression timer of the death animation.
 * isHurt Whether the Gloink is hurt.
 * hurtTimer The current progression timer of the hurt animation.
 */
void GloinksAnimation::applyAnimationTransforms(Vec3 center, bool isDead, float deathTimer, bool isHurt, float hurtTimer) const
{
    // Apply stiff fall to side (Minecraft style) if dead
    if (isDead)
    {
        float progress = deathTimer / 1.0f; // deathDuration = 1.0s
        if (progress > 1.0f) progress = 1.0f;
        float fallAngle = progress * 90.0f; // 0 to 90 degrees
        
        // --- Pivot Matrix Sandwich for Falling ---
        // 1. Translate bottom center of shape to origin
        glTranslatef(0.0f, -center.y, 0.0f);
        // 2. Rotate to drop sideways
        glRotatef(fallAngle, 0.0f, 0.0f, 1.0f);
        // 3. Translate back
        glTranslatef(0.0f, center.y, 0.0f);
    }

    // Apply hurt visual effect (squash & stretch from bottom pivot)
    if (!isDead && isHurt)
    {
        float hurtIntensity = 1.0f - (hurtTimer / 0.5f);
        if (hurtIntensity < 0.0f) hurtIntensity = 0.0f;

        // --- Pivot Matrix Sandwich for Squash and Stretch ---
        // 1. Translate bottom center of shape to origin
        glTranslatef(0.0f, -center.y, 0.0f);
        // 2. Apply scale distortion
        glScalef(1.0f + 0.3f * hurtIntensity, 1.0f - 0.5f * hurtIntensity, 1.0f + 0.3f * hurtIntensity);
        // 3. Translate back
        glTranslatef(0.0f, center.y, 0.0f);
    }
}

/**
 * Applies coloring/material properties based on the Gloink's current health state.
 * isDead Whether the Gloink is dead.
 * isHurt Whether the Gloink is hurt.
 * hurtTimer The current progression timer of the hurt animation.
 */
void GloinksAnimation::applyColorAndMaterial(bool isDead, bool isHurt, float hurtTimer) const
{
    if (isDead)
    {
        glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
        glDisable(GL_TEXTURE_2D);

        GLfloat redColor[]   = { 1.0f, 0.0f, 0.0f, 1.0f };
        GLfloat blackColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, redColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, redColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blackColor);

        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else if (isHurt)
    {
        float hurtIntensity = 1.0f - (hurtTimer / 0.5f);
        if (hurtIntensity < 0.0f) hurtIntensity = 0.0f;

        // Flash red
        glColor3f(1.0f, 1.0f - hurtIntensity, 1.0f - hurtIntensity);
    }
    else
    {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

/**
 * Applies rotation on the Z-axis for rolling/spinning motion during jumping.
 * jumpTimer The jump timer value of the Gloink.
 * isDead Whether the Gloink is dead.
 */
void GloinksAnimation::applyRotation(float jumpTimer, bool isDead) const
{
    float spinSpeed = 90.0f;
    if (!isDead)
    {
        glRotatef(jumpTimer * spinSpeed, 0.0f, 0.0f, 1.0f);
    }
}

/**
 * Restores normal material and lighting attributes.
 * isDead Whether the Gloink was dead.
 */
void GloinksAnimation::cleanupColorAndMaterial(bool isDead) const
{
    if (isDead)
    {
        glPopAttrib();
    }
}
