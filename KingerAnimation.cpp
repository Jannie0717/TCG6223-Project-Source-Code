#include "KingerAnimation.hpp"
#include <cmath>

KingerAnimation::KingerAnimation()
    : idleTimer(0.0f)
    , hoverOffset(0.0f)
    , clothRotation(0.0f)
    , armRotation(0.0f)
    , armSwayWeight(1.0f)
    , skillTimer(0.0f)
    , isCastingSkill(false)
    , skillArmRotation(0.0f)
    , skillBodyYOffset(0.0f)
    , skillBodyZOffset(0.0f)
    , armRecoilOffset(0.0f)
    , isBulletActive(false)
    , shootYaw(0.0f)
    , shootPitch(0.0f)
    , bulletPosX(20.0f)
    , bulletPosY(0.0f)
    , bulletPosZ(0.0f)
    , bulletStartX(0.0f)
    , bulletStartY(0.0f)
    , bulletStartZ(0.0f)
    , bulletDirX(0.0f)
    , bulletDirY(0.0f)
    , bulletDirZ(0.0f)
    , bulletDistance(0.0f)
    , isRolling(false)
    , rollTimer(0.0f)
    , rollPhase(0)
    , rollSquashY(1.0f)
    , showBallModel(false)
    , currentAmmo(MAX_AMMO)
    , isReloading(false)
    , reloadTimer(0.0f)
    , leftArmReloadPitch(0.0f)
    , leftArmReloadYaw(0.0f)
    , leftArmReloadYOffset(0.0f)
    , rightArmReloadPitch(0.0f)
    , rightArmReloadYaw(0.0f)
    , butterflyCharges(100)
    , isHealing(false)
    , healTimer(0.0f)
    , leftArmHealPitch(0.0f)
{
}

void KingerAnimation::castGunSkill()
{
    // Task 2: Ammo Logic & Guard Clauses
    // If the player is currently in a dodge roll, reloading, out of ammo, or healing, ignore the shooting command completely.
    if (isRolling || isReloading || currentAmmo <= 0 || isHealing) return;

    if (!isCastingSkill)
    {
        currentAmmo--;

        isCastingSkill  = true;
        skillTimer      = 0.0f;
        skillArmRotation  = 0.0f;
        skillBodyYOffset  = 0.0f;
        skillBodyZOffset  = 0.0f;
        armRecoilOffset   = 0.0f;
        // Ensure bullet is inactive at the start of every new cast
        isBulletActive = false;
        shootYaw       = 0.0f;
        shootPitch     = 0.0f;
        bulletPosX     = 0.0f;
        bulletPosY     = 0.0f;
        bulletPosZ     = 0.0f;
        bulletDistance = 0.0f;
        
        // Task 2: Auto-reload on empty
        if (currentAmmo <= 0)
        {
            castReload();
        }
    }
}

