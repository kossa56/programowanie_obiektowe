#include <vector>
#include <algorithm>
#include <functional> 
#include <memory>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include <raylib.h>
#include <raymath.h>

// --- UTILS ---
namespace Utils {
    inline static float RandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
    }
}

// --- TRANSFORM, PHYSICS, LIFETIME, RENDERABLE ---
struct TransformA {
    Vector2 position{};
    float rotation{};
};

struct Physics {
    Vector2 velocity{};
    float rotationSpeed{};
};

struct Renderable {
    enum Size { SMALL = 1, MEDIUM = 2, LARGE = 4 } size = SMALL;
};

// --- RENDERER ---
class Renderer {
public:
    static Renderer& Instance() {
        static Renderer inst;
        return inst;
    }

    void Init(int w, int h, const char* title) {
        InitWindow(w, h, title);
        SetTargetFPS(60);
        screenW = w;
        screenH = h;
    }

    void Begin() {
        BeginDrawing();
        ClearBackground(BLACK);
    }

    void End() {
        EndDrawing();
    }

    void DrawPoly(const Vector2& pos, int sides, float radius, float rot, Color color = WHITE) {
        DrawPolyLines(pos, sides, radius, rot, color);
    }

    int Width() const {
        return screenW;
    }

    int Height() const {
        return screenH;
    }

private:
    Renderer() = default;

    int screenW{};
    int screenH{};
};

// --- ASTEROID HIERARCHY ---
class Asteroid {
public:
    Asteroid(int screenW, int screenH) {
        init(screenW, screenH);
    }
    virtual ~Asteroid() = default;

    bool Update(float dt) {
        transform.position = Vector2Add(transform.position, Vector2Scale(physics.velocity, dt));
        transform.rotation += physics.rotationSpeed * dt;
        if (transform.position.x < -GetRadius() || transform.position.x > Renderer::Instance().Width() + GetRadius() ||
            transform.position.y < -GetRadius() || transform.position.y > Renderer::Instance().Height() + GetRadius())
            return false;
        return true;
    }
    virtual void Draw() const = 0;

    Vector2 GetPosition() const {
        return transform.position;
    }

    float GetRadius() const {
        return 16.f * (float)render.size;
    }

    int GetDamage() const {
        return baseDamage * static_cast<int>(render.size);
    }

    int GetSize() const {
        return static_cast<int>(render.size);
    }

    int GetPoints() const {
        return pointsValue * static_cast<int>(render.size);
    }

    Physics GetPhysics() const {
        return physics;
    }

    void SetPhysics(const Physics& phys) {
        physics = phys;
    }

protected:
    void init(int screenW, int screenH) {
        // Choose size
        render.size = static_cast<Renderable::Size>(1 << GetRandomValue(0, 2));

        // Spawn at random edge
        switch (GetRandomValue(0, 3)) {
        case 0:
            transform.position = { Utils::RandomFloat(0, screenW), -GetRadius() };
            break;
        case 1:
            transform.position = { screenW + GetRadius(), Utils::RandomFloat(0, screenH) };
            break;
        case 2:
            transform.position = { Utils::RandomFloat(0, screenW), screenH + GetRadius() };
            break;
        default:
            transform.position = { -GetRadius(), Utils::RandomFloat(0, screenH) };
            break;
        }

        // Aim towards center with jitter
        float maxOff = fminf(screenW, screenH) * 0.1f;
        float ang = Utils::RandomFloat(0, 2 * PI);
        float rad = Utils::RandomFloat(0, maxOff);
        Vector2 center = {
                                         screenW * 0.5f + cosf(ang) * rad,
                                         screenH * 0.5f + sinf(ang) * rad
        };

        Vector2 dir = Vector2Normalize(Vector2Subtract(center, transform.position));
        physics.velocity = Vector2Scale(dir, Utils::RandomFloat(SPEED_MIN, SPEED_MAX));
        physics.rotationSpeed = Utils::RandomFloat(ROT_MIN, ROT_MAX);

        transform.rotation = Utils::RandomFloat(0, 360);
    }

