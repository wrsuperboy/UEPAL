// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PAL.h"
#include "PALGameData.generated.h"

struct FObjectPlayer
{
	uint16 Reserved[2];    // always zero
	uint16 ScriptOnFriendDeath; // when friends in party dies, execute script from here
	uint16 ScriptOnDying;  // when dying, execute script from here
};

// items
struct FObjectItem
{
	uint16 Bitmap;         // bitmap number in BALL.MKF
	uint16 Price;          // price
	uint16 ScriptOnUse;    // script executed when using this item
	uint16 ScriptOnEquip;  // script executed when equipping this item
	uint16 ScriptOnThrow;  // script executed when throwing this item to enemy
	uint16 ScriptDesc;     // description script
	uint16 Flags;          // flags
};

enum EMagicFlag : uint16
{
	MagicFlagUsableOutsideBattle = (1 << 0),
	MagicFlagUsableInBattle = (1 << 1),
	MagicFlagUsableToEnemy = (1 << 3),
	MagicFlagApplyToAll = (1 << 4),
};

// magics
struct FObjectMagic
{
	uint16 MagicNumber;      // magic number, according to DATA.MKF #3
	uint16 Reserved1;        // always zero
	uint16 ScriptOnSuccess;  // when magic succeed, execute script from here
	uint16 ScriptOnUse;      // when use this magic, execute script from here
	uint16 ScriptDesc;       // description script
	uint16 Reserved2;        // always zero
	uint16 Flags;            // flags
};

// enemies
struct FObjectEnemy
{
	uint16 EnemyID;        // ID of the enemy, according to DATA.MKF #1. Also indicates the bitmap number in ABC.MKF.
	uint16 ResistanceToSorcery;  // resistance to sorcery and poison (0 min, 10 max)
	uint16 ScriptOnTurnStart;    // script executed when turn starts
	uint16 ScriptOnBattleEnd;    // script executed when battle ends
	uint16 ScriptOnReady;        // script executed when the enemy is ready
};

// poisons (scripts executed in each round)
struct FObjectPoison
{
	uint16 PoisonLevel;    // level of the poison
	uint16 Color;          // color of avatars
	uint16 PlayerScript;   // script executed when player has this poison (per round)
	uint16 Reserved;       // always zero
	uint16 EnemyScript;    // script executed when enemy has this poison (per round)
};

// DOS items
struct FObjectItemDOS
{
	uint16 Bitmap;         // bitmap number in BALL.MKF
	uint16 Price;          // price
	uint16 ScriptOnUse;    // script executed when using this item
	uint16 ScriptOnEquip;  // script executed when equipping this item
	uint16 ScriptOnThrow;  // script executed when throwing this item to enemy
	uint16 Flags;          // flags
};

// DOS magics
struct FObjectMagicDOS
{
	uint16 MagicNumber;      // magic number, according to DATA.MKF #3
	uint16 Reserved1;        // always zero
	uint16 ScriptOnSuccess;  // when magic succeed, execute script from here
	uint16 ScriptOnUse;      // when use this magic, execute script from here
	uint16 Reserved2;        // always zero
	uint16 Flags;            // flags
};

typedef union
{
	uint16 Data[6];
	FObjectPlayer Player;
	FObjectItemDOS Item;
	FObjectMagicDOS Magic;
	FObjectEnemy Enemy;
	FObjectPoison Poison;
} FObjectDOS;

typedef union
{
	uint16 Data[7];
	FObjectPlayer Player;
	FObjectItem Item;
	FObjectMagic Magic;
	FObjectEnemy Enemy;
	FObjectPoison Poison;
} FObject, FObjectWin;

struct FScriptEntry
{
	uint16 Operation;     // operation code
	uint16 Operand[3];  // operands
};

struct FGDScene
{
	uint16 MapNum;         // number of the map
	uint16 ScriptOnEnter;  // when entering this scene, execute script from here
	uint16 ScriptOnTeleport;  // when teleporting out of this scene, execute script from here
	uint16 EventObjectIndex;  // event objects in this scene begins from number wEventObjectIndex + 1
}; // add prefix to avoid name conflit

struct FStore
{
	uint16 Items[MAX_STORE_ITEM];
};