void KingerAnimation::updateSkillState(float deltaTime, float currentYaw, float currentPitch, float kingerX, float kingerY, float kingerZ)
{
    if (!isCastingSkill)
    {
        skillArmRotation = 0.0f;
        skillBodyYOffset = 0.0f;
        skillBodyZOffset = 0.0f;
        return;
    }

    // Apply the camera pitch to the arm so Kinger physically aims up/down at the crosshair.
    // We add this to skillArmRotation so it still does the wind-up and recoil animations!
    // Since cameraPitch is in radians, convert to degrees. Positive pitch = looking down, so invert it.
    float aimPitchDeg = -currentPitch * 57.2957795f;

    skillTimer += deltaTime;

    if (skillTimer <= 0.05f)
    {
        // Phase 1: The Kickback (0.0s to 0.05s)
        // Fire bullet immediately
        if (!isBulletActive)
        {
            isBulletActive = true;
            shootYaw       = currentYaw;
            shootPitch     = currentPitch - 75.0f; // Maintained from previous logic
            bulletDistance = 0.0f;

            // Task 1: Define the Hand Local Offsets
            // These match the translation/visual values of the RightHandwGun relative to the body
            const float HAND_OFFSET_X = 16.25f; // The gun sits on the right side of the body
            const float HAND_OFFSET_Y = 0.0f; // The vertical drop of the hand
            const float HAND_OFFSET_Z = -5.0f;  // The forward/back translation of the hand

            // Start the bullet at Kinger's chest/gun level, pushed forward to the tip of the gun.
            float spawnDist = -BULLET_SPAWN_Z; 
            
            // Apply the active body offsets (jump height + hover + recoil) to the local Y offset
            float baseY = (kingerY + 18.7f) + hoverOffset + skillBodyYOffset + HAND_OFFSET_Y + BULLET_BARREL_Y_BIAS;

            // Task 2 & 3: Calculate World Space Origin using 2D Rotation Matrix
            // We rotate the local X and Z offsets by Kinger's current facingYaw to find the true world coordinates
            // Forward is (-sin, -cos) and Right is (cos, -sin).
            float worldOffsetX = (HAND_OFFSET_X * std::cos(shootYaw)) + (HAND_OFFSET_Z * -std::sin(shootYaw));
            float worldOffsetZ = (HAND_OFFSET_X * -std::sin(shootYaw)) + (HAND_OFFSET_Z * -std::cos(shootYaw));

            // Set the absolute origin of the bullet and tracer trajectory
            bulletPosX = kingerX + worldOffsetX + spawnDist * (-std::sin(shootYaw) * std::cos(shootPitch));
            bulletPosY = baseY   + spawnDist * (-std::sin(shootPitch));
            bulletPosZ = kingerZ + worldOffsetZ + spawnDist * (-std::cos(shootYaw) * std::cos(shootPitch));
            
            // --- Task 1: Track the Trajectory Data ---
            // Save the exact origin point for the tracer line
            bulletStartX = bulletPosX;
            bulletStartY = bulletPosY;
            bulletStartZ = bulletPosZ;
            
            // Pre-calculate the normalized directional vector (avoids recalculating sin/cos every frame)
            bulletDirX = -std::sin(shootYaw) * std::cos(shootPitch);
            bulletDirY = -std::sin(shootPitch);
            bulletDirZ = -std::cos(shootYaw) * std::cos(shootPitch);
        }

        // Lerp arm recoil backward (+3.0f along local Z-axis)
        float t = skillTimer / 0.05f;
        armRecoilOffset = t * 3.0f;
    }
    else if (skillTimer <= 0.25f)
    {
        // Phase 2: The Recovery (0.05s to 0.25s)
        float t = (skillTimer - 0.05f) / 0.20f;
        // Smoothly lerp back to 0.0f
        float ease = 1.0f - std::cos(t * 3.14159265f / 2.0f); // Ease-in
        armRecoilOffset = 3.0f * (1.0f - ease);
    }
    else
    {
        // Reset: allow immediate re-firing
        isCastingSkill   = false;
        skillTimer       = 0.0f;
        armRecoilOffset  = 0.0f;
        skillArmRotation = 0.0f;
        skillBodyYOffset = 0.0f;
        skillBodyZOffset = 0.0f;
    }

    // Bullet travel & despawn — runs independently every tick while active.
    // Moves the bullet along the 3D directional vector captured when fired.
    if (isBulletActive)
    {
        float step = BULLET_TRAVEL_SPEED * deltaTime;
        bulletDistance += step;

        // Use the pre-calculated direction vector
        bulletPosX += step * bulletDirX;
        bulletPosY += step * bulletDirY;
        bulletPosZ += step * bulletDirZ;

        // Despawn based on total 3D distance traveled.
        if (bulletDistance > -BULLET_MAX_DISTANCE) // -(-150) = 150
        {
            isBulletActive = false;
        }
    }
}

