#include "KingerAnimation.hpp"
#include <cmath>

/**
 * Constructor that resets all states, timers, and factors to their defaults.
 */
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
    , isHurt(false)
    , hurtTimer(0.0f)
    , isDead(false)
    , deathTimer(0.0f)
{
}

/**
 * Casts the gun skill, reducing ammo count, and resetting shooting timers/recoil.
 */
void KingerAnimation::castGunSkill()
{
    if (isRolling || isReloading || currentAmmo <= 0 || isHealing || isHurt) return;

    if (!isCastingSkill)
    {
        currentAmmo--;

        isCastingSkill  = true;
        skillTimer      = 0.0f;
        skillArmRotation  = 0.0f;
        skillBodyYOffset  = 0.0f;
        skillBodyZOffset  = 0.0f;
        armRecoilOffset   = 0.0f;
        isBulletActive = false;
        shootYaw       = 0.0f;
        shootPitch     = 0.0f;
        bulletPosX     = 0.0f;
        bulletPosY     = 0.0f;
        bulletPosZ     = 0.0f;
        bulletDistance = 0.0f;
        
        if (currentAmmo <= 0)
        {
            castReload();
        }
    }
}

/**
 * Updates the gunshot recoil timeline and calculates active bullet positions.
 * deltaTime The elapsed frame time in seconds.
 * currentYaw Current facing horizontal yaw angle.
 * currentPitch Current aiming vertical pitch angle.
 * kingerX Current world X position of the player.
 * kingerY Current world Y position of the player.
 * kingerZ Current world Z position of the player.
 * modelScale Current uniform scale of the player model.
 */
void KingerAnimation::updateSkillState(float deltaTime, float currentYaw, float currentPitch, float kingerX, float kingerY, float kingerZ, float modelScale)
{
    if (!isCastingSkill)
    {
        skillArmRotation = 0.0f;
        skillBodyYOffset = 0.0f;
        skillBodyZOffset = 0.0f;
        return;
    }

    skillTimer += deltaTime;

    if (skillTimer <= 0.05f)
    {
        if (!isBulletActive)
        {
            isBulletActive = true;
            shootYaw       = currentYaw;
            shootPitch = currentPitch + (27.0f * 3.14159265f / 180.0f);
            bulletDistance = 0.0f;

            // 1. Define the pivot and barrel offsets (scaled)
            const float ARM_OFFSET_X = 16.25f * modelScale; 
            const float PIVOT_Y      = 20.0f * modelScale; 
            const float BARREL_Z     = -10.0f * modelScale;  

            // 2. Apply pitch rotation so the barrel tip moves up/down when aiming
            float localY = PIVOT_Y + (BARREL_Z * std::sin(shootPitch));
            float localZ = BARREL_Z * std::cos(shootPitch);

            // 3. Rotate these scaled offsets into world space using a 2D rotation matrix based on currentYaw
            float worldOffsetX = (ARM_OFFSET_X * std::cos(currentYaw)) + (localZ * std::sin(currentYaw));
            float worldOffsetZ = (-ARM_OFFSET_X * std::sin(currentYaw)) + (localZ * std::cos(currentYaw));

            // 4. Add the result to the player's world position
            bulletStartX = kingerX + worldOffsetX;
            bulletStartY = (kingerY * modelScale) + hoverOffset + skillBodyYOffset + localY + BULLET_BARREL_Y_BIAS;
            bulletStartZ = kingerZ + worldOffsetZ;
            
            bulletPosX = bulletStartX;
            bulletPosY = bulletStartY;
            bulletPosZ = bulletStartZ;
            
            // 4. Calculate the trajectory direction
            bulletDirX = -std::sin(shootYaw) * std::cos(shootPitch);
            bulletDirY = -std::sin(shootPitch);
            bulletDirZ = -std::cos(shootYaw) * std::cos(shootPitch);
        }

        float t = skillTimer / 0.05f;
        armRecoilOffset = t * 3.0f;
    }
    else if (skillTimer <= 0.25f)
    {
        float t = (skillTimer - 0.05f) / 0.20f;
        float ease = 1.0f - std::cos(t * 3.14159265f / 2.0f); 
        armRecoilOffset = 3.0f * (1.0f - ease);
    }
    else
    {
        isCastingSkill   = false;
        skillTimer       = 0.0f;
        armRecoilOffset  = 0.0f;
        skillArmRotation = 0.0f;
        skillBodyYOffset = 0.0f;
        skillBodyZOffset = 0.0f;
    }

    if (isBulletActive)
    {
        float step = BULLET_TRAVEL_SPEED * deltaTime;
        bulletDistance += step;

        bulletPosX += step * bulletDirX;
        bulletPosY += step * bulletDirY;
        bulletPosZ += step * bulletDirZ;

        if (bulletDistance > -BULLET_MAX_DISTANCE) 
        {
            isBulletActive = false;
        }
    }
}

