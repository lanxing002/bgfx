/*
 * Copyright 2011-2024 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
#include "logo.h"
#include "editor_ui.h"
#include "imgui/imgui.h"
#include <dear-imgui/imgui_internal.h>


class EditorUI
{
public:
	EditorUI();

	virtual void initialize();

	virtual void render();

private:
	void showEditorMenu(bool* opended);
	void showEditorWorldObjectsWindow(bool* opended);
	void showEditorGameWindow(bool* opended);

private:
	bool m_editor_menu_window_open = true;
	bool m_asset_window_open = true;
	bool m_game_engine_window_open = true;
	bool m_file_content_window_open = true;
	//bool m_detail_window_open = true;
	//bool m_scene_lights_window_open = true;
	//bool m_scene_lights_data_window_open = true;
};

EditorUI::EditorUI()
{
}

void EditorUI::initialize()
{

}

void EditorUI::render()
{
	showEditorMenu(&m_editor_menu_window_open);
	showEditorWorldObjectsWindow(&m_asset_window_open);
	showEditorGameWindow(&m_game_engine_window_open);
}

void EditorUI::showEditorMenu(bool* opened)
{
	ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;
	ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
		ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
	std::array<int, 2> window_size = g_editor_global_context.m_window_system->getWindowSize();
	ImGui::SetNextWindowSize(ImVec2((float)window_size[0], (float)window_size[1]), ImGuiCond_Always);

	ImGui::SetNextWindowViewport(main_viewport->ID);

	ImGui::Begin("Editor menu", p_open, window_flags);

	ImGuiID main_docking_id = ImGui::GetID("Main Docking");
	if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
	{
		ImGui::DockBuilderRemoveNode(main_docking_id);

		ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
		ImGui::DockBuilderSetNodePos(main_docking_id,
			ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 18.0f));
		ImGui::DockBuilderSetNodeSize(main_docking_id,
			ImVec2((float)window_size[0], (float)window_size[1] - 18.0f));

		ImGuiID center = main_docking_id;
		ImGuiID left;
		ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &left);

		ImGuiID left_other;
		ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

		ImGuiID left_game_engine;
		ImGuiID left_asset =
			ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.30f, nullptr, &left_game_engine);

		ImGui::DockBuilderDockWindow("World Objects", left_asset);
		ImGui::DockBuilderDockWindow("Components Details", right);
		ImGui::DockBuilderDockWindow("File Content", left_file_content);
		ImGui::DockBuilderDockWindow("Game Engine", left_game_engine);

		ImGui::DockBuilderFinish(main_docking_id);
	}

	ImGui::DockSpace(main_docking_id);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Reload Current Level"))
			{
				g_runtime_global_context.m_world_manager->reloadCurrentLevel();
				g_runtime_global_context.m_render_system->clearForLevelReloading();
				g_editor_global_context.m_scene_manager->onGObjectSelected(k_invalid_gobject_id);
			}
			if (ImGui::MenuItem("Save Current Level"))
			{
				g_runtime_global_context.m_world_manager->saveCurrentLevel();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::BeginMenu("Animation"))
				{
					if (ImGui::MenuItem(g_runtime_global_context.m_render_debug_config->animation.show_skeleton ? "off skeleton" : "show skeleton"))
					{
						g_runtime_global_context.m_render_debug_config->animation.show_skeleton = !g_runtime_global_context.m_render_debug_config->animation.show_skeleton;
					}
					if (ImGui::MenuItem(g_runtime_global_context.m_render_debug_config->animation.show_bone_name ? "off bone name" : "show bone name"))
					{
						g_runtime_global_context.m_render_debug_config->animation.show_bone_name = !g_runtime_global_context.m_render_debug_config->animation.show_bone_name;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Camera"))
				{
					if (ImGui::MenuItem(g_runtime_global_context.m_render_debug_config->camera.show_runtime_info ? "off runtime info" : "show runtime info"))
					{
						g_runtime_global_context.m_render_debug_config->camera.show_runtime_info = !g_runtime_global_context.m_render_debug_config->camera.show_runtime_info;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Game Object"))
				{
					if (ImGui::MenuItem(g_runtime_global_context.m_render_debug_config->gameObject.show_bounding_box ? "off bounding box" : "show bounding box"))
					{
						g_runtime_global_context.m_render_debug_config->gameObject.show_bounding_box = !g_runtime_global_context.m_render_debug_config->gameObject.show_bounding_box;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Exit"))
			{
				g_editor_global_context.m_engine_runtime->shutdownEngine();
				exit(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("World Objects", nullptr, &m_asset_window_open);
			ImGui::MenuItem("Game", nullptr, &m_game_engine_window_open);
			ImGui::MenuItem("File Content", nullptr, &m_file_content_window_open);
			ImGui::MenuItem("Detail", nullptr, &m_detail_window_open);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();

}

void EditorUI::showEditorWorldObjectsWindow(bool* opened)
{

}

void EditorUI::showEditorGameWindow(bool* opened)
{

}

namespace
{

class ExampleHelloWorld : public entry::AppI
{
public:
	ExampleHelloWorld(const char* _name, const char* _description, const char* _url)
		: entry::AppI(_name, _description, _url)
	{
	}

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_TEXT;
		m_reset  = BGFX_RESET_VSYNC;

		bgfx::Init init;
		init.type     = args.m_type;
		init.vendorId = args.m_pciId;
		init.platformData.nwh  = entry::getNativeWindowHandle(entry::kDefaultWindowHandle);
		init.platformData.ndt  = entry::getNativeDisplayHandle();
		init.platformData.type = entry::getNativeWindowHandleType();
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);

		imguiCreate();
	}

	virtual int shutdown() override
	{
		imguiDestroy();

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
			imguiBeginFrame(m_mouseState.m_mx
				,  m_mouseState.m_my
				, (m_mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
				,  m_mouseState.m_mz
				, uint16_t(m_width)
				, uint16_t(m_height)
				);

			showExampleDialog(this);

			imguiEndFrame();

			// Set view 0 default viewport.
			bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

			// Use debug font to print information about this example.
			bgfx::dbgTextClear();

			const bgfx::Stats* stats = bgfx::getStats();

			bgfx::dbgTextImage(
				  bx::max<uint16_t>(uint16_t(stats->textWidth/2), 20)-20
				, bx::max<uint16_t>(uint16_t(stats->textHeight/2),  6)-6
				, 40
				, 12
				, s_logo
				, 160
				);
			bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

			bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
			bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

			bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
				, stats->width
				, stats->height
				, stats->textWidth
				, stats->textHeight
				);

			// Advance to next frame. Rendering thread will be kicked to
			// process submitted rendering primitives.
			bgfx::frame();

			return true;
		}

		return false;
	}

	entry::MouseState m_mouseState;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
};

} // namespace

ENTRY_IMPLEMENT_MAIN(
	  ExampleHelloWorld
	, "00-helloworld"
	, "Initialization and debug text."
	, "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
	);


