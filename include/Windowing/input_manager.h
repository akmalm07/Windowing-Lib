#pragma once


#include "window_input.h"

namespace windowing
{

	template<typename T>
	using view_ptr_non_const = const T*;

	class InputManager
		// This class manages the registration and invocation of input callbacks.
		// It allows for registering callbacks for different input types and invoking them when the input is received.
	{
	public:

		InputManager() = default;

		template<CallbackInputConcept InputStruct, typename... Args> // must provide the updater if intended to use callbacks for window, otherwise, optional
		void register_callback(const InputStruct& input, std::function<void(Args...)> cb, const std::string& name, std::optional<std::function<void(std::function<void(Args...)>)>> updater = std::nullopt);


		template<CallbackInputConcept InputStruct, typename... Args>
		InputStruct& get_input(const InputStruct& input, Args... args);

		template<CallbackInputConcept InputStruct>
		InputStruct& get_input(const std::string& name);


		template<CallbackInputConcept InputStruct, typename... Args>
		void emit(const InputStruct& input, Args... args);

		template<CallbackInputConcept InputStruct, typename... Args>
		void emit(const std::string& name, Args... args);


		template<CallbackInputConcept InputStruct>
		void emit_and_update(const InputStruct& input);

		template<CallbackInputConcept InputStruct>
		void emit_and_update(const std::string& name);


		template<CallbackInputConcept InputStruct>
		void delete_callback(const std::string& name);

		template<CallbackInputConcept InputStruct, typename... Args>
		void delete_callback(const InputStruct& input, Args... args);



		//const std::vector<std::unique_ptr<InputBase>>& list_entries_const_ref(InputType type) const;

		template<CallbackInputConcept InputStruct>
		const std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& list_entries_const_ref() const; // this returns only to the callbacks that have no parameters

	protected:

		template<CallbackInputConcept InputStruct>
		std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& get_proper_list_ref();

		template<CallbackInputConcept InputStruct>
		const std::unordered_map<std::string, std::unique_ptr<InputEntry<InputStruct>>>& get_proper_list_ref() const;


	protected:
		//std::unordered_map<InputType, std::vector<std::unique_ptr<InputBase>>> _registry;// ADD Naming system as well here
		
		std::unordered_map<std::string, std::unique_ptr<InputEntry<KeyCombInputOne>>> _keyInputs;

		std::unordered_map<std::string, std::unique_ptr<InputEntry<KeyCombInputPoly>>> _keyInputsPoly;

		std::unordered_map<std::string, std::unique_ptr<InputEntry<MouseButtonInput>>> _mouseButtonInputs;

		std::unordered_map<std::string, std::unique_ptr<InputEntry<AABButtonInput>>> _AABBInputs;

		std::unordered_map<std::string, std::unique_ptr<InputEntry<MouseMoveInput>>> _mouseMoveInputs;
		
		std::unordered_map<std::string, std::unique_ptr<InputEntry<MouseScrollInput>>> _mouseScrollInputs;


		// Better Yet, make different constainers for all the different inputs

		// TEST: wether having sub sections is faster then not
	};




	class WindowInputManager : private InputManager
		// This class is a specialized version of InputManager that manages inputs for a specific window.
		// It allows for registering and invoking callbacks that are specific to the window.
	{
	public:

		WindowInputManager() = default;

		template<CallbackInputConcept InputStruct, typename... Args> // must provide the updater if intended to use callbacks for window, otherwise, optional
		void register_callback(const InputStruct& input, std::function<void(Args...)> cb, const std::string& name, std::optional<std::function<void(std::function<void(Args...)>)>> updater = std::nullopt);


		template<CallbackInputConcept InputStruct>
		const std::vector<view_ptr_non_const<InputEntry<InputStruct>>> list_entires_const() const;


		template<CallbackInputConcept InputStruct>
		void emit(const InputStruct& input);


		template<CallbackInputConcept InputStruct>
		void emit(const std::string& name);


	private:

		template<CallbackInputConcept InputStruct>
		std::vector<view_ptr_non_const<InputEntry<InputStruct>>>& get_proper_window_list_ref();

		template<CallbackInputConcept InputStruct>
		const std::vector < view_ptr_non_const<InputEntry<InputStruct>>>& get_proper_window_list_ref() const;

	private:

		std::vector<view_ptr_non_const<InputEntry<KeyCombInputOne>>> _keyInputsWindow; // This is to maintain cache friendliness

		std::vector <view_ptr_non_const<InputEntry<KeyCombInputPoly>>> _keyInputsPolyWindow;

		std::vector<view_ptr_non_const<InputEntry<MouseButtonInput>>> _mouseButtonInputsWindow;

		std::vector<view_ptr_non_const<InputEntry<AABButtonInput>>> _AABBInputsWindow;

		std::vector<view_ptr_non_const<InputEntry<MouseMoveInput>>> _mouseMoveInputsWindow;

		std::vector<view_ptr_non_const<InputEntry<MouseScrollInput>>> _mouseScrollInputsWindow;

	};



}

#include "input_manager.inl"