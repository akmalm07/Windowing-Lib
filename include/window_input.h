#pragma once


#include "keys.h"
#include "window_input_bundles.h"

namespace windowing 
{

	template<typename T>
	using view_ptr = const T* const;


	template<CallbackInputConcept InputStruct>
	view_ptr<InputStruct> cast_to_type(view_ptr<CallbackInput> incoming);
	
	template<typename T> struct InputTypeResolver;

	template<> struct InputTypeResolver<KeyCombInputOne> {
		static constexpr InputType value = InputType::Key;
	};
	template<> struct InputTypeResolver<KeyCombInputPoly> {
		static constexpr InputType value = InputType::KeyPoly;
	};
	template<> struct InputTypeResolver<MouseButtonInput> {
		static constexpr InputType value = InputType::MouseButton;
	};
	template<> struct InputTypeResolver<AABButtonInput> {
		static constexpr InputType value = InputType::AABButton;
	};
	template<> struct InputTypeResolver<MouseMoveInput> {
		static constexpr InputType value = InputType::MouseMovement;
	};

	template<InputType>
	struct InputTypeResolverInv;

	template<>
	struct InputTypeResolverInv<InputType::AABButton> {
		using type = AABButtonInput;
	};

	template<>
	struct InputTypeResolverInv<InputType::KeyPoly> {
		using type = KeyCombInputPoly;
	};

	template<>
	struct InputTypeResolverInv<InputType::Key> {
		using type = KeyCombInputOne;
	};

	template<>
	struct InputTypeResolverInv<InputType::MouseMovement> {
		using type = MouseMoveInput;
	};

	template<>
	struct InputTypeResolverInv<InputType::MouseButton> {
		using type = MouseButtonInput;
	};

	template<typename... Args>
	class CallbackDispatcher
	{
	public:

		void add_callback(std::function<void(Args...)> cb);

		void invoke_all(Args... args);

	private:
		std::vector<std::function<void(Args...)>> _callbacks;
	};


	struct InputBase
	{
	public:
		virtual bool matches(const CallbackInput& incoming) const = 0;

		virtual std::string_view get_name() const = 0;

		virtual ~InputBase() = default;
	};

	template<typename Child>
	struct InputEntryBase : public InputBase
		// This is a CRTP base class for input entries, allowing for static polymorphism.
		// It ensures that the derived class implements the matches_impl method.
	{
	public:
		virtual ~InputEntryBase() = default;
		virtual bool matches_impl(const CallbackInput& input) const = 0;

		bool matches(const CallbackInput& incoming) const final
		{
			return static_cast<const Child*>(this)->matches_impl(incoming);
		}

		template<typename InputEntry>
		bool is() const
		{
			if constexpr (std::same_as<Child, InputEntry>)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};



	template<CallbackInputConcept InputStruct, typename... Args>
	struct InputEntryConcrete; // Forward declaration


	template<CallbackInputConcept InputStruct>
	struct InputEntry : public InputEntryBase<InputEntry<InputStruct>>
	{
	public:
		InputStruct input;
		std::string name;

		InputEntry(InputStruct i, std::string_view name);

		virtual void emit_and_update() const = 0;

		template<typename... Args>
		void emit(Args&&... args)
		{
			static_cast<const InputEntryConcrete<InputStruct>*>(this)->emit(std::forward<Args>(args)...);
		}

		bool matches_impl(const CallbackInput& incoming) const = 0;
	};



	template<>
	struct InputEntryBase<InputEntry<AABButtonInput>> : public InputBase
	{
	public:
		virtual ~InputEntryBase() = default;
		virtual bool matches_impl(const CallbackInput& input) const = 0;

		bool matches(const CallbackInput& incoming) const override final
		{
			return static_cast<const InputEntry<AABButtonInput>*>(this)->matches_impl(incoming);
		}

		bool is_hovering(float x, float y, Action action, Mouse button) const
		{
			return static_cast<const InputEntry<AABButtonInput>*>(this)->input.is_touching(x, y, action, button);
		}

	};


	template<CallbackInputConcept InputStruct, typename... Args>
	struct InputEntryConcrete : public InputEntry<InputStruct>
	{
	public:

		using InputEntry<InputStruct>::input;
		using InputEntry<InputStruct>::name;

		std::function<void(Args...)> callback;
		std::optional<std::function<void(std::function<void(Args...)>)>> updater;

		InputEntryConcrete(InputStruct i, std::function<void(Args...)> cb, std::string_view name, std::optional<std::function<void(std::function<void(Args...)>)>> updater = std::nullopt);
		
		void emit_and_update() const override;

		std::string_view get_name() const override;

		void emit(Args... args) const
		{
			callback(args...);
		}

		bool matches_impl(const CallbackInput& incoming) const override;
	};





} // namespace windowing


#include "window_input.inl"