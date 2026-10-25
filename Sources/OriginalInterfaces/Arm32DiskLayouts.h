// Generated from original ARMv6 Objective-C metadata. Declarations only; no implementations.
#pragma once

// Disk/address evidence only. HM32 pointer fields are uint32_t addresses, never native pointers.
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#pragma pack(push, 4)

typedef struct HM32CGPoint HM32CGPoint;
typedef struct HM32CGSize HM32CGSize;
typedef struct HM32TColVectorList HM32TColVectorList;
typedef struct HM32TItemInfo HM32TItemInfo;
typedef struct HM32TLoadMapInfo HM32TLoadMapInfo;
typedef struct HM32TPassiveSlot HM32TPassiveSlot;
typedef struct HM32TUpgradeUI HM32TUpgradeUI;
typedef struct HM32TWeaponItem HM32TWeaponItem;
typedef struct HM32CGRect HM32CGRect;
typedef struct HM32TEnemyRegenInfo HM32TEnemyRegenInfo;
typedef struct HM32TObjectRegenInfo HM32TObjectRegenInfo;
typedef struct HM32TSaveSlotData HM32TSaveSlotData;
typedef struct HM32TTrailPos HM32TTrailPos;
typedef struct HM32TDataFile HM32TDataFile;

struct HM32CGPoint {
    float x;
    float y;
};

struct HM32CGSize {
    float width;
    float height;
};

struct HM32TColVectorList {
    uint32_t field0;
};

struct HM32TItemInfo {
    int16_t itemType;
    int16_t regenRate;
    int8_t name[24];
};

struct HM32TLoadMapInfo {
    int32_t loadingStep;
    float delayTime;
    bool toSelMission;
    uint32_t colOrigin;
    uint32_t colSecond;
    int32_t jointCount;
    uint32_t prevColVector;
    uint32_t loadingBG;
    uint32_t imgTypo;
    uint32_t baseBtn;
};

struct HM32TPassiveSlot {
    bool use;
    uint32_t imgIcon;
    uint32_t txtLeftTime;
    float fTime;
};

struct HM32TUpgradeUI {
    uint32_t imgBG;
    uint32_t imgSel;
    uint32_t imgGauge;
    int32_t step;
    int32_t upgradeSel;
};

struct HM32TWeaponItem {
    int8_t name[24];
    int32_t charge;
};

struct HM32CGRect {
    HM32CGPoint origin;
    HM32CGSize size;
};

struct HM32TEnemyRegenInfo {
    bool field0;
    bool field1;
    int8_t field2[24];
    HM32CGPoint field3;
};

struct HM32TObjectRegenInfo {
    bool field0;
    uint32_t field1;
    HM32CGPoint field2;
    int8_t field3;
    float field4;
};

struct HM32TSaveSlotData {
    int32_t createTime;
    int32_t lastAccess;
    bool use;
    int32_t mission;
    int32_t stage;
    int32_t stageIndex;
    int32_t hp;
    int32_t hpMax;
    int32_t exp;
    float expScale;
    int32_t level;
    uint8_t upgradePoint;
    uint8_t weaponUpgrade;
    uint8_t topUpgrade;
    uint8_t bodyUpgrade;
    int32_t weaponCnt;
    HM32TWeaponItem weaponItem[32];
    int16_t itemRegenStep;
    int32_t dummy[64];
};

struct HM32TTrailPos {
    HM32CGPoint field0;
    HM32CGPoint field1;
    float field2;
};

struct HM32TDataFile {
    HM32TSaveSlotData slot[3];
};

#pragma pack(pop)
#if defined(__cplusplus)
#define HM32_ASSERT static_assert
#else
#define HM32_ASSERT _Static_assert
#endif
HM32_ASSERT(sizeof(HM32CGPoint) == 8, "Original ARM32 CGPoint size");
HM32_ASSERT(sizeof(HM32CGSize) == 8, "Original ARM32 CGSize size");
HM32_ASSERT(sizeof(HM32TColVectorList) == 4, "Original ARM32 TColVectorList size");
HM32_ASSERT(sizeof(HM32TItemInfo) == 28, "Original ARM32 TItemInfo size");
HM32_ASSERT(sizeof(HM32TLoadMapInfo) == 40, "Original ARM32 TLoadMapInfo size");
HM32_ASSERT(sizeof(HM32TPassiveSlot) == 16, "Original ARM32 TPassiveSlot size");
HM32_ASSERT(sizeof(HM32TUpgradeUI) == 20, "Original ARM32 TUpgradeUI size");
HM32_ASSERT(sizeof(HM32TWeaponItem) == 28, "Original ARM32 TWeaponItem size");
HM32_ASSERT(sizeof(HM32CGRect) == 16, "Original ARM32 CGRect size");
HM32_ASSERT(sizeof(HM32TEnemyRegenInfo) == 36, "Original ARM32 TEnemyRegenInfo size");
HM32_ASSERT(sizeof(HM32TObjectRegenInfo) == 24, "Original ARM32 TObjectRegenInfo size");
HM32_ASSERT(sizeof(HM32TSaveSlotData) == 1208, "Original ARM32 TSaveSlotData size");
HM32_ASSERT(sizeof(HM32TTrailPos) == 20, "Original ARM32 TTrailPos size");
HM32_ASSERT(sizeof(HM32TDataFile) == 3624, "Original ARM32 TDataFile size");
#undef HM32_ASSERT
