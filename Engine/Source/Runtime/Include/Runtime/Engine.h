//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <memory>
#include <functional>

#include <Common/Path.h>
#include <Runtime/Input.h>
#include <Runtime/Config.h>
#include <Runtime/Application.h>

namespace Runtime{
    class World;

    struct EngineInitializer {
        IApplication* application;
        std::string execFile;
        std::string projectFile;
        std::string map;
    };

    class Engine {
    public:
        static Engine& Get();
        ~Engine();

        void Initialize(const EngineInitializer& inInitializer);
        void Tick();

        IApplication* GetApplication();
        Common::PathMapper& GetPathMapper();
        InputManager& GetInputManager();
        ConfigManager& GetConfigManager();

        void SetActiveWorld(World* inWorld);

        void AddOnInitListener(std::function<void()> listener);
        void AddOnTickListener(std::function<void()> listener);

    private:
        struct Listeners {
            std::vector<std::function<void()>> onInits;
            std::vector<std::function<void()>> onTicks;
        };

        Engine();

        void InitPathMapper(const std::string& execFile, const std::string& projectFile);
        void InitInputManager();
        void InitConfigManager();

        World* activeWorld;
        IApplication* application;
        std::unique_ptr<Common::PathMapper> pathMapper;
        std::unique_ptr<InputManager> inputManager;
        std::unique_ptr<ConfigManager> configManager;
        Listeners listeners;
    };
}