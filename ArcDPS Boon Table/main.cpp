/*
* arcdps combat api example
*/

#include <cstdint>
#include <Windows.h>
#include <string>
#include <regex>
#include <d3d9.h>

#include "imgui/imgui.h"
#include "extension/arcdps_structs.h"
#include "HOMEUI.h"
#include "imgex.hpp"

#define __VERSION__ (__DATE__)

/* proto/globals */
arcdps_exports arc_exports{};
char* arcvers;
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, void* imguicontext, IDirect3DDevice9* id3dd9, HMODULE arcdll, void* mallocfn, void* freefn);
extern "C" __declspec(dllexport) void* get_release_addr();
arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision);
uintptr_t mod_imgui(uint32_t not_charsel_or_loading); /* id3dd9::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
uintptr_t mod_options(); /* id3dd9::present callback, appending to the end of options window in arcdps, fn() */
uintptr_t mod_options_windows(const char* windowname); // fn(char* windowname) 
void readArcExports();
bool modsPressed();
bool canMoveWindows();

typedef uint64_t(*arc_export_func_u64)();

HMODULE arc_dll;
//HMODULE self_dll;
IDirect3DDevice9* id3dd9;

// get exports
arc_export_func_u64 arc_export_e6;
arc_export_func_u64 arc_export_e7;


// arc globals
WPARAM arc_global_mod1;
WPARAM arc_global_mod2;
WPARAM arc_global_mod_multi;
bool arc_hide_all = false;
bool arc_panel_always_draw = false;
bool arc_movelock_altui = false;
bool arc_clicklock_altui = false;
bool arc_window_fastclose = false;
MountTOOL::HOMEUI homeui;





/* dll main -- winapi */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved) {
	switch (ulReasonForCall) {
	case DLL_PROCESS_ATTACH:
		homeui.self_dll = hModule;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return 1;
}

/* export -- arcdps looks for this exported function and calls the address it returns */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversionstr, void* imguicontext, IDirect3DDevice9* new_id3dd9, HMODULE new_arcdll, void* mallocfn, void* freefn) {
	// set all arcdps stuff
	arcvers = arcversionstr;
	arc_dll = new_arcdll;
	arc_export_e6 = (arc_export_func_u64)GetProcAddress(arc_dll, "e6");
	arc_export_e7 = (arc_export_func_u64)GetProcAddress(arc_dll, "e7");
	// set imgui context && allocation for arcdps dll space
	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(imguicontext));
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))mallocfn, (void (*)(void*, void*))freefn);

	id3dd9 = new_id3dd9;

	return mod_init;
}

/* export -- arcdps looks for this exported function and calls the address it returns */
extern "C" __declspec(dllexport) void* get_release_addr() {
	arcvers = 0;
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks */
arcdps_exports* mod_init()
{
	bool loading_successful = true;
	std::string error_message = "Unknown error";
	
	try {
		// 加载配置
		//settings.readFromFile();

		//加载图片
		homeui.init_tracked_buffs(id3dd9);
		homeui.LoadConfgs();
	} catch (std::exception& e) {
		loading_successful = false;
		error_message = "Error starting up: ";
		error_message.append(e.what());
	}
	
	/* for arcdps */
	arc_exports.imguivers = IMGUI_VERSION_NUM;

	if (homeui.__CheckLocale())
	{
		arc_exports.out_name = "坐骑插件V1-03";
	}
	else
	{
		arc_exports.out_name = "Mount plug-in V1-03";
	}
	
	arc_exports.out_build = __VERSION__;

	if (loading_successful) {
		arc_exports.size = sizeof(arcdps_exports);
		arc_exports.sig = 0x64703268;//from random.org
		arc_exports.wnd_nofilter = mod_wnd;
		arc_exports.combat = mod_combat;
		arc_exports.imgui = mod_imgui;
		arc_exports.options_end = mod_options;
		//arc_exports.options_windows = mod_options_windows;
	} else {
		arc_exports.sig = 0;
		const std::string::size_type size = error_message.size();
		char* buffer = new char[size + 1]; //we need extra char for NUL
		memcpy(buffer, error_message.c_str(), size + 1);
		arc_exports.size = (uintptr_t)buffer;
	}
	return &arc_exports;
}

/* release mod -- return ignored */
uintptr_t mod_release()
{
	return 0;
}

bool setting_to_wiatkay = false;
bool keywait = false;
int wiatkeyid = -1;
bool havecontt = false;
/* window callback -- return is assigned to umsg (return zero to not be processed by arcdps or game) */
uintptr_t mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (homeui.settingUI)
	{
		if (!havecontt)
		{
			havecontt = true;
			homeui.imguiIMMCommunication.subclassify(hWnd);
			//;
		}
	}
	else
	{
		havecontt = false;
		//homeui.imguiIMMCommunication.~ImGUIIMMCommunication();
	}

	//return uMsg;
	auto const io = &ImGui::GetIO();

	switch (uMsg)
	{
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		const int vkey = (int)wParam;
		io->KeysDown[vkey] = 0;
		if (vkey == VK_CONTROL)
		{
			io->KeyCtrl = false;
		}
		else if (vkey == VK_MENU)
		{
			io->KeyAlt = false;
		}
		else if (vkey == VK_SHIFT)
		{
			io->KeyShift = false;
		}

		if (vkey != VK_LBUTTON && homeui.sequence_number_of_kay != -1)
		{
			homeui.tracked_buffs[homeui.sequence_number_of_kay].kays = vkey;
			homeui.sequence_number_of_kay = -1;
		}


		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		const int vkey = (int)wParam;
		io->KeysDown[vkey] = 1;
		if (vkey == VK_CONTROL)
		{
			io->KeyCtrl = true;
		}
		else if (vkey == VK_MENU)
		{
			io->KeyAlt = true;
		}
		else if (vkey == VK_SHIFT)
		{
			io->KeyShift = true;
		}
		break;
	}
	case WM_ACTIVATEAPP:
	{
		if (!wParam)
		{
			io->KeysDown[arc_global_mod1] = false;
			io->KeysDown[arc_global_mod2] = false;
		}
		break;
	}
	}

	if (io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2])
	{
		//if (io->KeysDown[settings.getTableKey()]) return 0;
	}

	

	return uMsg;
}

