#include "HOMEUI.h"
#include "imgui/imgui_internal.h"
#include "resource.h"
#include <sstream>



namespace MountTOOL
{
	HOMEUI::BoonDef::BoonDef(std::vector<uint32_t> new_ids, std::string new_name, StackingType new_stacking_type, Icon* new_icon, int new_kay)
		:ids(new_ids), name(new_name), stacking_type(new_stacking_type), kays(new_kay), icon(new_icon)
	{

	}

	HOMEUI::HOMEUI(): table_ini(true)
	{
		(void)std::locale::global(std::locale(""));
		common_control_initialize();
	}

	HOMEUI::~HOMEUI()
	{
		SaveConfgs();
	}


	std::vector<std::string> langs;



	void HOMEUI::init_tracked_buffs(IDirect3DDevice9* d3d9device) {
		
		if (__CheckLocale())
		{
			langs.push_back("鸟龙");
			langs.push_back("兔子");
			langs.push_back("飞鱼");
			langs.push_back("沙狼");
			langs.push_back("狮鹫");
			langs.push_back("甲虫");
			langs.push_back("战爪");
			langs.push_back("麟龙");
			langs.push_back("在鼠标位置显示");
			langs.push_back("在屏幕中心显示");//9

			langs.push_back("坐骑插件设置");
			langs.push_back("坐骑按键设置:");
			langs.push_back("空");//12
			langs.push_back("设置");//13
			langs.push_back("呼出按键设置:");
			langs.push_back("以上按键只支持单键!!");
			langs.push_back("一般选项设置:");
			langs.push_back("呼出界面大小");//17
			langs.push_back("颜色设置:");
			langs.push_back("图标颜色");//19
			langs.push_back("界面背景颜色");//20
			langs.push_back("使用偏好坐骑");//22
			langs.push_back("偏好");//22
		}
		else
		{
			langs.push_back("Raptor");
			langs.push_back("Springer");
			langs.push_back("Skimmer");
			langs.push_back("Jackal");
			langs.push_back("Griffon");
			langs.push_back("Beetle");
			langs.push_back("Warclaw");
			langs.push_back("Skyscale");
			langs.push_back("Display at mouse position");
			langs.push_back("Display at screen center");

			langs.push_back("Mounts Plugin Settings");
			langs.push_back("Mounts Keys Setting:");
			langs.push_back("Null");
			langs.push_back("Setting");
			langs.push_back("Exhale key setting:");
			langs.push_back("The above key supports single key only!!");
			langs.push_back("General Options Settings:");
			langs.push_back("Exhalation interface size");
			langs.push_back("Color Settings:");
			langs.push_back("Icon color");
			langs.push_back("Background color");
			langs.push_back("Use preference mount");//21
			langs.push_back("preference");//22
		}

		
		
		tracked_buffs.emplace_back(std::vector<uint32_t>{0}, langs[0], Type_PVE, new Icon(ID_Raptor, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{1}, langs[1], Type_PVE, new Icon(ID_Springer, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{2}, langs[2], Type_PVE, new Icon(ID_Skimmer, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{3}, langs[3], Type_PVE, new Icon(ID_Jackal, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{4}, langs[4], Type_PVE, new Icon(ID_Griffon, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{5}, langs[5], Type_PVE, new Icon(ID_Beetle, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{6}, langs[6], Type_WVW, new Icon(ID_Warclaw, self_dll, d3d9device), -1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{7}, langs[7], Type_PVE, new Icon(ID_Skyscale, self_dll, d3d9device), -1);
		//
		tracked_buffs.emplace_back(std::vector<uint32_t>{8}, langs[8], Type_PVE, nullptr ,-1);
		tracked_buffs.emplace_back(std::vector<uint32_t>{9}, langs[9], Type_PVE, nullptr,-1);
		
	}

	std::optional<ImVec4> ImVec4_color_from_string(const std::string& vec4str) {
		// check if string is suitable
		size_t count = std::count(vec4str.begin(), vec4str.end(), '|');
		if (count < 3) {
			// read color invalid
			return std::nullopt;
		}

		std::stringstream stream(vec4str);
		std::string buf;
		float val[4]{};
		// Iterate over 4 first sections split by '|'
		for (int i = 0; std::getline(stream, buf, '|') && i < 4; ++i) {
			val[i] = std::stof(buf);
		}
		return ImVec4(val[0], val[1], val[2], val[3]);
	}

	void HOMEUI::LoadConfgs()
	{
		table_ini.LoadFile("addons\\arcdps\\arcdps_ARCDPS_MOUNT.ini");

		for (auto &i : tracked_buffs)
		{
			std::string pszValue = table_ini.GetValue("allkeys", i.name.c_str(), "-1");
			i.kays = std::stoi(pszValue);
		}


		UI_disc_size = table_ini.GetDoubleValue("general","UI_disc_size",1.0);

		usPreferencesMount = table_ini.GetBoolValue("general","usPreferencesMount",false);
		if (usPreferencesMount)
		{
			usPreferencesMount_choose = std::stoi(table_ini.GetValue("general", "usPreferencesMount_choose", "-1"));
			selected = usPreferencesMount_choose;
		}
		const char* value1 = table_ini.GetValue("colors", "ICON_color", "1.000000|1.000000|1.000000|1.000000");
		ICON_color = ImVec4_color_from_string(value1);

		//float self_color[4]{};
		//float self_color_BG[4]{};
		self_color[0] = ICON_color.value().x;
		self_color[1] = ICON_color.value().y;
		self_color[2] = ICON_color.value().z;
		self_color[3] = ICON_color.value().w;
		const char* value2 = table_ini.GetValue("colors", "Mount_BG_color", "0.000000|0.000000|0.000000|0.250000");
		Mount_BG_color = ImVec4_color_from_string(value2);
		self_color_BG[0] = Mount_BG_color.value().x;
		self_color_BG[1] = Mount_BG_color.value().y;
		self_color_BG[2] = Mount_BG_color.value().z;
		self_color_BG[3] = Mount_BG_color.value().w;
	}

	std::string to_string(const ImVec4& vec4) {
		std::string colorCombined;
		colorCombined.append(std::to_string(vec4.x));
		colorCombined.append("|");
		colorCombined.append(std::to_string(vec4.y));
		colorCombined.append("|");
		colorCombined.append(std::to_string(vec4.z));
		colorCombined.append("|");
		colorCombined.append(std::to_string(vec4.w));
		return colorCombined;
	}

	void HOMEUI::SaveConfgs()
	{
		for (auto &i : tracked_buffs)
		{
			table_ini.SetValue("allkeys", i.name.c_str(), std::to_string(i.kays).c_str());//按键储存
		}
		table_ini.SetDoubleValue("general", "UI_disc_size", UI_disc_size);

		table_ini.SetBoolValue("general", "usPreferencesMount", usPreferencesMount);
		if (usPreferencesMount)
		{
			table_ini.SetValue("general", "usPreferencesMount_choose", std::to_string(usPreferencesMount_choose).c_str());
		}
		if (ICON_color) {
			table_ini.SetValue("colors", "ICON_color", to_string(ICON_color.value()).c_str());
		}
		if (Mount_BG_color) {
			table_ini.SetValue("colors", "Mount_BG_color", to_string(Mount_BG_color.value()).c_str());
		}
		table_ini.SaveFile("addons\\arcdps\\arcdps_ARCDPS_MOUNT.ini");
	}

	//获取电脑语言
	bool HOMEUI::__CheckLocale()
	{
		LCID lcid = GetSystemDefaultLCID();
		return lcid == 0x404 || lcid == 0x804;
	}

	//std::mutex HEAL_TABLE_OPTIONS_MUTEX;
	void HOMEUI::Display_ArcDpsOptions()
	{

		if (!settingUI)
		{
			return;
		}
		ImGui::Begin(langs[10].c_str(), &settingUI);
		//if (ImGui::Button("坐骑插件设置"))
		//{

		//}
		static char tmtmmoo[1024*10000];
		ImGui::InputText("笔记", tmtmmoo,IM_ARRAYSIZE(tmtmmoo));


		ImGui::Text(langs[11].c_str());
		for (auto &trackedBuff : tracked_buffs) {
			if (trackedBuff.ids[0] < 8)
			{
				ImGui::Image(trackedBuff.icon->texture, ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "AA"));
				ImGui::SameLine();
				ImGui::Text(trackedBuff.name.c_str());
				ImGui::SameLine();
				std::string tetxt = "##imput";
				if (trackedBuff.kays == -1)
				{
					tetxt = langs[12] + "##imput" + std::to_string(trackedBuff.ids[0]);
				}
				else
				{
					tetxt += std::to_string(trackedBuff.ids[0]);
				}

				ImGui::SetNextItemWidth(100.0f);
				ImGui::InputText(tetxt.c_str(), std::to_string(trackedBuff.kays).data(), std::to_string(trackedBuff.kays).size(), ImGuiInputTextFlags_ReadOnly);
				std::string tetxt2 = langs[13]+"##settingkey";
				tetxt2 += std::to_string(trackedBuff.ids[0]);

				if (sequence_number_of_kay == -1)
				{
					ImGui::SameLine();
					if (ImGui::Button(tetxt2.c_str()))
					{
						sequence_number_of_kay = trackedBuff.ids[0];
					}
				}
			}

			
		}
		ImGui::Text(langs[14].c_str());
		for (size_t i = 8; i < tracked_buffs.size(); i++)
		{
			ImGui::Text(tracked_buffs[i].name.c_str());
			ImGui::SameLine();
			std::string tetxt = "##imput";
			if (tracked_buffs[i].kays == -1)
			{
				tetxt = langs[12]+"##imput" + std::to_string(tracked_buffs[i].ids[0]);
			}
			else
			{
				tetxt += std::to_string(tracked_buffs[i].ids[0]);
			}
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputText(tetxt.c_str(), std::to_string(tracked_buffs[i].kays).data(), std::to_string(tracked_buffs[i].kays).size(), ImGuiInputTextFlags_ReadOnly);
			std::string tetxt2 = langs[13]+"##settingkey";
			tetxt2 += std::to_string(tracked_buffs[i].ids[0]);
			if (sequence_number_of_kay == -1)
			{
				ImGui::SameLine();
				if (ImGui::Button(tetxt2.c_str()))
				{
					sequence_number_of_kay = tracked_buffs[i].ids[0];
				}
			}
		}

		ImGui::Text(langs[15].c_str());

		//ImGui::Text("当前0 x: %f,y: %f,z: %f", _MumbleLink.danqiangaodu0().x, _MumbleLink.danqiangaodu0().y, _MumbleLink.danqiangaodu0().z);
		//ImGui::Text("当前1 x: %f,y: %f,z: %f", _MumbleLink.danqiangaodu1().x, _MumbleLink.danqiangaodu1().y, _MumbleLink.danqiangaodu1().z);
		//ImGui::Text("当前2 x: %f,y: %f,z: %f", _MumbleLink.danqiangaodu2().x, _MumbleLink.danqiangaodu2().y, _MumbleLink.danqiangaodu2().z);
		//themaptype
		//ImGui::Text("地图类型: %d", _MumbleLink.themaptype());
		ImGui::Text(langs[16].c_str());
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat(langs[17].c_str(), &UI_disc_size, 0.02f, 1.0f, 3.0f, "%02f", 0.01f);

		ImGui::Checkbox(langs[21].c_str(),&usPreferencesMount);
		if (usPreferencesMount)
		{
			const char* contents_type_names[] = { langs[0].c_str(), langs[1].c_str(), langs[2].c_str(),langs[3].c_str(),langs[4].c_str(),langs[5].c_str(),langs[6].c_str(),langs[7].c_str() };
			if (ImGui::Combo(langs[22].c_str(), &usPreferencesMount_choose, contents_type_names, IM_ARRAYSIZE(contents_type_names)))
			{
				selected = usPreferencesMount_choose;
			}
			
		}

		ImGui::Text(langs[18].c_str());
		if (ImGui::ColorEdit4(langs[19].c_str(), self_color, ImGuiColorEditFlags_AlphaBar)) {
			if (ICON_color) {
				ICON_color->x = self_color[0];
				ICON_color->y = self_color[1];
				ICON_color->z = self_color[2];
				ICON_color->w = self_color[3];
			}
			else {
				ICON_color = ImVec4(self_color[0], self_color[1], self_color[2], self_color[3]);
			}
		}
		if (ImGui::ColorEdit4(langs[20].c_str(), self_color_BG,ImGuiColorEditFlags_AlphaBar)) {
			if (Mount_BG_color) {
				Mount_BG_color->x = self_color_BG[0];
				Mount_BG_color->y = self_color_BG[1];
				Mount_BG_color->z = self_color_BG[2];
				Mount_BG_color->w = self_color_BG[3];
			}
			else {
				Mount_BG_color = ImVec4(self_color_BG[0], self_color_BG[1], self_color_BG[2], self_color_BG[3]);
			}
		}
		ImGui::End();
		
	}



	void HOMEUI::Display_ArcDpsMountUI()
	{
		

		if (open_mount_ui == 0 || _MumbleLink.isPvP()|| _MumbleLink.isInCombat())
		{
			return;
		}

		if (!_MumbleLink.gameHasFocus()|| _MumbleLink.isMapOpen()|| _MumbleLink.textboxHasFocus())
		{
			if (ImGui::IsPopupOpen("##piepopup")|| ImGui::IsPopupOpen("##piepopup2"))
			{
				ImGui::CloseCurrentPopup();
			}
			open_mount_ui = 0;
			return;
		}

		if (_MumbleLink.themaptype() == 4 && 
			(_MumbleLink.mapId() == 371 
				|| _MumbleLink.mapId() == 248
				|| _MumbleLink.mapId() == 650
				|| _MumbleLink.mapId() == 647
				|| _MumbleLink.mapId() == 375))
		{
			//家园
			open_mount_ui = 0;
			return;
		}

		if (_MumbleLink.mapId() == 1206)
		{
			//雾锁庇护所
			open_mount_ui = 0;
			return;
		}


		if (_MumbleLink.isMounted() && _MumbleLink.currentMount() != -1)
		{
			keybd_event(tracked_buffs[_MumbleLink.currentMount()].kays, MapVirtualKey(tracked_buffs[_MumbleLink.currentMount()].kays, 0), 0, 0);
			keybd_event(tracked_buffs[_MumbleLink.currentMount()].kays, MapVirtualKey(tracked_buffs[_MumbleLink.currentMount()].kays, 0), KEYEVENTF_KEYUP, 0);
			open_mount_ui = 0;
			return;
		}

		

		const char* items[] = { "鸟龙","兔子","飞鱼","沙狼","甲虫","狮鹫","战爪","麟龙" };

		
		int items_count = sizeof(items) / sizeof(*items);
		if (_MumbleLink.isInWvW() || _MumbleLink.isSwimmingOnSurface() || _MumbleLink.isUnderwater())
		{
			items_count = 1;
		}
		//鼠标位置
		if (open_mount_ui == 1)
		{
			ImGui::OpenPopup("##piepopup");
			postkayinx = PiePopupSelectMenu(pie_menu_center, "##piepopup", items, items_count, &selected);
		}
		//屏幕中心
		if (open_mount_ui == 2)
		{
			ImVec2 pie_menu_center2 = ImGui::GetIO().DisplaySize / 2;
			if (!lockmouse)
			{
				lockmouse = true;
				::SetCursorPos(pie_menu_center2.x, pie_menu_center2.y);
				//mouse_event(MOUSEEVENTF_MOVE, (DWORD)pie_menu_center2.x / 6, (DWORD)pie_menu_center2.y / 6, 0, 0);
			}
			ImGui::OpenPopup("##piepopup2");
			postkayinx = PiePopupSelectMenu(pie_menu_center2, "##piepopup2", items, items_count, &selected);

		}

		if (postkayinx != -1)
		{
			postkayinx = -1;
		}
	}


	int HOMEUI::PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected)
	{
		int ret = -1;

		// FIXME: Missing a call to query if Popup is open so we can move the PushStyleColor inside the BeginPopupBlock (e.g. IsPopupOpen() in imgui.cpp)
		// FIXME: Our PathFill function only handle convex polygons, so we can't have items spanning an arc too large else inner concave edge artifact is too visible, hence the ImMax(7,items_count)
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		if (ImGui::BeginPopup(popup_id, ImGuiWindowFlags_NoBackground))
		{
			const ImVec2 drag_delta = ImVec2(ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y);
			const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

			const ImGuiStyle& style = ImGui::GetStyle();
			const float RADIUS_MIN = 30.0f * UI_disc_size;
			const float RADIUS_MAX = 120.0f * UI_disc_size;
			const float RADIUS_INTERACT_MIN = 20.0f;
			const int ITEMS_MIN = 6;

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			//ImGuiWindow* window = ImGui::GetCurrentWindow();
			draw_list->PushClipRectFullScreen();
			draw_list->PathArcTo(center, (RADIUS_MIN + RADIUS_MAX) * 0.5f, 0.0f, IM_PI * 2.0f * 0.99f, 720);   // FIXME: 0.99f look like full arc with closed thick stroke has a bug now
			draw_list->PathStroke(ImGui::ColorConvertFloat4ToU32(Mount_BG_color.value()), true, RADIUS_MAX - RADIUS_MIN);

			const float item_arc_span = 2 * IM_PI / ImMax(ITEMS_MIN, items_count);
			float drag_angle = atan2f(drag_delta.y, drag_delta.x);
			if (drag_angle < -0.5f * item_arc_span)
				drag_angle += 2.0f * IM_PI;
			//ImGui::Text("%f", drag_angle);    // [Debug]

			int item_hovered = -1;

			if (items_count == 1)
			{
				int item_n = 0;
				const char* item_label = items[item_n];
				const float item_ang_min = item_arc_span * (item_n + 0.02f) - item_arc_span * 0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
				const float item_ang_max = item_arc_span * (item_n + 0.98f) - item_arc_span * 0.5f;

				bool hovered = false;
				if (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN)
				{
					if (drag_angle >= item_ang_min && drag_angle < item_ang_max)
						hovered = true;
				}
				bool selected = p_selected && (*p_selected == item_n);

				int arc_segments = (int)(32 * item_arc_span / (2 * IM_PI)) + 1;
				draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max, arc_segments);
				draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min, arc_segments);
				draw_list->PathFillConvex(hovered ? ImColor(100, 100, 150, 110) : selected ? ImColor(120, 120, 140, 200) : ImColor(70, 70, 70, 125));

				//ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "AA");
				ImVec2 text_size(52 * UI_disc_size, 52 * UI_disc_size);
				ImVec2 text_pos = ImVec2(
					center.x + cosf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
					center.y + sinf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);
				
				int havetomountit = 0;

				if (_MumbleLink.isInWvW())
				{
					havetomountit = 6;//启动战爪
				}
				if (_MumbleLink.isSwimmingOnSurface() || _MumbleLink.isUnderwater())
				{
					havetomountit = 2;//启动飞鱼
				}

				draw_list->AddImage(tracked_buffs[havetomountit].icon->texture, text_pos, ImVec2{ text_pos.x + text_size.x  ,text_pos.y + text_size.x }, { 0,0 }, { 1,1 }, ImGui::ColorConvertFloat4ToU32(ICON_color.value()));
				if (hovered)
					item_hovered = havetomountit;
			}
			else
			{
				for (int item_n = 0; item_n < items_count; item_n++)
				{
					const char* item_label = items[item_n];
					const float item_ang_min = item_arc_span * (item_n + 0.02f) - item_arc_span * 0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
					const float item_ang_max = item_arc_span * (item_n + 0.98f) - item_arc_span * 0.5f;

					bool hovered = false;
					if (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN)
					{
						if (drag_angle >= item_ang_min && drag_angle < item_ang_max)
							hovered = true;
					}
					bool selected = p_selected && (*p_selected == item_n);

					int arc_segments = (int)(32 * item_arc_span / (2 * IM_PI)) + 1;
					draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max, arc_segments);
					draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min, arc_segments);
					draw_list->PathFillConvex(hovered ? ImColor(100, 100, 150, 110) : selected ? ImColor(120, 120, 140, 200) : ImColor(70, 70, 70, 125));

					//ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, "AA");
					ImVec2 text_size(52 * UI_disc_size, 52 * UI_disc_size);
					ImVec2 text_pos = ImVec2(
						center.x + cosf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
						center.y + sinf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);
					draw_list->AddImage(tracked_buffs[item_n].icon->texture, text_pos, ImVec2{ text_pos.x + text_size.x  ,text_pos.y + text_size.x }, { 0,0 }, { 1,1 }, ImGui::ColorConvertFloat4ToU32(ICON_color.value()));
					if (hovered)
						item_hovered = item_n;
				}
			}



			draw_list->PopClipRect();

			if (ImGui::IsMouseReleased(0))
			{
				ImGui::CloseCurrentPopup();
				ret = item_hovered;
				if (p_selected)
					*p_selected = item_hovered;
				open_mount_ui = 0;
				if (lockmouse == true)
				{
					lockmouse = false;
				}
				if (item_hovered != -1)
				{
					keybd_event(tracked_buffs[item_hovered].kays, MapVirtualKey(tracked_buffs[item_hovered].kays, 0), 0, 0);
					keybd_event(tracked_buffs[item_hovered].kays, MapVirtualKey(tracked_buffs[item_hovered].kays, 0), KEYEVENTF_KEYUP, 0);
					if (usPreferencesMount && usPreferencesMount_choose != -1)
					{
						if (p_selected)
							*p_selected = usPreferencesMount_choose;
					}
				}
				else
				{
					if (usPreferencesMount && usPreferencesMount_choose != -1)
					{
						keybd_event(tracked_buffs[usPreferencesMount_choose].kays, MapVirtualKey(tracked_buffs[usPreferencesMount_choose].kays, 0), 0, 0);
						keybd_event(tracked_buffs[usPreferencesMount_choose].kays, MapVirtualKey(tracked_buffs[usPreferencesMount_choose].kays, 0), KEYEVENTF_KEYUP, 0);
						if (p_selected)
							*p_selected = usPreferencesMount_choose;
					}
				}
			}


			ImGui::EndPopup();
		}
		ImGui::PopStyleColor(2);
		return usPreferencesMount? usPreferencesMount_choose :ret;
	}