struct FEnemy
{
	uint16 IdleFrames;         // total number of frames when idle
	uint16 MagicFrames;        // total number of frames when using magics
	uint16 AttackFrames;       // total number of frames when doing normal attack
	uint16 IdleAnimSpeed;      // speed of the animation when idle
	uint16 ActWaitFrames;      // FIXME: ???
	uint16 YPosOffset;
	int16 AttackSound;        // sound played when this enemy uses normal attack
	int16 ActionSound;        // FIXME: ???
	int16 MagicSound;         // sound played when this enemy uses magic
	int16 DeathSound;         // sound played when this enemy dies
	int16 CallSound;          // sound played when entering the battle
	uint16 Health;             // total HP of the enemy
	uint16 Exp;                // How many EXPs we'll get for beating this enemy
	uint16 Cash;               // how many cashes we'll get for beating this enemy
	uint16 Level;              // this enemy's level
	uint16 Magic;              // this enemy's magic number
	uint16 MagicRate;          // chance for this enemy to use magic
	uint16 AttackEquivItem;    // equivalence item of this enemy's normal attack
	uint16 AttackEquivItemRate;// chance for equivalence item
	uint16 StealItem;          // which item we'll get when stealing from this enemy
	uint16 StealItemCount;     // total amount of the items which can be stolen
	uint16 AttackStrength;     // normal attack strength
	uint16 MagicStrength;      // magical attack strength
	uint16 Defense;            // resistance to all kinds of attacking
	uint16 Dexterity;          // dexterity
	uint16 FleeRate;           // chance for successful fleeing
	uint16 PoisonResistance;   // resistance to poison
	uint16 ElemResistances[NUM_MAGIC_ELEMENTAL]; // resistance to elemental magics
	uint16 PhysicalResistance; // resistance to physical attack
	uint16 DualMove;           // whether this enemy can do dual move or not
	uint16 CollectValue;       // value for collecting this enemy for items
};

struct FEnemyTeam
{
	uint16 Enemy[MAX_ENEMIES_IN_TEAM];
};

typedef uint16 PLAYERS[MAX_PLAYER_ROLES];

struct FPlayerRoles
{
	PLAYERS Avatar;             // avatar (shown in status view)
	PLAYERS SpriteNumInBattle;  // sprite displayed in battle (in F.MKF)
	PLAYERS SpriteNum;          // sprite displayed in normal scene (in MGO.MKF)
	PLAYERS Name;               // name of player class (in WORD.DAT)
	PLAYERS AttackAll;          // whether player can attack everyone in a bulk or not
	PLAYERS Unknown1;           // FIXME: ???
	PLAYERS Level;              // level
	PLAYERS MaxHP;              // maximum HP
	PLAYERS MaxMP;              // maximum MP
	PLAYERS HP;                 // current HP
	PLAYERS MP;                 // current MP
	uint16 Equipments[MAX_PLAYER_EQUIPMENTS][MAX_PLAYER_ROLES]; // equipments
	PLAYERS AttackStrength;     // normal attack strength
	PLAYERS MagicStrength;      // magical attack strength
	PLAYERS Defense;            // resistance to all kinds of attacking
	PLAYERS Dexterity;          // dexterity
	PLAYERS FleeRate;           // chance of successful fleeing
	PLAYERS PoisonResistance;   // resistance to poison
	uint16 ElementalResistances[NUM_MAGIC_ELEMENTAL][MAX_PLAYER_ROLES]; // resistance to elemental magics
	PLAYERS Unknown2;           // FIXME: ???
	PLAYERS Unknown3;           // FIXME: ???
	PLAYERS Unknown4;           // FIXME: ???
	PLAYERS CoveredBy;          // who will cover me when I am low of HP or not sane
	uint16 Magics[MAX_PLAYER_MAGICS][MAX_PLAYER_ROLES]; // magics
	PLAYERS WalkFrames;         // walk frame (???)
	PLAYERS CooperativeMagic;   // cooperative magic
	PLAYERS Unknown5;           // FIXME: ???
	PLAYERS Unknown6;           // FIXME: ???
	PLAYERS DeathSound;         // sound played when player dies
	PLAYERS AttackSound;        // sound played when player attacks
	PLAYERS WeaponSound;        // weapon sound (???)
	PLAYERS CriticalSound;      // sound played when player make critical hits
	PLAYERS MagicSound;         // sound played when player is casting a magic
	PLAYERS CoverSound;         // sound played when player cover others
	PLAYERS DyingSound;         // sound played when player is dying
};

// state of event object, used by the State field of the FEventObject struct
enum EObjectState : int16
{
	ObjStateHidden = 0,
	ObjStateNormal = 1,
	ObjStateBlocker = 2
};

enum ETriggerMode : uint16
{
	TriggerNone = 0,
	SearchNear = 1,
	SearchNormal = 2,
	SearchFar = 3,
	TouchNear = 4,
	TouchNormal = 5,
	TouchFar = 6,
	TouchFarther = 7,
	TouchFarthest = 8
};

