#ifndef ENTITY_HPP
#define ENTITY_HPP

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <string>

// ==================== 前置声明 ====================
class Egg;

// ==================== Chick ====================

class Chick {
private:
    std::unique_ptr<sf::Sprite> sprite;
    const sf::Texture* texture = nullptr;
    
    // 动画
    int frameCount = 14;
    int frameWidth = 0;
    int frameHeight = 0;
    int currentFrame = 0;
    float animationTimer = 0.f;
    static constexpr float animationSpeed = 0.02f;

    // 物理
    sf::Vector2f velocity{0.f, 0.f};
    static constexpr float gravity = 980.f;
    static constexpr float jumpVelocity = -650.f;

    // 边界（地面 Y，在 GameState 中设置）
    float groundY = 500.f;
    float leftBound = 50.f;
    float rightBound = 750.f;

    // 蛋 —— 使用 unique_ptr 包装 vector
    std::unique_ptr<std::vector<Egg>> eggs;
    // 状态
    int health = 3;
    int currentEggs = 0;
    int maxEggs = 3;
    int maxHealth = 5;
    bool isOnGround = true;
    bool isJumpKeyHeld = false;
    sf::Clock jumpHoldClock;
    float jumpHoldDuration = 0.f;
    static constexpr float maxJumpHoldTime = 3.2f;
    static constexpr float jumpBoostForce = 600.f;
    bool invincible = false;
    float invincibleTimer = 0.f;
    static constexpr float invincibleDuration = 1.5f;

    // 音效
    sf::SoundBuffer jumpBuffer;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer scoreBuffer;
    std::unique_ptr<sf::Sound> hitSound;
    std::unique_ptr<sf::Sound> jumpSound;
    std::unique_ptr<sf::Sound> healSound;
    std::unique_ptr<sf::Sound> scoreSound;
    //NOTE - 障碍物被鸡蛋炸毁的声音
    sf::SoundBuffer explosionBuffer;
    std::unique_ptr<sf::Sound> explosionSound;
    bool soundsLoaded = false;

public:
    Chick();
    ~Chick();

    void init(const sf::Texture* tex, int frameCount,
              float startX, float startY, float groundY,
              float scale = 1.0f);
    bool loadSounds(const std::string& jumpPath, const std::string& hitPath,
        const std::string& scorePath, const std::string& healPath, const std::string& explosionPath);

    void handleInput();
    void update(float dt);
    void draw(sf::RenderWindow& window);

    /// 返回蛋容器的引用，允许外部添加/遍历蛋
    std::vector<Egg>& getEggs();
    /// 向鸡仔添加一枚蛋
    void addEgg(Egg egg);
    sf::FloatRect getBounds() const;
    void takeDamage();
    sf::Sprite getSprite() const { return *sprite;}
    int getHealth() const { return health; }
    void setHealth(int newHealth);
    int getMaxHealth() const;
    int getCurrentEggs() const { return currentEggs; };
    void setNewEggs(int newEgg);
    int getMaxEggs() const { return maxEggs; };
    bool isDead() const { return health <= 0; }
    bool isInvincible() const { return invincible; }

    void reset(float x, float y);
    void initEggs(const sf::Texture* eggTex);
    void launchEgg();
    sf::Vector2f getPosition() const { return sprite ? sprite->getPosition() : sf::Vector2f{0.f, 0.f}; }

    bool areSoundsLoaded() const { return soundsLoaded; }

    // Get Chick Sound
    sf::Sound& getHitSound() { return *hitSound; }
    sf::Sound& getJumpSound() { return *jumpSound; }
    sf::Sound& getScoreSound() { return *scoreSound; }
    sf::Sound& getHealSound() { return *healSound; }
    sf::Sound& getExplosionSound() { return *explosionSound; }
    
    //NOTE - 小鸡衍生物
    friend class Egg;
};

// ==================== Obstacle ====================

class Obstacle {
private:
    std::unique_ptr<sf::Sprite> sprite;
    const sf::Texture* texture = nullptr;

    float speed = 200.f;      // 向左移动速度 (px/s)
    float screenWidth = 800.f;
    bool scored = false;      // 玩家是否已经越过这个障碍物（计分用）
    bool consumed = false;    // 治疗物是否已被吃掉

public:
    Obstacle() = default;

    void init(const sf::Texture* tex, float x, float y, float speed, float screenW);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool isOffScreen() const;
    bool hasScored() const { return scored; }
    void markScored() { scored = true; }
    bool isConsumed() const { return consumed; }
    void markConsumed() { consumed = true; }
    float getRightEdge() const;
};

// ========== Egg ============

class Egg {
private:
    std::unique_ptr<sf::Sprite> eggSprite;
    const sf::Texture* eggTexture = nullptr;

    //NOTE - 蛋生成位置
    float startX = 0.f;
    float startY = 0.f;
    bool isHitBy = false;
    //NOTE - 蛋的物理
    sf::Vector2f eggVelocity{0.f, 0.f};
    static constexpr float shootVelocity = 300.f;
    static constexpr float dropVelocity = 600.f;
    //NOTE - 蛋的状态
    bool launched = false;
    bool dead = false;
public:
    Egg() = default;

    // unique_ptr 成员不可拷贝，显式删除拷贝操作，保留移动语义
    Egg(const Egg&) = delete;
    Egg& operator=(const Egg&) = delete;
    Egg(Egg&&) = default;
    Egg& operator=(Egg&&) = default;

    void init(const sf::Texture* tex, float spawnX, float spawnY);
    void setPosition(sf::Vector2f pos);
    void launch(sf::Vector2f velocity);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return eggSprite ? eggSprite->getPosition() : sf::Vector2f{0.f, 0.f}; }
    sf::Sprite getSprite() const { return *eggSprite; };
    bool isLaunched() const { return launched; }
    bool isAlive() const { return launched && !dead; }
    bool markDead();
};
#endif // ENTITY_HPP
