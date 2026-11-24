#ifndef PRIVACY_SCENARIO_H
#define PRIVACY_SCENARIO_H

#include <array>

class PrivacyScenario {
public:
    enum class App { Instagram, Health, Maps, Music, Shop };
    enum class Permission { Pending, Granted, Denied };
    explicit PrivacyScenario(App app = App::Instagram);

    void answerPermission(bool allow);
    Permission getPermission() const;
    bool canGoBack() const;
    bool canGoNext() const;
    void previousStep();
    void nextStep();
    int getStep() const;
    const char* getAppName() const;
    const char* getAccessName() const;
    const char* getPrompt() const;
    const char* getStatus() const;
    void drawInsights() const;

private:
    // Each object owns its state; changing apps cannot change another decision.
    // This abstraction omits actual collection, retention and sharing behavior.
    App app_;
    Permission permission_ = Permission::Pending;
    int step_ = 0;
    struct Content {
        const char* name;
        const char* access;
        const char* prompt;
        std::array<const char*, 3> request;
        std::array<const char*, 4> inferences;
        std::array<const char*, 4> denied;
        std::array<const char*, 2> benefit;
        std::array<const char*, 2> risk;
    };
    const Content& content() const;
};
#endif