	int HOMEUI::common_control_initialize()
	{
		HMODULE comctl32 = nullptr;
		if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"Comctl32.dll", &comctl32)) {
			return EXIT_FAILURE;
		}

		assert(comctl32 != nullptr);
		if (comctl32) {
			{ // InitCommonControlsEx を使用して初期化を試みる
				typename std::add_pointer< decltype(InitCommonControlsEx) >::type lpfnInitCommonControlsEx =
					reinterpret_cast<typename std::add_pointer< decltype(InitCommonControlsEx) >::type>(GetProcAddress(comctl32, "InitCommonControlsEx"));
				// InitCommonControlsEx が見つかった場合
				if (lpfnInitCommonControlsEx) {
					const INITCOMMONCONTROLSEX initcommoncontrolsex = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
					if (!lpfnInitCommonControlsEx(&initcommoncontrolsex)) {
						assert(!" InitCommonControlsEx(&initcommoncontrolsex) ");
						return EXIT_FAILURE;
					}
					OutputDebugStringW(L"initCommonControlsEx Enable\n");
					return 0;
				}
			}
			{ //InitCommonControls を使用して初期化を試みる
				InitCommonControls();
				OutputDebugStringW(L"initCommonControls Enable\n");
				return 0;
			}
		}
		return 1;
	}
}
