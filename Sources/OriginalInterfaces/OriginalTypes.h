// Generated from original ARMv6 Objective-C metadata. Declarations only; no implementations.
#pragma once

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#include <stdint.h>
#include <stdbool.h>

@protocol EAGLViewDelegate;
@class HeavyMachAppDelegate, EAGLView, GameMain, GobHvMach, Collider, ColVector, Texture2D, Tile2D, Vector2, QobBase, QobButton, QobImage, QobImageFont, QobParticle, GobBullet, GobWorld, ResMgr_Tile, QobCamera, GuiTitle, QobManager, GlobalInfo, PartsInfo, GobMachParts, GobMachWeapon, SoundManager, SoundID, GobHvM_Enemy, QVisualLayer, WeaponInfo, GuiGame, GuiSelectSlot, GuiSelectMission, GuiSelectMode, GobTrailEffect, QList, GobHvM_Player, GobItemBox;

typedef struct TColVectorList TColVectorList;
typedef struct TItemInfo TItemInfo;
typedef struct TLoadMapInfo TLoadMapInfo;
typedef struct TPassiveSlot TPassiveSlot;
typedef struct TUpgradeUI TUpgradeUI;
typedef struct TWeaponItem TWeaponItem;
typedef struct TEnemyRegenInfo TEnemyRegenInfo;
typedef struct TObjectRegenInfo TObjectRegenInfo;
typedef struct TSaveSlotData TSaveSlotData;
typedef struct TTrailPos TTrailPos;
typedef struct TDataFile TDataFile;

struct TColVectorList {
    QList * field0; // Field name absent from runtime encoding.
};

struct TItemInfo {
    int16_t itemType;
    int16_t regenRate;
    int8_t name[24];
};

struct TLoadMapInfo {
    int32_t loadingStep;
    float delayTime;
    bool toSelMission;
    Vector2 * colOrigin;
    Vector2 * colSecond;
    int32_t jointCount;
    ColVector * prevColVector;
    QobImage * loadingBG;
    QobImage * imgTypo;
    QobBase * baseBtn;
};

struct TPassiveSlot {
    bool use;
    QobImage * imgIcon;
    QobImageFont * txtLeftTime;
    float fTime;
};

struct TUpgradeUI {
    QobImage * imgBG;
    QobImage * imgSel;
    QobImage * imgGauge;
    int32_t step;
    int32_t upgradeSel;
};

struct TWeaponItem {
    int8_t name[24];
    int32_t charge;
};

struct TEnemyRegenInfo {
    bool field0; // Field name absent from runtime encoding.
    bool field1; // Field name absent from runtime encoding.
    int8_t field2[24]; // Field name absent from runtime encoding.
    CGPoint field3; // Field name absent from runtime encoding.
};

struct TObjectRegenInfo {
    bool field0; // Field name absent from runtime encoding.
    NSString * field1; // Field name absent from runtime encoding.
    CGPoint field2; // Field name absent from runtime encoding.
    int8_t field3; // Field name absent from runtime encoding.
    float field4; // Field name absent from runtime encoding.
};

struct TSaveSlotData {
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
    TWeaponItem weaponItem[32];
    int16_t itemRegenStep;
    int32_t dummy[64];
};

struct TTrailPos {
    CGPoint field0; // Field name absent from runtime encoding.
    CGPoint field1; // Field name absent from runtime encoding.
    float field2; // Field name absent from runtime encoding.
};

struct TDataFile {
    TSaveSlotData slot[3];
};
