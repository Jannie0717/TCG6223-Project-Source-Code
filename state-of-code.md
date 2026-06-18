# State of Code — TCG6223 Fixed-Function OpenGL Game Project
**Technical Onboarding Guide & Architecture Documentation**

---

## 1. High-Level Project Overview

This project is a 3D third-person action game set in a circus-arena environment, heavily inspired by the characters of **The Amazing Digital Circus (TADC)**. It is built in C++ using the fixed-function OpenGL and GLUT libraries.

### Core Gameplay Loop
The player controls **Kinger** as a third-person character who can move, jump, shoot, dodge roll, reload, and heal using butterfly summons. Kinger faces the boss **Caine** (the Ringmaster), who stands in the center of the arena, and waves of **Gloinks** (hazard enemies).

### Rendering & Update Pipeline
The game runs on a single-threaded message loop managed by GLUT.
1. **GLUT Events**: Keyboard, mouse clicks, mouse passive movement, and window resize events update input states (`keyStates` array, `cameraYaw`/`cameraPitch` tracking).
2. **Display Loop (`myDisplayFunc`)**:
   - Recomputes the over-the-shoulder third-person camera orientation via `gluLookAt` using follow distance, height, and right-shoulder offsets.
   - Sets up light source positions.
   - Iterates through the virtual world instance to render the environment, characters, bullets, and summon particles.
   - Switches to 2D orthographic projection to overlay the aim crosshair.
3. **Frame Tick (`tickTime`)**:
   - Computes the elapsed time (`deltaTime`) since the last frame.
   - Updates the physics and animation states for Kinger, Caine, and all active Gloinks.

---

## 2. Directory & File Structure

The codebase is organized into modules. Each character/entity separates its raw rendering (drawing calls) from its logical variables and state calculations.

```
[Root]
├── CNAmain.cpp / .hpp          # Entry point, GLUT callbacks, input polling, follow camera, 2D UI
├── CNAWorld.cpp / .hpp         # Main World controller: owns all models, handles initialization & ticks
├── ObjModel.cpp / .hpp         # Custom OBJ mesh parser (.txt format) & low-level OpenGL draw calls
├── TextureLoader.cpp / .hpp    # STB Image wrapper to load PNG/JPG images into OpenGL textures
├── Environment.cpp / .hpp      # Skybox, floor, walls, pillars, and glitch digital background effects
├── AudioManager.cpp / .hpp     # Sound loading and playing placeholders (WinMM API wrapper)
├── Kinger.cpp / .hpp           # Kinger visual mesh draw calls, texturing, and physical coordinates
├── KingerAnimation.cpp / .hpp  # Kinger logical updates: idle sway, shooting recoil, reload, roll, heal
├── KingerRoll.cpp / .hpp       # Auxiliary roll-ball mesh model wrapper used during the dodge roll
├── Caine.cpp / .hpp            # Caine visual drawing calls for 14 distinct hierarchical body parts
├── CaineAnimation.cpp / .hpp   # Caine state machine: idle, shoot, lay down, lean forward, death/respawn
├── Gloinks.cpp / .hpp          # Gloinks drawing calls for 6 variant shapes (star, moon, Pin, etc.)
├── GloinksAnimation.cpp / .hpp # Spawning, bounce/roll update math, damage flashing, and death falls
└── Butterfly.cpp / .hpp        # Visual particle butterfly mesh drawings (left/right wings, body)
```

### Separation of Concerns: Visuals vs. Logic
- **`Name.hpp` / `Name.cpp` (Visuals)**:
  Contains texture bindings, model instances, and drawing functions. Coordinates transformations via matrix stacks based on the current state.
  *Example*: `Caine.cpp` uses `glPushMatrix()` and rotations to arrange and draw Caine's hat, jaws, eyes, and hands.
- **`NameAnimation.hpp` / `NameAnimation.cpp` (Logic)**:
  Tracks logical state variables, timers, interpolation factors, and health pools. Uses delta time (`deltaTime`) to advance timelines.
  *Example*: `CaineAnimation.cpp` updates `idleTimer` to calculate breathing sways, handles transition speeds for posture shifts, and ticks the death sequence.

---

## 3. Core Systems & Design Patterns

### DeltaTime Update Loop
To ensure gameplay runs at a consistent speed regardless of frame rate, all movement and animation updates are scaled by `deltaTime` (the fraction of a second since the last frame):

```cpp
float currentLeanPitch;
float targetLeanPitch;
const float LEAN_SPEED = 3.0f;

// Smooth interpolation independent of frame rate
currentLeanPitch += (targetLeanPitch - currentLeanPitch) * LEAN_SPEED * deltaTime;
```

### Posture & Ability State Machines
Animations and abilities are governed by discrete states. Mutual exclusion is enforced via guard clauses inside the skill casting functions.
- **Kinger States**: Idle, Casting Skill (Shooting), Rolling, Reloading, Healing, Hurt (Damage Flash), and Dead.
- **Caine States**: Idle, Shooting State, Laying Down (Key `8`), Leaning Forward (Key `9`), and Dead (Key `0`).
- **Gloinks States**: Bouncing (Alive), Hurt, and Dead (Minecraft-style fall).

