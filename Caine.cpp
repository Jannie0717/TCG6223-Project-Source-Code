#include <GL/glut.h>
#include <iostream>
#include "Caine.hpp"
#include <cmath>    

// Touched for ObjModel/Environment header dependency rebuild
using namespace ProjectCaine;

/**
 * Construct a new Caine object and initialize all loading flags, positions, scales, and spawn state.
 */
Caine::Caine()
{
    hatLoaded = false;
    leftHandLoaded = false;
    leftLegLoaded = false;
    leftPalmLoaded = false;
    lowerJawLoaded = false;
    rightHandLoaded = false;
    rightLegLoaded = false;
    rightPalmLoaded = false;
    staffLoaded = false;
    tongueLoaded = false;
    tursoLoaded = false;
    upperJawLoaded = false;
    leftEyeLoaded = false;
    rightEyeLoaded = false;
    uniformScale = 1.0f;
    posX = 0.0f;
    posY = 0.0f;
    posZ = 0.0f;
    spawnX = 0.0f;
    spawnY = 0.0f;
    spawnZ = 0.0f;
    spawnPositionSaved = false;
}

/**
 * Update Caine's animation state variables and check death timeline/respawn.
 * deltaTime Elapsed frame time (in seconds).
 */
void Caine::update(float deltaTime)
{
    // Capture the initial spawn position on the first update pass
    if (!spawnPositionSaved)
    {
        spawnX = posX;
        spawnY = posY;
        spawnZ = posZ;
        spawnPositionSaved = true;
    }

    // Accumulate death timer if Caine is dead
    if (animation.isDead)
    {
        animation.deathTimer += deltaTime;
        // Automatically respawn/reanimate Caine after exactly 2 seconds
        if (animation.deathTimer >= 2.0f)
        {
            animation.isDead = false;
            animation.deathTimer = 0.0f;
        }
    }

    // Forward updates to sub-animations
    animation.updateIdleState(deltaTime);
    animation.updateShootingState(deltaTime);
    animation.updateLayDown(deltaTime);
    animation.updateLeanForward(deltaTime);
    animation.updateHurtState(deltaTime);
}

/**
 * Handle death initialization, resetting all active posture modifiers and snapping to spawn position.
 */
void Caine::triggerDeath()
{
    animation.isDead = true;
    animation.deathTimer = 0.0f;
    animation.isLayingDown = false;
    animation.layDownFactor = 0.0f;
    animation.isLeaningForward = false;
    animation.leanForwardFactor = 0.0f;
    animation.isShootingState = false;
    animation.shootingTimer = 0.0f;
    animation.isHurt = false;
    animation.hurtTimer = 0.0f;

    if (spawnPositionSaved)
    {
        posX = spawnX;
        posY = spawnY;
        posZ = spawnZ;
    }
}

/**
 * Handle hurt initialization, resetting active shooting postures and starting the flashing timer.
 */
void Caine::triggerHurt()
{
    animation.triggerHurt();
}

/**
 * Set the uniform scale multiplier of the model.
 * scale Uniform scale factor.
 */
void Caine::setScale(float scale)
{
    uniformScale = scale;
}

bool Caine::loadHat(const std::string& filePath)
{
    hatLoaded = hatModel.loadFromObjText(filePath);
    return hatLoaded;
}

bool Caine::loadLeftHand(const std::string& filePath)
{
    leftHandLoaded = leftHandModel.loadFromObjText(filePath);
    return leftHandLoaded;
}

bool Caine::loadLeftLeg(const std::string& filePath)
{
    leftLegLoaded = leftLegModel.loadFromObjText(filePath);
    return leftLegLoaded;
}

bool Caine::loadLeftPalm(const std::string& filePath)
{
    leftPalmLoaded = leftPalmModel.loadFromObjText(filePath);
    return leftPalmLoaded;
}

bool Caine::loadLowerJaw(const std::string& filePath)
{
    lowerJawLoaded = lowerJawModel.loadFromObjText(filePath);
    return lowerJawLoaded;
}

bool Caine::loadRightHand(const std::string& filePath)
{
    rightHandLoaded = rightHandModel.loadFromObjText(filePath);
    return rightHandLoaded;
}

bool Caine::loadRightLeg(const std::string& filePath)
{
    rightLegLoaded = rightLegModel.loadFromObjText(filePath);
    return rightLegLoaded;
}

