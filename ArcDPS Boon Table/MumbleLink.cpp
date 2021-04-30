#include "MumbleLink.h"
#include <cstdint>
//#include <memoryapi.h>
#include <handleapi.h>
#include <memoryapi.h>



namespace MountTOOL
{
	struct LinkedMem
	{
#ifdef WIN32
		UINT32	uiVersion;
		DWORD	uiTick;
#else
		uint32_t uiVersion;
		uint32_t uiTick;
#endif
		fVector3	fAvatarPosition;
		fVector3	fAvatarFront;
		fVector3	fAvatarTop;
		wchar_t		name[256];
		fVector3	fCameraPosition;
		fVector3	fCameraFront;
		fVector3	fCameraTop;
		wchar_t		identity[256];
#ifdef WIN32
		UINT32	context_len;
#else
		uint32_t context_len;
#endif
		unsigned char context[256];
		wchar_t description[2048];
	};

	struct MumbleContext {
		std::byte serverAddress[28]; // contains sockaddr_in or sockaddr_in6
		uint32_t mapId;
		uint32_t mapType;
		uint32_t shardId;
		uint32_t instance;
		uint32_t buildId;
		// Additional data beyond the 48 bytes Mumble uses for identification
		uint32_t uiState; // Bitmask: Bit 1 = IsMapOpen, Bit 2 = IsCompassTopRight, Bit 3 = DoesCompassHaveRotationEnabled, Bit 4 = Game has focus, Bit 5 = Is in Competitive game mode, Bit 6 = Textbox has focus, Bit 7 = Is in Combat
		uint16_t compassWidth; // pixels
		uint16_t compassHeight; // pixels
		float compassRotation; // radians
		float playerX; // continentCoords
		float playerY; // continentCoords
		float mapCenterX; // continentCoords
		float mapCenterY; // continentCoords
		float mapScale;
		uint32_t processId;
		uint8_t mountIndex;
	};

	MumbleLink::MumbleLink() {
		fileMapping_ = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(LinkedMem), L"MumbleLink");
		if (!fileMapping_)
			return;

