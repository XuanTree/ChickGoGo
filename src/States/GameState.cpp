#include "Game.hpp"
#include "States/GameState.hpp"
#include "ResourceManager.hpp"
#include <SFML/Window/Keyboard.hpp>
//#include <iostream>
#include <ctime>
#include <algorithm>

// ==================== GameState ====================

GameState::GameState(Game& game)
    : game(game)
    , rng(static_cast<unsigned>(std::time(nullptr)))
    , obstacleYDist(200.f, 432.f)   // 障碍物Y范围
    , healingYDist(200.f, 432.f)
{
    // 加载纹理
    auto* cache = ResourceCache::GetInstance();
    auto* chickTex = cache->LoadTexture(string("chick"), string("assets/sprites/chick.png"));
    auto* obsTex = cache->LoadTexture(string("obstacle"), string("assets/sprites/spike.png"));
    auto* heartTex = cache->LoadTexture(string("heart"), string("assets/sprites/heart.png"));
    auto* healingTex = cache->LoadTexture(string("heal"), string("assets/sprites/heal.png"));
    
    if (!chickTex) {
        // std::cerr << "[GameState] Failed to load chick texture!" << std::endl;
    }
    if (!obsTex) {
        // std::cerr << "[GameState] Failed to load obstacle texture!" << std::endl;
    }
    if (!healingTex) {
        // std::cerr << "[GameState] Failed to load healing obstacle texture!" << std::endl;
    }
    if (!chick.loadSounds("assets/sounds/jump.wav", "assets/sounds/hit.wav"
        , "assets/sounds/nice.wav", "assets/sounds/heal.wav")) {
        // std::cerr << "[GameState] Failed to load chick sounds!" << std::endl;
    }
    if (!heartTex) {
        // std::cerr << "[GameState] Failed to load heart texture!" << std::endl; 
    }

    // 加载背景音乐
    if (!bgm.openFromFile("assets/music/bgm.mp3")) {
        // std::cerr << "[GameState] Failed to load bgm!" << std::endl;
    }
    bgm.setLooping(true);
    bgm.play();

    // 创建障碍物纹理 && HUD纹理
    obstacleTexLoaded = obstacleTexture.loadFromFile("assets/sprites/spike.png");
    heartTexLoaded = heartTexture.loadFromFile("assets/sprites/heart.png");
    healingObTexLoaded = healingTexture.loadFromFile("assets/sprites/heal.png");

    // SFML3 Sprite 没有默认构造函数，需要在纹理加载后构造
    if (heartTexLoaded) {
        int maxHP = chick.getMaxHealth();  // 5
        heartSprites.reserve(maxHP);
        for (int i = 0; i < maxHP; ++i) {
            heartSprites.emplace_back(heartTexture);
        }
    }

    // 加载字体
    if (font.openFromFile("assets/fonts/unifont.otf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Font Loaded Failed!" << std::endl;
    }

    // 初始化 Chick（14帧动画，精灵表水平排列）
    // sprite sheet: 4200x300, 14 frames → each 300x300
    float chickScale = 80.f / 300.f;
    chick.init(chickTex ? chickTex->GetTexture() : nullptr,
               14,                                       // frameCount
               100.f,                                    // startX
               500.f - 80.f,                             // startY
               500.f,                                    // groundY
               chickScale);
    // 地面
    ground.setSize({800.f, 100.f});
    ground.setPosition({0.f, 500.f});
    ground.setFillColor(sf::Color(100, 60, 20));  // 棕色地面
}

static sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {
    float windowRatio = static_cast<float>(windowWidth) / windowHeight;
    float viewRatio = view.getSize().x / view.getSize().y;

    float sizeX = 1.f;
    float sizeY = 1.f;
    float posX = 0.f;
    float posY = 0.f;

    bool horizontalSpacing = (windowRatio >= viewRatio);
    if (horizontalSpacing)
    {
        // 黑边在左右两侧
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else
    {
        // 黑边在上下两侧
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect({ posX, posY }, { sizeX, sizeY }));
    return view;
}

void GameState::handleInput() {
    sf::View gameView = game.getWindow().getView();
    while (auto event = game.getWindow().pollEvent()) {
        game.getWindow().handleEvents(
            [&](const sf::Event::Closed&) {
                game.getWindow().close();
            },
            [&](const sf::Event::Resized& resized) {
                int newWidth = resized.size.x;
                int newHeight = resized.size.y;

                gameView = getLetterboxView(gameView, newWidth, newHeight);
                game.getWindow().setView(gameView);
            });
        // 暂停中只响应 ESC 和 q(quit)
        if (paused) {
            bgm.pause();
            static bool escWasPressed = false;
            static bool qWasPressed = false;
            bool escNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
            bool qNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
            if (escNow && !escWasPressed) {
                paused = false;
                bgm.setLooping(true);
                bgm.play();
            }
            else if (qNow && !qWasPressed) {
                game.getWindow().close();
            }
            qWasPressed = qNow;
            escWasPressed = escNow;
            return;
        }

        // 传递给 chick
        chick.handleInput();

        // ESC → 暂停（带边沿检测防连触发）
        static bool escPressed = false;
        bool escNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
        if (escNow && !escPressed) {
            paused = true;
        }
        escPressed = escNow;
    }
}

void GameState::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    if (paused) return;

    if (chick.isDead()) {
        bgm.pause();
        // 死亡后等待一小段时间，按回车可以重新开始
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            chick.reset(100.f, 300.f);
            obstacles.clear();
            healingObs.clear();             // 清除治疗物
            score = 0;
            spawnTimer = 0.f;
            spawnInterval = 2.0f;
            spawnTimerHeal = 0.f;           // 重置治疗物计时器
            spawnIntervalHeal = 7.f;        // 重置治疗物生成间隔
            bgm.setLooping(true);
            bgm.play();
        }
        return;
    }

    // 更新 chick
    chick.update(dt);

    // 更新障碍物
    for (auto& obs : obstacles) {
        obs.update(dt);
    }

    // 更新治疗物品
    for (auto& heal : healingObs) {
        heal.update(dt);
    }

    // 障碍物生成
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.f;
        spawnObstacle();
        // 难度递增
        if (spawnInterval > minSpawnInterval) {
            spawnInterval -= difficultyIncrease;
        }
    }

    // 治疗物生成
    spawnTimerHeal += dt;
    if (spawnTimerHeal >= spawnIntervalHeal) {
        spawnTimerHeal = 0.f;
        spawnHeal();
        // 难度递增
        if (spawnIntervalHeal < maxSpawnInterval) {
            spawnIntervalHeal += difficultyIncrease;
        }
    }

    // 碰撞检测
    checkCollisions();

    // 清理出屏障碍物
    cleanupObstacles();
}