    TransformA transform;
    Physics    physics;
    Renderable render;

    int baseDamage = 0;
    int pointsValue = 10;
    static constexpr float SPEED_MIN = 125.f;
    static constexpr float SPEED_MAX = 250.f;
    static constexpr float ROT_MIN = 50.f;
    static constexpr float ROT_MAX = 240.f;
};

class TriangleAsteroid : public Asteroid {
public:
    TriangleAsteroid(int w, int h) : Asteroid(w, h) { 
        baseDamage = 5; 
        pointsValue = 15;
    }
    void Draw() const override {
        Renderer::Instance().DrawPoly(transform.position, 3, GetRadius(), transform.rotation, ORANGE);
    }
};

class SquareAsteroid : public Asteroid {
public:
    SquareAsteroid(int w, int h) : Asteroid(w, h) { 
        baseDamage = 10; 
        pointsValue = 25;
    }
    void Draw() const override {
        Renderer::Instance().DrawPoly(transform.position, 4, GetRadius(), transform.rotation, RED);
    }
};

class PentagonAsteroid : public Asteroid {
public:
    PentagonAsteroid(int w, int h) : Asteroid(w, h) { 
        baseDamage = 15; 
        pointsValue = 40;
    }
    void Draw() const override {
        Renderer::Instance().DrawPoly(transform.position, 5, GetRadius(), transform.rotation, BLUE);
    }
};

class StarAsteroid : public Asteroid {
public:
    StarAsteroid(int w, int h) : Asteroid(w, h) { 
        baseDamage = 20; 
        pointsValue = 60;
    }
    void Draw() const override {
        float radius = GetRadius();
        Vector2 center = transform.position;
        float rotation = transform.rotation;
        
        Vector2 points[12];
        float angleStep = PI / 3.0f;
        
        for (int i = 0; i < 6; i++) {
            float angle = angleStep * i + rotation * DEG2RAD;
            points[i*2] = {
                center.x + cosf(angle) * radius,
                center.y + sinf(angle) * radius
            };
            
            angle = angleStep * (i + 0.5f) + rotation * DEG2RAD;
            points[i*2 + 1] = {
                center.x + cosf(angle) * (radius * 0.5f),
                center.y + sinf(angle) * (radius * 0.5f)
            };
        }
        
        for (int i = 0; i < 11; i++) {
            DrawLineV(points[i], points[i+1], PURPLE);
        }
        DrawLineV(points[11], points[0], PURPLE);
    }
};

// Shape selector
enum class AsteroidShape { TRIANGLE = 3, SQUARE = 4, PENTAGON = 5, STAR = 6, RANDOM = 0 };

// Factory
static inline std::unique_ptr<Asteroid> MakeAsteroid(int w, int h, AsteroidShape shape) {
    switch (shape) {
    case AsteroidShape::TRIANGLE:
        return std::make_unique<TriangleAsteroid>(w, h);
    case AsteroidShape::SQUARE:
        return std::make_unique<SquareAsteroid>(w, h);
    case AsteroidShape::PENTAGON:
        return std::make_unique<PentagonAsteroid>(w, h);
    case AsteroidShape::STAR:
        return std::make_unique<StarAsteroid>(w, h);
    default: {
        return MakeAsteroid(w, h, static_cast<AsteroidShape>(3 + GetRandomValue(0, 3)));
    }
    }
}

// --- EXPLOSION EFFECT ---
struct Explosion {
    Vector2 position;
    float radius;
    float maxRadius;
    float duration;
    float timer;
    Color color;

    Explosion(Vector2 pos, float maxRad, float dur, Color col) 
        : position(pos), radius(0), maxRadius(maxRad), duration(dur), timer(0), color(col) {}

    bool Update(float dt) {
        timer += dt;
        radius = maxRadius * (timer / duration);
        return timer >= duration;
    }

