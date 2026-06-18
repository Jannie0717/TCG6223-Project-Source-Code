#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "Gloinks.hpp"

using namespace ProjectGloinks;

/**
 * Constructor that resets model loading flags and sets the initial uniform scale.
 */
Gloinks::Gloinks()
{
    gloinksBowlingPinLoaded = false;
    gloinksCircleLoaded = false;
    gloinksCubeLoaded = false;
    gloinksMoonLoaded = false;
    gloinksStarLoaded = false;
    gloinksTriangularLoaded = false;
    uniformScale = 1.0f;
}

/**
 * Sets the uniform scale of the Gloink shapes.
 * scale The scaling factor to apply uniformly.
 */
void Gloinks::setScale(float scale)
{
    uniformScale = scale;
}

// ==========================================
// OBJ File Loaders
// ==========================================

bool Gloinks::loadGloinksBowlingPin(const std::string& filePath)
{
    gloinksBowlingPinLoaded = gloinksBowlingPinModel.loadFromObjText(filePath);
    return gloinksBowlingPinLoaded;
}

bool Gloinks::loadGloinksCircle(const std::string& filePath)
{
    gloinksCircleLoaded = gloinksCircleModel.loadFromObjText(filePath);
    return gloinksCircleLoaded;
}

bool Gloinks::loadGloinksCube(const std::string& filePath)
{
    gloinksCubeLoaded = gloinksCubeModel.loadFromObjText(filePath);
    return gloinksCubeLoaded;
}

bool Gloinks::loadGloinksMoon(const std::string& filePath)
{
    gloinksMoonLoaded = gloinksMoonModel.loadFromObjText(filePath);
    return gloinksMoonLoaded;
}

bool Gloinks::loadGloinksStar(const std::string& filePath)
{
    gloinksStarLoaded = gloinksStarModel.loadFromObjText(filePath);
    return gloinksStarLoaded;
}

bool Gloinks::loadGloinksTriangular(const std::string& filePath)
{
    gloinksTriangularLoaded = gloinksTriangularModel.loadFromObjText(filePath);
    return gloinksTriangularLoaded;
}

// ==========================================
// Rendering Methods for Individual Shapes
// ==========================================

/**
 * Render the bowling pin shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksBowlingPin(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksBowlingPinLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, BowlingPinTextureID);

    Vec3 center = gloinksBowlingPinModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    // Translate bottom of model to the ground-level Y coordinate
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    // Apply animation transforms, materials, and rotations
    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    // Translate back to model local coordinates
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksBowlingPinModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the circle shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksCirle(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksCircleLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, CircleTextureID);

    Vec3 center = gloinksCircleModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksCircleModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the cube shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksCube(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksCubeLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, CubeTextureID);

    Vec3 center = gloinksCubeModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksCubeModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the crescent moon shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksMoon(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksMoonLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, MoonTextureID);

    Vec3 center = gloinksMoonModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksMoonModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the star shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksStar(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksStarLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, StarTextureID);

    Vec3 center = gloinksStarModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksStarModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the triangular shape Gloink with texture, scale, and animation transforms.
 */
void Gloinks::drawGloinksTriangular(float jumpTimer, float posX, float posY, float posZ, bool isHurt, float hurtTimer, bool isDead, float deathTimer) const
{
    if (!gloinksTriangularLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TriangularTextureID);

    Vec3 center = gloinksTriangularModel.getCenter();
    float scale = 4.0f * uniformScale;
    float worldCenterY = posY + (center.y * scale);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    
    glTranslatef(posX, worldCenterY, posZ);
    glScalef(scale, scale, scale);

    animation.applyAnimationTransforms(center, isDead, deathTimer, isHurt, hurtTimer);
    animation.applyColorAndMaterial(isDead, isHurt, hurtTimer);
    animation.applyRotation(jumpTimer, isDead);
    
    glTranslatef(-center.x, -center.y, -center.z);
    gloinksTriangularModel.draw();
    animation.cleanupColorAndMaterial(isDead);

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Renders all active Gloinks in the scene.
 */
void Gloinks::draw() const
{
    glPushMatrix();
    
    // Reset color to pure white so textures aren't tinted by previously drawn objects
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& gloink : animation.activeGloinks)
    {
        switch (gloink.shapeType)
        {
            case 0: drawGloinksBowlingPin(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
            case 1: drawGloinksCirle(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
            case 2: drawGloinksCube(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
            case 3: drawGloinksMoon(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
            case 4: drawGloinksStar(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
            case 5: drawGloinksTriangular(gloink.jumpTimer, gloink.posX, gloink.posY, gloink.posZ, gloink.isHurt, gloink.hurtTimer, gloink.isDead, gloink.deathTimer); break;
        }
    }

    glPopMatrix();
}

/**
 * Initializes all spawn variables and parameters for the collection of Gloinks.
 */
void Gloinks::initGloinks()
{
    animation.initGloinks();
}

/**
 * Updates positions, timers, and states for all active Gloinks.
 * deltaTime The elapsed frame time in seconds.
 */
void Gloinks::updateGloinks(float deltaTime)
{
    animation.updateGloinks(deltaTime);
}

/**
 * Damages a Gloink at the specified index.
 * index The index of the Gloink to damage.
 */
void Gloinks::hurtGloink(int index)
{
    animation.hurtGloink(index);
}
