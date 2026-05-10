// Name - Shehani Kaveenya Mukadange
// ULan ID - G21364691
// UCL ID - 3001085
//
// Required text files in the media folder:
//   media/scene.txt      - checkpoints, walls, isles, tanks, barrels, garages,
//   media/waypoints.txt  - NPC racing route
//
// Controls:
//   W/S       - accelerate / reverse
//   A/D       - steer
//   Space     - start race / boost during race
//   1         - chase camera
//   2         - first-person camera
//   R         - restart race

#include <TL-Engine.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace tle;
using namespace std;

// ====================================================
// Game constants and tuning values
// ====================================================

//General Math Constants
const float kPi = 3.14159265f;
const float kDegToRad = kPi / 180.0f;
const float kRadToDeg = 180.0f / kPi;

//Game Rules & Limits
const int   kTotalLaps = 3;
const int   kMaxHealth = 100;
const int   kBoostDisabledHealth = 20;

//Car Movement & Physics
const float kForwardThrust = 70.0f;
const float kReverseThrust = 25.0f; 
const float kTurnSpeed = 75.0f;
const float kDrag = 1.3f;
const float kMaxSpeed = 60.0f;
const float kMetresPerUnit = 0.25f;

//Boost System
const float kBoostMultiplier = 1.4f;
const float kBoostMaxTime = 3.0f;
const float kBoostCooldownTime = 5.0f;
const float kOverheatDragMultiplier = 2.0f;

//Hover & Car Visual Effects
const float kHoverBaseY = 5.0f;
const float kHoverAmplitude = 0.35f;
const float kHoverFrequency = 4.2f;
const float kLeanAmount = 12.0f;
const float kPitchAmount = 6.0f;
const float kCollisionBounceHeight = 0.9f;
const float kCollisionBounceDecay = 4.5f;

//Collision Settings
const float kCarRadius = 1.35f;
const float kNpcRadius = 1.15f;
const float kTankRadius = 2.2f;
const float kCheckpointPostRadius = 2.0f;
const float kWallHalfSize = 4.6;
const float kWallBounce = 0.45f;
const float kTankBounce = 0.35f;
const float kNpcBounce = 0.45f;
const float kDamageCooldownTime = 0.35f;

//Checkpoint System
const float kCheckpointHalfWidth = 7.0f;
const float kCheckpointHalfDepth = 11.0f;
const float kCheckpointPostOffset = 8.5f;
const float kCrossLifeTime = 2.5f;
const float kCountdownStep = 1.0f;

//NPC Behaviour
const float kNpcSpeed = 20.0f;
const float kNpcTurnSpeed = 170.0f;
const float kNpcWaypointReach = 7.0f;

//Camera Controls
const float kCameraMoveSpeed = 28.0f;
const float kCameraTurnSpeed = 0.08f;
const float kCameraPitchMin = -15.0f;
const float kCameraPitchMax = 30.0f;

//NPC Obstacle Avoidance
const float kNpcObstacleOriginalX = 0.0f;
const float kNpcObstacleOriginalZ = 30.0f;
const float kNpcObstacleCheckXRange = 0.5f;
const float kNpcObstacleCheckZRange = 3.0f;
const float kNpcObstacleAvoidX = -7.0f;
const float kNpcObstacleAvoidZ = 28.0f;

//First-Person Camera Settings
const float kFirstPersonForwardOffset = 2.5f;
const float kFirstPersonCameraHeight = 3.5f;

//Exhaust Particle Settings
const float kExhaustRearOffset = 2.2f;
const float kExhaustVelocityBlend = 0.3f;
const float kExhaustMinYSpeed = 0.5f;
const float kExhaustMaxYSpeed = 2.0f;

//UI & Display Settings
const int kUIBackdropHeightOffset = 125;

//Bomb & Explosion System
const float kBombModelY = 0.5f;
const float kBombScale = 0.4f;
const float kBombRadius = 8.0f;
const float kBombDamage = 15;
const float kCameraShakeTime = 0.6f;
const float kCameraShakeAmount = 0.8f;
const float kExplosionEmitY = 1.0f;

//Hidden Positions
const float kHiddenY = -500.0f;
const float kCrossHiddenY = -100.0f;

//Barrel Fire System
const float kBarrelFireEmitInterval = 0.035f;
const float kBarrelFireEmitY = 1.2f;

//Smoke System
const float kSmokeBaseInterval = 0.15f;
const float kSmokeHealthIntervalScale = 0.2f;
const float kSmokeEmitHeightOffset = 1.5f;
const int   kHealthSmokeThreshold = 40;
const float kSmokeUpwardAcceleration = 1.8f;

//NPC Recovery & Stuck Handling
const float kNpcRecoverySpeedMultiplier = 0.35f;
const float kNpcStuckMoveThreshold = 0.05f;
const float kNpcStuckVelocityThreshold = 1.0f;
const float kNpcStuckTimeLimit = 0.8f;
const float kNpcTankAvoidRadius = 1.6f;
const float kNpcCollisionBounce = 0.25f;

// ====================================================
// Particle System (Fire, Exhaust, Explosion, Smoke)
// ====================================================
const int   kFireParticleCount = 36; 
const float kFireSpeedMin = 7.0f;  
const float kFireSpeedMax = 13.0f; 
const float kFireGravity = -2.2f;  
const float kFireLifeMin = 0.8f;  
const float kFireLifeMax = 1.5f;  
const float kFireSpread = 0.8f;   

const int   kExhaustParticleCount = 25;
const float kExhaustSpeed = 6.0f;
const float kExhaustLife = 0.35f;
const float kExhaustSpread = 0.9f;

const int   kBombParticleCount = 100;
const int   kBigExplosionSmokeCount = 18;
const float kBombSpeed = 12.0f;
const float kBombGravity = -9.0f;
const float kBombLife = 0.7f;

const int   kSmokeParticleCount = 12;
const float kSmokeSpeedMin = 1.0f;
const float kSmokeSpeedMax = 2.5f;
const float kSmokeLifeMin = 0.8f;
const float kSmokeLifeMax = 1.6f;
const float kSmokeSpread = 0.6f;


// ====================================================
// Data structures and enums
// ====================================================
struct Vec2 { float x, z; };

struct SceneItem
{
    string type;
    float x, z, rotY;
};

struct CheckpointData
{
    IModel* model;
    float x, z, rotY;
};

struct CrossData
{
    IModel* model;
    float timer;
};

struct BoxObject
{
    IModel* model;
    float minX, maxX, minZ, maxZ;
};


struct Particle
{
    IModel* model;
    float   velX, velY, velZ;   
    float   life;               
    bool    active;
};

struct BombData
{
    IModel* model;
    float   x, z;
    bool    exploded;
};

enum EGameState
{
    WaitingToStart,
    Countdown3,
    Countdown2,
    Countdown1,
    GoText,
    Racing,
    RaceComplete,
    GameOver
};

enum ECameraMode
{
    ChaseCam,
    FirstPersonCam
};

// ====================================================
// Vector, maths, and formatting helpers
// ====================================================
Vec2 AddVec(Vec2 a, Vec2 b) { return { a.x + b.x, a.z + b.z }; }
Vec2 SubVec(Vec2 a, Vec2 b) { return { a.x - b.x, a.z - b.z }; }
Vec2 MulVec(Vec2 v, float s) { return { v.x * s, v.z * s }; }