bool Caine::loadRightPalm(const std::string& filePath)
{
    rightPalmLoaded = rightPalmModel.loadFromObjText(filePath);
    return rightPalmLoaded;
}

bool Caine::loadStaff(const std::string& filePath)
{
    staffLoaded = staffModel.loadFromObjText(filePath);
    return staffLoaded;
}

bool Caine::loadTongue(const std::string& filePath)
{
    tongueLoaded = tongueModel.loadFromObjText(filePath);
    return tongueLoaded;
}

bool Caine::loadTurso(const std::string& filePath)
{
    tursoLoaded = tursoModel.loadFromObjText(filePath);
    return tursoLoaded;
}

bool Caine::loadUpperJaw(const std::string& filePath)
{
    upperJawLoaded = upperJawModel.loadFromObjText(filePath);
    return upperJawLoaded;
}

bool Caine::loadRightEye(const std::string& filePath)
{
    rightEyeLoaded = rightEyeModel.loadFromObjText(filePath);
    return rightEyeLoaded;
}

bool Caine::loadLeftEye(const std::string& filePath)
{
    leftEyeLoaded = leftEyeModel.loadFromObjText(filePath);
    return leftEyeLoaded;
}

// ==========================================
// Rendering Methods for Individual Parts
// ==========================================

/**
 * Render Caine's hat.
 */
