#ifndef __DEFINES__
#define __DEFINES__

#define ACTION_TYPE_MOVE   (1)
#define ACTION_TYPE_ROTATE (3)
#define ACTION_TYPE_ATTACK (6)
#define ACTION_TYPE_USE_SKILL (10)

// Parameters
/// Stats
#define STAT_BEGIN                 (0)
#define STAT_END                   (199)
#define STAT_COUNT                 (STAT_END-STAT_BEGIN+1)
#define STAT_EXT_BEGIN             (32)
#define STAT_EXT_END               (63)
#define ST_STRENGTH                 (0) // Used in engine +++
#define ST_PERCEPTION               (1)
#define ST_ENDURANCE                (2)
#define ST_CHARISMA                 (3)
#define ST_INTELLECT                (4)
#define ST_AGILITY                  (5)
#define ST_LUCK                     (6)
#define ST_MAX_LIFE                 (7)
#define ST_ACTION_POINTS            (8)
#define ST_ARMOR_CLASS              (9)
#define ST_MELEE_DAMAGE             (10)
#define ST_CARRY_WEIGHT             (11)
#define ST_SEQUENCE                 (12)
#define ST_HEALING_RATE             (13)
#define ST_CRITICAL_CHANCE          (14)
#define ST_MAX_CRITICAL             (15)
#define ST_NORMAL_ABSORB            (16)
#define ST_LASER_ABSORB             (17)
#define ST_FIRE_ABSORB              (18)
#define ST_PLASMA_ABSORB            (19)
#define ST_ELECTRO_ABSORB           (20)
#define ST_EMP_ABSORB               (21)
#define ST_EXPLODE_ABSORB           (22)
#define ST_NORMAL_RESIST            (23)
#define ST_LASER_RESIST             (24)
#define ST_FIRE_RESIST              (25)
#define ST_PLASMA_RESIST            (26)
#define ST_ELECTRO_RESIST           (27)
#define ST_EMP_RESIST               (28)
#define ST_EXPLODE_RESIST           (29)
#define ST_RADIATION_RESISTANCE     (30)
#define ST_POISON_RESISTANCE        (31)
#define ST_STRENGTH_EXT             (32)
#define ST_PERCEPTION_EXT           (33)
#define ST_ENDURANCE_EXT            (34)
#define ST_CHARISMA_EXT             (35)
#define ST_INTELLECT_EXT            (36)
#define ST_AGILITY_EXT              (37)
#define ST_LUCK_EXT                 (38)
#define ST_MAX_LIFE_EXT             (39)
#define ST_ACTION_POINTS_EXT        (40)
#define ST_ARMOR_CLASS_EXT          (41)
#define ST_MELEE_DAMAGE_EXT         (42)
#define ST_CARRY_WEIGHT_EXT         (43)
#define ST_SEQUENCE_EXT             (44)
#define ST_HEALING_RATE_EXT         (45)
#define ST_CRITICAL_CHANCE_EXT      (46)
#define ST_MAX_CRITICAL_EXT         (47)
#define ST_NORMAL_ABSORB_EXT        (48)
#define ST_LASER_ABSORB_EXT         (49)
#define ST_FIRE_ABSORB_EXT          (50)
#define ST_PLASMA_ABSORB_EXT        (51)
#define ST_ELECTRO_ABSORB_EXT       (52)
#define ST_EMP_ABSORB_EXT           (53)
#define ST_EXPLODE_ABSORB_EXT       (54)
#define ST_NORMAL_RESIST_EXT        (55)
#define ST_LASER_RESIST_EXT         (56)
#define ST_FIRE_RESIST_EXT          (57)
#define ST_PLASMA_RESIST_EXT        (58)
#define ST_ELECTRO_RESIST_EXT       (59)
#define ST_EMP_RESIST_EXT           (60)
#define ST_EXPLODE_RESIST_EXT       (61)
#define ST_RADIATION_RESISTANCE_EXT (62)
#define ST_POISON_RESISTANCE_EXT    (63) // Used in engine ---
#define ST_TOXIC                    (64)
#define ST_RADIOACTIVE              (65)
#define ST_KILL_EXPERIENCE          (66)
#define ST_BODY_TYPE                (67)
#define ST_LOCOMOTION_TYPE          (68) // See Locomotion types
#define ST_DAMAGE_TYPE              (69)
#define ST_AGE                      (70) // Used in engine
#define ST_GENDER                   (71) // Used in engine
#define ST_CURRENT_HP               (72) // Used in engine
#define ST_POISONING_LEVEL          (73) // Used in engine
#define ST_RADIATION_LEVEL          (74) // Used in engine
#define ST_CURRENT_AP               (75) // Used in engine
#define ST_EXPERIENCE               (76) // Used in engine
#define ST_LEVEL                    (77) // Used in engine
#define ST_UNSPENT_SKILL_POINTS     (78) // Used in engine
#define ST_UNSPENT_PERKS            (79) // Used in engine
#define ST_KARMA                    (80) // Used in engine
#define ST_FOLLOW_CRIT              (81) // Used in engine
#define ST_REPLICATION_MONEY        (82) // Used in engine
#define ST_REPLICATION_COUNT        (83) // Used in engine
#define ST_REPLICATION_TIME         (84) // Used in engine
#define ST_REPLICATION_COST         (85) // Used in engine
#define ST_TURN_BASED_AC            (86) // Used in engine
#define ST_MAX_MOVE_AP              (87) // Used in engine
#define ST_MOVE_AP                  (88) // Used in engine
#define ST_NPC_ROLE                 (89) // Used in engine
#define ST_VAR0                     (90)
#define ST_VAR1                     (91)
#define ST_VAR2                     (92)
#define ST_VAR3                     (93)
#define ST_VAR4                     (94)
#define ST_VAR5                     (95)
#define ST_VAR6                     (96)
#define ST_VAR7                     (97)
#define ST_VAR8                     (98)
#define ST_VAR9                     (99)
#define ST_PLAYER_KARMA             (100)
#define ST_BONUS_LOOK               (101) // Used in engine
#define ST_HANDS_ITEM_AND_MODE      (102) // Used in engine
#define ST_FREE_BARTER_PLAYER       (103) // Used in engine
#define ST_DIALOG_ID                (104) // Used in engine
#define ST_AI_ID                    (105) // Used in engine
#define ST_TEAM_ID                  (106) // Used in engine
#define ST_BAG_ID                   (107) // Used in engine
#define ST_LAST_STEAL_CR_ID         (108)
#define ST_STEAL_COUNT              (109)
#define ST_LAST_WEAPON_ID           (110) // Used in engine
#define ST_LAST_WEAPON_USE          (111)
#define ST_BASE_CRTYPE              (112) // Used in engine
#define ST_DEAD_BLOCKER_ID          (113)
#define ST_CURRENT_ARMOR_PERK       (114)
#define ST_TALK_DISTANCE            (115) // Used in engine; if zero than taked __TalkDistance
#define ST_SCALE_FACTOR             (116) // Used in engine
#define ST_WALK_TIME                (117) // Hardcoded
#define ST_RUN_TIME                 (118) // Hardcoded
#define ST_MAX_TALKERS              (119) // Hardcoded
// 120..129 reserved for hardcoded values