float LengthVec(Vec2 v)
{
    return sqrt(v.x * v.x + v.z * v.z);
}

Vec2 NormaliseVec(Vec2 v)
{
    float len = LengthVec(v);
    if (len <= 0.0001f) return { 0.0f, 0.0f };
    return { v.x / len, v.z / len };
}

float ClampFloat(float value, float minValue, float maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

float WrapAngle(float angle)
{
    while (angle < 0.0f) angle += 360.0f;
    while (angle >= 360.0f) angle -= 360.0f;
    return angle;
}

float Distance2D(float x1, float z1, float x2, float z2)
{
    float dx = x1 - x2;
    float dz = z1 - z2;
    return sqrt(dx * dx + dz * dz);
}

string ToStringInt(int value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

string FormatTime(float seconds)
{
    int mins = (int)(seconds / 60.0f);
    float secs = seconds - mins * 60.0f;
    stringstream ss;
    ss << mins << ":" << fixed << setprecision(2) << setw(5) << setfill('0') << secs;
    return ss.str();
}

float GetSpeedKmh(Vec2 velocity)
{
    return LengthVec(velocity) * kMetresPerUnit * 3.6f;
}

// ====================================================
// Scene and waypoint file loading
// ====================================================
bool LoadSceneFile(const string& fileName, vector<SceneItem>& sceneItems)
{
    ifstream file(fileName.c_str());
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line))
    {
        if (line == "") continue;
        if (line[0] == '#') continue;

        stringstream ss(line);
        SceneItem item;
        ss >> item.type >> item.x >> item.z >> item.rotY;

        if (!ss.fail()) sceneItems.push_back(item);
    }

    file.close();
    return true;
}

bool LoadWaypointsFile(const string& fileName, vector<Vec2>& waypoints)
{
    ifstream file(fileName.c_str());
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line))
    {
        if (line == "") continue;
        if (line[0] == '#') continue;

        stringstream ss(line);
        Vec2 point;
        ss >> point.x >> point.z;

        if (!ss.fail()) waypoints.push_back(point);
    }

    file.close();
    return true;
}

BoxObject MakeBox(IModel* model, float x, float z)
{
    return { model, x - kWallHalfSize, x + kWallHalfSize, z - kWallHalfSize, z + kWallHalfSize };
}

// ====================================================
// Collision detection and response helpers
// ====================================================
bool SphereToSphere(float x1, float z1, float r1, float x2, float z2, float r2)
{
    float dx = x1 - x2;
    float dz = z1 - z2;
    float r = r1 + r2;
    return dx * dx + dz * dz <= r * r;
}

bool PointInCheckpointGap(float pointX, float pointZ, const CheckpointData& checkpoint)
{
    float localX = pointX - checkpoint.x;
    float localZ = pointZ - checkpoint.z;

    if (fabs(checkpoint.rotY - 90.0f) < 1.0f || fabs(checkpoint.rotY - 270.0f) < 1.0f)
    {
        return fabs(localX) < kCheckpointHalfDepth && fabs(localZ) < kCheckpointHalfWidth;
    }

    return fabs(localX) < kCheckpointHalfWidth && fabs(localZ) < kCheckpointHalfDepth;
}

void GetCheckpointPosts(const CheckpointData& checkpoint, float& leftX, float& leftZ, float& rightX, float& rightZ)
{
    leftX = checkpoint.x;
    leftZ = checkpoint.z;
    rightX = checkpoint.x;
    rightZ = checkpoint.z;

    if (fabs(checkpoint.rotY - 90.0f) < 1.0f || fabs(checkpoint.rotY - 270.0f) < 1.0f)
    {
        leftZ = checkpoint.z - kCheckpointPostOffset;
        rightZ = checkpoint.z + kCheckpointPostOffset;
    }
    else
    {
        leftX = checkpoint.x - kCheckpointPostOffset;
        rightX = checkpoint.x + kCheckpointPostOffset;
    }
}

bool ResolveSphereBox(IModel* car, Vec2& velocity, float radius, const BoxObject& box, float bounce)
{
    float x = car->GetX();
    float z = car->GetZ();

    float closestX = ClampFloat(x, box.minX, box.maxX);
    float closestZ = ClampFloat(z, box.minZ, box.maxZ);
    float dx = x - closestX;
    float dz = z - closestZ;

    if (dx * dx + dz * dz > radius * radius) return false;

    float overlapLeft = fabs((x + radius) - box.minX);
    float overlapRight = fabs(box.maxX - (x - radius));
    float overlapTop = fabs((z + radius) - box.minZ);
    float overlapBottom = fabs(box.maxZ - (z - radius));

    float smallest = overlapLeft;
    int side = 0;
    if (overlapRight < smallest) { smallest = overlapRight; side = 1; }
    if (overlapTop < smallest) { smallest = overlapTop; side = 2; }
    if (overlapBottom < smallest) { smallest = overlapBottom; side = 3; }

    if (side == 0)
    {
        car->SetX(box.minX - radius);
        velocity.x = -fabs(velocity.x) * bounce;
    }
    else if (side == 1)
    {
        car->SetX(box.maxX + radius);
        velocity.x = fabs(velocity.x) * bounce;
    }
    else if (side == 2)
    {
        car->SetZ(box.minZ - radius);
        velocity.z = -fabs(velocity.z) * bounce;
    }
    else
    {
        car->SetZ(box.maxZ + radius);
        velocity.z = fabs(velocity.z) * bounce;
    }

    return true;
}

bool ResolveSpherePoint(IModel* car, Vec2& velocity, float radius, float otherX, float otherZ, float otherRadius, float bounce)
{
    float x = car->GetX();
    float z = car->GetZ();

    float dx = x - otherX;
    float dz = z - otherZ;
    float dist = sqrt(dx * dx + dz * dz);
    float minDist = radius + otherRadius;

    if (dist >= minDist) return false;

    Vec2 normal;
    if (dist <= 0.001f)
    {
        normal = { 1.0f, 0.0f };
        dist = 0.001f;
    }
    else
    {
        normal = { dx / dist, dz / dist };
    }

    float penetration = minDist - dist;
    car->SetPosition(x + normal.x * penetration, car->GetY(), z + normal.z * penetration);

    float dot = velocity.x * normal.x + velocity.z * normal.z;
    if (dot < 0.0f)
    {
        velocity.x = (velocity.x - 2.0f * dot * normal.x) * bounce;
        velocity.z = (velocity.z - 2.0f * dot * normal.z) * bounce;
    }
    else
    {
        velocity = MulVec(velocity, -bounce);
    }

    return true;
}

void DamagePlayer(int& health, float& damageCooldown)
{
    if (damageCooldown <= 0.0f)
    {
        health--;
        if (health < 0) health = 0;
        damageCooldown = kDamageCooldownTime;
    }
}

// ====================================================
// Car visuals, checkpoint crosses, and cameras
// ====================================================
void ApplyCarVisuals(IModel* car, float rotY, float lean, float pitch)
{
    float x = car->GetX();
    float y = car->GetY();
    float z = car->GetZ();

    car->ResetOrientation();
    car->SetPosition(x, y, z);
    car->RotateY(rotY);
    car->RotateLocalZ(lean);
    car->RotateLocalX(pitch);
}

