#include "ResourceManager.hpp"
//#include <iostream>

using namespace GameEngine;

//INFO === TextureManager ===

bool TextureManager::LoadResource() {
    if (!texture.loadFromFile(path.c_str())) {
        /*std::cerr << "[TextureManager] Failed to load texture: "
                << name.c_str() << " from " << path.c_str() << std::endl;*/
        return false;
    }
   /* std::cout << "[TextureManager] Loaded texture: " << name.c_str()
                << " (" << texture.getSize().x << "x" << texture.getSize().y << ")"
                << std::endl;*/
    return true;
}

void TextureManager::UnloadResource() {
    texture = sf::Texture();
    referenceCount = 0;
}

sf::Texture* TextureManager::GetTexture() {
    return &texture;
}

const sf::Texture* TextureManager::GetTexture() const {
    return &texture;
}

//INFO === SpriteManager ===

void SpriteManager::SetTextureManager(TextureManager* tm) {
    textureManager = tm;
}

void SpriteManager::SetTextureRect(const sf::IntRect& rect) {
    textureRect = rect;
    hasTextureRect = true;
}

bool SpriteManager::LoadResource() {
    // 检查纹理管理器
    if (!textureManager) {
       /* std::cerr << "[SpriteManager] No TextureManager assigned for: "
                << name.c_str() << std::endl;*/
        return false;
    }

    // 获取纹理指针
    sf::Texture* tex = textureManager->GetTexture();
    if (!tex) {
        /*std::cerr << "[SpriteManager] Texture not loaded for: "
                << name.c_str() << std::endl;*/
        return false;
    }

    // 创建精灵并设置纹理
    if (hasTextureRect) {
        sprite = std::make_unique<sf::Sprite>(*tex, textureRect);
    } else {
        sprite = std::make_unique<sf::Sprite>(*tex);
    }

    //std::cout << "[SpriteManager] Loaded sprite: " << name.c_str() << std::endl;
    return true;
}

void SpriteManager::UnloadResource() {
    sprite.reset();
}

sf::Sprite* SpriteManager::GetSprite() {
    return sprite.get();
}

const sf::Sprite* SpriteManager::GetSprite() const {
    return sprite.get();
}

//INFO === AudioManager ===

bool AudioManager::LoadResource() {
    if (!soundBuffer.loadFromFile(path.c_str())) {
       /* std::cerr << "[AudioManager] Failed to load audio: "
                << name.c_str() << " from " << path.c_str() << std::endl;*/
        return false;
    }

    sound = std::make_unique<sf::Sound>(soundBuffer);

    //std::cout << "[AudioManager] Loaded audio: " << name.c_str() << std::endl;
    return true;
}

void AudioManager::UnloadResource() {
    sound.reset();
    soundBuffer = sf::SoundBuffer();
}

sf::Sound* AudioManager::GetSound() {
    return sound.get();
}

const sf::Sound* AudioManager::GetSound() const {
    return sound.get();
}


// ResourceCache

ResourceCache* ResourceCache::instance = nullptr;

ResourceCache* ResourceCache::GetInstance() {
    if (!instance) {
        instance = new ResourceCache();
    }
    return instance;
}

void ResourceCache::DestroyInstance() {
    delete instance;
    instance = nullptr;
}

ResourceCache::~ResourceCache() {
    UnloadAll();
}


// TextureCacheManagement

TextureManager* ResourceCache::LoadTexture(const string& name, const string& path) {
    // 检查是否已缓存
    auto it = textures.find(name);
    if (it != textures.end()) {
        it->second->AddReference();
        //std::cout << "[ResourceCache] Reusing cached texture: " << name.c_str() << std::endl;
        return it->second;
    }

    // 创建新纹理
    auto* tm = new TextureManager(name, path);
    if (!tm->LoadResource()) {
        delete tm;
        return nullptr;
    }

    tm->AddReference(); // 初始引用
    textures[name] = tm;
    return tm;
}