    void Draw() const {
        float alpha = 1.0f - (timer / duration);
        Color fadeColor = { color.r, color.g, color.b, static_cast<unsigned char>(alpha * 255) };
        DrawCircleLines(position.x, position.y, radius, fadeColor);
    }
};

// --- PROJECTILE HIERARCHY ---
enum class WeaponType { LASER, BULLET, COUNT };
class Projectile {
public:
    Projectile(Vector2 pos, Vector2 vel, int dmg, WeaponType wt)
    {
        transform.position = pos;
        physics.velocity = vel;
        baseDamage = dmg;
        type = wt;
    }
    bool Update(float dt) {
        transform.position = Vector2Add(transform.position, Vector2Scale(physics.velocity, dt));

        if (transform.position.x < 0 ||
            transform.position.x > Renderer::Instance().Width() ||
            transform.position.y < 0 ||
            transform.position.y > Renderer::Instance().Height())
        {
            return true;
        }
        return false;
    }
    void Draw() const {
        if (type == WeaponType::BULLET) {
            DrawCircleV(transform.position, 6.f, ORANGE);
            DrawCircleV(transform.position, 3.f, WHITE);
        }
        else {
            static constexpr float LASER_LENGTH = 40.f;
            static constexpr float LASER_WIDTH = 6.f;
            
            Rectangle lr = { 
                transform.position.x - LASER_WIDTH * 0.5f, 
                transform.position.y - LASER_LENGTH, 
                LASER_WIDTH, 
                LASER_LENGTH 
            };
            DrawRectangleRec(lr, BLUE);
            
            Rectangle innerLr = {
                transform.position.x - LASER_WIDTH * 0.25f,
                transform.position.y - LASER_LENGTH * 0.9f,
                LASER_WIDTH * 0.5f,
                LASER_LENGTH * 0.8f
            };
            DrawRectangleRec(innerLr, SKYBLUE);
            
            DrawLineEx(
                {transform.position.x, transform.position.y},
                {transform.position.x, transform.position.y - LASER_LENGTH * 0.85f},
                1.5f, WHITE);
        }
    }
    Vector2 GetPosition() const {
        return transform.position;
    }

    float GetRadius() const {
        return (type == WeaponType::BULLET) ? 6.f : 3.f;
    }

    int GetDamage() const {
        return baseDamage;
    }

private:
    TransformA transform;
    Physics    physics;
    int        baseDamage;
    WeaponType type;
};

inline static Projectile MakeProjectile(WeaponType wt,
    const Vector2 pos,
    float speed)
{
    Vector2 vel{ 0, -speed };
    if (wt == WeaponType::LASER) {
        return Projectile(pos, vel, 20, wt);
    }
    else {
        return Projectile(pos, vel, 10, wt);
    }
}

// --- SHIP HIERARCHY ---
class Ship {
public:
    Ship(int screenW, int screenH) {
        transform.position = {
                                         screenW * 0.5f,
                                         screenH * 0.5f
        };
        hp = 100;
        maxHp = 100;
        speed = 250.f;
        alive = true;

        fireRateLaser = 18.f;
        fireRateBullet = 22.f;
        spacingLaser = 40.f;
        spacingBullet = 20.f;
    }
    virtual ~Ship() = default;
    virtual void Update(float dt) = 0;
    virtual void Draw() const = 0;

    void TakeDamage(int dmg) {
        if (!alive) return;
        hp -= dmg;
        if (hp <= 0) alive = false;
    }

    void Heal(int amount) {
        hp = std::min(hp + amount, maxHp);
    }

    void UpgradeWeapon(WeaponType wt) {
        if (wt == WeaponType::LASER) {
            fireRateLaser *= 1.2f;
            spacingLaser *= 1.1f;
        }
        else {
            fireRateBullet *= 1.2f;
            spacingBullet *= 1.1f;
        }
    }

    bool IsAlive() const {
        return alive;
    }