// post-wipe todo: move those to 130+ to avoid conflicts with new hardcoded values
#define ST_FACTION_RANK             (121) // former tens digit of teamid
#define ST_FACTION_MODE             (122) // former units digit of teamid
// 121..150
#define ST_REP_DECAY				(130) // for slow reputation decay in critter_idle
#define ST_SCENARIO			        (131) // index of the scenario critter participates in actually (it was rather for some prototype, not used currently)
#define ST_WEAPON_BLUEPRINTS        (132)
#define ST_ARMOR_BLUEPRINTS         (133)
#define ST_MISC_BLUEPRINTS          (134)
#define ST_EXT_SNEAK                (135) // extra sneak from armour, for faster check_look
#define ST_HEALTH_LEVEL             (136) // critter damage level; updated on hp change, sent to process simple look clientside
#define ST_SNEAK_FLAGS              (137) // has stealth boy on
#define ST_DESCRIPTION1				(138)
#define ST_DESCRIPTION2				(139)
#define ST_DEFAULT_ARMOR_PID		(140)
#define ST_DEFAULT_HELMET_PID		(141)
#define ST_OVERRIDE_CRTYPE			(143)
#define ST_TURN_BASED_HEX			(144)
#define ST_CURRENT_HELMET_PERK		(145) // we're running out of space here :)
#define ST_FACTION_UPDATE_SEQ		(146)
#define ST_MINIGAME_DATA            (147) // custom minigame data (first byte is reserved to store minigame team and id)

	// Initial values of animation 3d layers
#ifdef PLAYERS_3D
#define ST_ANIM3D_LAYERS            (150)
#endif
// 150..179 reserved for 30 layers
// 180..199

	/// Skills