TextureManager* ResourceCache::GetTexture(const string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

bool ResourceCache::UnloadTexture(const string& name) {
    auto it = textures.find(name);
    if (it == textures.end()) {
        return false;
    }

    TextureManager* tm = it->second;
    tm->RemoveReference();

    if (tm->GetReferenceCount() <= 0) {
        tm->UnloadResource();
        delete tm;
        textures.erase(it);
        //std::cout << "[ResourceCache] Unloaded texture: " << name.c_str() << std::endl;
    }

    return true;
}

// SpriteManagement

SpriteManager* ResourceCache::CreateSprite(
    const string& name,
    const string& textureName,
    const sf::IntRect* textureRect
) {
    // 检查是否已存在同名精灵
    auto it = sprites.find(name);
    if (it != sprites.end()) {
        //std::cerr << "[ResourceCache] Sprite already exists: " << name.c_str() << std::endl;
        return nullptr;
    }

    // 获取纹理
    TextureManager* tm = GetTexture(textureName);
    if (!tm) {
        /*std::cerr << "[ResourceCache] Texture not found for sprite: "
                << name.c_str() << " (texture: " << textureName.c_str() << ")" << std::endl;*/
        return nullptr;
    }

    // 创建精灵
    auto* sm = new SpriteManager(name, string(""));
    sm->SetTextureManager(tm);

    if (textureRect) {
        sm->SetTextureRect(*textureRect);
    }

    if (!sm->LoadResource()) {
        delete sm;
        return nullptr;
    }

    sprites[name] = sm;
    return sm;
}

SpriteManager* ResourceCache::GetSprite(const string& name) {
    auto it = sprites.find(name);
    if (it != sprites.end()) {
        return it->second;
    }
    return nullptr;
}

bool ResourceCache::UnloadSprite(const string& name) {
    auto it = sprites.find(name);
    if (it == sprites.end()) {
        return false;
    }

    SpriteManager* sm = it->second;
    sm->UnloadResource();
    delete sm;
    sprites.erase(it);

    //std::cout << "[ResourceCache] Unloaded sprite: " << name.c_str() << std::endl;
    return true;
}

// FontManagement

bool FontManager::LoadResource() {
    if (!font.openFromFile(path.c_str())) {
       /* std::cerr << "[FontManager] Failed to load fonts: "
            << name.c_str() << " from " << path.c_str() << std::endl;*/
        return false;
    }
    return true;
}

void FontManager::UnloadResource() {
    font = sf::Font();
    referenceCount = 0;
}

sf::Font* FontManager::GetFont() {
    return &font;
}

const sf::Font* FontManager::GetFont() const {
    return &font;
}

// AudioManagement

AudioManager* ResourceCache::LoadAudio(const string& name, const string& path) {
    // 检查是否已缓存
    auto it = audios.find(name);
    if (it != audios.end()) {
        //std::cout << "[ResourceCache] Reusing cached audio: " << name.c_str() << std::endl;
        return it->second;
    }

    // 创建新音频
    auto* am = new AudioManager(name, path);
    if (!am->LoadResource()) {
        delete am;
        return nullptr;
    }

    audios[name] = am;
    return am;
}

AudioManager* ResourceCache::GetAudio(const string& name) {
    auto it = audios.find(name);
    if (it != audios.end()) {
        return it->second;
    }
    return nullptr;
}

bool ResourceCache::UnloadAudio(const string& name) {
    auto it = audios.find(name);
    if (it == audios.end()) {
        return false;
    }

    AudioManager* am = it->second;
    am->UnloadResource();
    delete am;
    audios.erase(it);

    //std::cout << "[ResourceCache] Unloaded audio: " << name.c_str() << std::endl;
    return true;
}

// ResourceManagement

void ResourceCache::UnloadAll() {
    // 卸载所有精灵
    for (auto& [name, sm] : sprites) {
        sm->UnloadResource();
        delete sm;
    }
    sprites.clear();

    // 卸载所有纹理
    for (auto& [name, tm] : textures) {
        tm->UnloadResource();
        delete tm;
    }
    textures.clear();

    // 卸载所有音频
    for (auto& [name, am] : audios) {
        am->UnloadResource();
        delete am;
    }
    audios.clear();

    //std::cout << "[ResourceCache] All resources unloaded." << std::endl;
}
  