void ShowCross(CrossData& cross, float x, float z)
{
    cross.model->SetPosition(x, 0.2f, z);
    cross.timer = kCrossLifeTime;
}

void UpdateCrosses(vector<CrossData>& crosses, float frameTime)
{
    for (int i = 0; i < (int)crosses.size(); ++i)
    {
        if (crosses[i].timer > 0.0f)
        {
            crosses[i].timer -= frameTime;
            if (crosses[i].timer <= 0.0f)
            {
                crosses[i].timer = 0.0f;
                crosses[i].model->SetPosition(crosses[i].model->GetX(), kCrossHiddenY, crosses[i].model->GetZ());
            }
        }
    }
}

void ResetChaseCamera(float& camOffsetX, float& camOffsetY, float& camOffsetZ, float& cameraYaw, float& cameraPitch)
{
    camOffsetX = 0.0f;
    camOffsetY = 8.0f;
    camOffsetZ = -14.0f;
    cameraYaw = 0.0f;
    cameraPitch = 15.0f;
}

void UpdateFirstPersonCamera(ICamera* camera, IModel* car, float carRotY)
{
    float angle = carRotY * kDegToRad;
    camera->SetPosition(
        car->GetX() + sin(angle) * kFirstPersonForwardOffset,
        car->GetY() + kFirstPersonCameraHeight,
        car->GetZ() + cos(angle) * kFirstPersonForwardOffset
    );
    camera->ResetOrientation();
    camera->RotateY(carRotY);
}

void UpdateChaseCamera(I3DEngine* engine, ICamera* camera, IModel* car, float carRotY,
    float& camOffsetX, float& camOffsetY, float& camOffsetZ,
    float& cameraYaw, float& cameraPitch, float frameTime)
{
    if (engine->KeyHeld(Key_Up))    camOffsetZ += kCameraMoveSpeed * frameTime;
    if (engine->KeyHeld(Key_Down))  camOffsetZ -= kCameraMoveSpeed * frameTime;
    if (engine->KeyHeld(Key_Left))  camOffsetX -= kCameraMoveSpeed * frameTime;
    if (engine->KeyHeld(Key_Right)) camOffsetX += kCameraMoveSpeed * frameTime;

    cameraYaw += engine->GetMouseMovementX() * kCameraTurnSpeed;
    cameraPitch -= engine->GetMouseMovementY() * kCameraTurnSpeed;
    cameraPitch = ClampFloat(cameraPitch, kCameraPitchMin, kCameraPitchMax);

    float totalYaw = carRotY + cameraYaw;
    float angle = totalYaw * kDegToRad;

    float behindX = sin(angle) * camOffsetZ;
    float behindZ = cos(angle) * camOffsetZ;
    float sideX = cos(angle) * camOffsetX;
    float sideZ = -sin(angle) * camOffsetX;

    camera->SetPosition(car->GetX() + behindX + sideX, car->GetY() + camOffsetY, car->GetZ() + behindZ + sideZ);
    camera->ResetOrientation();
    camera->RotateY(totalYaw);
    camera->RotateLocalX(cameraPitch);
}

// ====================================================
// NPC waypoint movement and steering
// ====================================================
void UpdateNPC(IModel* npcCar, float& npcRotY, Vec2& npcVelocity, const vector<Vec2>& waypoints, int& npcWaypoint, float frameTime)
{
    if (waypoints.empty()) return;

    if (npcWaypoint < 0 || npcWaypoint >= (int)waypoints.size())
    {
        npcWaypoint = 0;
    }

    Vec2 npcPos = { npcCar->GetX(), npcCar->GetZ() };

    Vec2 target = waypoints[npcWaypoint];

    if (fabs(target.x - kNpcObstacleOriginalX) < kNpcObstacleCheckXRange &&
        fabs(target.z - kNpcObstacleOriginalZ) < kNpcObstacleCheckZRange)
    {
        target.x = kNpcObstacleAvoidX;
        target.z = kNpcObstacleAvoidZ;
    }

    float distanceToWaypoint = Distance2D(npcPos.x, npcPos.z, target.x, target.z);
    if (distanceToWaypoint < kNpcWaypointReach)
    {
        npcWaypoint++;
        if (npcWaypoint >= (int)waypoints.size()) npcWaypoint = 0;

        target = waypoints[npcWaypoint];
        if (fabs(target.x - kNpcObstacleOriginalX) < kNpcObstacleCheckXRange &&
            fabs(target.z - kNpcObstacleOriginalZ) < kNpcObstacleCheckZRange)
        {
            target.x = kNpcObstacleAvoidX;
            target.z = kNpcObstacleAvoidZ;
        }
    }

    Vec2 toTarget = SubVec(target, npcPos);
    Vec2 direction = NormaliseVec(toTarget);

    if (LengthVec(direction) <= 0.0001f) return;

    float wantedAngle = atan2(direction.x, direction.z) * kRadToDeg;
    float angleDifference = wantedAngle - npcRotY;

    while (angleDifference > 180.0f) angleDifference -= 360.0f;
    while (angleDifference < -180.0f) angleDifference += 360.0f;

    float maxTurn = kNpcTurnSpeed * frameTime;
    angleDifference = ClampFloat(angleDifference, -maxTurn, maxTurn);
    npcRotY = WrapAngle(npcRotY + angleDifference);

    float turnAmount = 0.0f;
    if (maxTurn > 0.0001f)
    {
        turnAmount = fabs(angleDifference) / maxTurn;
    }
    float speedFactor = 1.0f - ClampFloat(turnAmount * 0.25f, 0.0f, 0.25f);

    float angle = npcRotY * kDegToRad;
    Vec2 forward = { sin(angle), cos(angle) };
    npcVelocity = MulVec(forward, kNpcSpeed * speedFactor);

    npcCar->SetPosition(npcCar->GetX() + npcVelocity.x * frameTime, npcCar->GetY(), npcCar->GetZ() + npcVelocity.z * frameTime);
    ApplyCarVisuals(npcCar, npcRotY, 0.0f, 0.0f);
}

// ====================================================
// Race reset, progress scoring, and UI drawing
// ====================================================
void ResetRace(IModel* car, IModel* npcCar, vector<CrossData>& crosses,
    float& carRotY, float& npcRotY, Vec2& velocity, Vec2& npcVelocity, int& npcWaypoint,
    EGameState& gameState, string& stageMsg, string& winnerText,
    float& playerTime, float& npcTime, int& health, float& damageCooldown,
    int& nextCheckpoint, int& npcNextCheckpoint, int& playerLap, int& npcLap,
    bool& boostReady, bool& isBoosting, bool& boostOverheated,
    float& boostTimer, float& boostCooldownTimer, float& pitchAngle, float& collisionBounce)
{
    car->SetPosition(0.0f, kHoverBaseY, -15.0f);
    npcCar->SetPosition(6.0f, kHoverBaseY, -15.0f);
    carRotY = 0.0f;
    npcRotY = 0.0f;
    car->ResetOrientation();
    car->RotateY(carRotY);

    npcCar->ResetOrientation();
    npcCar->RotateY(npcRotY);

    velocity = { 0.0f, 0.0f };
    npcVelocity = { 0.0f, 0.0f };
    npcWaypoint = 1;

    for (int i = 0; i < (int)crosses.size(); ++i)
    {
        crosses[i].timer = 0.0f;
        crosses[i].model->SetPosition(crosses[i].model->GetX(), kCrossHiddenY, crosses[i].model->GetZ());
    }

    gameState = WaitingToStart;
    stageMsg = "Hit Space to Start";
    winnerText = "";
    playerTime = 0.0f;
    npcTime = 0.0f;
    health = kMaxHealth;
    damageCooldown = 0.0f;
    nextCheckpoint = 0;
    npcNextCheckpoint = 0;
    playerLap = 1;
    npcLap = 1;
    boostReady = true;
    isBoosting = false;
    boostOverheated = false;
    boostTimer = 0.0f;
    boostCooldownTimer = 0.0f;
    pitchAngle = 0.0f;
    collisionBounce = 0.0f;
}