void GameState::checkHealth() {
    auto& window = game.getWindow();

    // 对爱心纹理进行检查，若没有正确加载，直接结束游戏
    if (!heartTexLoaded) {
        return;
    }

    for (int i = 0; i < chick.getHealth(); i++) {
        heartSprites[i].setPosition({ 10.f+32.f*(i + 1), 40 });
        window.draw(heartSprites[i]);
    }
}

void GameState::spawnObstacle() {
    Obstacle obs;
    obs.init(&obstacleTexture,
             850.f,                        // 屏幕右侧外
             obstacleYDist(rng),           // 随机Y
             200.f + score * 2.f,          // 速度随分数递增
             800.f);
    obstacles.push_back(std::move(obs));

}

void GameState::spawnHeal() {
    // 修复: 检查纹理是否加载成功
    if (!healingObTexLoaded) {
        return;
    }

    Obstacle heals;
    heals.init(&healingTexture,
        850.f,
        healingYDist(rng),
        200.f + score * 2.f,
        800.f);
    healingObs.push_back(std::move(heals));
}

void GameState::checkCollisions() {
    if (chick.isInvincible()) return;

    sf::FloatRect chickBounds = chick.getBounds();

    for (auto& obs : obstacles) {
        if (chickBounds.findIntersection(obs.getBounds())) {
            chick.takeDamage();
            chick.getHitSound().play();
        }

        // 计分：鸡仔完全越过障碍才记一分
        if (!obs.hasScored() && obs.getRightEdge() < chick.getPosition().x) {
            obs.markScored();
            if (obs.getBounds().position.y > chick.getPosition().y) {
                score++;
                chick.getScoreSound().play();
            }
        }
    }

    // 治疗物碰撞检测修复: 碰撞后标记为已消费，防止每帧重复治疗
    for (auto& heal : healingObs) {
        if (heal.isConsumed()) continue;  // 跳过已消费的治疗物

        if (chickBounds.findIntersection(heal.getBounds())) {
            if (chick.getHealth() < chick.getMaxHealth()) {
                chick.setHealth(chick.getHealth() + 1);
                chick.getHealSound().play();
            }
            heal.markConsumed();  // 标记为已消费，不再重复触发
        }
    }
}

