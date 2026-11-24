#include "PrivacyScenario.h"
#include "raylib.h"

PrivacyScenario::PrivacyScenario(App app) : app_(app) {}

const PrivacyScenario::Content& PrivacyScenario::content() const {
    static const std::array<Content, 5> all = {{
        {"Instagram", "Device location", "Allow location?",
         {"This scenario requests device location", "to attach a place to a post.", "Choose Allow or Don't Allow on the phone."},
         {"Repeated location records could suggest:", "- Frequently visited places", "- Possible home and work locations", "- Daily movement patterns"},
         {"Device location access is denied here.", "Public posts and photos could still", "provide location clues in this scenario.", "Denied does not mean anonymous."},
         {"Location tags can help describe", "where a photo was taken."},
         {"Repeated records may reveal routines.", "Access alone does not explain sharing."}},
        {"Health", "Health records", "Allow health data?",
         {"This demo requests steps and heart-rate", "records to display a wellness summary.", "All numbers on the phone are sample data."},
         {"In this fictional scenario, records could", "suggest activity and sleep routines.", "Missing or inaccurate readings can make", "these interpretations unreliable."},
         {"Health record access is denied here.", "The demo shows no connected records.", "Any information entered manually would", "be a separate source to consider."},
         {"A combined summary could help someone", "review their activity over time."},
         {"Personal routines may be sensitive.", "A wellness summary is not a diagnosis."}},
        {"Maps", "Device location", "Allow location?",
         {"This demo requests device location", "to show a route from your position.", "The route and places are fictional."},
         {"If repeated positions were retained,", "they could suggest regular routes,", "frequent destinations and daily timing.", "A single point tells a smaller story."},
         {"Current location access is denied here.", "A manually selected starting point", "could still support route planning.", "Typed destinations are separate data."},
         {"A current position could make route", "planning more convenient."},
         {"A route history could expose routines.", "A location switch hides retention details."}},
        {"Music", "Microphone", "Allow microphone?",
         {"This demo requests microphone access", "for a fictional song-identification tool.", "No recording or recognition takes place."},
         {"If a sound sample were collected,", "it might contain a song plus background", "speech or other nearby sounds.", "Access does not imply constant recording."},
         {"Microphone access is denied here.", "The demo keeps a manual search option.", "Searches and listening history would", "be separate sources of information."},
         {"A short sound sample could help", "identify an unfamiliar song."},
         {"Nearby speech might enter a recording.", "The switch hides when samples are kept."}},
        {"Shop", "Personalization", "Allow personalizing?",
         {"This fictional shop asks to use browsing", "and purchase history for recommendations.", "This is in-app consent, not an OS prompt."},
         {"Combined shopping records could suggest", "interests, preferred prices or habits.", "A gift purchase may misrepresent", "the buyer's own preferences."},
         {"Personalization is declined here.", "The demo shows a generic catalog.", "Orders would still need separate handling;", "this choice does not erase past orders."},
         {"Recommendations could make relevant", "items easier to discover."},
         {"A profile can be incomplete or wrong.", "One choice hides many data-use decisions."}}
    }};
    return all.at(static_cast<std::size_t>(app_));
}

void PrivacyScenario::answerPermission(bool allow) {
    permission_ = allow ? Permission::Granted : Permission::Denied;
    step_ = 0;
}
PrivacyScenario::Permission PrivacyScenario::getPermission() const { return permission_; }
bool PrivacyScenario::canGoBack() const { return step_ > 0; }
bool PrivacyScenario::canGoNext() const { return permission_ != Permission::Pending && step_ < 2; }
void PrivacyScenario::previousStep() { if (canGoBack()) --step_; }
void PrivacyScenario::nextStep() { if (canGoNext()) ++step_; }
int PrivacyScenario::getStep() const { return step_; }
const char* PrivacyScenario::getAppName() const { return content().name; }
const char* PrivacyScenario::getAccessName() const { return content().access; }
const char* PrivacyScenario::getPrompt() const { return content().prompt; }
const char* PrivacyScenario::getStatus() const {
    switch (permission_) {
        case Permission::Granted: return "ALLOWED";
        case Permission::Denied: return "DENIED";
        default: return "PENDING";
    }
}

void PrivacyScenario::drawInsights() const {
    const Content& c = content();
    const Color statusColor = permission_ == Permission::Pending ? ORANGE :
                              permission_ == Permission::Granted ? DARKGREEN : MAROON;
    const char* titles[] = {"01 - PERMISSION", "02 - POSSIBLE INFERENCES", "03 - PRIVACY IMPACT"};
    DrawText(titles[step_], 830, 270, 20, DARKBLUE);
    if (step_ == 0) {
        DrawText(TextFormat("%s: %s", c.access, getStatus()), 830, 315, 18, statusColor);
        for (int i = 0; i < 3; ++i) DrawText(c.request[i], 830, 360 + i * 28, 17, BLACK);
        DrawText("Use Permission settings to change your choice.", 830, 480, 16, DARKGRAY);
    } else if (step_ == 1) {
        const auto& lines = permission_ == Permission::Granted ? c.inferences : c.denied;
        for (int i = 0; i < 4; ++i) DrawText(lines[i], 830, 330 + i * 32, 17, BLACK);
    } else {
        DrawText(permission_ == Permission::Granted ? "POTENTIAL BENEFIT" : "PRIVACY BENEFIT", 830, 320, 18, DARKGREEN);
        if (permission_ == Permission::Granted) {
            for (int i = 0; i < 2; ++i) DrawText(c.benefit[i], 830, 350 + i * 25, 17, BLACK);
        } else {
            DrawText("This requested use is restricted in the demo.", 830, 350, 17, BLACK);
            DrawText("Other data sources remain separate choices.", 830, 375, 17, BLACK);
        }
        DrawText("WHAT THE SWITCH HIDES", 830, 425, 18, DARKBLUE);
        for (int i = 0; i < 2; ++i) DrawText(c.risk[i], 830, 455 + i * 25, 17, BLACK);
        DrawText("A simple choice abstracts a complex process.", 830, 525, 16, DARKGRAY);
    }
}
