# State of Code

This document is a quick guide to the structure of the project, the role of each source file, and the flow of the application.

**Last updated:** 2026-06-15

---

## 1. Project Overview

This is a C++ OpenGL/GLUT computer graphics project that renders a 3D scene of a circus/battle-arena world. Models are loaded from plain-text OBJ-style files stored in the `Model/` folder. The project features multiple fully-implemented characters, an animated environment, texture mapping, local lighting, background music, and a real-time animation state machine for the main character.

**The scene currently renders all of the following simultaneously:**
- Kinger (main character — fully animated)
- Gloinks (scene props — 6 pieces)
- Caine (supporting character — fully modelled and textured)
- Butterfly (scene prop — 3 pieces, alpha-blended wings)
- KingerRoll (additional Kinger variant model)
- Environment (arena — skybox, ground, roof, walls, pillars, spheres, cubes, and a digital glitch effect)

---

## 2. File Map

### Entry and Runtime Control

#### `CNAmain.cpp`
- Main program entry point (`main()`).
- Initializes GLUT, creates the window, sets OpenGL state, and starts `glutMainLoop()`.
- Contains user input callbacks:
  - **`a/d/w/s/q/e`** — move world on X/Y/Z axis
  - **Arrow keys** — rotate world X/Y
  - **`SPACE`** — triggers `kinger.animation.castGunSkill()` (Kinger's gunshot skill)
  - **`HOME`** — restores all defaults via `myDataInit()`
  - **`ESC`** — exits the application
  - **`F1`** — toggle fill / wireframe mode
  - **`F2`** — toggle world axis rendering
  - **`F3`** — toggle OpenGL lighting on/off
  - **Mouse left drag** — rotate world X/Y
  - **Mouse right drag** — rotate world around Z
- Calls `myInit()`, `myViewingInit()`, `myLightingInit()`, and `myvirtualworld.init()`.
- Prints a welcome/help message to the console on startup.

#### `CNAmain.hpp`
- Header for the runtime system.
- Declares callback functions and utility structs:
  - `MyWindow` — window title, position, and size
  - `MyWorld` — world-level position, rotation, and scale transforms
  - `MyViewer` — camera eye, center, up vectors, FOV, near/far planes
  - `MySetting` — keyboard/mouse state, shading mode, increment values
  - `MyAxis` — world axis draw toggle helper

---

### World Composition

#### `CNAWorld.hpp`
- Declares the top-level world controller class `MyVirtualWorld` inside `namespace ProjectWorld`.
- Members:
  - `ProjectKinger::Kinger kinger`
  - `ProjectGloinks::Gloinks gloinks`
  - `ProjectCaine::Caine caine`
  - `ProjectButterfly::Butterfly butterfly`
  - `ProjectKingerRoll::KingerRoll kingerRoll`
  - `ProjectEnvironment::Environment environment`
  - `AudioManager audioManager`
- Exposes: `init()`, `draw()`, `tickTime()`

#### `CNAWorld.cpp`
- Implements `MyVirtualWorld::init()`, `draw()`, and `tickTime()`.
- **`init()`** loads all model `.txt` files and textures for every character and environment object. Also calls `setupEnvironmentLighting()` and `audioManager.playBackgroundMusic(...)` at the end.
- **`draw()`** renders the whole scene in this order:
  1. `environment.draw()`
  2. `kinger.draw()`
  3. `gloinks.draw()`
  4. `caine.draw()`
  5. `butterfly.draw()`
  6. `kingerRoll.draw()`
- **`tickTime()`** updates per-frame animations:
  - `environment.tickTime()` — updates environment animation clock
  - `kinger.update(0.016f)` — ticks Kinger's idle and skill animations
- Defines `setupEnvironmentLighting()` — a scene-wide global light setup using GL_LIGHT0 with a warm-dim neutral directional light from upper-left.

---

### Character Systems

#### Kinger (Main Character) — `Kinger.hpp` / `Kinger.cpp`

**Status: Fully implemented with real-time animation.**

**10 model parts:**
| Part | Texture |
|---|---|
| Head | WoodTexture.jpg |
| HeadPiece | WoodTexture.jpg |
| LeftEye | LeftEye.png |
| RightEyeN | RightEyeN.png |
| Body | (flat skin color) |
| Cloth | Kinger_Gown.png |
| LeftHand | (flat white) |
| RightHandwGun | RightHandwGun.png |
| Bucket | (flat white) |
| BucketHandle | (no texture) |

**Extra draw element:**
- `drawBullet()` — renders a `glutSolidSphere` projectile at runtime when the gun skill is active.

**Animation:** Kinger holds a public `KingerAnimation animation` member. Every draw call reads animation state offsets (hover, arm rotation, cloth rotation, body Y/Z offsets, bullet position) from it.

**`update(float deltaTime)`** calls both `animation.updateIdleState()` and `animation.updateSkillState()`.

---

#### KingerAnimation — `KingerAnimation.hpp` / `KingerAnimation.cpp`

**Status: Fully implemented state machine.**

**Idle animation (always running):**
- `hoverOffset` — smooth Y-axis body oscillation using `sin(idleTimer * 2.0)`
- `clothRotation` — Z-axis cloth flutter using `cos(idleTimer * 2.5)`
- `armRotation` — breathing arm sway using `sin(idleTimer * 1.8)`

**Gun skill animation (triggered by SPACE):**

`castGunSkill()` starts the sequence. `updateSkillState()` drives it through 4 phases:

| Phase | Duration | Action |
|---|---|---|
| 1 — Wind-up | 0.0s – 0.5s | Arm rotates back –45°, body dips |
| 2 — Execution | 0.5s – 0.7s | Arm snaps forward to +90°, bullet spawns |
| 3 — Recoil/Recovery | 0.7s – 1.2s | Arm returns to 0°, body jolts backward then recovers |
| 4 — Reset | > 1.2s | All skill variables reset |

**Bullet projectile:**
- Constants: `BULLET_TRAVEL_SPEED = 80.0f`, `BULLET_SPAWN_Z = -10.0f`, `BULLET_MAX_DISTANCE = -150.0f`
- Travels along the Z axis each tick while `isBulletActive` is true.
- Auto-despawns when it exceeds `BULLET_MAX_DISTANCE`.

---

#### Caine (Supporting Character) — `Caine.hpp` / `Caine.cpp`

**Status: Fully implemented. Loaded and drawn. No animation yet.**

**14 model parts (all textured):**
- Hat, UpperJaw, LowerJaw, Tongue, Turso (torso)
- LeftHand, RightHand, LeftPalm, RightPalm
- LeftLeg, RightLeg
- Staff
- LeftEye, RightEye

All parts placed at `glTranslatef(0, -19.5, 20)` with `glRotatef(180, 0, 1, 0)` and `glScalef(3, 3, 3)`. The LowerJaw uses a slightly different transform with a –20° X-axis rotation to open the mouth.

No animation state machine exists yet for Caine.

---

#### Butterfly — `Butterfly.hpp` / `Butterfly.cpp`

**Status: Fully implemented. Loaded and drawn. No animation yet.**

**3 model parts (all textured):**
- LeftWing, RightWing — rendered with `GL_BLEND` (alpha transparency)
- Turso

All three parts share the same transform: `glTranslatef(30, 0, 0)`, `glRotatef(90, 0, 1, 0)`, `glScalef(3, 3, 3)`.

No wing-flap animation yet.

---

#### KingerRoll — `KingerRoll.hpp` / `KingerRoll.cpp`

**Status: Fully implemented. Single-part model. No animation.**

**1 model part:**
- Roll — textured with `Kinger_Roll.png`, placed at `glTranslatef(-30, -19.3, 0)` at 5× scale facing backwards (180° Y).

---

#### Gloinks — `Gloinks.hpp` / `Gloinks.cpp`

**Status: Fully implemented. All 6 parts loaded, textured, and drawn. No animation.**

**6 model parts (all textured):**
- BowlingPin, Circle, Cube, Moon, Star, Triangular

---

### Environment — `Environment.hpp` / `Environment.cpp`

**Status: Fully implemented with animations, local lighting, and a glitch effect.**

**9 mesh models:**
| Model | Notes |
|---|---|
| SkyBox | Scaled 15× from center, always drawn first, lighting disabled |
| Ground | Scaled 15×, placed at Y = –18.7 |
| Roof | Rotates continuously (time × 8°) on Y axis, placed at Y = +18.7 |
| CastleWall | Two instances: back-left and front-left |
| Cube | 4 animated instances — linear movement, circular orbit, slower drift |
| CubeGrouped | Static, placed right-back at (220, –18.7, 150) |
| IrregularCube | 2 instances — circus props with `CircusObject1` and `CircusObject2` textures |
| Pillar | 2 instances — front-left and back-right |
| Sphere | 5 floating instances, each with phase-offset Y oscillation and Y rotation |

**12 textures loaded by `loadTextures()`:**
CircusStrip.jpg, FloorStrip.png, Circus.jpg, CastleWall.jpg, Cube1.png, Cube2.png, CubeGrouped.png, CircusObject1.png, CircusObject2.png, Pillar.png, Sky.jpg, GlitchEffect.png

**`tickTime()`:** Advances `animationTime` using a true delta-time clock via `glutGet(GLUT_ELAPSED_TIME)`. Clamps delta to 0.1s. Resets to 0 when it exceeds 1000.

**`drawDigitalEffect()`:** Draws a screen-space glitch overlay:
- 9 textured floating panels with additive blending, UV shift, and flicker
- 18 thin cyan scan lines using `GL_LINES` with animated blink and horizontal drift
- Uses `glDepthMask(GL_FALSE)` so it does not write to the depth buffer

**Local Lighting** — `draw()` wraps each section in paired `enableLocalEnvironmentLight()` / `disableLocalEnvironmentLight()` calls using `GL_LIGHT1`:

| Section | Light Color | Notes |
|---|---|---|
| Ground + Roof | Dim white-blue | Overhead stage light |
| CastleWall | Blue | Cold side light |
| Cubes + CubeGrouped | Red | Circus/battle light |
| IrregularCube | Purple/Magenta | Abstract props |
| Pillars | Warm beige | Soft spotlight |
| Spheres | Blue | Sky glow |

---

### Audio — `AudioManager.hpp` / `AudioManager.cpp`

**Status: Implemented. Plays one looping WAV file.**

- Uses `Windows.h` + `mmsystem.h` → `PlaySound()` with `SND_ASYNC | SND_LOOP`.
- `playBackgroundMusic(filePath)` — plays the file; guards against double-play with `bgmPlaying` flag.
- `stopBackgroundMusic()` — calls `PlaySound(NULL, NULL, 0)` to stop.
- Current BGM: `Audio\BGM\[Jigoku Shoujo OST] Ake ni Somaru - The Faustian (128k).wav`

---

### Generic Model Loader — `ObjModel.hpp` / `ObjModel.cpp`

**Status: Fully implemented and shared by all character/environment objects.**

- Parses plain-text OBJ-style files:
  - `v` — vertex
  - `vn` — normal
  - `vt` — texture coordinate
  - `f` — triangle face (index references)
- Stores parsed data in vectors and draws using OpenGL triangle primitives.
- Used by every character and environment model in the project.

---

### Texture Loader — `TextureLoader.hpp` / `TextureLoader.cpp`

**Status: Fully implemented.**

- Uses `stb_image.h` to decode PNG/JPG images.
- Returns a `GLuint` texture ID ready for `glBindTexture()`.

---

## 3. Asset Folders

### `Model/Kinger/`
10 mesh `.txt` files + `Textures/` subfolder (WoodTexture.jpg, Kinger_Gown.png, RightHandwGun.png, LeftEye.png, RightEyeN.png)

### `Model/Gloinks/`
6 mesh `.txt` files + `Textures/` subfolder (6 PNGs)

### `Model/Caine/`
14 mesh `.txt` files + `Textures/` subfolder (14 PNGs)

### `Model/Butterfly/`
3 mesh `.txt` files + `Textures/` subfolder (Butterfly_LeftWing.png, Butterfly_RightWing.png, Butterfly_Turso.png)

### `Model/Kinger_Roll/`
1 mesh `.txt` file + `Textures/` subfolder (Kinger_Roll.png)

### `Model/Environment/`
9 mesh `.txt` files + `Textures/` subfolder (12 images)

### `Audio/BGM/`
1 WAV file for the background music loop.

---

## 4. Runtime Flow

```
main()
  └── myWelcome()           — print console help
  └── myInit()
        ├── myDataInit()    — set all default values
        ├── GLUT window/callbacks setup
        ├── OpenGL state init (depth test, culling, etc.)
        ├── myViewingInit() — perspective camera
        ├── myLightingInit()— global GL_LIGHT0 setup
        └── myvirtualworld.init()
              ├── Load all model .txt files (Kinger, Gloinks, Caine, Butterfly, KingerRoll, Environment)
              ├── Load all textures
              ├── setupEnvironmentLighting()
              └── audioManager.playBackgroundMusic()

GLUT main loop ──► myDisplayFunc()
  ├── glClear
  ├── Apply world transforms (translate, rotate, scale)
  ├── myvirtualworld.draw()
  │     ├── environment.draw()   (skybox → ground → roof → walls → cubes → pillars → spheres → glitch)
  │     ├── kinger.draw()        (10 parts + bullet)
  │     ├── gloinks.draw()       (6 parts)
  │     ├── caine.draw()         (14 parts)
  │     ├── butterfly.draw()     (3 parts)
  │     └── kingerRoll.draw()    (1 part)
  ├── glutSwapBuffers
  └── myvirtualworld.tickTime()
        ├── Real delta-time clock computed via glutGet(GLUT_ELAPSED_TIME)
        ├── kinger.update(deltaTime) — tick idle + skill animations
        └── environment.tickTime()  — advance animationTime
```

---

## 5. Known Issues / Things to Note

- ~~`kinger.update()` used a hardcoded `0.016f` delta~~ — **FIXED:** `tickTime()` now computes a real delta time using `glutGet(GLUT_ELAPSED_TIME)`, clamped to 0.1s, matching `environment.tickTime()`.
- ~~Bullet despawn inversion bug in `KingerAnimation::updateSkillState()`~~ — **FIXED:** Changed `bulletZOffset += ...` to `bulletZOffset -= ...` so the offset grows more negative each frame and correctly crosses `BULLET_MAX_DISTANCE = -150.0f`.
- Caine, Butterfly, and KingerRoll have no animation state machine yet — they are rendered statically.
- Caine and the other characters share the same transform origin. They are visible as separate characters because of their individual `glTranslatef` values; there is no dedicated scene-graph positioning yet.

---

## 6. Suggested Reading Order

To understand the project quickly, read these files in this order:

1. `CNAmain.cpp` — understand the GLUT loop and input system
2. `CNAWorld.hpp` — see all scene objects at a glance
3. `CNAWorld.cpp` — see how everything is loaded and drawn
4. `ObjModel.cpp` — understand the model file parser
5. `KingerAnimation.hpp` / `KingerAnimation.cpp` — understand the animation system
6. `Kinger.cpp` — see how animation offsets drive rendering
7. `Environment.cpp` — see local lighting and animated environment objects
8. `Caine.cpp` / `Butterfly.cpp` / `KingerRoll.cpp` — simpler static characters
9. `AudioManager.cpp` — audio system
10. `TextureLoader.cpp` — texture pipeline