    Vector2 GetPosition() const {
        return transform.position;
    }

    virtual float GetRadius() const = 0;

    int GetHP() const {
        return hp;
    }

    int GetMaxHP() const {
        return maxHp;
    }

    float GetFireRate(WeaponType wt) const {
        return (wt == WeaponType::LASER) ? fireRateLaser : fireRateBullet;
    }

    float GetSpacing(WeaponType wt) const {
        return (wt == WeaponType::LASER) ? spacingLaser : spacingBullet;
    }

protected:
    TransformA transform;
    int        hp;
    int        maxHp;
    float      speed;
    bool       alive;
    float      fireRateLaser;
    float      fireRateBullet;
    float      spacingLaser;
    float      spacingBullet;
};

class PlayerShip :public Ship {
public:
    PlayerShip(int w, int h) : Ship(w, h) {
        texture = LoadTexture("spaceship1.png");
        GenTextureMipmaps(&texture);
        SetTextureFilter(texture, 2);
        scale = 0.25f;
    }
    ~PlayerShip() {
        UnloadTexture(texture);
    }

    void Update(float dt) override {
        if (alive) {
            if (IsKeyDown(KEY_W)) transform.position.y -= speed * dt;
            if (IsKeyDown(KEY_S)) transform.position.y += speed * dt;
            if (IsKeyDown(KEY_A)) transform.position.x -= speed * dt;
            if (IsKeyDown(KEY_D)) transform.position.x += speed * dt;

            // Keep ship within bounds
            transform.position.x = std::clamp(transform.position.x, GetRadius(), Renderer::Instance().Width() - GetRadius());
            transform.position.y = std::clamp(transform.position.y, GetRadius(), Renderer::Instance().Height() - GetRadius());
        }
        else {
            transform.position.y += speed * dt;
        }
    }

    void Draw() const override {
        if (!alive && fmodf(GetTime(), 0.4f) > 0.2f) return;
        Vector2 dstPos = {
                                         transform.position.x - (texture.width * scale) * 0.5f,
                                         transform.position.y - (texture.height * scale) * 0.5f
        };
        DrawTextureEx(texture, dstPos, 0.0f, scale, WHITE);

        // Draw health bar
        if (alive) {
            float healthPercentage = static_cast<float>(hp) / maxHp;
            Rectangle backBar = { transform.position.x - 50, transform.position.y - GetRadius() - 20, 100, 10 };
            Rectangle healthBar = { backBar.x, backBar.y, backBar.width * healthPercentage, backBar.height };
            
            DrawRectangleRec(backBar, RED);
            DrawRectangleRec(healthBar, GREEN);
            DrawRectangleLinesEx(backBar, 2, DARKGRAY);
        }
    }

    float GetRadius() const override {
        return (texture.width * scale) * 0.5f;
    }

private:
    Texture2D texture;
    float     scale;
};

// --- POWERUP ---
enum class PowerUpType { HEALTH, WEAPON_UPGRADE };

struct PowerUp {
    Vector2 position;
    PowerUpType type;
    float radius = 15.f;
    float rotation = 0.f;
    float rotationSpeed = 90.f;
    float lifetime = 10.f;
    float timer = 0.f;

    PowerUp(Vector2 pos, PowerUpType t) : position(pos), type(t) {}

    bool Update(float dt) {
        rotation += rotationSpeed * dt;
        timer += dt;
        return timer >= lifetime;
    }

    void Draw() const {
        if (type == PowerUpType::HEALTH) {
            DrawCircleV(position, radius, GREEN);
            DrawCircleV(position, radius * 0.6f, LIME);
            DrawText("+", position.x - 10, position.y - 10, 20, DARKGREEN);
        }
        else {
            DrawCircleV(position, radius, BLUE);
            DrawCircleV(position, radius * 0.6f, SKYBLUE);
            DrawText("W", position.x - 10, position.y - 10, 20, DARKBLUE);
        }
    }
};