void Caine::drawHat() const
{
    if (!hatLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, hatTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);
    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    hatModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's left hand, including laying down, lean forward, and shooting transitions.
 */
void Caine::drawLeftHand() const
{
    if (!leftHandLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leftHandTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    Vec3 center = leftHandModel.getCenter();
    center.x -= 1.25f; // Offset to line up shoulder pivot
    float scale = 3.0f;

    // --- Pivot Matrix Sandwich for Left Hand ---
    // 1. Translate to shoulder pivot center in scaled space
    glTranslatef(center.x * scale, center.y * scale, center.z * scale);
    
    // 2. Apply shooting/pointing rotation sequence
    animation.applyShootingAnimation();

    // 3. Apply raising arm rotations when laying down
    glRotatef(animation.layDownFactor * 45.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(animation.layDownFactor * 90.0f, 1.0f, 0.0f, 0.0f);

    // 4. Apply forward arm pitch when leaning forward
    glRotatef(animation.leanForwardFactor * 45.0f, 0.0f, 1.0f, 0.0f);

    // 5. Translate back from the shoulder pivot center
    glTranslatef(-center.x * scale, -center.y * scale, -center.z * scale);

    glScalef(scale, scale, scale);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    leftHandModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's left leg.
 */
void Caine::drawLeftLeg() const
{
    if (!leftLegLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leftLegTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);
    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    leftLegModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's left palm, tracking parent hand position.
 */
void Caine::drawLeftPalm() const
{
    if (!leftPalmLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leftPalmTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    Vec3 center = leftHandModel.getCenter();
    center.x -= 1.25f; // Match parental shoulder offset
    float scale = 3.0f;

    // --- Pivot Matrix Sandwich for Left Palm (inherits hand transformations) ---
    glTranslatef(center.x * scale, center.y * scale, center.z * scale);
    animation.applyShootingAnimation();
    glRotatef(animation.layDownFactor * 45.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(animation.layDownFactor * 90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(animation.leanForwardFactor * 45.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-center.x * scale, -center.y * scale, -center.z * scale);

    glScalef(scale, scale, scale);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    leftPalmModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's lower jaw, handling breathing mouth motions and death jaw drop.
 */
void Caine::drawLowerJaw() const
{
    if (!lowerJawLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lowerJawTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    // Idle breathing TMJ swing
    glRotatef(animation.mouthOpenFactor * -5.0f, 1.0f, 0.0f, 0.0f);

    // Lean forward jaw rotation
    glRotatef(animation.leanForwardFactor * 10.0f, 0.0f, 1.0f, 0.0f);

    // Death wide mouth drop
    if (animation.isDead)
    {
        float deathFactor = animation.deathTimer;
        if (deathFactor > 1.0f) deathFactor = 1.0f;
        glRotatef(deathFactor * 15.0f, 0.0f, 1.0f, 0.0f);
    }

    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    lowerJawModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's right hand.
 */
void Caine::drawRightHand() const
{
    if (!rightHandLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rightHandTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    Vec3 center = rightHandModel.getCenter();
    center.x += 1.25f; // Symmetrical shoulder alignment offset
    float scale = 3.0f;

    // --- Pivot Matrix Sandwich for Right Hand ---
    glTranslatef(center.x * scale, center.y * scale, center.z * scale);
    
    // Resting roll angle (fades out when laying down)
    float rightRoll = 30.0f * (1.0f - animation.layDownFactor);
    glRotatef(rightRoll, 0.0f, 0.0f, 1.0f);

    // Arm raising when laying down
    glRotatef(animation.layDownFactor * -45.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(animation.layDownFactor * 90.0f, 1.0f, 0.0f, 0.0f);

    // Arm swing forward when leaning forward
    glRotatef(animation.leanForwardFactor * -45.0f, 0.0f, 1.0f, 0.0f);

    glTranslatef(-center.x * scale, -center.y * scale, -center.z * scale);

    glScalef(scale, scale, scale);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    rightHandModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's right leg.
 */
void Caine::drawRightLeg() const
{
    if (!rightLegLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rightLegTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);
    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    rightLegModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's right palm.
 */
void Caine::drawRightPalm() const
{
    if (!rightPalmLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rightPalmTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    if (rightHandLoaded)
    {
        Vec3 center = rightHandModel.getCenter();
        center.x += 1.25f; // Match right shoulder offset
        float scale = 3.0f;

        // Pivot Matrix Sandwich for Right Palm (inherits hand transformations)
        glTranslatef(center.x * scale, center.y * scale, center.z * scale);
        
        float rightRoll = 30.0f * (1.0f - animation.layDownFactor);
        glRotatef(rightRoll, 0.0f, 0.0f, 1.0f);
        
        glRotatef(animation.layDownFactor * -45.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(animation.layDownFactor * 90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(animation.leanForwardFactor * -45.0f, 0.0f, 1.0f, 0.0f);
        
        glTranslatef(-center.x * scale, -center.y * scale, -center.z * scale);
    }

    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    rightPalmModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's staff, shifting alignment to the left hand grip space.
 */
void Caine::drawStaff() const
{
    if (!staffLoaded) return;

    // Local alignment offsets for Left Hand grip
    float staffOffsetX = 30.0f;
    float staffOffsetY = 18.0f;
    float staffOffsetZ = 3.0f;

    float staffGripRotX = 0.0f;
    float staffGripRotY = 0.0f;
    float staffGripRotZ = 0.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, staffTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    Vec3 centerLeft = leftHandModel.getCenter();
    centerLeft.x -= 1.25f;
    float scale = 3.0f;

    // Apply left hand shooting/pointing offsets
    glTranslatef(centerLeft.x * scale, centerLeft.y * scale, centerLeft.z * scale);
    animation.applyShootingAnimation();
    glRotatef(animation.layDownFactor * 45.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(animation.layDownFactor * 90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(-centerLeft.x * scale, -centerLeft.y * scale, -centerLeft.z * scale);

    // Shift coordinate reference frame from right hand to left hand
    if (rightHandLoaded)
    {
        Vec3 centerRight = rightHandModel.getCenter();
        Vec3 shift;
        shift.x = centerLeft.x - centerRight.x;
        shift.y = centerLeft.y - centerRight.y;
        shift.z = centerLeft.z - centerRight.z;
        glTranslatef(shift.x * scale, shift.y * scale, shift.z * scale);
    }

    // Apply custom offsets, grip orientations, body breathing tilt, and scales
    glTranslatef(staffOffsetX, staffOffsetY, staffOffsetZ);
    glRotatef(staffGripRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(staffGripRotY, 0.0f, 1.0f, 0.0f);
    glRotatef(staffGripRotZ, 0.0f, 0.0f, 1.0f);
    glRotatef(animation.bodyTiltAngle, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    staffModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's tongue, with mouth opening movement on death.
 */
void Caine::drawTongue() const
{
    if (!tongueLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tongueTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);
    
    // Slight tongue drop flat on death mouth opening (rotated around pitch axis)
    if (animation.isDead)
    {
        float deathFactor = animation.deathTimer;
        if (deathFactor > 1.0f) deathFactor = 1.0f;
        glRotatef(deathFactor * 15.0f, 0.0f, 1.0f, 0.0f);
    }

    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    tongueModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's torso (Turso).
 */
void Caine::drawTurso() const
{
    if (!tursoLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tursoTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);
    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    tursoModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's upper jaw, handling breathing TMJ movement, lean mouth open, and death mouth open.
 */
void Caine::drawUpperJaw() const
{
    if (!upperJawLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, upperJawTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(0.0f, -19.5f, 20.0f);

    // Breathing mouth flapping counter-rotation
    glRotatef(animation.mouthOpenFactor * -3.0f, 1.0f, 0.0f, 0.0f);

    // Lean forward mouth open rotation
    glRotatef(animation.leanForwardFactor * 5.0f, 1.0f, 0.0f, 0.0f);

    // Death mouth open rotation (pitch axis)
    if (animation.isDead)
    {
        float deathFactor = animation.deathTimer;
        if (deathFactor > 1.0f) deathFactor = 1.0f;
        glRotatef(deathFactor * 15.0f, 0.0f, 1.0f, 0.0f);
    }

    glScalef(3.0f, 3.0f, 3.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    upperJawModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's right eye.
 */
void Caine::drawRightEye() const
{
    if (!rightEyeLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rightEyeTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(-0.5f, -9.0f, 20.0f);
    glScalef(4.0f, 4.0f, 4.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    rightEyeModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render Caine's left eye.
 */
void Caine::drawLeftEye() const
{
    if (!leftEyeLoaded) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leftEyeTextureID);

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    glTranslatef(-0.5f, -9.0f, 20.0f);
    glScalef(4.0f, 4.0f, 4.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    leftEyeModel.draw();
    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/**
 * Render the entire Caine character, nesting all components under world space and posture transforms.
 */
void Caine::draw() const
{
    // Disappear completely after 1 second of dying
    if (animation.isDead && animation.deathTimer >= 1.0f)
        return;

    bool shouldRender = true;
    if (animation.isHurt)
    {
        // Toggle visibility every 0.05 seconds (10 Hz blink rate)
        if (std::fmod(animation.hurtTimer, 0.1f) < 0.05f)
        {
            shouldRender = false;
        }
    }

    if (shouldRender)
    {
        glPushMatrix();
        
        // 1. Move to world space coordinates
        glTranslatef(posX, posY, posZ);

        // 2. Apply breathing hover offset
        glTranslatef(0.0f, animation.hoverOffset, 0.0f);

        // 3. Shift Caine upwards when laying down to keep above ground
        glTranslatef(0.0f, animation.layDownFactor * 20.0f, 0.0f);

        // 4. Transform to Caine's posture pivot center, apply posture rotations, scale, then translate back
        glTranslatef(0.0f, -19.5f, -20.0f);
        glRotatef(animation.bodyTiltAngle, 0.0f, 0.0f, 1.0f);          // Idle breathing body tilt
        glRotatef(animation.layDownFactor * 70.0f, 0.0f, 0.0f, 1.0f);    // Lay down transition rotation
        glRotatef(animation.leanForwardFactor * 45.0f, 1.0f, 0.0f, 0.0f); // Lean forward transition rotation
        glScalef(uniformScale, uniformScale, uniformScale);
        glTranslatef(0.0f, 19.5f, 20.0f);

        if (animation.isHurt || animation.isDead)
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

        // 5. Draw the lower body limbs and torso
        drawTurso();
        drawLeftHand();
        drawRightHand();
        drawLeftPalm();
        drawRightPalm();
        drawLeftLeg();
        drawRightLeg();
        drawStaff();

        // 6. Draw the head parts grouped under a death tilt transform
        glPushMatrix();
        if (animation.isDead)
        {
            float deathFactor = animation.deathTimer;
            if (deathFactor > 1.0f) deathFactor = 1.0f;
            // Symmetrically rotate the head around local neck pivot
            glTranslatef(0.0f, 3.0f, -20.0f);
            glRotatef(deathFactor * 20.0f, 0.0f, 0.0f, 1.0f); // Tilt head to left
            glTranslatef(0.0f, -3.0f, 20.0f);
        }
        drawHat();
        drawUpperJaw();
        drawLowerJaw();
        drawTongue();
        drawRightEye();
        drawLeftEye();
        glPopMatrix();

        if (animation.isHurt || animation.isDead)
        {
            glPopAttrib();
        }

        glPopMatrix();
    }
}
