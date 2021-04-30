#pragma once
#include "imgui/imgui.h"
#include <windows.h>
#include <string>
#include "resource.h"
#define PARSE_FLAG_BOOL(name, offset) [[nodiscard]] inline bool name() const { return (uiState() & (1 << offset)) != 0; }

namespace MountTOOL
{
//	class MumbleLink
//	{
//	public:
//		MumbleLink();
//		~MumbleLink();
//
//		bool isWvW() const;
//		bool isPvP() const;
//		unsigned int id() const;
//		unsigned int type() const;
//		ImVec4 pxy() const;
//
//		PARSE_FLAG_BOOL(isInCombat, 6);
//
//	protected:
//		const struct MumbleContext* context() const;
//		HANDLE fileMapping_ = nullptr;
//		struct LinkedMem* linkedMemory_ = nullptr;
//	};
//
	//typedef unsigned int             uint;
	//enum class MountType : uint
	//{
	//	NONE = 0xFFFFFFFF,
	//	RAPTOR = ID_Raptor,
	//	SPRINGER = ID_Springer,
	//	SKIMMER = ID_Skimmer,
	//	JACKAL = ID_Jackal,
	//	GRIFFON = ID_Griffon,
	//	BEETLE = ID_Beetle,
	//	WARCLAW = ID_Warclaw,
	//	SKYSCALE = ID_Skyscale,

	//	FIRST = RAPTOR,
	//	LAST = SKYSCALE
	//};

	//const unsigned int MountTypeCount = 7;

	typedef struct fVector3 {
		float x;
		float y;
		float z;
	}         fVector3;

	struct LinkedMem;
	struct MumbleContext;

	class MumbleLink
	{
	public:
		//enum class Profession : uint8_t {
		//	NONE = 0,
		//	GUARDIAN = 1,
		//	WARRIOR = 2,
		//	ENGINEER = 3,
		//	RANGER = 4,
		//	THIEF = 5,
		//	ELEMENTALIST = 6,
		//	MESMER = 7,
		//	NECROMANCER = 8,
		//	REVENANT = 9
		//};

		//enum class Race : uint8_t {
		//	ASURA = 0,
		//	CHARR = 1,
		//	HUMAN = 2,
		//	NORN = 3,
		//	SYLVARI = 4
		//};

		MumbleLink();
		~MumbleLink();

		//void OnUpdate();

		[[nodiscard]] bool isInWvW() const;
		[[nodiscard]] bool isPvP() const;

		[[nodiscard]] int themaptype() const;

		// uiState flags
		PARSE_FLAG_BOOL(isMapOpen, 0);//打开了地图
		PARSE_FLAG_BOOL(isCompassTopRight, 1);//是罗盘的右上方
		PARSE_FLAG_BOOL(doesCompassHaveRotationEnabled, 2);//指南针是否启用了旋转
		PARSE_FLAG_BOOL(gameHasFocus, 3);//正在游戏界面
		PARSE_FLAG_BOOL(isInCompetitiveMode, 4);
		PARSE_FLAG_BOOL(textboxHasFocus, 5); //正在使用文本框
		PARSE_FLAG_BOOL(isInCombat, 6);//战斗中

		[[nodiscard]] fVector3 position() const;

		[[nodiscard]] int currentMount() const;//当前坐骑
		[[nodiscard]] bool isMounted() const; //正在使用坐骑
		[[nodiscard]] bool isInMap() const;
		
		[[nodiscard]] uint32_t mapId() const;
		//[[nodiscard]] std::wstring characterName() const;
		[[nodiscard]] bool isSwimmingOnSurface() const; //在游泳
		[[nodiscard]] bool isUnderwater() const; //在水面上
		[[nodiscard]] bool isFallingThroughTheSky() const; //在天空中坠落

		[[nodiscard]] fVector3 danqiangaodu0() const;
		[[nodiscard]] fVector3 danqiangaodu1() const;
		[[nodiscard]] fVector3 danqiangaodu2() const;

		//[[nodiscard]] Profession characterProfession() const {
		//	return identity_.profession;
		//}//职业

		//[[nodiscard]] uint8_t characterSpecialization() const {
		//	return identity_.specialization;
		//}//专精

		//[[nodiscard]] Race characterRace() const {
		//	return identity_.race;
		//}//种族

		//[[nodiscard]] bool isCommander() const {
		//	return identity_.commander;
		//}//指挥官

		//[[nodiscard]] float fov() const {
		//	return identity_.fov;
		//}

		//[[nodiscard]] uint8_t uiScale() const {
		//	return identity_.uiScale;
		//}

	protected:
		[[nodiscard]] uint32_t uiState() const;

		//struct Identity
		//{
		//	//Profession profession = Profession::NONE;
		//	uint8_t specialization = 0;
		//	//Race race = Race::ASURA;
		//	bool commander = false;
		//	float fov = 0.f;
		//	uint8_t uiScale = 0;
		//	std::string name;
		//};

		[[nodiscard]] const MumbleContext* context() const;
		HANDLE fileMapping_ = nullptr;
		LinkedMem* linkedMemory_ = nullptr;

		//Identity identity_;
	};
}

