#pragma once

#include "input_manager.h"



namespace windowing
{
	template<CallbackInputConcept InputStruct, typename ...Args>
	void InputManager::register_callback(const InputStruct& input, std::function<void(Args...)> cb, const std::string& name, std::optional<std::function<void(std::function<void(Args...)>)>> updater)
	{
		auto& properList = get_proper_list_ref<InputStruct>();
	
		if (properList.find(name) != properList.end())
		{
			throw std::runtime_error("Callback with the same name already exists");
		}

		properList.emplace(
			name,
			std::make_unique<InputEntryConcrete<InputStruct, Args...>>(input, std::move(cb), name, std::move(updater))
		);
	}

	template<CallbackInputConcept InputStruct, typename ...Args>
	inline InputStruct& InputManager::get_input(const InputStruct& input, Args ...args)
	{
		constexpr InputType type = InputTypeResolver<InputStruct>::value;
		auto& properList = get_proper_list_ref<InputStruct>();

		for (const auto& entry : properList->second)
		{
			if (entry->matches(input))
			{
				return entry->input;
			}
		}
		throw std::runtime_error("Input not found");
		return const_cast<InputStruct&>(input);
	}

	template<CallbackInputConcept InputStruct>
	inline InputStruct& InputManager::get_input(const std::string& name)
	{
		auto& properList = get_proper_list_ref<InputStruct>();
		auto it = properList.find(name);
		if (it == properList.end())
			throw std::runtime_error("Input not found");
		else
			return static_cast<InputEntry<InputStruct>*>(it->second.get())->input;
	}

	template<CallbackInputConcept InputStruct, typename ...Args>
	inline void InputManager::emit(const InputStruct& input, Args ...args)
	{
		constexpr InputType type = InputTypeResolver<InputStruct>::value;

		auto& properList = get_proper_list_ref<InputStruct>();

		for (auto& [name, entry] : properList)
		{
			view_ptr<InputEntryConcrete<InputStruct, Args...>> typed = static_cast<view_ptr<InputEntryConcrete<InputStruct, Args...>>>(entry.get());
			if (typed && typed->matches(input))
			{
				typed->callback(args...);
			}
		}

		/*
		auto it = _registry.find(type);
		if (it == _registry.end()) return;

		for (auto& entry : it->second)
		{
			view_ptr<InputEntry<InputStruct>> typed = static_cast<view_ptr<InputEntry<InputStruct, Args...>>>(entry.get());
			if (typed && typed->matches(input))
			{
				typed->callback(args...);
			}
		}
		*/
	}

	template<CallbackInputConcept InputStruct>
	inline void InputManager::emit_and_update(const std::string& name)
	{
		auto& properList = get_proper_list_ref<InputStruct>();

		auto it = properList.find(name);
		if (it == properList.end()) 
			return;
		else
			it->second->emit_and_update();
	}

	
	template<CallbackInputConcept InputStruct>
	inline void InputManager::delete_callback(const std::string& name) //TEST TEMPLATE...
	{
		auto& properList = get_proper_list_ref<InputStruct>();
		auto it = properList.find(name);
		if (it != properList.end())
		{
			const auto& ptr = get_proper_window_list_ref<InputStruct>();
			ptr.erase(
				std::remove_if(ptr.begin(), ptr.end(),
					[&name](const view_ptr<InputEntry<InputStruct>>& entry) 
					{

						return entry->get_name() == name; 
					}),
				ptr.end()
			);
			properList.erase(it);
		}



	}

	template<CallbackInputConcept InputStruct, typename ...Args>
	inline void InputManager::delete_callback(const InputStruct& input, Args ...args)
	{
		constexpr InputType type = InputTypeResolver<InputStruct>::value;
		auto& properList = get_proper_list_ref<InputStruct>();
		for (auto it = properList.begin(); it != properList.end(); ++it)
		{
			view_ptr<InputEntryConcrete<InputStruct, Args...>> typed = static_cast<view_ptr<InputEntryConcrete<InputStruct, Args...>>>(it->second.get());
			if (typed && typed->matches(input))
			{
				const auto& ptr = get_proper_window_list_ref<InputStruct>();
				ptr.erase(
					std::remove_if(ptr.begin(), ptr.end(),
						[](const view_ptr<InputEntry<InputStruct>>& entry)
						{

							return entry->get_name() == it->first;
						}),
					ptr.end()
				);

				properList.erase(it);
			}
		}
	}


	template<CallbackInputConcept InputStruct, typename ...Args>
	inline void InputManager::emit(const std::string& name, Args ...args)
	{
		auto& properList = get_proper_list_ref<InputStruct>();
		auto it = properList.find(name);
		if (it == properList.end())
			return;
		else
			it->second->emit(args...);
	}


	template<CallbackInputConcept InputStruct>
	inline void InputManager::emit_and_update(const InputStruct& input)
	{
		auto& properList = get_proper_list_ref<InputStruct>();
		for (auto& [name, entry] : properList)
		{
			view_ptr<InputEntry<InputStruct>> typed = static_cast<view_ptr<InputEntry<InputStruct>>>(entry.get());
			if (typed && typed->matches(input))
			{
				typed->emit_and_update();
			}
		}
	}