// --- APPLICATION ---
class Application {
public:
    static Application& Instance() {
        static Application inst;
        return inst;
    }

    void Run() {
        srand(static_cast<unsigned>(time(nullptr)));
        Renderer::Instance().Init(C_WIDTH, C_HEIGHT, "Asteroids OOP");

        auto player = std::make_unique<PlayerShip>(C_WIDTH, C_HEIGHT);

        float spawnTimer = 0.f;
        float spawnInterval = Utils::RandomFloat(C_SPAWN_MIN, C_SPAWN_MAX);
        WeaponType currentWeapon = WeaponType::LASER;
        float shotTimer = 0.f;
        int score = 0;
        int level = 1;
        int asteroidsDestroyed = 0;
        int asteroidsToNextLevel = 10;
        float gameTime = 0.f;

        while (!WindowShouldClose()) {
            float dt = GetFrameTime();
            spawnTimer += dt;
            gameTime += dt;

            // Update player
            player->Update(dt);

            // Restart logic
            if (!player->IsAlive() && IsKeyPressed(KEY_R)) {
                player = std::make_unique<PlayerShip>(C_WIDTH, C_HEIGHT);
                asteroids.clear();
                projectiles.clear();
                explosions.clear();
                powerups.clear();
                spawnTimer = 0.f;
                spawnInterval = Utils::RandomFloat(C_SPAWN_MIN, C_SPAWN_MAX);
                score = 0;
                level = 1;
                asteroidsDestroyed = 0;
                asteroidsToNextLevel = 10;
                gameTime = 0.f;
            }
            
            // Asteroid shape switch
            if (IsKeyPressed(KEY_ONE)) {
                currentShape = AsteroidShape::TRIANGLE;
            }
            if (IsKeyPressed(KEY_TWO)) {
                currentShape = AsteroidShape::SQUARE;
            }
            if (IsKeyPressed(KEY_THREE)) {
                currentShape = AsteroidShape::PENTAGON;
            }
            if (IsKeyPressed(KEY_FOUR)) {
                currentShape = AsteroidShape::STAR;
            }
            if (IsKeyPressed(KEY_FIVE)) {
                currentShape = AsteroidShape::RANDOM;
            }

            // Weapon switch
            if (IsKeyPressed(KEY_TAB)) {
                currentWeapon = static_cast<WeaponType>((static_cast<int>(currentWeapon) + 1) % static_cast<int>(WeaponType::COUNT));
            }

            // Shooting
            if (player->IsAlive() && IsKeyDown(KEY_SPACE)) {
                shotTimer += dt;
                float interval = 1.f / player->GetFireRate(currentWeapon);
                float projSpeed = player->GetSpacing(currentWeapon) * player->GetFireRate(currentWeapon);

                while (shotTimer >= interval) {
                    Vector2 p = player->GetPosition();
                    p.y -= player->GetRadius();
                    projectiles.push_back(MakeProjectile(currentWeapon, p, projSpeed));
                    shotTimer -= interval;
                }
            }
            else {
                float maxInterval = 1.f / player->GetFireRate(currentWeapon);
                if (shotTimer > maxInterval) {
                    shotTimer = fmodf(shotTimer, maxInterval);
                }
            }

            // Spawn asteroids with level-based difficulty
            if (spawnTimer >= spawnInterval && asteroids.size() < MAX_AST) {
                auto asteroid = MakeAsteroid(C_WIDTH, C_HEIGHT, currentShape);
                
                // Increase speed based on level
                float speedMultiplier = 1.0f + (level * 0.1f);
                Physics phys = asteroid->GetPhysics();
                phys.velocity = Vector2Scale(phys.velocity, speedMultiplier);
                asteroid->SetPhysics(phys);
                
                asteroids.push_back(std::move(asteroid));
                spawnTimer = 0.f;
                spawnInterval = Utils::RandomFloat(C_SPAWN_MIN / (1 + level * 0.1f), C_SPAWN_MAX / (1 + level * 0.1f));
            }

            // Update projectiles
            {
                auto projectile_to_remove = std::remove_if(projectiles.begin(), projectiles.end(),
                    [dt](auto& projectile) {
                        return projectile.Update(dt);
                    });
                projectiles.erase(projectile_to_remove, projectiles.end());
            }

            // Projectile-Asteroid collisions
            for (auto pit = projectiles.begin(); pit != projectiles.end();) {
                bool removed = false;

                for (auto ait = asteroids.begin(); ait != asteroids.end(); ++ait) {
                    float dist = Vector2Distance((*pit).GetPosition(), (*ait)->GetPosition());
                    if (dist < (*pit).GetRadius() + (*ait)->GetRadius()) {
                        // Add score
                        score += (*ait)->GetPoints();
                        asteroidsDestroyed++;
                        
                        // Create explosion
                        explosions.emplace_back((*ait)->GetPosition(), (*ait)->GetRadius() * 2.0f, 0.5f, 
                            (*ait)->GetSize() == 1 ? YELLOW : (*ait)->GetSize() == 2 ? ORANGE : RED);
                        
                        // Chance to spawn powerup (20%)
                        if (GetRandomValue(0, 4) == 0) {
                            PowerUpType type = GetRandomValue(0, 1) ? PowerUpType::HEALTH : PowerUpType::WEAPON_UPGRADE;
                            powerups.emplace_back((*ait)->GetPosition(), type);
                        }
                        
                        ait = asteroids.erase(ait);
                        pit = projectiles.erase(pit);
                        removed = true;
                        break;
                    }
                }
                if (!removed) {
                    ++pit;
                }
            }

            // Asteroid-Ship collisions
            {
                auto remove_collision =
                    [&player, this, dt](auto& asteroid_ptr_like) -> bool {
                    if (player->IsAlive()) {
                        float dist = Vector2Distance(player->GetPosition(), asteroid_ptr_like->GetPosition());

                        if (dist < player->GetRadius() + asteroid_ptr_like->GetRadius()) {
                            player->TakeDamage(asteroid_ptr_like->GetDamage());
                            explosions.emplace_back(asteroid_ptr_like->GetPosition(), 
                                asteroid_ptr_like->GetRadius() * 1.5f, 0.4f, RED);
                            return true;
                        }
                    }
                    if (!asteroid_ptr_like->Update(dt)) {
                        return true;
                    }
                    return false;
                };
                auto asteroid_to_remove = std::remove_if(asteroids.begin(), asteroids.end(), remove_collision);
                asteroids.erase(asteroid_to_remove, asteroids.end());
            }

            // Update explosions
            {
                auto explosion_to_remove = std::remove_if(explosions.begin(), explosions.end(),
                    [dt](auto& explosion) {
                        return explosion.Update(dt);
                    });
                explosions.erase(explosion_to_remove, explosions.end());
            }

            // Update powerups
            {
                auto powerup_to_remove = std::remove_if(powerups.begin(), powerups.end(),
                    [dt](auto& powerup) {
                        return powerup.Update(dt);
                    });
                powerups.erase(powerup_to_remove, powerups.end());
            }

            // Powerup collection
            if (player->IsAlive()) {
                for (auto it = powerups.begin(); it != powerups.end();) {
                    float dist = Vector2Distance(player->GetPosition(), it->position);
                    if (dist < player->GetRadius() + it->radius) {
                        if (it->type == PowerUpType::HEALTH) {
                            player->Heal(25);
                        }
                        else {
                            player->UpgradeWeapon(currentWeapon);
                        }
                        it = powerups.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }

            // Level progression
            if (asteroidsDestroyed >= asteroidsToNextLevel) {
                level++;
                asteroidsDestroyed = 0;
                asteroidsToNextLevel = 10 + level * 5;
                
                // Flash screen when level up
                explosions.emplace_back(
                    Vector2{Renderer::Instance().Width()/2.0f, Renderer::Instance().Height()/2.0f},
                    Renderer::Instance().Width() * 0.8f,
                    1.0f,
                    GREEN);
            }

            // Render everything
            {
                Renderer::Instance().Begin();

                // Draw HUD
                DrawText(TextFormat("HP: %d/%d", player->GetHP(), player->GetMaxHP()), 10, 10, 20, GREEN);
                DrawText(TextFormat("Score: %d", score), 10, 40, 20, YELLOW);
                DrawText(TextFormat("Level: %d", level), 10, 70, 20, BLUE);
                DrawText(TextFormat("Time: %.1f", gameTime), 10, 100, 20, WHITE);
                
                const char* weaponName = (currentWeapon == WeaponType::LASER) ? "LASER" : "BULLET";
                DrawText(TextFormat("Weapon: %s (TAB to switch)", weaponName), 10, 130, 20, SKYBLUE);
                
                // Draw controls info
                DrawText("Controls: WASD - Move, SPACE - Shoot, 1-5 - Asteroid Shapes, R - Restart", 
                    10, Renderer::Instance().Height() - 30, 20, GRAY);

                // Draw explosions
                for (const auto& explosion : explosions) {
                    explosion.Draw();
                }

                // Draw powerups
                for (const auto& powerup : powerups) {
                    powerup.Draw();
                }

                // Draw projectiles
                for (const auto& projPtr : projectiles) {
                    projPtr.Draw();
                }

                // Draw asteroids
                for (const auto& astPtr : asteroids) {
                    astPtr->Draw();
                }

                // Draw player
                player->Draw();

                // Game over screen
                if (!player->IsAlive()) {
                    DrawRectangle(0, 0, Renderer::Instance().Width(), Renderer::Instance().Height(), Fade(BLACK, 0.7f));
                    DrawText("GAME OVER", 
                        Renderer::Instance().Width()/2 - MeasureText("GAME OVER", 60)/2, 
                        Renderer::Instance().Height()/2 - 100, 60, RED);
                    DrawText(TextFormat("Final Score: %d", score), 
                        Renderer::Instance().Width()/2 - MeasureText(TextFormat("Final Score: %d", score), 40)/2, 
                        Renderer::Instance().Height()/2, 40, WHITE);
                    DrawText("Press R to restart", 
                        Renderer::Instance().Width()/2 - MeasureText("Press R to restart", 30)/2, 
                        Renderer::Instance().Height()/2 + 100, 30, GREEN);
                }

                // Level up notification
                if (asteroidsDestroyed >= asteroidsToNextLevel - 3 && asteroidsDestroyed < asteroidsToNextLevel) {
                    DrawText(TextFormat("Next level in: %d", asteroidsToNextLevel - asteroidsDestroyed),
                        Renderer::Instance().Width()/2 - MeasureText(TextFormat("Next level in: %d", asteroidsToNextLevel - asteroidsDestroyed), 30)/2,
                        50, 30, GREEN);
                }

                Renderer::Instance().End();
            }
        }
    }

private:
    Application()
    {
        asteroids.reserve(1000);
        projectiles.reserve(10'000);
        explosions.reserve(100);
        powerups.reserve(20);
    };

    std::vector<std::unique_ptr<Asteroid>> asteroids;
    std::vector<Projectile> projectiles;
    std::vector<Explosion> explosions;
    std::vector<PowerUp> powerups;

    AsteroidShape currentShape = AsteroidShape::TRIANGLE;

    static constexpr int C_WIDTH = 1600;
    static constexpr int C_HEIGHT = 900;
    static constexpr size_t MAX_AST = 150;
    static constexpr float C_SPAWN_MIN = 0.5f;
    static constexpr float C_SPAWN_MAX = 3.0f;

    static constexpr int C_MAX_ASTEROIDS = 1000;
    static constexpr int C_MAX_PROJECTILES = 10'000;
};

int main() {
    Application::Instance().Run();
    return 0;
}