/* combat callback -- may be called asynchronously. return ignored */
/* one participant will be party/squad, or minion of. no spawn statechange events. despawn statechange only on marked boss npcs */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision) {
	if (!ev) {

	}
	else {
		/* statechange */
		if (ev->is_statechange) {
			//if (ev->is_statechange == CBTS_ENTERCOMBAT) {
			//	//进入战斗
			//	homeui.ispaylercomrt = true;
			//}
			//else if (ev->is_statechange == CBTS_EXITCOMBAT) {
			//	//离开战斗
			//	homeui.ispaylercomrt = false;
			//}
			//else if (ev->is_statechange == CBTS_STATRESET) {
			//	//状态重置
			//	homeui.ispaylercomrt = false;
			//	//homeui.canmount = true;
			//}
			//else if (ev->is_statechange == CBTS_MAPID)
			//{
			//	homeui.mapID = (int)ev->src_agent;
			//}
		}
		/* activation */
		else if (ev->is_activation) {
		}


		//else if (ev->is_buffremove)
		//{
		//	if (ev->skillid == 43406)//if it's stability
		//	{
		//		homeui.canmount = true;
		//	}
		//}
		//else if (ev->buff)
		//{
		//	if (ev->skillid == 43406)//if it's stability
		//	{
		//		homeui.canmount = false;
		//	}
		//}
	}
	return 0;
}



uintptr_t mod_imgui(uint32_t not_charsel_or_loading)
{
	// ImGui::ShowDemoWindow();
	readArcExports();
	homeui.Display_ArcDpsOptions();
	if (homeui.settingUI)
	{
		homeui.imguiIMMCommunication();
	}
	
	if (!not_charsel_or_loading) return 0;

	auto io = &ImGui::GetIO();

	if (io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2])
	{
		//if (ImGui::IsKeyPressed(settings.getTableKey()))
		//{
		//	settings.setShowChart(0, !settings.isShowChart(0));
		//}
	}

	//if (homeui.)
	//{

	//}

	if (ImGui::IsKeyPressed(homeui.tracked_buffs[8].kays)&& !homeui.ispaylercomrt())
	{
		homeui.pie_menu_center = io->MousePos;
		homeui.open_mount_ui = 1;
	}
	if (ImGui::IsKeyPressed(homeui.tracked_buffs[9].kays) && !homeui.ispaylercomrt())
	{
		homeui.open_mount_ui = 2;
	}

	
	homeui.Display_ArcDpsMountUI();

	return 0;
}




uintptr_t mod_options()
{


	if (ImGui::Button(homeui.__CheckLocale()?"坐骑插件设置":"Mounts plugin Settings"))
	{
		homeui.settingUI = !homeui.settingUI;
	}
	return 0;
}

/**
 * @return true to disable this option
 */
uintptr_t mod_options_windows(const char* windowname) {
	//if (!windowname) {
	//	//mod_options();
	//}
	return 0;
}

void readArcExports()
{
	uint64_t e6_result = arc_export_e6();
	uint64_t e7_result = arc_export_e7();

	arc_hide_all = (e6_result & 0x01);
	arc_panel_always_draw = (e6_result & 0x02);
	arc_movelock_altui = (e6_result & 0x04);
	arc_clicklock_altui = (e6_result & 0x08);
	arc_window_fastclose = (e6_result & 0x10);


	uint16_t* ra = (uint16_t*)&e7_result;
	if (ra)
	{
		arc_global_mod1 = ra[0];
		arc_global_mod2 = ra[1];
		arc_global_mod_multi = ra[2];
	}
}

bool modsPressed()
{
	auto io = &ImGui::GetIO();

	return io->KeysDown[arc_global_mod1] && io->KeysDown[arc_global_mod2];
}

bool canMoveWindows()
{
	if (!arc_movelock_altui)
	{
		return true;
	}
	else
	{
		return modsPressed();
	}
}