struct FEventObject
{
	uint16 VanishTime;         // vanish time (?)
	uint16 X;                   // X coordinate on the map
	uint16 Y;                   // Y coordinate on the map
	int16 Layer;              // layer value
	uint16 TriggerScript;      // Trigger script entry
	uint16 AutoScript;         // Auto script entry
	int16 State;              // state of this object
	uint16 TriggerMode;        // trigger mode
	uint16 SpriteNum;          // number of the sprite
	uint16 SpriteFrames;       // total number of frames of the sprite
	uint16 Direction;          // direction
	uint16 CurrentFrameNum;    // current frame number
	uint16 ScriptIdleFrame;    // count of idle frames, used by trigger script
	uint16 SpritePtrOffset;    // FIXME: ???
	uint16 SpriteFramesAuto;   // total number of frames of the sprite, used by auto script
	uint16 ScriptIdleFrameCountAuto;     // count of idle frames, used by auto script
};

struct FMagic
{
	uint16 Effect;               // effect sprite
	uint16 Type;                 // type of this magic
	uint16 XOffset;
	uint16 YOffset;
	uint16 SummonEffect;         // summon effect sprite (in F.MKF)
	int16 Speed;                // speed of the effect
	uint16 KeepEffect;           // FIXME: ???
	uint16 FireDelay;            // start frame of the magic fire stage
	uint16 EffectTimes;          // total times of effect
	uint16 Shake;                // shake screen
	uint16 Wave;                 // wave screen
	uint16 Unknown;              // FIXME: ???
	uint16 CostMP;               // MP cost
	uint16 BaseDamage;           // base damage
	uint16 Elemental;            // elemental (0 = No Elemental, last = poison)
	int16 Sound;                // sound played when using this magic
};

struct FBattleField
{
	uint16 ScreenWave;                      // level of screen waving
	int16 MagicEffects[NUM_MAGIC_ELEMENTAL]; // effect of attributed magics
};

// magics learned when level up
struct FLevelUpMagic
{
	uint16 Level;    // level reached
	uint16 Magic;    // magic learned
};

struct FLevelUpMagicAll
{
	FLevelUpMagic LevelUpMagic[MAX_PLAYABLE_PLAYER_ROLES];
};

struct FPos
{
	uint16 X;
	uint16 Y;
};

struct FEnemyPos
{
	FPos Pos[MAX_ENEMIES_IN_TEAM][MAX_ENEMIES_IN_TEAM];
};

enum EPALDirection : uint16
{
	South,
	West,
	North,
	East,
	Unknown
};

struct FInventoryItem
{
	int16 Item;             // item object code
	SIZE_T Amount;           // amount of this item
	SIZE_T InUseAmount;      // in-use amount of this item

	FInventoryItem(int16 Item, SIZE_T Amount, SIZE_T InUseAmount = 0) : Item(Item), Amount(Amount), InUseAmount(InUseAmount) {}
};

struct FPoisonStatus
{
	int16 PoisonID;       // kind of the poison
	int16 PoisonScript;   // script entry
};

struct FExperience
{
	int16 Exp;                // current experience points
	int16 Reserved;
	int16 Level;              // current level
	int16 Count;
};

struct FExperienceAll
{
	FExperience PrimaryExp[MAX_PLAYER_ROLES];
	FExperience HealthExp[MAX_PLAYER_ROLES];
	FExperience MagicExp[MAX_PLAYER_ROLES];
	FExperience AttackExp[MAX_PLAYER_ROLES];
	FExperience MagicPowerExp[MAX_PLAYER_ROLES];
	FExperience DefenseExp[MAX_PLAYER_ROLES];
	FExperience DexterityExp[MAX_PLAYER_ROLES];
	FExperience FleeExp[MAX_PLAYER_ROLES];
};

// body parts of equipments
enum EPALBodyPart : uint16
{
	Head,
	Body,
	Shoulder,
	Hand,
	Feet,
	Wear,
	Extra,
	_BodyPartCount
};

enum EPALRoleStatus : uint16
{
	Confused,  // attack friends randomly
	Paralyzed,     // paralyzed
	Sleep,         // not allowed to move
	Silence,       // cannot use magic
	Puppet,        // for dead players only, continue attacking
	Bravery,       // more power for physical attacks
	Protect,       // more defense value
	Haste,         // faster
	DualAttack,    // dual attack
	_RoleStatusCount
};

enum EPALItemFlag : uint16
{
	ItemFlagUsable = (1 << 0),
	ItemFlagEquipable = (1 << 1),
	ItemFlagThrowable = (1 << 2),
	ItemFlagConsuming = (1 << 3),
	ItemFlagApplyToAll = (1 << 4),
	ItemFlagSellable = (1 << 5),
	ItemFlagEquipableByPlayerRole_First = (1 << 6)
};

// player party
struct FGDParty
{
	uint16 PlayerRole;         // player role
	int16 X, Y;                // position
	uint16 Frame;              // current frame number
	uint16 ImageOffset;        // FIXME: ???
};