int GetProgressScore(int lap, int checkpoint, int totalCheckpoints)
{
    return (lap - 1) * totalCheckpoints + checkpoint;
}

void DrawUI(IFont* font, I3DEngine* engine, EGameState gameState, const string& stageMsg,
    float playerTime, int health, int lap, int totalLaps, int checkpointNumber, int totalCheckpoints,
    bool boostReady, bool isBoosting, bool boostOverheated, float boostTimer, float boostCooldownTimer,
    int racePosition, const string& winnerText, Vec2 velocity,
    bool sceneLoaded, bool waypointsLoaded)
{
    int uiY = engine->GetHeight() - kUIBackdropHeightOffset;
    font->Draw("Time: " + FormatTime(playerTime), 20, uiY + 10, kWhite);
    font->Draw("Health: " + ToStringInt(health), 20, uiY + 35, kWhite);

    int shownLap = lap;
    if (shownLap > totalLaps) shownLap = totalLaps;
    font->Draw("Lap: " + ToStringInt(shownLap) + " / " + ToStringInt(totalLaps), 20, uiY + 60, kWhite);

    font->Draw("Checkpoint: " + ToStringInt(checkpointNumber) + " / " + ToStringInt(totalCheckpoints), 240, uiY + 10, kWhite);
    font->Draw("Speed: " + ToStringInt((int)GetSpeedKmh(velocity)) + " km/h", 240, uiY + 35, kWhite);
    font->Draw("Position: " + ToStringInt(racePosition) + " / 2", 240, uiY + 60, kWhite);

    font->Draw("1 Chase | 2 First Person | R Restart", 500, uiY + 10, kWhite);
    font->Draw(stageMsg, 500, uiY + 45, kBlue);

    if (!sceneLoaded) font->Draw("ERROR: media/scene.txt not loaded", 500, uiY + 70, kRed);
    else if (!waypointsLoaded) font->Draw("ERROR: media/waypoints.txt not loaded", 500, uiY + 70, kRed);
    else font->Draw("Scene + waypoints loaded", 500, uiY + 70, kWhite);

    if (health < kBoostDisabledHealth)
    {
        font->Draw("Boost disabled: health below 20", 850, uiY + 10, kRed);
    }
    else if (boostOverheated)
    {
        stringstream ss;
        ss << fixed << setprecision(1) << boostCooldownTimer;
        font->Draw("Boost overheated", 850, uiY + 10, kRed);
        font->Draw("Cooldown: " + ss.str(), 850, uiY + 35, kWhite);
    }
    else if (isBoosting)
    {
        font->Draw("BOOST ACTIVE", 850, uiY + 10, kGreen);
        if (boostTimer >= 2.0f) font->Draw("Warning: overheating", 850, uiY + 35, kRed);
    }
    else if (boostReady)
    {
        font->Draw("Boost ready", 850, uiY + 10, kGreen);
    }

    if (gameState == RaceComplete)
    {
        font->Draw(winnerText, engine->GetWidth() / 2 - 180, 40, kYellow);
    }
    if (gameState == GameOver)
    {
        font->Draw("GAME OVER - Press R to Restart", engine->GetWidth() / 2 - 180, 40, kRed);
    }
}

// ====================================================
// Particle creation, emission, and updates
// ====================================================
float RandFloat(float minVal, float maxVal)
{
    return minVal + (maxVal - minVal) * ((float)rand() / (float)RAND_MAX);
}

void EmitFireParticle(vector<Particle>& pool, float emitX, float emitY, float emitZ, float spread, float speedMin, float speedMax, float lifeMin, float lifeMax)
{
    for (int i = 0; i < (int)pool.size(); ++i)
    {
        if (!pool[i].active)
        {
            pool[i].model->SetPosition(emitX, emitY, emitZ);
            float angle = RandFloat(0.0f, 6.2832f);
            float horizSpeed = RandFloat(0.0f, spread);
            pool[i].velX = cos(angle) * horizSpeed;
            pool[i].velY = RandFloat(speedMin, speedMax);
            pool[i].velZ = sin(angle) * horizSpeed;
            pool[i].life = RandFloat(lifeMin, lifeMax);
            pool[i].active = true;
            return;
        }
    }
}

void EmitExplosion(vector<Particle>& pool, float emitX, float emitY, float emitZ)
{
    for (int i = 0; i < (int)pool.size(); ++i)
    {
        pool[i].model->SetPosition(emitX, emitY, emitZ);
        float angle = RandFloat(0.0f, 6.2832f);
        float elevat = RandFloat(0.1f, 1.0f);
        float speed = RandFloat(kBombSpeed, kBombSpeed * 2.2f);
        pool[i].velX = cos(angle) * speed * (1.0f - elevat);
        pool[i].velY = speed * elevat;
        pool[i].velZ = sin(angle) * speed * (1.0f - elevat);
        pool[i].life = RandFloat(kBombLife, kBombLife * 2.0f);
        pool[i].active = true;
    }
}

void EmitExplosionFire(vector<Particle>& pool, float emitX, float emitY, float emitZ)
{
    for (int i = 0; i < (int)pool.size(); ++i)
    {
        pool[i].model->SetPosition(emitX, emitY, emitZ);

        float angle = RandFloat(0.0f, 6.2832f);
        float speed = RandFloat(4.0f, 10.0f);

        pool[i].velX = cos(angle) * speed;
        pool[i].velY = RandFloat(6.0f, 14.0f); 
        pool[i].velZ = sin(angle) * speed;

        pool[i].life = RandFloat(0.8f, 1.6f); 
        pool[i].active = true;
    }
}

void EmitBigExplosionSmoke(vector<Particle>& pool, float emitX, float emitY, float emitZ)
{
    for (int i = 0; i < (int)pool.size(); ++i)
    {
        pool[i].model->SetPosition(emitX, emitY, emitZ);

        float angle = RandFloat(0.0f, 6.2832f);
        float speed = RandFloat(1.5f, 4.0f);

        pool[i].velX = cos(angle) * speed;
        pool[i].velY = RandFloat(3.0f, 6.0f);
        pool[i].velZ = sin(angle) * speed;
        pool[i].life = RandFloat(1.5f, 2.8f);
        pool[i].active = true;
    }
}

