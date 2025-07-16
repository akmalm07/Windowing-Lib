#pragma once

#include "keys.h"


namespace windowing
{
	struct CallbackInput 
	{

		virtual InputType get_type() const = 0;
		virtual ~CallbackInput() = default; 
	};

	struct KeyCombInputOne : CallbackInput
	{
		Keys number;
		Action action;
		Mods mod = Mods::None;

		KeyCombInputOne(Keys key, Action action)
			: number(key), action(action) {
		}

		KeyCombInputOne(Keys key, Action action, Mods mod)
			: number(key), action(action), mod(mod) {
		}

		InputType get_type() const override 
		{
			return InputType::Key;
		}
	};

	struct MouseScrollInput : CallbackInput
	{
		MouseChange offset = MouseChange::None;

		MouseScrollInput(MouseChange action)
			: offset(action) {
		}

		InputType get_type() const override
		{
			return InputType::MouseScroll;
		}
	};

	struct KeyCombInputPoly : CallbackInput
	{
		std::array<Keys, 2> numbers;
		Action action;
		Mods mod = Mods::None;

		KeyCombInputPoly(std::array<Keys, 2> key, Action action)
			: numbers(key), action(action) {
		}

		InputType get_type() const override
		{
			return InputType::KeyPoly;
		}
	};

	struct MouseButtonInput : CallbackInput
	{
		Mouse button;
		Action action;
		MouseButtonInput(Mouse button, Action action)
			: button(button), action(action) {
		}

		InputType get_type() const override
		{
			return InputType::MouseButton;
		}
	};

	struct AABButtonInput : CallbackInput
	{
	public:
		float cordX, cordY, width, height;
		Action action;
		Mouse button;
		std::string_view name;

		AABButtonInput(float x, float y, float w, float h, Action action, Mouse button, std::string_view name)
			: cordX(x), cordY(y), width(w), height(h), action(action), button(button), name(name) {
		}

		InputType get_type() const override
		{
			return InputType::AABButton;
		}

		bool is_touching(float x, float y, Action action, Mouse button) const
		{
			return (x >= cordX && x <= cordX + width && y >= cordY && y <= cordY + height) && (action == this->action) && (button == this->button);
		}
	};

	struct MouseMoveInput : CallbackInput
	{
		MouseChange change;
		Mouse button = Mouse::None;

		MouseMoveInput(MouseChange change, Mouse button = Mouse::None)
			: change(change), button(button) {
		}

		InputType get_type() const override
		{
			return InputType::MouseMovement;
		}
	};


	template<typename T>
	concept CallbackInputConcept = std::is_base_of_v<CallbackInput, T>;
	

	using MouseMove = windowing::MouseMoveInput;

	using Key = windowing::KeyCombInputOne;

	using KeyPoly = windowing::KeyCombInputPoly;

	using MouseBut = windowing::MouseButtonInput;

	using AABBut = windowing::AABButtonInput;
}

	//struct ThreadControlInfo
	//{
	//public:
	//    ThreadControlInfo(const std::shared_ptr<ConditionalVariuble>& windowInput,
	//        const std::shared_ptr<ConditionalVariuble>& changingParams, const std::shared_ptr<std::mutex>& mtx);

	//    std::shared_ptr<ConditionalVariuble> windowInputWait;
	//    std::shared_ptr<ConditionalVariuble> changingParamsWait;
	//    std::shared_ptr<std::mutex> lock;
	//};