void KingerAnimation::updateIdleState(float deltaTime)
{
    idleTimer += deltaTime;

    // Smooth hover animation: Y-axis oscillation for the body
    hoverOffset = 1.2f * std::sin(idleTimer * 2.0f);

    // Fluttering cloth animation: Z-axis rotation oscillation
    clothRotation = 3.0f * std::cos(idleTimer * 2.5f);

    // --- Task 1: Condition the Idle Arm Sway ---
    // If the shooting skill is active, we rapidly fade out the arm's breathing weight.
    // This isolates the right arm so the gun stops wobbling while firing.
    if (isCastingSkill)
    {
        armSwayWeight -= deltaTime * 15.0f; // Rapid fade out
        if (armSwayWeight < 0.0f) armSwayWeight = 0.0f;
    }
    else
    {
        armSwayWeight += deltaTime * 5.0f; // Smooth fade back in
        if (armSwayWeight > 1.0f) armSwayWeight = 1.0f;
    }

    // Breathing arms animation: X-axis rotation oscillation (scaled by the weight)
    armRotation = (4.0f * std::sin(idleTimer * 1.8f)) * armSwayWeight;
}

// Task 2: Implement Roll Logic
void KingerAnimation::castRollSkill()
{
    // Guard clause: Ensure mutually exclusive states
    if (isCastingSkill || isRolling || isReloading || isHealing) return;

    if (!isRolling)
    {
        isRolling = true;
        rollTimer = 0.0f;
        rollPhase = 0;
        rollSquashY = 1.0f;
        showBallModel = false;
    }
}

void KingerAnimation::updateRollState(float deltaTime)
{
    if (!isRolling) return;

    rollTimer += deltaTime;

    // Phase 0 (0.0s - 0.1s): Squash down
    if (rollTimer <= 0.1f)
    {
        rollPhase = 0;
        showBallModel = false;
        // Lerp rollSquashY from 1.0f to 0.3f
        rollSquashY -= (1.0f - 0.3f) * (deltaTime / 0.1f);
        if (rollSquashY < 0.3f) rollSquashY = 0.3f;
    }
    // Phase 1 (0.1s - 0.4s): The Ball Form
    else if (rollTimer <= 0.4f)
    {
        rollPhase = 1;
        showBallModel = true; // Swap to the KingerRoll model
        rollSquashY = 1.0f;   // Reset squash since the ball itself isn't squashed
    }
    // Phase 2 (0.4s - 0.5s): Recovery Squash
    else if (rollTimer <= 0.5f)
    {
        if (rollPhase == 1) // First frame of recovery
        {
            showBallModel = false; // Swap back to normal body
            rollSquashY = 0.3f;    // Start from squashed state
        }
        rollPhase = 2;
        // Lerp rollSquashY from 0.3f back to 1.0f
        rollSquashY += (1.0f - 0.3f) * (deltaTime / 0.1f);
        if (rollSquashY > 1.0f) rollSquashY = 1.0f;
    }
    // Phase 3 (> 0.5s): End Roll
    else
    {
        rollPhase = 3;
        isRolling = false;
        showBallModel = false;
        rollSquashY = 1.0f;
    }
}

// Task 3: Smooth Left Arm Animation
void KingerAnimation::castReload()
{
    if (isReloading || isRolling || isHealing) return;

    isReloading = true;
    reloadTimer = 0.0f;
    leftArmReloadPitch = 0.0f;
    leftArmReloadYaw = 0.0f;
    leftArmReloadYOffset = 0.0f;
    rightArmReloadPitch = 0.0f;
    rightArmReloadYaw = 0.0f;
}