#define SKILL_BEGIN                (__SkillBegin)
#define SKILL_END                  (__SkillEnd)
#define SKILL_COUNT                (SKILL_END-SKILL_BEGIN+1)
#define MAX_SKILL_VAL              (__SkillMaxValue)
#define SK_SMALL_GUNS               (200) // Used in engine
#define SK_BIG_GUNS                 (201) // Used in engine
#define SK_ENERGY_WEAPONS           (202) // Used in engine
#define SK_UNARMED                  (203) // Used in engine
#define SK_MELEE_WEAPONS            (204) // Used in engine
#define SK_THROWING                 (205) // Used in engine
#define SK_FIRST_AID                (206) // Used in engine
#define SK_DOCTOR                   (207) // Used in engine
#define SK_SNEAK                    (208) // Used in engine
#define SK_LOCKPICK                 (209) // Used in engine
#define SK_STEAL                    (210) // Used in engine
#define SK_TRAPS                    (211) // Used in engine
#define SK_SCIENCE                  (212) // Used in engine
#define SK_REPAIR                   (213) // Used in engine
#define SK_SPEECH                   (214) // Used in engine
#define SK_BARTER                   (215) // Used in engine
#define SK_GAMBLING                 (216)
#define SK_OUTDOORSMAN              (217) // Used in engine
// 218..225

	/// Tag skills
#define TAG_BEGIN					(226)
#define TAG_END						(229)
#define TAG_SKILL1                  (226) // Used in engine
#define TAG_SKILL2                  (227) // Used in engine
#define TAG_SKILL3                  (228) // Used in engine
#define TAG_SKILL4                  (229) // Used in engine

	/// Timeouts
#define TIMEOUT_BEGIN              (__TimeoutBegin)
#define TIMEOUT_END                (__TimeoutEnd)
#define TIMEOUT_COUNT              (TIMEOUT_END-TIMEOUT_BEGIN+1)
#define TO_SK_FIRST_AID             (230)
#define TO_SK_DOCTOR                (231)
#define TO_SK_REPAIR                (232) // Used in engine
#define TO_SK_SCIENCE               (233) // Used in engine
#define TO_SK_LOCKPICK              (234)
#define TO_SK_STEAL                 (235)
#define TO_WEAKENED					(236)
#define TO_FIXBOY					(237)
#define TO_BATTLE                   (238) // Used in engine
#define TO_TRANSFER                 (239) // Used in engine
#define TO_REMOVE_FROM_GAME         (240) // Used in engine
#define TO_REPLICATION              (241)
#define TO_KARMA_VOTING             (242) // Used in engine
#define TO_GATHERING				(243)
#define TO_SNEAK                    (244)
#define TO_HEALING                  (245)
#define TO_SLEEPY                   (246)

// 246..249
#define TO_AGGRESSOR                (249)
#define TO_SLEEPY_STOPPED			(250)
#define TO_SLEEPY_CUMULATE			(251)
#define TO_IMMUNITY_TIME			(252)

// 253..259

	/// Kills
// Reserved 260..299 (BT_MEN..BT_BIG_BAD_BOSS)
#define KILL_BEGIN                 (__KillBegin)
#define KILL_END                   (__KillEnd)
#define KILL_COUNT                 (KILL_END-KILL_BEGIN+1)
// 279..299

#define GAMESTATE_MENU       (1)
#define GAMESTATE_GAME       (5)
#define GAMESTATE_WM         (6)
#define GAMESTATE_LOADING    (7)

#define STATE_MOVE_WM        (0)
#define STATE_ATTACK_CRITTER (1)
#define STATE_FIRST_AID      (2)

#define CRITTER_PLAYER              (0x00010000)
#define CRITTER_NPC                 (0x00020000)
#define CRITTER_DISCONNECT          (0x00080000)
#define CRITTER_CHOSEN              (0x00100000)
#define CRITTER_RULEGROUP           (0x00200000)

#define MSGTYPE_DOT_LIGHTGREEN (1)
#define MSGTYPE_DOT_RED		   (2)
#define MSGTYPE_DOT_DARKGREEN  (3)
#define MSGTYPE_NO_DOT		   (4)

#define FT_NOBREAK                 ( 0x0001 )
#define FT_NOBREAK_LINE            ( 0x0002 )
#define FT_CENTERX                 ( 0x0004 )
#define FT_CENTERY                 ( 0x0008 )
#define FT_CENTERR                 ( 0x0010 )
#define FT_BOTTOM                  ( 0x0020 )
#define FT_UPPER                   ( 0x0040 )
#define FT_NO_COLORIZE             ( 0x0080 )
#define FT_ALIGN                   ( 0x0100 )
#define FT_BORDERED                ( 0x0200 )

#define FONT_FO                (0)
#define FONT_NUM               (1)
#define FONT_BIG_NUM           (2)
#define FONT_SAND_NUM          (3)
#define FONT_SPECIAL           (4)
#define FONT_DEFAULT           (5)
#define FONT_THIN              (6)
#define FONT_FAT               (7)
#define FONT_BIG               (8)

#endif