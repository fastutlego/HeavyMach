#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <string_view>
namespace heavymach {
// First getSound appearances: GlobalInfo0xfd44 common15, WeaponInfo0x10e00
// file-order unique11, GameMain0x2ecc loading2, first GuiTitle0xefbc title3.
// The first26 exist before SetMaxDistance800; the final5 retain AL defaults.
inline constexpr std::array<std::string_view,31> originalInitialSoundNames={
 "UI_Click","UI_Btn_OK","UI_Btn_Cancel","Game_GetItem","Game_ChangeWeapon","Game_WeaponEnd","Game_LevelUp","Game_LowHP","SFX_HitBody01","SFX_HitBody02","SFX_HitBody03","SFX_HitBody04","SFX_GroundHit","SFX_ExplGround","SFX_ExplMach",
 "Wpn_Gun01","Wpn_Gun02","Wpn_Gun03","Wpn_Shotgun","Wpn_Canon","Wpn_Missile","Wpn_Laser","Wpn_Default","Wpn_EnGun","Wpn_EnCanon","Wpn_EnMissile",
 "Loading_Open","Loading_Close","Title_TitleIn","Title_Bang","Title_TapToStart"
};
inline float originalSoundMaxDistance(std::string_view name){
 for(size_t i=0;i<26;++i)if(originalInitialSoundNames[i]==name)return 800.f;
 return std::numeric_limits<float>::max();
}
// The legacy event field named pan contains OpenAL source X, NOT a pan ratio.
// All observed game callers use Y=Z=0, listener=(0,0,0). Defaults: inverse
// distance clamped, reference=1, rolloff=1, gain=1. No game setter overrides them.
inline float originalSoundGain(std::string_view name,float sourceX){
 if(!std::isfinite(sourceX))return 0; // native API guard; original finite callers
 return 1.f/std::clamp(std::fabs(sourceX),1.f,originalSoundMaxDistance(name));
}
}
