#pragma once
#include "extension/Icon.h"
#include <vector>
#include <string>
#include "imgui/imgui.h"
#include "simpleini/SimpleIni.h"
#include <optional>
#include "MumbleLink.h"
#include <commctrl.h>
#include "imgui_imm32_onthespot.h"


namespace MountTOOL 
{

	/** initialize windows common controls. */



	class HOMEUI
	{
	public:
		enum StackingType
		{
			Type_PVE,
			Type_WVW
		};

		struct BoonDef
		{
			std::vector<uint32_t> ids = {};
			std::string name = "";
			StackingType stacking_type = Type_PVE;
			Icon* icon;
			int kays;
			BoonDef(std::vector<uint32_t> new_ids, std::string new_name, StackingType new_stacking_type, Icon* new_icon, int new_kay);
		};


		std::vector<BoonDef> tracked_buffs;

		
		HOMEUI();
		~HOMEUI();


		void init_tracked_buffs(IDirect3DDevice9* d3d9device);
		
		void setwindows(HWND windo);
		
		void LoadConfgs();
		void SaveConfgs();

		bool __CheckLocale();
		

		float UI_disc_size = 1.0f;
		bool settingUI = false;
		HMODULE self_dll;
		//The key sequence number to be set
		int sequence_number_of_kay = -1;
		bool lockmouse = false;
		bool usPreferencesMount = false;
		int usPreferencesMount_choose = -1;
		int open_mount_ui = 0;
		int selected = -1;
		ImVec2 pie_menu_center;
		int postkayinx = -1;
		
		std::optional<ImVec4> ICON_color;
		std::optional<ImVec4> Mount_BG_color;
		float self_color[4]{};
		float self_color_BG[4]{};

		
		//bool canmount = true;
		
		bool ispaylercomrt() { return _MumbleLink.isInCombat(); };
		void Display_ArcDpsOptions();
		void Display_ArcDpsMountUI();
		int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected);

		ImGUIIMMCommunication imguiIMMCommunication{};
		

		static int common_control_initialize();

	private:
		CSimpleIniA table_ini;
		MumbleLink _MumbleLink;
		HWND window;
		
	};
}


