#pragma once
#include "Visuals.hpp"
#include <array>
#include <functional>
#include <span>
namespace heavymach {
enum class LoadingEventKind { CloseSound, RefreshScreen, OpenSound };
struct LoadingEvent { LoadingEventKind kind; int screen; };
// GameMain 0x2ecc/0x3374/0x3890. One step per original game tick, no dt.
class LoadingTransition {
public:
 using EventSink=std::function<void(LoadingEvent)>;
 LoadingTransition();
 // Accepts 1..7. Repeated valid requests restart state 0, even mid-transition.
 bool changeScreen(int screen);
 // RefreshScreen callback is synchronous, before state=4/OpenSound. Call child
 // (menu/world) ticks AFTER this method, including while the shutters are moving.
 void tick(const EventSink&);
 int state() const { return state_; }
 int screen() const { return screen_; }
 bool visible() const { return visible_; }
 // Screen coordinates, +Y up, 480x320. Center anchors; layer6, no camera quake.
 std::span<const DrawSprite> sprites() const { return visible_?std::span<const DrawSprite>(sprites_):std::span<const DrawSprite>(); }
 const std::array<DrawSprite,4>& panels() const { return sprites_; }
private:
 int state_=6,screen_=1;
 bool visible_=false;
 std::array<DrawSprite,4> sprites_;
};
}
