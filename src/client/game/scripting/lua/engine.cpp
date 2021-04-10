#include <std_include.hpp>
#include "engine.hpp"
#include "context.hpp"

#include "../execution.hpp"

#include <utils/io.hpp>

namespace scripting::lua::engine
{
	namespace
	{
		auto& get_scripts()
		{
			static std::vector<std::unique_ptr<context>> scripts{};
			return scripts;
		}

		void load_scripts()
		{
			const auto script_dir = "s1x/scripts/"s;

			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				if (std::filesystem::is_directory(script) && utils::io::file_exists(script + "/__init__.lua"))
				{
					get_scripts().push_back(std::make_unique<context>(script));
				}
			}
		}
	}

	void start()
	{
		// No SP until there is a concept
		if (game::environment::is_sp())
		{
			return;
		}

		clear_custom_fields();
		get_scripts().clear();
		load_scripts();
	}

	void stop()
	{
		get_scripts().clear();
	}

	void notify(const event& e)
	{
		if (e.entity.get_entity_id() == *game::levelEntityId
			&& e.name == "exitLevel_called")
		{
			get_scripts().clear();
		}

		for (auto& script : get_scripts())
		{
			script->notify(e);
		}
	}

	void run_frame()
	{
		for (auto& script : get_scripts())
		{
			script->run_frame();
		}
	}
}