	template<CallbackInputConcept InputStruct>
	inline const std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& InputManager::list_entries_const_ref() const
	{
		const auto& properList = get_proper_list_ref<InputStruct>();

		return properList; 
	}

	template<CallbackInputConcept InputStruct>
	inline std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& InputManager::get_proper_list_ref()
	{
		if constexpr (std::same_as<KeyCombInputOne, InputStruct>)
			return _keyInputs;
		else if constexpr (std::same_as<KeyCombInputPoly, InputStruct>)
			return _keyInputsPoly;
		else if constexpr (std::same_as<MouseButtonInput, InputStruct>)
			return _mouseButtonInputs;
		else if constexpr (std::same_as<AABButtonInput, InputStruct>)
			return _AABBInputs;
		else if constexpr (std::same_as<MouseMoveInput, InputStruct>)
			return _mouseMoveInputs;
		else if constexpr (std::same_as<MouseScrollInput, InputStruct>)
			return _mouseScrollInputs;
		else
			throw std::runtime_error("Unknown input type");
	}

	// Const version
	template<CallbackInputConcept InputStruct>
	inline const std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& InputManager::get_proper_list_ref() const
	{
		if constexpr (std::same_as<KeyCombInputOne, InputStruct>)
			return _keyInputs;
		else if constexpr (std::same_as<KeyCombInputPoly, InputStruct>)
			return _keyInputsPoly;
		else if constexpr (std::same_as<MouseButtonInput, InputStruct>)
			return _mouseButtonInputs;
		else if constexpr (std::same_as<AABButtonInput, InputStruct>)
			return _AABBInputs;
		else if constexpr (std::same_as<MouseMoveInput, InputStruct>)
			return _mouseMoveInputs;
		else if constexpr (std::same_as<MouseScrollInput, InputStruct>)
			return _mouseScrollInputs;
		else
			throw std::runtime_error("Unknown input type");
	}





	template<CallbackInputConcept InputStruct>
	inline const std::vector<view_ptr_non_const<InputEntry<InputStruct>>> WindowInputManager::list_entires_const() const
	{
		const auto& properList = get_proper_window_list_ref<InputStruct>();

		return properList;
	}

	template<CallbackInputConcept InputStruct>
	inline void WindowInputManager::emit(const InputStruct& input)
	{
		InputManager::emit_and_update<InputStruct>(input); // This will call the InputManager's emit function, which will handle the input properly
	}

	template<CallbackInputConcept InputStruct>
	inline void WindowInputManager::emit(const std::string& name)
	{
		InputManager::emit_and_update<InputStruct>(name);// The key and the mouse take different pathways the the emit, loook into it
	}


	template<CallbackInputConcept InputStruct>
	inline std::vector<view_ptr_non_const<InputEntry<InputStruct>>>& WindowInputManager::get_proper_window_list_ref()
	{
		if constexpr (std::same_as<KeyCombInputOne, InputStruct>)
			return _keyInputsWindow;
		else if constexpr (std::same_as<KeyCombInputPoly, InputStruct>)
			return _keyInputsPolyWindow;
		else if constexpr (std::same_as<MouseButtonInput, InputStruct>)
			return _mouseButtonInputsWindow;
		else if constexpr (std::same_as<AABButtonInput, InputStruct>)
			return _AABBInputsWindow;
		else if constexpr (std::same_as<MouseMoveInput, InputStruct>)
			return _mouseMoveInputsWindow;
		else if constexpr (std::same_as<MouseScrollInput, InputStruct>)
			return _mouseScrollInputsWindow;
		else
			throw std::runtime_error("Unknown input type");
	}

	template<CallbackInputConcept InputStruct>
	inline const std::vector<view_ptr_non_const<InputEntry<InputStruct>>>& WindowInputManager::get_proper_window_list_ref() const
	{
		if constexpr (std::same_as<KeyCombInputOne, InputStruct>)
			return _keyInputsWindow;
		else if constexpr (std::same_as<KeyCombInputPoly, InputStruct>)
			return _keyInputsPolyWindow;
		else if constexpr (std::same_as<MouseButtonInput, InputStruct>)
			return _mouseButtonInputsWindow;
		else if constexpr (std::same_as<AABButtonInput, InputStruct>)
			return _AABBInputsWindow;
		else if constexpr (std::same_as<MouseMoveInput, InputStruct>)
			return _mouseMoveInputsWindow;
		else if constexpr (std::same_as<MouseScrollInput, InputStruct>)
			return _mouseScrollInputsWindow;
		else
			throw std::runtime_error("Unknown input type");

	}


	template<CallbackInputConcept InputStruct, typename ...Args>
	inline void WindowInputManager::register_callback(const InputStruct& input, std::function<void(Args...)> cb, const std::string& name, std::optional<std::function<void(std::function<void(Args...)>)>> updater)
	{
		InputManager::register_callback<InputStruct, Args...>(input, std::move(cb), name, std::move(updater));

		if constexpr (sizeof...(Args) == 0)
		{
			get_proper_window_list_ref<InputStruct>().emplace_back(InputManager::get_proper_list_ref<InputStruct>()[name].get());
		}
		else
		{
			if (updater.has_value())
			{
				get_proper_window_list_ref<InputStruct>().emplace_back(InputManager::get_proper_list_ref<InputStruct>()[name].get());
			}
		}
	}
	

}