/**
 * Updates the idle animation timer, hover heights, and cloth sway angles.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateIdleState(float deltaTime)
{
    idleTimer += deltaTime;

    hoverOffset = 1.2f * std::sin(idleTimer * 2.0f);

    clothRotation = 3.0f * std::cos(idleTimer * 2.5f);

    if (isCastingSkill)
    {
        armSwayWeight -= deltaTime * 15.0f; 
        if (armSwayWeight < 0.0f) armSwayWeight = 0.0f;
    }
    else
    {
        armSwayWeight += deltaTime * 5.0f; 
        if (armSwayWeight > 1.0f) armSwayWeight = 1.0f;
    }

    armRotation = (4.0f * std::sin(idleTimer * 1.8f)) * armSwayWeight;
}

/**
 * Triggers the rolling movement ability, resetting the phase and squash states.
 */
void KingerAnimation::castRollSkill()
{
    if ( isRolling || isReloading || isHealing) return;

    if (!isRolling)
    {
        isRolling = true;
        rollTimer = 0.0f;
        rollPhase = 0;
        rollSquashY = 1.0f;
        showBallModel = false;
    }
}

/**
 * Updates the roll timeline, phases, and squash factors.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateRollState(float deltaTime)
{
    if (!isRolling) return;

    rollTimer += deltaTime;

    if (rollTimer <= 0.1f)
    {
        rollPhase = 0;
        showBallModel = false;
        rollSquashY -= (1.0f - 0.3f) * (deltaTime / 0.1f);
        if (rollSquashY < 0.3f) rollSquashY = 0.3f;
    }
    else if (rollTimer <= 0.4f)
    {
        rollPhase = 1;
        showBallModel = true;
        rollSquashY = 1.0f;   
    }
    else if (rollTimer <= 0.5f)
    {
        if (rollPhase == 1) 
        {
            showBallModel = false; 
            rollSquashY = 0.3f;
        }
        rollPhase = 2;
        rollSquashY += (1.0f - 0.3f) * (deltaTime / 0.1f);
        if (rollSquashY > 1.0f) rollSquashY = 1.0f;
    }
    else
    {
        rollPhase = 3;
        isRolling = false;
        showBallModel = false;
        rollSquashY = 1.0f;
    }
}

/**
 * Triggers the gun reload animation timeline.
 */
void KingerAnimation::castReload()
{
    if (isReloading || isRolling || isHealing || isHurt) return;

    isReloading = true;
    reloadTimer = 0.0f;
    leftArmReloadPitch = 0.0f;
    leftArmReloadYaw = 0.0f;
    leftArmReloadYOffset = 0.0f;
    rightArmReloadPitch = 0.0f;
    rightArmReloadYaw = 0.0f;
}

/**
 * Updates the reload timeline, animating left/right arms dynamically.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateReloadState(float deltaTime)
{
    if (!isReloading) return;

    reloadTimer += deltaTime;

    if (reloadTimer <= 0.3f)
    {
        leftArmReloadPitch -= (120.0f) * (deltaTime / 0.3f);
        leftArmReloadYaw -= (135.0f) * (deltaTime / 0.3f);
        leftArmReloadYOffset += (10.0f) * (deltaTime / 0.3f); 
        rightArmReloadPitch -= (60.0f) * (deltaTime / 0.3f); 
        
        if (leftArmReloadPitch < -120.0f) leftArmReloadPitch = -120.0f;
        if (leftArmReloadYaw < -135.0f) leftArmReloadYaw = -135.0f;
        if (leftArmReloadYOffset > 10.0f) leftArmReloadYOffset = 10.0f;
        
        if (rightArmReloadPitch < -60.0f) rightArmReloadPitch = -60.0f;
    }
    else if (reloadTimer <= 0.7f)
    {
        leftArmReloadPitch = -120.0f + 5.0f * std::sin(reloadTimer * 30.0f);
        leftArmReloadYOffset = 10.0f; 
        rightArmReloadPitch = -60.0f + 2.0f * std::cos(reloadTimer * 30.0f); 
    }
    else if (reloadTimer <= 1.0f)
    {
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
        isReloading = false;
        currentAmmo = MAX_AMMO;
        leftArmReloadPitch = 0.0f;
        leftArmReloadYaw = 0.0f;
        leftArmReloadYOffset = 0.0f;
        rightArmReloadPitch = 0.0f;
        rightArmReloadYaw = 0.0f;
    }
}

/**
 * Triggers the butterfly healing skill, reducing butterfly charges and restoring health.
 * currentHealth Reference to the character's current health.
 * maxHealth The maximum health threshold.
 */