void UpdateParticles(vector<Particle>& pool, float gravity, float frameTime)
{
    for (int i = 0; i < (int)pool.size(); ++i)
    {
        if (!pool[i].active) continue;

        pool[i].life -= frameTime;
        if (pool[i].life <= 0.0f)
        {
            pool[i].active = false;
            pool[i].model->SetPosition(0.0f, kHiddenY, 0.0f); 
            continue;
        }

        pool[i].velY += gravity * frameTime;

        pool[i].model->MoveX(pool[i].velX * frameTime);
        pool[i].model->MoveY(pool[i].velY * frameTime);
        pool[i].model->MoveZ(pool[i].velZ * frameTime);
    }
}

void EmitExhaustParticle(vector<Particle>& pool, IModel* car, float carRotY, float carVelX, float carVelZ)
{
    float angle = carRotY * kDegToRad;
    float rearX = car->GetX() - sin(angle) * kExhaustRearOffset;
    float rearZ = car->GetZ() - cos(angle) * kExhaustRearOffset;

    for (int i = 0; i < (int)pool.size(); ++i)
    {
        if (!pool[i].active)
        {
            pool[i].model->SetPosition(rearX, car->GetY(), rearZ);
            float spread = RandFloat(-kExhaustSpread, kExhaustSpread);
            pool[i].velX = (-sin(angle) * kExhaustSpeed + carVelX * kExhaustVelocityBlend) + cos(angle) * spread;
            pool[i].velY = RandFloat(kExhaustMinYSpeed, kExhaustMaxYSpeed);
            pool[i].velZ = (-cos(angle) * kExhaustSpeed + carVelZ * kExhaustVelocityBlend) + sin(angle) * spread;
            pool[i].life = kExhaustLife;
            pool[i].active = true;
            return;
        }
    }
}

vector<Particle> CreateParticlePool(IMesh* mesh, int count)
{
    vector<Particle> pool;
    for (int i = 0; i < count; ++i)
    {
        Particle p;
        p.model = mesh->CreateModel(0.0f, kHiddenY, 0.0f);
        p.velX = p.velY = p.velZ = 0.0f;
        p.life = 0.0f;
        p.active = false;
        pool.push_back(p);
    }
    return pool;
}

void ResetBombs(vector<BombData>& bombs)
{
    for (int i = 0; i < (int)bombs.size(); ++i)
    {
        bombs[i].exploded = false;
        bombs[i].model->SetPosition(bombs[i].x, kBombModelY, bombs[i].z);
    }
}