### The "Matrix Sandwich" (Pivot Point Rotation)
To rotate a model part around a specific offset pivot (like Caine's jaw or head) rather than the global origin `(0,0,0)`, the code uses a standard translation sandwich:
1. Push matrix: `glPushMatrix()`
2. Translate to pivot point offset: `glTranslatef(pivotX, pivotY, pivotZ)`
3. Apply rotations: `glRotatef(...)`
4. Translate back by the negative offset: `glTranslatef(-pivotX, -pivotY, -pivotZ)`
5. Draw the local geometry
6. Pop matrix: `glPopMatrix()`

#### Code Snippet: Caine's Jaw Opening
```cpp
// 1. Move to jaw attachment joint, combining hover and body offsets
glTranslatef(0.0f, animation.hoverOffset + caineBodyY, 0.0f);

// 2. Translate to jaw pivot joint center (Matrix Sandwich translation)
glTranslatef(0.0f, -6.0f, -2.0f);

// 3. Apply rotation along pitch axis (X axis) to open the mouth
glRotatef(-caineJawAngle, 1.0f, 0.0f, 0.0f);

// 4. Translate back (Matrix Sandwich inverse translation)
glTranslatef(0.0f, 6.0f, 2.0f);

// 5. Draw the lower jaw model
drawLowerJaw();
```

#### Code Snippet: Caine's Head Tilting (Death Animation)
During Caine's death animation, his head tilts to the left at a pivot located in the center of the head `(0.0f, 3.0f, -20.0f)`:
```cpp
// 1. Position relative to body base
glTranslatef(0.0f, animation.hoverOffset + caineBodyY, 0.0f);

// 2. Translate to neck/head pivot point
glTranslatef(0.0f, 3.0f, -20.0f);

// 3. Rotate head (left tilt on roll axis) during death sequence
if (animation.isDead)
{
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f); // Tilt 45 degrees left
}

// 4. Translate back
glTranslatef(0.0f, -3.0f, 20.0f);

// 5. Draw Head components
drawHeadParts();
```

---

## 4. Key Variables & Global States

Incoming developers must respect the following bounds and values during integration:

### Hotkeys and Controls
- **Mouse Click / LMB Hold**: Fire gun skill.
- **`W`/`A`/`S`/`D`**: Move Kinger relative to camera horizontal direction.
- **`SPACE`**: Jump.
- **`R`**: Reload ammo capacity.
- **`C`**: Execute dodge roll (gain speed, invulnerability, and turn into checker ball).
- **`F`**: Summon butterfly to heal +30 HP (locks movement for 2.0 seconds).
- **`8`**: Toggle Caine body Laying Down state.
- **`9`**: Toggle Caine body Leaning Forward (45 degrees) state.
- **`0`**: Trigger Caine Death state (snaps to default position, tilts head left, opens mouth, and disappears).

### Arena Environment Constants
- **Ground Level**: `posY = -18.7f` (Kinger's base height).
- **Arena Boundaries**: Cylindrical boundary centered around the origin `(0, 0)` with radius of `400.0` units. Keep characters clamped within this circle.
- **Caine Default Spawn Point**: `(0.0f, 0.0f, -120.0f)`.

### Follow Camera Properties
- **Follow Distance**: 60.0 units behind Kinger.
- **Right-Shoulder Offset**: 18.0 units right.
- **Vertical Base Height**: 30.0 units above Kinger's chest height.

---

## 5. Current Status & Next Steps

### 100% Complete & Stable
*   **Aesthetics**: Beautifully lit circus arena, floating geometric elements, digital background glitch effects, and particle indicators.
*   **Kinger Controls**: Normalised horizontal movement, gravity/jump physics, dodge roll model swap, auto-fire shoot with Z-recoil and parallax tracers, reload cycle, and butterfly heal path.
*   **Gloink Animations**: Jumping squashes, Z-axis roll-spins, and Minecraft-style death falls.
*   **Caine Animations**: Breathing hover, jaw flapping, laying down, leaning forward, and death tilting/jaw-opening sequences.

### TODO Roadmap (For Incoming Teammates)
The following tasks are pending to turn this animation model viewer into a complete game:

- [ ] **Spawning Waves Manager**:
  Create an automated spawn cycle in `GloinksAnimation` or a new spawner manager. It should spawn Gloinks at random points within the arena boundaries on a timer, up to a maximum concurrent count.
- [ ] **Collision Detection**:
  - **Bullet vs. Gloinks**: Add bounding box or radius checks in `KingerAnimation::updateSkillState` against all active Gloinks. Trigger `GloinksAnimation::hurtGloink(index)` on contact.
  - **Bullet vs. Caine**: Check collision against Caine's central position. Trigger damage on Caine.
  - **Gloinks vs. Kinger**: Add proximity damage. If an active Gloink hits Kinger, reduce Kinger's health pool (`Kinger::takeDamage`).
- [ ] **Player HUD & Boss Health UI**:
  Render graphical indicators or health bars. Switch to 2D orthographic projection in `CNAmain.cpp` and draw:
  - Kinger's current health pool (HP bar) and remaining ammunition capacity.
  - Caine's boss health bar at the top of the screen when he is active.
- [ ] **Game State Manager**:
  Implement state flows:
  - **Start Menu**: Interactive title screen.
  - **Active Battle**: Spawns Gloinks and enables Caine's AI attack patterns.
  - **Win Screen**: Triggered when Caine's health drops to 0.
  - **Lose Screen**: Triggered when Kinger's health drops to 0.
- [ ] **Audio Integration**:
  Uncomment or complete the `AudioManager` sound triggers (using WinMM API) to play background music and shoot/reload sound effects.