void KingerAnimation::castHealSkill(int& currentHealth, int maxHealth)
{
    if (isHealing || isRolling || butterflyCharges <= 0 || isHurt) return;

    isHealing = true;
    healTimer = 0.0f;
    leftArmHealPitch = 0.0f;
    butterflyCharges--;

    currentHealth += 30;
    if (currentHealth > maxHealth) currentHealth = maxHealth;
}

/**
 * Updates the heal timeline, raising the left arm and spawning a butterfly.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateHealState(float deltaTime)
{
    if (!isHealing) return;

    healTimer += deltaTime;

    if (healTimer <= 0.5f)
    {
        leftArmHealPitch -= (180.0f) * (deltaTime / 0.5f);
        if (leftArmHealPitch < -180.0f) leftArmHealPitch = -180.0f;
    }
    else if (healTimer <= 1.5f)
    {
        leftArmHealPitch = -180.0f;
    }
    else if (healTimer <= 2.0f)
    {
        leftArmHealPitch += (180.0f) * (deltaTime / 0.5f);
        if (leftArmHealPitch > 0.0f) leftArmHealPitch = 0.0f;
    }
    else
    {
        isHealing = false;
        leftArmHealPitch = 0.0f;
    }
}

/**
 * Updates the damage stun flashing timer.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateHurtState(float deltaTime)
{
    if (!isHurt) return;

    hurtTimer += deltaTime;
    if (hurtTimer >= HURT_DURATION)
    {
        isHurt = false;
        hurtTimer = 0.0f;
    }
}

/**
 * Activates the hurt state, immediately interrupting any ongoing cast/reloads.
 */
void KingerAnimation::triggerHurt()
{
    isHurt = true;
    hurtTimer = 0.0f;

    // Immediately stop ongoing skills
    isCastingSkill = false;
    skillTimer = 0.0f;
    skillArmRotation = 0.0f;
    skillBodyYOffset = 0.0f;
    skillBodyZOffset = 0.0f;
    armRecoilOffset = 0.0f;
    isBulletActive = false;
    bulletDistance = 0.0f;

    isRolling = false;
    rollTimer = 0.0f;
    rollPhase = 0;
    rollSquashY = 1.0f;
    showBallModel = false;

    isReloading = false;
    reloadTimer = 0.0f;
    leftArmReloadPitch = 0.0f;
    leftArmReloadYaw = 0.0f;
    leftArmReloadYOffset = 0.0f;
    rightArmReloadPitch = 0.0f;
    rightArmReloadYaw = 0.0f;

    isHealing = false;
    healTimer = 0.0f;
    leftArmHealPitch = 0.0f;
}

/**
 * Updates the death collapse timer.
 * deltaTime The elapsed frame time in seconds.
 */
void KingerAnimation::updateDeathState(float deltaTime)
{
    if (!isDead) return;

    deathTimer += deltaTime;
    if (deathTimer > RESPAWN_DELAY)
    {
        deathTimer = RESPAWN_DELAY;
    }
}

/**
 * Activates the death state, immediately interrupting any active skills.
 */
void KingerAnimation::triggerDeath()
{
    isDead = true;
    deathTimer = 0.0f;

    // Immediately stop ongoing skills
    isCastingSkill = false;
    skillTimer = 0.0f;
    skillArmRotation = 0.0f;
    skillBodyYOffset = 0.0f;
    skillBodyZOffset = 0.0f;
    armRecoilOffset = 0.0f;
    isBulletActive = false;
    bulletDistance = 0.0f;

    isRolling = false;
    rollTimer = 0.0f;
    rollPhase = 0;
    rollSquashY = 1.0f;
    showBallModel = false;

    isReloading = false;
    reloadTimer = 0.0f;
    leftArmReloadPitch = 0.0f;
    leftArmReloadYaw = 0.0f;
    leftArmReloadYOffset = 0.0f;
    rightArmReloadPitch = 0.0f;
    rightArmReloadYaw = 0.0f;

    isHealing = false;
    healTimer = 0.0f;
    leftArmHealPitch = 0.0f;

    isHurt = false;
    hurtTimer = 0.0f;
}
