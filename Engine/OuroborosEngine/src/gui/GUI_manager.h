#pragma once
#include <functional>
#include <list>
#include <queue>
#include <map>

#include "base.h"
#include "gui_definitions.h"

namespace OE
{
	namespace GUI
	{

		class SliderSpeed;
		class GUI_Base;
		namespace _manager_internal
		{
			struct Behavior
			{
				Behavior() = default;
				Behavior(const std::function<void(bool&)>& f) : f(f) {}
				Behavior(std::function<void(bool&)>&& f) : f(std::move(f)) {}
				Behavior(const Behavior& other) : f(other.f), open(other.open) {}
				Behavior(Behavior&& other) noexcept : f(std::move(other.f)), open(other.open) {}
				void operator()()
				{
					f(open);
				}
				std::function<void(bool&)> f;
				bool open = false;
			};

			struct MenuNode
			{
				std::map<std::string, Behavior> behaviors;
				std::map<std::string, MenuNode*> child_nodes;
				MenuNode* parent;

				void InsertOrCreate(const std::string& behavior_name, const Behavior& behavior, std::queue<std::string>& categories)
				{
					if(categories.empty())
					{
						if(!behaviors.contains(behavior_name))
							behaviors.emplace(behavior_name, behavior);
						return;
					}
					const std::string category_name = categories.front();
					categories.pop();
					const bool has_menu_item = child_nodes.contains(category_name);
					if(!has_menu_item)
					{
						child_nodes.emplace(category_name, new MenuNode);
					}

					child_nodes[category_name]->InsertOrCreate(behavior_name, behavior, categories);
				}

				Behavior* FindBehavior(const std::string& behavior_name, std::queue<std::string>& categories);

				void DrawMenu();
			};
		}

		class ViewPort;
		class GUI_manager
		{
			friend class SliderSpeed;
		public:
			GUI_manager();
			void Update();
			void RegisterMenuItem(const std::initializer_list<std::string>& categories, const std::string& behavior_name, const std::function<void(bool&)>& behavior) const;
			void AddPanel(GUI_Base* panel);
			void RemovePanel(GUI_Base* pointer);
			float GetSliderSpeed();

			_manager_internal::Behavior* GetBehavior(const std::initializer_list<std::string>& categories, const std::string& behavior_name) const;
			void RunBehavior(const std::initializer_list<std::string>& categories, const std::string& behavior_name) const;
			template<typename T>
			std::enable_if_t<std::is_base_of_v<GUI_Base, T>, T>* TryGetPanel();
			ViewPort* const GetViewportPanel() const { return viewport_panel; }
		private:
			void DrawMenu();
			bool HasPanel(const std::string& panel_name);
			float slider_speed;

			ViewPort* viewport_panel = nullptr;
			std::vector<GUI_Base*> gui_panels;
			std::queue<GUI_Base*> remove_queue_gui_panels;
			std::queue<GUI_Base*> add_queue_gui_panels;

			_manager_internal::MenuNode* head_menu;
		};

		template <typename T>
		std::enable_if_t<std::is_base_of_v<GUI_Base, T>, T>* GUI_manager::TryGetPanel()
		{
			for (auto gui_panel : gui_panels)
			{
				if(T* panel = dynamic_cast<T*>(gui_panel))
				{
					return panel;
				}
			}
			return nullptr;
		}
	}
}