// ====================================================
// Main program setup and game loop
// ====================================================
void main()
{
    I3DEngine* engine = New3DEngine(kTLX);
    engine->StartWindowed();
    srand((unsigned int)time(nullptr));

    engine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
    engine->AddMediaFolder(".\\media");

    IMesh* carMesh = engine->LoadMesh("race2.x");
    IMesh* checkpointMesh = engine->LoadMesh("Checkpoint.x");
    IMesh* isleMesh = engine->LoadMesh("IsleStraight.x");
    IMesh* wallMesh = engine->LoadMesh("Wall.x");
    IMesh* groundMesh = engine->LoadMesh("ground.x");
    IMesh* skyMesh = engine->LoadMesh("Skybox 07.x");
    IMesh* tankMesh1 = engine->LoadMesh("TankSmall1.x");
    IMesh* tankMesh2 = engine->LoadMesh("TankSmall2.x");
    IMesh* crossMesh = engine->LoadMesh("Cross.x");
    IMesh* garageMesh = engine->LoadMesh("GarageLarge.x");
    IMesh* fireMesh = engine->LoadMesh("quad.x");    
    IMesh* smokeMesh = engine->LoadMesh("quad.x");   
    IMesh* exhaustMesh = engine->LoadMesh("quad.x"); 
    IMesh* bombModelMesh = engine->LoadMesh("Flare.x"); 
    IMesh* barrelMesh = engine->LoadMesh("TankSmall1.x"); 

    IModel* ground = groundMesh->CreateModel(0.0f, -1.0f, 0.0f);
    IModel* sky = skyMesh->CreateModel(0.0f, -750.0f, 0.0f);

    IModel* car = carMesh->CreateModel(0.0f, kHoverBaseY, -15.0f);
    car->Scale(0.6f);

    IModel* npcCar = carMesh->CreateModel(6.0f, kHoverBaseY, -15.0f);
    npcCar->Scale(0.6f);
    npcCar->SetSkin("sp01.jpg");

    vector<CheckpointData> checkpoints;
    vector<CrossData> checkpointCrosses;
    vector<BoxObject> walls;
    vector<BoxObject> isles;
    vector<IModel*> tanks;
    vector<IModel*> garages;
    vector<IModel*> barrels;

    vector<Particle> fireParticles;    
    vector<Particle> exhaustParticles; 
    vector<Particle> bombParticles;    
    vector<Particle> smokeParticles;   

    vector<BombData> bombs;

    float cameraShakeTimer = 0.0f;

    struct BarrelEmitter { float x, z; float timer; };
    vector<BarrelEmitter> barrelEmitters;

    vector<SceneItem> sceneItems;
    bool sceneLoaded = LoadSceneFile(".\\media\\scene.txt", sceneItems);
    if (!sceneLoaded)
    {
        sceneLoaded = LoadSceneFile("scene.txt", sceneItems);
    }

    for (int i = 0; i < (int)sceneItems.size(); ++i)
    {
        SceneItem item = sceneItems[i];

        if (item.type == "Checkpoint")
        {
            IModel* model = checkpointMesh->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(1.35f);

            CheckpointData cp;
            cp.model = model;
            cp.x = item.x;
            cp.z = item.z;
            cp.rotY = item.rotY;
            checkpoints.push_back(cp);

            CrossData cross;
            cross.model = crossMesh->CreateModel(item.x, kCrossHiddenY, item.z);
            cross.model->Scale(0.8f);
            cross.timer = 0.0f;
            checkpointCrosses.push_back(cross);
        }
        else if (item.type == "Isle")
        {
            IModel* model = isleMesh->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(1.4f);
            isles.push_back(MakeBox(model, item.x, item.z));
        }
        else if (item.type == "Wall")
        {
            IModel* model = wallMesh->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(1.6f);
            walls.push_back(MakeBox(model, item.x, item.z));
        }
        else if (item.type == "Tank1")
        {
            IModel* model = tankMesh1->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(1.15f);
            tanks.push_back(model);
        }
        else if (item.type == "Tank2")
        {
            IModel* model = tankMesh2->CreateModel(item.x, -1.8f, item.z); 
            model->RotateY(item.rotY);
            model->RotateZ(25.0f); 
            model->Scale(1.15f);
            tanks.push_back(model);
        }
        else if (item.type == "Garage")
        {
            IModel* model = garageMesh->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(4.0f);
            garages.push_back(model);
        }
        else if (item.type == "Barrel")
        {
            IModel* model = barrelMesh->CreateModel(item.x, 0.0f, item.z);
            model->RotateY(item.rotY);
            model->Scale(0.35f);
            barrels.push_back(model);
            barrelEmitters.push_back({ item.x, item.z, 0.0f });
        }
    }

    vector<Vec2> npcWaypoints;
    bool waypointsLoaded = LoadWaypointsFile(".\\media\\waypoints.txt", npcWaypoints);
    if (!waypointsLoaded)
    {
        waypointsLoaded = LoadWaypointsFile("waypoints.txt", npcWaypoints);
    }

    fireParticles = CreateParticlePool(fireMesh, kFireParticleCount * 4);
    exhaustParticles = CreateParticlePool(exhaustMesh, kExhaustParticleCount);
    bombParticles = CreateParticlePool(fireMesh, kBombParticleCount);
    smokeParticles = CreateParticlePool(smokeMesh, kSmokeParticleCount + kBigExplosionSmokeCount);

    for (int i = 0; i < (int)fireParticles.size(); ++i) fireParticles[i].model->Scale(0.35f);
    for (int i = 0; i < (int)exhaustParticles.size(); ++i) exhaustParticles[i].model->Scale(0.06f);
    for (int i = 0; i < (int)bombParticles.size(); ++i) bombParticles[i].model->Scale(0.35f);
    for (int i = 0; i < (int)smokeParticles.size(); ++i) smokeParticles[i].model->Scale(0.45f);

    for (int i = 0; i < (int)fireParticles.size(); ++i)
    {
        const char* fireSkins[] = { "fire1.png", "fire2.png", "fire3.png" };
        fireParticles[i].model->SetSkin(fireSkins[i % 3]);
    }
    for (int i = 0; i < (int)bombParticles.size(); ++i)
    {
        const char* fireSkins[] = { "fire2.png", "fire3.png", "fire1.png" };
        bombParticles[i].model->SetSkin(fireSkins[i % 3]);
    }
    for (int i = 0; i < (int)smokeParticles.size(); ++i) smokeParticles[i].model->SetSkin("smoke1.png");
    for (int i = 0; i < (int)exhaustParticles.size(); ++i) exhaustParticles[i].model->SetSkin("fire2.png");

    auto AddBomb = [&](float x, float z)
        {
            BombData b;
            b.model = bombModelMesh->CreateModel(x, kBombModelY, z);
            b.model->Scale(kBombScale);            
            b.x = x; b.z = z;
            b.exploded = false;
            bombs.push_back(b);
        };
    AddBomb(-5.0f, 20.0f);
    AddBomb(30.0f, 45.0f);
    AddBomb(22.0f, -10.0f);
    AddBomb(-8.0f, 85.0f);
    AddBomb(18.0f, 105.0f);
    AddBomb(34.0f, 10.0f);
    AddBomb(-42.0f, -35.0f);
    AddBomb(-60.0f, -8.0f);

    ICamera* camera = engine->CreateCamera(kManual);
    IFont* font = engine->LoadFont("Arial", 25);
    ISprite* uiBackdrop = engine->CreateSprite("ui_backdrop.jpg", 0.0f, 0.0f);

    EGameState gameState = WaitingToStart;
    ECameraMode cameraMode = ChaseCam;

    float camOffsetX, camOffsetY, camOffsetZ, cameraYaw, cameraPitch;
    ResetChaseCamera(camOffsetX, camOffsetY, camOffsetZ, cameraYaw, cameraPitch);

    string stageMsg = "Hit Space to Start";
    string winnerText = "";

    float countdownTimer = 0.0f;
    float playerTime = 0.0f;
    float npcTime = 0.0f;
    int health = kMaxHealth;
    float damageCooldown = 0.0f;

    float carRotY = 0.0f;
    float npcRotY = 0.0f;
    ApplyCarVisuals(car, carRotY, 0.0f, 0.0f);
    ApplyCarVisuals(npcCar, npcRotY, 0.0f, 0.0f);
    Vec2 velocity = { 0.0f, 0.0f };
    Vec2 npcVelocity = { 0.0f, 0.0f };
    int npcWaypoint = 1;
    float npcStuckTimer = 0.0f;

    int nextCheckpoint = 0;
    int npcNextCheckpoint = 0;
    int playerLap = 1;
    int npcLap = 1;

    bool boostReady = true;
    bool isBoosting = false;
    bool boostOverheated = false;
    float boostTimer = 0.0f;
    float boostCooldownTimer = 0.0f;

    float pitchAngle = 0.0f;
    float leanAngle = 0.0f;
    float collisionBounce = 0.0f;
    float smokeEmitTimer = 0.0f;

    //Main Game Loop
    while (engine->IsRunning())
    {
        engine->DrawScene();
        float frameTime = engine->Timer();

        //Timers & Hover Effects
        if (damageCooldown > 0.0f) damageCooldown -= frameTime;
        if (collisionBounce > 0.0f)
        {
            collisionBounce -= collisionBounce * kCollisionBounceDecay * frameTime;
            if (collisionBounce < 0.01f) collisionBounce = 0.0f;
        }

        float hoverY = kHoverBaseY + sin(playerTime * kHoverFrequency) * kHoverAmplitude + collisionBounce;
        float npcHoverY = kHoverBaseY + sin(npcTime * kHoverFrequency) * kHoverAmplitude;

        UpdateCrosses(checkpointCrosses, frameTime);

        // ---- Particle system updates ----

        // Barrel fire emitters
        for (int bi = 0; bi < (int)barrelEmitters.size(); ++bi)
        {
            barrelEmitters[bi].timer -= frameTime;
            if (barrelEmitters[bi].timer <= 0.0f)
            {
                barrelEmitters[bi].timer = kBarrelFireEmitInterval; 
                EmitFireParticle(fireParticles,
                    barrelEmitters[bi].x, kBarrelFireEmitY, barrelEmitters[bi].z,
                    kFireSpread, kFireSpeedMin, kFireSpeedMax,
                    kFireLifeMin, kFireLifeMax);
            }
        }

        UpdateParticles(fireParticles, kFireGravity, frameTime);
        UpdateParticles(exhaustParticles, 0.0f, frameTime);

        // Smoke when health is low
        if (health > 0 && health < kHealthSmokeThreshold && gameState == Racing)
        {
            smokeEmitTimer -= frameTime;

            if (smokeEmitTimer <= 0.0f)
            {
                smokeEmitTimer = kSmokeBaseInterval + (health / (float)kHealthSmokeThreshold) * kSmokeHealthIntervalScale;

                EmitFireParticle(smokeParticles,
                    car->GetX(), car->GetY() + kSmokeEmitHeightOffset, car->GetZ(),
                    kSmokeSpread, kSmokeSpeedMin, kSmokeSpeedMax,
                    kSmokeLifeMin, kSmokeLifeMax);
            }
        }

        UpdateParticles(smokeParticles, kSmokeUpwardAcceleration, frameTime); 
        UpdateParticles(bombParticles, kBombGravity, frameTime);

        //Bomb System
        for (int bi = 0; bi < (int)bombs.size(); ++bi)
        {
            if (bombs[bi].exploded) continue;
            float distPlayer = Distance2D(car->GetX(), car->GetZ(), bombs[bi].x, bombs[bi].z);

            if (distPlayer < kBombRadius)
            {
                bombs[bi].exploded = true;
                bombs[bi].model->SetPosition(bombs[bi].x, kHiddenY, bombs[bi].z);

                EmitExplosion(bombParticles, bombs[bi].x, kExplosionEmitY, bombs[bi].z);
                EmitExplosionFire(fireParticles, bombs[bi].x, kExplosionEmitY, bombs[bi].z);
                EmitBigExplosionSmoke(smokeParticles, bombs[bi].x, kExplosionEmitY, bombs[bi].z);

                cameraShakeTimer = kCameraShakeTime;
                health -= (int)kBombDamage;
                if (health < 0) health = 0;
                stageMsg = "BOMB hit!";
            }
        }

        if (cameraShakeTimer > 0.0f)
        {
            cameraShakeTimer -= frameTime;
        }

        //Input Handling
        if (engine->KeyHit(Key_R))
        {
            ResetRace(car, npcCar, checkpointCrosses, carRotY, npcRotY, velocity, npcVelocity, npcWaypoint,
                gameState, stageMsg, winnerText, playerTime, npcTime, health, damageCooldown,
                nextCheckpoint, npcNextCheckpoint, playerLap, npcLap, boostReady, isBoosting,
                boostOverheated, boostTimer, boostCooldownTimer, pitchAngle, collisionBounce);
            ResetBombs(bombs);
            smokeEmitTimer = 0.0f;
            cameraMode = ChaseCam;
            ResetChaseCamera(camOffsetX, camOffsetY, camOffsetZ, cameraYaw, cameraPitch);
        }

        if (engine->KeyHit(Key_1))
        {
            ResetBombs(bombs);
            smokeEmitTimer = 0.0f;
            cameraMode = ChaseCam;
            ResetChaseCamera(camOffsetX, camOffsetY, camOffsetZ, cameraYaw, cameraPitch);
        }
        if (engine->KeyHit(Key_2)) cameraMode = FirstPersonCam;

        //Game State Machine
        switch (gameState)
        {
        case WaitingToStart:
            stageMsg = "Hit Space to Start";
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            if (engine->KeyHit(Key_Space))
            {
                gameState = Countdown3;
                countdownTimer = kCountdownStep;
            }
            break;

        case Countdown3:
            stageMsg = "3";
            countdownTimer -= frameTime;
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            if (countdownTimer <= 0.0f)
            {
                gameState = Countdown2;
                countdownTimer = kCountdownStep;
            }
            break;

        case Countdown2:
            stageMsg = "2";
            countdownTimer -= frameTime;
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            if (countdownTimer <= 0.0f)
            {
                gameState = Countdown1;
                countdownTimer = kCountdownStep;
            }
            break;

        case Countdown1:
            stageMsg = "1";
            countdownTimer -= frameTime;
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            if (countdownTimer <= 0.0f)
            {
                gameState = GoText;
                countdownTimer = 0.5f;
            }
            break;

        case GoText:
            stageMsg = "GO!";
            countdownTimer -= frameTime;
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            if (countdownTimer <= 0.0f) gameState = Racing;
            break;

        //Main Racing Logic
        case Racing:
        {
            playerTime += frameTime;
            npcTime += frameTime;

            if (health < kBoostDisabledHealth)
            {
                isBoosting = false;
                boostReady = false;
                boostTimer = 0.0f;
            }
            else if (boostOverheated)
            {
                isBoosting = false;
                boostCooldownTimer -= frameTime;
                if (boostCooldownTimer <= 0.0f)
                {
                    boostCooldownTimer = 0.0f;
                    boostOverheated = false;
                    boostReady = true;
                }
            }
            else
            {
                boostReady = true;
                if (engine->KeyHeld(Key_Space))
                {
                    isBoosting = true;
                    boostTimer += frameTime * 0.7f;
                    for (int i = 0; i < 3; ++i)
                    {
                        EmitExhaustParticle(exhaustParticles, car, carRotY, velocity.x, velocity.z);
                    }
                    if (boostTimer > kBoostMaxTime)
                    {
                        isBoosting = false;
                        boostOverheated = true;
                        boostReady = false;
                        boostCooldownTimer = kBoostCooldownTime;
                        boostTimer = 0.0f;
                    }
                }
                else
                {
                    isBoosting = false;
                    boostTimer -= frameTime * 0.75f;
                    if (boostTimer < 0.0f) boostTimer = 0.0f;
                }
            }

            Vec2 acceleration = { 0.0f, 0.0f };
            float angle = carRotY * kDegToRad;
            Vec2 forward = { sin(angle), cos(angle) };

            float targetLean = 0.0f;
            float targetPitch = 0.0f;

            if (engine->KeyHeld(Key_W))
            {
                float thrust = kForwardThrust;
                if (isBoosting) thrust *= kBoostMultiplier;
                acceleration = AddVec(acceleration, MulVec(forward, thrust));
                targetPitch = kPitchAmount;
            }
            if (engine->KeyHeld(Key_S))
            {
                acceleration = AddVec(acceleration, MulVec(forward, -kReverseThrust));
                targetPitch = -kPitchAmount;
            }
            if (engine->KeyHeld(Key_A))
            {
                carRotY -= kTurnSpeed * frameTime;
                targetLean = kLeanAmount;
            }
            if (engine->KeyHeld(Key_D))
            {
                carRotY += kTurnSpeed * frameTime;
                targetLean = -kLeanAmount;
            }
            carRotY = WrapAngle(carRotY);

            leanAngle += (targetLean - leanAngle) * ClampFloat(7.0f * frameTime, 0.0f, 1.0f);
            pitchAngle += (targetPitch - pitchAngle) * ClampFloat(6.0f * frameTime, 0.0f, 1.0f);

            float currentDrag = kDrag;
            if (boostOverheated) currentDrag *= kOverheatDragMultiplier;

            Vec2 dragForce = MulVec(velocity, -currentDrag);
            velocity = AddVec(velocity, MulVec(AddVec(acceleration, dragForce), frameTime));

            if (LengthVec(velocity) > kMaxSpeed) velocity = MulVec(NormaliseVec(velocity), kMaxSpeed);

            car->SetPosition(
                car->GetX() + velocity.x * frameTime,
                hoverY,
                car->GetZ() + velocity.z * frameTime
            );

            bool collided = false;

            for (int i = 0; i < (int)walls.size(); ++i)
            {
                if (ResolveSphereBox(car, velocity, kCarRadius, walls[i], kWallBounce))
                {
                    collided = true;
                }
            }
            for (int i = 0; i < (int)isles.size(); ++i)
            {
                if (ResolveSphereBox(car, velocity, kCarRadius, isles[i], kWallBounce)) collided = true;
            }
            for (int i = 0; i < (int)tanks.size(); ++i)
            {
                if (ResolveSpherePoint(car, velocity, kCarRadius, tanks[i]->GetX(), tanks[i]->GetZ(), kTankRadius, kTankBounce))
                {
                    collided = true;
                    DamagePlayer(health, damageCooldown);
                    stageMsg = "Hit tank";
                }
            }
            for (int i = 0; i < (int)checkpoints.size(); ++i)
            {
                float lx, lz, rx, rz;
                GetCheckpointPosts(checkpoints[i], lx, lz, rx, rz);
                if (ResolveSpherePoint(car, velocity, kCarRadius, lx, lz, kCheckpointPostRadius, kWallBounce)) collided = true;
                if (ResolveSpherePoint(car, velocity, kCarRadius, rx, rz, kCheckpointPostRadius, kWallBounce)) collided = true;
            }

            if (collided)
            {
                collisionBounce = kCollisionBounceHeight;
            }

            ApplyCarVisuals(car, carRotY, leanAngle, pitchAngle);

            if (!checkpoints.empty() && nextCheckpoint < (int)checkpoints.size())
            {
                if (PointInCheckpointGap(car->GetX(), car->GetZ(), checkpoints[nextCheckpoint]))
                {
                    ShowCross(checkpointCrosses[nextCheckpoint],
                        checkpoints[nextCheckpoint].x,
                        checkpoints[nextCheckpoint].z);

                    int completedCheckpoint = nextCheckpoint + 1;
                    nextCheckpoint++;

                    if (nextCheckpoint >= (int)checkpoints.size())
                    {
                        nextCheckpoint = 0;
                        playerLap++;

                        if (playerLap > kTotalLaps)
                        {
                            stageMsg = "Race complete";
                            winnerText = "PLAYER Wins! Race Time: " + FormatTime(playerTime) + " | Press R";
                            gameState = RaceComplete;
                        }
                        else
                        {
                            stageMsg = "Stage " + ToStringInt(completedCheckpoint) + " complete";
                        }
                    }
                    else
                    {
                        stageMsg = "Stage " + ToStringInt(completedCheckpoint) + " complete";
                    }
                }
            }

            float previousNpcX = npcCar->GetX();
            float previousNpcZ = npcCar->GetZ();

            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            UpdateNPC(npcCar, npcRotY, npcVelocity, npcWaypoints, npcWaypoint, frameTime);

            bool npcBlocked = false;
            for (int i = 0; i < (int)walls.size(); ++i)
            {
                if (ResolveSphereBox(npcCar, npcVelocity, kNpcRadius, walls[i], kNpcCollisionBounce)) 
                    npcBlocked = true;
            }
            for (int i = 0; i < (int)tanks.size(); ++i)
            {
                if (ResolveSpherePoint(npcCar, npcVelocity, kNpcRadius, tanks[i]->GetX(), tanks[i]->GetZ(), kNpcTankAvoidRadius, kNpcCollisionBounce))
                {
                    npcBlocked = true;
                }
            }

            if (npcBlocked && !npcWaypoints.empty())
            {
                Vec2 npcPos = { npcCar->GetX(), npcCar->GetZ() };
                Vec2 target = npcWaypoints[npcWaypoint];
                Vec2 dir = NormaliseVec(SubVec(target, npcPos));

                if (LengthVec(dir) > 0.0001f)
                {
                    npcRotY = WrapAngle(atan2(dir.x, dir.z) * kRadToDeg);

                    npcCar->SetPosition(
                        npcCar->GetX() + dir.x * kNpcSpeed * kNpcRecoverySpeedMultiplier * frameTime,
                        npcHoverY,
                        npcCar->GetZ() + dir.z * kNpcSpeed * kNpcRecoverySpeedMultiplier * frameTime
                    );
                }
            }

            float npcMovedAmount = Distance2D(previousNpcX, previousNpcZ, npcCar->GetX(), npcCar->GetZ());

            if (npcMovedAmount < kNpcStuckMoveThreshold && LengthVec(npcVelocity) > kNpcStuckVelocityThreshold)
            {
                npcStuckTimer += frameTime;
            }
            else
            {
                npcStuckTimer = 0.0f;
            }

            if (npcStuckTimer > kNpcStuckTimeLimit)
            {
                npcStuckTimer = 0.0f;

                if (npcNextCheckpoint == 0)
                {
                    npcCar->SetPosition(2.5f, npcHoverY, -15.0f);
                    npcRotY = 0.0f;
                    npcWaypoint = 1;
                }
                else if (npcNextCheckpoint == 1)
                {
                    npcCar->SetPosition(0.0f, npcHoverY, 10.0f);
                    npcRotY = 0.0f;
                    npcWaypoint = 3;
                }
                else if (npcNextCheckpoint == 2)
                {
                    npcCar->SetPosition(10.0f, npcHoverY, 95.0f);
                    npcRotY = 0.0f;
                    npcWaypoint = 8;
                }
                else if (npcNextCheckpoint == 3)
                {
                    npcCar->SetPosition(25.0f, npcHoverY, 35.0f);
                    npcRotY = 180.0f;
                    npcWaypoint = 12;
                }
                else if (npcNextCheckpoint == 4)
                {
                    npcCar->SetPosition(16.0f, npcHoverY, -10.0f);
                    npcRotY = 180.0f;
                    npcWaypoint = 15;
                }

                npcVelocity = { 0.0f, 0.0f };
            }
            if (!checkpoints.empty() && npcNextCheckpoint < (int)checkpoints.size())
            {
                if (PointInCheckpointGap(npcCar->GetX(), npcCar->GetZ(), checkpoints[npcNextCheckpoint]))
                {
                    npcNextCheckpoint++;

                    if (npcNextCheckpoint >= (int)checkpoints.size())
                    {
                        npcNextCheckpoint = 0;
                        npcLap++;

                        if (npcLap > kTotalLaps)
                        {
                            winnerText = "NPC Wins! Race Time: " + FormatTime(npcTime) + " | Press R";
                            gameState = RaceComplete;
                        }
                    }
                }
            }

            // Player vs NPC collision
            if (SphereToSphere(car->GetX(), car->GetZ(), kCarRadius, npcCar->GetX(), npcCar->GetZ(), kNpcRadius))
            {
                velocity = MulVec(velocity, -kNpcBounce);
                DamagePlayer(health, damageCooldown);
                collisionBounce = kCollisionBounceHeight;
                stageMsg = "Hit NPC";
            }

            if (health <= 0)
            {
                health = 0;
                gameState = GameOver;
                stageMsg = "GAME OVER - Press R to Restart";
            }

            break;
        }

        case RaceComplete:
            velocity = { 0.0f, 0.0f };
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            stageMsg = "Race Complete - Press R to Restart";
            break;

        case GameOver:
            velocity = { 0.0f, 0.0f };
            car->SetPosition(car->GetX(), hoverY, car->GetZ());
            npcCar->SetPosition(npcCar->GetX(), npcHoverY, npcCar->GetZ());
            break;
        }

        //Race Position Calculation
        int playerProgress = GetProgressScore(playerLap, nextCheckpoint, (int)checkpoints.size());
        int npcProgress = GetProgressScore(npcLap, npcNextCheckpoint, (int)checkpoints.size());
        int racePosition = (playerProgress >= npcProgress) ? 1 : 2;

        //Camera Update
        float shakeOffset = 0.0f;
        if (cameraShakeTimer > 0.0f)
            shakeOffset = RandFloat(-kCameraShakeAmount, kCameraShakeAmount);
        float shakenCamY = camOffsetY + shakeOffset;

        if (cameraMode == FirstPersonCam) UpdateFirstPersonCamera(camera, car, carRotY);
        else UpdateChaseCamera(engine, camera, car, carRotY, camOffsetX, shakenCamY, camOffsetZ, cameraYaw, cameraPitch, frameTime);

        //UI Rendering
        int displayCheckpoint = nextCheckpoint + 1;
        if (displayCheckpoint > (int)checkpoints.size()) displayCheckpoint = (int)checkpoints.size();

        DrawUI(font, engine, gameState, stageMsg, playerTime, health, playerLap, kTotalLaps,
            displayCheckpoint, (int)checkpoints.size(), boostReady, isBoosting, boostOverheated,
            boostTimer, boostCooldownTimer, racePosition, winnerText, velocity, sceneLoaded, waypointsLoaded);
    }

    engine->Delete();
}
