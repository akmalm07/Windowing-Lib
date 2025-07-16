#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <array>
#include <functional>


namespace windowing
{

	template<typename T>
	concept Enum = std::is_enum_v<T>;

	enum class Mods
	{
		None = 0,
		Shift = GLFW_MOD_SHIFT,
		Control = GLFW_MOD_CONTROL,
		Alt = GLFW_MOD_ALT,
		Super = GLFW_MOD_SUPER,
		CapsLock = GLFW_MOD_CAPS_LOCK,
		NumLock = GLFW_MOD_NUM_LOCK, 
		count = 7
	};

	enum class Keys
	{
		None = GLFW_KEY_UNKNOWN,
		Up = GLFW_KEY_UP,
		Down = GLFW_KEY_DOWN,
		Left = GLFW_KEY_LEFT,
		Right = GLFW_KEY_RIGHT,
		Space = GLFW_KEY_SPACE,
		Apost = GLFW_KEY_APOSTROPHE, 
		Comma = GLFW_KEY_COMMA, 
		Minus = GLFW_KEY_MINUS, 
		Period = GLFW_KEY_PERIOD, 
		Slash = GLFW_KEY_SLASH, 
		Zero = GLFW_KEY_0, 
		One = GLFW_KEY_1, 
		Two = GLFW_KEY_2, 
		Three = GLFW_KEY_3, 
		Four = GLFW_KEY_4, 
		Five = GLFW_KEY_5, 
		Six = GLFW_KEY_6, 
		Seven = GLFW_KEY_7, 
		Eight = GLFW_KEY_8, 
		Nine = GLFW_KEY_9, 
		Semicol = GLFW_KEY_SEMICOLON, 
		Equal = GLFW_KEY_EQUAL, 
		A = GLFW_KEY_A, 
		B = GLFW_KEY_B, 
		C = GLFW_KEY_C, 
		D = GLFW_KEY_D, 
		E = GLFW_KEY_E, 
		F = GLFW_KEY_F, 
		G = GLFW_KEY_G, 
		H = GLFW_KEY_H, 
		I = GLFW_KEY_I, 
		J = GLFW_KEY_J, 
		K = GLFW_KEY_K, 
		L = GLFW_KEY_L, 
		M = GLFW_KEY_M, 
		N = GLFW_KEY_N, 
		O = GLFW_KEY_O, 
		P = GLFW_KEY_P, 
		Q = GLFW_KEY_Q, 
		R = GLFW_KEY_R, 
		S = GLFW_KEY_S, 
		T = GLFW_KEY_T, 
		U = GLFW_KEY_U, 
		V = GLFW_KEY_V, 
		W = GLFW_KEY_W, 
		X = GLFW_KEY_X, 
		Y = GLFW_KEY_Y, 
		Z = GLFW_KEY_Z, 
		Backsl = GLFW_KEY_BACKSLASH, 
		Rbracket = GLFW_KEY_RIGHT_BRACKET,
		Lbracket = GLFW_KEY_LEFT_BRACKET,
		GravAcc = GLFW_KEY_GRAVE_ACCENT,
		Esc = GLFW_KEY_ESCAPE,	
		Count = 48
	};


	enum class Mouse
	{
		None = GLFW_KEY_UNKNOWN,
		Left = GLFW_MOUSE_BUTTON_LEFT,
		Right = GLFW_MOUSE_BUTTON_RIGHT,
		Count = 2
	};

	enum class MouseChange
	{
		None = 0,
		MoveX = 1 << 0,
		MoveY = 1 << 1,
		MoveXY = MoveX | MoveY,
		Count = 3
	};

	enum class Action : uint32_t
	{
		None = 0,
		Release = 1 << 0, 
		Press = 1 << 1,  
		Repeat = 1 << 2,
	};

	enum class InputType
	{
		None = 0,
		MouseButton = 1 << 0,
		AABButton = 1 << 1,
		Key = 1 << 2,
		KeyPoly = 1 << 3,
		MouseMovement = 1 << 4,
		MouseScroll = 1 << 5,
		Count = 6
	};

	enum class Direction
	{
		None = 0,
		Up = 1 << 0,
		Down = 1 << 1,
		Right = 1 << 2,
		Left = 1 << 3,
		Forward = 1 << 4,
		Backward = 1 << 5,
		Count = 6
	};




	template <typename T>
	inline Keys KEYS(T x)
	{
		return static_cast<Keys>(x);
	}

	template <typename T>
	inline Mods MODS(T x)
	{
		return static_cast<Mods>(x);
	}

	template <typename T>
	inline Mouse MOUSE(T x)
	{
		return static_cast<Mouse>(x);
	}

	template <typename T>
	inline Action ACTION(T x)
	{
		return static_cast<Action>(x);
	}




	template <Enum T>
	constexpr T operator|(T a, T b) {
		return static_cast<T>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	template <Enum T>
	constexpr T operator&(T a, T b) {	
		return static_cast<T>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}
	
	template <Enum T>
	constexpr bool has(T a, T b) {
		using U = std::underlying_type_t<T>;
		return (static_cast<U>(a) & static_cast<U>(b)) != 0;
	}


	template <Enum T>
	constexpr T operator^(T a, T b) {
		return static_cast<T>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
	}

	template <Enum T>
	constexpr T operator~(T a) {
		return static_cast<T>(~static_cast<uint32_t>(a));
	}

	template <Enum T>
	constexpr T& operator|=(T& a, T b) {
		return a = a | b;
	}

	template <Enum T>
	constexpr T& operator&=(T& a, T b) {
		return a = a & b;
	}
}

namespace std 
{
	template <class T1, class T2>
		requires std::is_enum_v<T1> && std::is_enum_v<T2>
	struct std::hash<std::pair<T1, T2>> {
		size_t operator()(const std::pair<T1, T2>& p) const noexcept {
			size_t hash1 = std::hash<T1>{}(p.first);
			size_t hash2 = std::hash<T2>{}(p.second);

			return hash1 ^ (hash2 * 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
		}
	};

	template <class Keys, class Mods>
	struct std::hash<std::pair<std::array<Keys, 2>, Mods>> {
		size_t operator()(const std::pair<std::array<Keys, 2>, Mods>& p) const noexcept {
			size_t hash1 = 0;
			size_t hash2 = std::hash<Mods>{}(p.second);

			for (const auto& key : p.first) {
				hash1 ^= std::hash<Keys>{}(key)+0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
			}

			return hash1 ^ (hash2 * 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
		}
	};
}