// player trail, used for other party members to follow the main party member
struct FGDTrail
{
	uint16 X, Y;         // position
	uint16 Direction;    // direction
};

struct FGDInventory
{
	uint16 Item;             // item object code
	uint16 Amount;           // amount of this item
	uint16 AmountInUse;      // in-use amount of this item
};

typedef struct
{
	uint16  SavedTimes;             // saved times
	uint16  ViewportX, ViewportY;  // viewport location
	uint16  PartyMember;            // number of members in party
	uint16  NumScene;               // scene number
	uint16  PaletteOffset;
	uint16  PartyDirection;         // party direction
	uint16  NumMusic;               // music number
	uint16  NumBattleMusic;         // battle music number
	uint16  NumBattleField;         // battle field number
	uint16  ScreenWave;             // level of screen waving
	uint16  BattleSpeed;            // battle speed
	uint16  CollectValue;           // value of "collected" items
	uint16  Layer;
	uint16  ChaseRange;
	uint16  ChasespeedChangeCycles;
	uint16  Follower;
	uint16  Reserved2[3];         // unused
	uint32  Cash;                  // amount of cash
	FGDParty  Party[MAX_PLAYABLE_PLAYER_ROLES];       // player party
	FGDTrail  Trail[MAX_PLAYABLE_PLAYER_ROLES];       // player trail
	FExperienceAll Exp;                     // experience data
	FPlayerRoles PlayerRoles;
	FPoisonStatus PoisonStatus[MAX_POISONS][MAX_PLAYABLE_PLAYER_ROLES]; // poison status
	FGDInventory Inventory[MAX_INVENTORY];               // inventory status
	FGDScene Scene[MAX_SCENES];
	FObjectDOS Object[MAX_OBJECTS];
	FEventObject EventObject[MAX_EVENT_OBJECTS];
} FSavedGameDOS;

typedef struct
{
	uint16 SavedTimes;             // saved times
	uint16 ViewportX, ViewportY;  // viewport location
	uint16 PartyMember;            // number of members in party
	uint16 NumScene;               // scene number
	uint16 PaletteOffset;
	uint16 PartyDirection;         // party direction
	uint16 NumMusic;               // music number
	uint16 NumBattleMusic;         // battle music number
	uint16 NumBattleField;         // battle field number
	uint16 ScreenWave;             // level of screen waving
	uint16 BattleSpeed;            // battle speed
	uint16 CollectValue;           // value of "collected" items
	uint16 Layer;
	uint16 ChaseRange;
	uint16 ChasespeedChangeCycles;
	uint16 Follower;
	uint16 Reserved2[3];         // unused
	uint32 Cash;                  // amount of cash
	FGDParty Party[MAX_PLAYABLE_PLAYER_ROLES];       // player party
	FGDTrail Trail[MAX_PLAYABLE_PLAYER_ROLES];       // player trail
	FExperienceAll Exp;                     // experience data
	FPlayerRoles PlayerRoles;
	FPoisonStatus PoisonStatus[MAX_POISONS][MAX_PLAYABLE_PLAYER_ROLES]; // poison status
	FGDInventory Inventory[MAX_INVENTORY];               // inventory status
	FGDScene Scene[MAX_SCENES];
	FObject Object[MAX_OBJECTS];
	FEventObject EventObject[MAX_EVENT_OBJECTS];
} FSavedGame, FSavedGameWin;

class UPALCommon;

/**
 * 
 */
UCLASS()
class PAL_API UPALGameData : public UObject
{
	GENERATED_BODY()

	friend class UPALCommon;
	
public:
	FEventObject* _EventObjects = nullptr;
	SIZE_T EventObjectCount;

	//  game state data has a copy of this
	FGDScene _Scenes[MAX_SCENES];

	FObject _Objects[MAX_OBJECTS];

	FScriptEntry* ScriptEntries = nullptr;
	SIZE_T ScriptEntryCount;

	FStore* Stores = nullptr;
	SIZE_T StoreCount;

	FEnemy* Enemies = nullptr;
	SIZE_T EnemyCount;

	FEnemyTeam* EnemyTeams = nullptr;
	SIZE_T EnemyTeamCount;

	// player state data has a copy of this
	FPlayerRoles _PlayerRoles;

	FMagic* Magics = nullptr;
	SIZE_T MagicCount;

	FBattleField* BattleFields = nullptr;
	SIZE_T BattleFieldCount;

	FLevelUpMagicAll* LevelUpMagicAlls = nullptr;
	SIZE_T LevelUpMagicAllCount;

	FEnemyPos EnemyPos;
	uint16  LevelUpExps[MAX_LEVELS + 1];

	uint16 BattleEffectIndex[10][2];

public:
	virtual void FinishDestroy() override;
};