void GameState::cleanupObstacles() {
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
                       [](const Obstacle& o) { return o.isOffScreen(); }),
        obstacles.end());
    healingObs.erase(
        std::remove_if(healingObs.begin(), healingObs.end(),
            [](const Obstacle& o) { return o.isOffScreen() || o.isConsumed(); }),
        healingObs.end());
}

void GameState::draw() {
    auto& window = game.getWindow();

    // 背景
    window.clear(sf::Color(135, 206, 235));  // 天蓝色

    // 地面
    window.draw(ground);

    // 障碍物
    for (auto& obs : obstacles) {
        obs.draw(window);
    }

    // 绘制治疗物品
    for (auto& heal : healingObs) {
        heal.draw(window);
    }

    // chick
    chick.draw(window);

    // HUD
    if (fontLoaded && heartTexLoaded) {
        // 生命
        sf::Text healthText(font);
        healthText.setString("HP: ");
        healthText.setCharacterSize(24);
        healthText.setFillColor(sf::Color::White);
        healthText.setOutlineColor(sf::Color::Black);
        healthText.setOutlineThickness(1.2f);
        healthText.setPosition({ 10.f, 40.f });
        window.draw(healthText);

        checkHealth();

        // 分数
        sf::Text scoreText(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineColor(sf::Color::Black);
        scoreText.setOutlineThickness(1.2f);
        scoreText.setPosition({ 10.f, 70.f });
        window.draw(scoreText);

        // 暂停
        sf::Text pauseHint(font);
        pauseHint.setString("ESC");
        pauseHint.setCharacterSize(24);
        pauseHint.setFillColor(sf::Color::Yellow);
        pauseHint.setOutlineColor(sf::Color::Black);
        pauseHint.setOutlineThickness(1.5f);
        pauseHint.setPosition({ 10.f, 10.f });
        window.draw(pauseHint);
    }

    // 暂停遮罩
    if (paused) {
        sf::RectangleShape overlay({800.f, 600.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(overlay);

        if (fontLoaded) {
            sf::Text pauseText(font);
            pauseText.setString(U"暂停");
            pauseText.setCharacterSize(48);
            pauseText.setFillColor(sf::Color::White);
            pauseText.setOutlineColor(sf::Color::Black);
            pauseText.setOutlineThickness(2.f);
            sf::FloatRect pb = pauseText.getLocalBounds();
            pauseText.setOrigin({pb.size.x / 2.f, pb.size.y / 2.f});
            pauseText.setPosition({400.f, 250.f});
            window.draw(pauseText);

            sf::Text hint(font);
            hint.setString(U"按ESC继续 或 按Q退出");
            hint.setCharacterSize(24);
            hint.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect hb = hint.getLocalBounds();
            hint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
            hint.setPosition({400.f, 320.f});
            window.draw(hint);
        }
    }

    // 死亡画面
    if (chick.isDead()) {
        sf::RectangleShape overlay({800.f, 600.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));

        window.draw(overlay);

        if (fontLoaded) {
            sf::Text deadText(font);
            deadText.setString("GAME OVER");
            deadText.setCharacterSize(48);
            deadText.setFillColor(sf::Color::Red);
            deadText.setOutlineColor(sf::Color::Black);
            deadText.setOutlineThickness(2.f);
            sf::FloatRect bounds = deadText.getLocalBounds();
            deadText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            deadText.setPosition({400.f, 250.f});
            window.draw(deadText);

            sf::Text restartText(font);
            restartText.setString(U"按Enter键重来");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            sf::FloatRect rb = restartText.getLocalBounds();
            restartText.setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
            restartText.setPosition({400.f, 320.f});
            window.draw(restartText);
        }
    }
}