		linkedMemory_ = static_cast<LinkedMem*>(MapViewOfFile(fileMapping_, FILE_MAP_READ, 0, 0, sizeof(LinkedMem)));
		if (!linkedMemory_)
		{
			CloseHandle(fileMapping_);
			fileMapping_ = nullptr;
		}
	}

	MumbleLink::~MumbleLink() {
		if (linkedMemory_)
		{
			UnmapViewOfFile(linkedMemory_);
			linkedMemory_ = nullptr;
		}
		if (fileMapping_)
		{
			CloseHandle(fileMapping_);
			fileMapping_ = nullptr;
		}
	}

	//void MumbleLink::OnUpdate()
	//{
	//	//identity_ = {};
	//	//auto identityUtf8 = utf8_encode(linkedMemory_->identity);
	//	//auto json = nlohmann::json::parse(identityUtf8, nullptr, false);

	//	//auto updateIfExists = [&json](auto& value, const char* key)
	//	//{
	//	//	auto f = json.find(key);
	//	//	if (f != json.end())
	//	//		value = *f;
	//	//};

	//	//updateIfExists(identity_.commander, "commander");
	//	//updateIfExists(identity_.fov, "fov");
	//	//updateIfExists(identity_.uiScale, "uisz");
	//	//updateIfExists(identity_.race, "race");
	//	//updateIfExists(identity_.specialization, "spec");
	//	//updateIfExists(identity_.profession, "profession");
	//	//updateIfExists(identity_.name, "name");
	//}

	bool MumbleLink::isInMap() const {
		if (!linkedMemory_)
			return false;

		return context()->mapId != 0;
	}

	uint32_t MumbleLink::mapId() const {
		if (!linkedMemory_)
			return 0;

		return context()->mapId;
	}

	//std::wstring MumbleLink::characterName() const
	//{
	//	if (!linkedMemory_)
	//		return L"";

	//	return utf8_decode(identity_.name);
	//}

	const float MinSurfaceThreshold = -1.15f;

	bool MumbleLink::isSwimmingOnSurface() const {
		if (!linkedMemory_)
			return false;

		return linkedMemory_->fAvatarPosition.y <= -1.f && linkedMemory_->fAvatarPosition.y >= MinSurfaceThreshold;
	}

	bool MumbleLink::isFallingThroughTheSky() const {
		if (!linkedMemory_)
			return false;

		return linkedMemory_->fAvatarPosition.y >= 3.f && linkedMemory_->fAvatarPosition.y >= MinSurfaceThreshold;
	}

	fVector3 MumbleLink::danqiangaodu0() const {
		if (!linkedMemory_)
			return {0,0,0};
		return linkedMemory_->fAvatarFront;
	}

	fVector3 MumbleLink::danqiangaodu1() const {
		if (!linkedMemory_)
			return { 0,0,0 };
		return linkedMemory_->fAvatarPosition;
	}

	fVector3 MumbleLink::danqiangaodu2() const {
		if (!linkedMemory_)
			return { 0,0,0 };
		return linkedMemory_->fAvatarTop;
	}

	bool MumbleLink::isUnderwater() const {
		if (!linkedMemory_)
			return false;

		return linkedMemory_->fAvatarPosition.y < MinSurfaceThreshold;
	}


	bool MumbleLink::isInWvW() const {
		if (!linkedMemory_)
			return false;

		auto mt = context()->mapType;

		return mt == 18 || (mt >= 9 && mt <= 15 && mt != 13);
	}

	uint32_t MumbleLink::uiState() const {
		if (!linkedMemory_)
			return 0;

		return context()->uiState;
	}

	fVector3 MumbleLink::position() const {
		if (!linkedMemory_)
			return { 0, 0, 0 };

		return linkedMemory_->fAvatarPosition;
	}

	int MumbleLink::currentMount() const {
		if (!linkedMemory_)
			return -1;

		enum class AnetMountType : uint32_t {
			None,
			Jackal,
			Griffon,
			Springer,
			Skimmer,
			Raptor,
			RollerBeetle,
			Warclaw,
			Dragon,
		};
		//	NONE = 0xFFFFFFFF,
//	RAPTOR = ID_Raptor,
//	SPRINGER = ID_Springer,
//	SKIMMER = ID_Skimmer,
//	JACKAL = ID_Jackal,
//	GRIFFON = ID_Griffon,
//	BEETLE = ID_Beetle,
//	WARCLAW = ID_Warclaw,
//	SKYSCALE = ID_Skyscale,
		switch ((AnetMountType)context()->mountIndex) {
		default:
		case AnetMountType::None:
			return -1;
		case AnetMountType::Raptor:
			return 0;
		case AnetMountType::Springer:
			return 1;
		case AnetMountType::Skimmer:
			return 2;
		case AnetMountType::Jackal:
			return 3;
		case AnetMountType::Griffon:
			return 4;
		case AnetMountType::RollerBeetle:
			return 5;
		case AnetMountType::Warclaw:
			return 6;
		case AnetMountType::Dragon:
			return 7;
		}
	}

	bool MumbleLink::isMounted() const {
		if (!linkedMemory_)
			return false;

		return context()->mountIndex != 0;
	}

	const MumbleContext* MumbleLink::context() const {
		if (!linkedMemory_)
			return nullptr;

		return reinterpret_cast<const MumbleContext*>(&linkedMemory_->context);
	}

    bool MumbleLink::isPvP() const
    {
        if (!linkedMemory_)
            return false;

        auto mt = context()->mapType;
        //5 迷雾之心
        //2 房间
        return mt == 2;
    }

	int MumbleLink::themaptype() const {
		if (!linkedMemory_)
			return -100;
		//5 迷雾之心
		//2 房间
		return context()->mapType;
	}
}