void KingerAnimation::updateReloadState(float deltaTime)
{
    if (!isReloading) return;

    reloadTimer += deltaTime;

    if (reloadTimer <= 0.3f)
    {
        // Phase 1 (0.0s to 0.3s): Reach across chest and bring gun in
        leftArmReloadPitch -= (120.0f) * (deltaTime / 0.3f);
        leftArmReloadYaw -= (135.0f) * (deltaTime / 0.3f);
        leftArmReloadYOffset += (10.0f) * (deltaTime / 0.3f); // Lift the hand higher
        rightArmReloadPitch -= (60.0f) * (deltaTime / 0.3f); // Bring gun up
        
        if (leftArmReloadPitch < -120.0f) leftArmReloadPitch = -120.0f;
        if (leftArmReloadYaw < -135.0f) leftArmReloadYaw = -135.0f;
        if (leftArmReloadYOffset > 10.0f) leftArmReloadYOffset = 10.0f;
        
        if (rightArmReloadPitch < -60.0f) rightArmReloadPitch = -60.0f;
    }
    else if (reloadTimer <= 0.7f)
    {
        // Phase 2 (0.3s to 0.7s): Wiggle (Reload action)
        leftArmReloadPitch = -120.0f + 5.0f * std::sin(reloadTimer * 30.0f);
        leftArmReloadYOffset = 10.0f; // Hold the lifted height
        rightArmReloadPitch = -60.0f + 2.0f * std::cos(reloadTimer * 30.0f); // slight shake on gun too
    }
    else if (reloadTimer <= 1.0f)
    {
        // Phase 3 (0.7s to 1.0s): Return to normal
        leftArmReloadPitch += (120.0f) * (deltaTime / 0.3f);
        leftArmReloadYaw += (135.0f) * (deltaTime / 0.3f);
        leftArmReloadYOffset -= (10.0f) * (deltaTime / 0.3f);
        rightArmReloadPitch += (60.0f) * (deltaTime / 0.3f);
        
        if (leftArmReloadPitch > 0.0f) leftArmReloadPitch = 0.0f;
        if (leftArmReloadYaw > 0.0f) leftArmReloadYaw = 0.0f;
        if (leftArmReloadYOffset < 0.0f) leftArmReloadYOffset = 0.0f;
        if (rightArmReloadPitch > 0.0f) rightArmReloadPitch = 0.0f;
    }
    else
    {
        // Completion (> 1.0s)
        isReloading = false;
        currentAmmo = MAX_AMMO;
        leftArmReloadPitch = 0.0f;
        leftArmReloadYaw = 0.0f;
        leftArmReloadYOffset = 0.0f;
        rightArmReloadPitch = 0.0f;
        rightArmReloadYaw = 0.0f;
    }
}

// Task 2: Heal State Logic
void KingerAnimation::castHealSkill(int& currentHealth, int maxHealth)
{
    if (isHealing || isRolling || butterflyCharges <= 0) return;

    isHealing = true;
    healTimer = 0.0f;
    leftArmHealPitch = 0.0f;
    butterflyCharges--;

    currentHealth += 30;
    if (currentHealth > maxHealth) currentHealth = maxHealth;
}

void KingerAnimation::updateHealState(float deltaTime)
{
    if (!isHealing) return;

    healTimer += deltaTime;

    if (healTimer <= 0.5f)
    {
        // Phase 1 (0.0s to 0.5s): Raise arm straight up/forward
        leftArmHealPitch -= (180.0f) * (deltaTime / 0.5f);
        if (leftArmHealPitch < -180.0f) leftArmHealPitch = -180.0f;
    }
    else if (healTimer <= 1.5f)
    {
        // Phase 2 (0.5s to 1.5s): Hold the arm up
        leftArmHealPitch = -180.0f;
    }
    else if (healTimer <= 2.0f)
    {
        // Phase 3 (1.5s to 2.0s): Return arm back to normal
        leftArmHealPitch += (180.0f) * (deltaTime / 0.5f);
        if (leftArmHealPitch > 0.0f) leftArmHealPitch = 0.0f;
    }
    else
    {
        isHealing = false;
        leftArmHealPitch = 0.0f;
    }
}
