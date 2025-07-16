#pragma once

#include "keys.h"

#include "window_input.h"

#include "window_input_bundles.h"

#include "timer.h"
#include "input_manager.h"




namespace windowing 
{

	struct OrthoBundle
	{
		float left;
		float right;
		float top;
		float bottom;
	};

	class Window
	{
	public:
		Window();

		Window(float windowWidth, float windowHeight, const std::string& name, bool enableFPSTag, bool createWindowT);

		Window(Window&& other) noexcept;

		Window& operator=(Window&& other) noexcept;

		bool create_window(const std::string& name, bool disableCursor);

		void set_disable_cursor(bool disableCursor);

		float get_aspect_ratio() const;

		void make_window_context_current();

		int get_buffer_width();

		int get_buffer_height();

		const int* get_buffer_width_p();

		const int* get_buffer_height_p();

		GLFWwindow* get_window() const;

		bool set_window(GLFWwindow* window);

		float get_width() const;

		float get_height() const;

		//void set_movement_callbacks();

		bool is_key_active(Keys key, Action act) const;

		std::string get_name() const;

		void set_name(const std::string& name);

		bool get_should_close() const;

		void set_should_close(bool trueOrFalse);

		void swap_buffers() const;

		void clear_window();

		void poll_events() const;

		void wait_events() const;

		void set_escape_button(Keys key, Action action, std::optional<Mods> mod = std::nullopt);

		bool is_updated();

		void update_fps_tag();

		void update();


		template<CallbackInputConcept InputStruct, typename... Args>
		void register_callback(const InputStruct& input, std::function<void(Args...)>& cb, std::function<void(std::function<void(Args...)>)>& updater, const std::string& name);

		template<CallbackInputConcept InputStruct>
		void register_callback(const InputStruct& input, std::function<void()>& cb, const std::string& name);

		template<CallbackInputConcept InputStruct>
		void emit(const InputStruct& input);

		template<CallbackInputConcept InputStruct>
		void emit(const InputStruct& input, const std::string& name);

		void reset_delta_time();

		float get_delta_time_sec();
		
		float get_delta_time_ms();
		
		~Window();

	protected:

		WindowInputManager _inputManager;

		//WindowT Vars
		static bool _calledBufferSize;

		static uint32_t g_numOfWindows;

		float _aspectRatio;

		GLFWwindow* _mainWindow = nullptr;

		float _width = 0.0f, _height = 0.0f;
		int _bufferWidth = 0, _bufferHeight = 0;

		windowing::Timer _timer; 

		std::string _name = "";

		bool _updated = false; // develop this to know if the window has been updated or not, used in the main loop to check for updates.
		bool _enableFpsTag = false; 
		std::array<bool, 1024> _keys{}; 



		double _mouseChangeX = 0.0;
		double _mouseChangeY = 0.0;

		double _scrollOffsetX = 0.0;
		double _scrollOffsetY = 0.0;

		double _mouseCurrentX = 0.0;
		double _mouseCurrentY = 0.0;

	protected:

		void init(float windowWidth, float windowHeight, const std::string& name, bool enableFPSTag, bool createWindow);

		void HandleKeys(int key, int code, int action, int mode);
		void HandleMouseCursor(double posX, double posY);
		void HandleMouseScroll(double xOffset, double yOffset);
		void HandleMouseButtons(int mouseButton, int action, int mods);

		void CreateCallbacks();
		static void m_HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
		static void m_HandleMouseCursor(GLFWwindow* window, double posX, double posY);
		static void m_HandleMouseScroll(GLFWwindow* window, double xOffset, double yOffset);
		static void m_HandleMouseButtons(GLFWwindow* window, int button, int action, int mods);


		int convert_to_glfw(Action action) const; 
	};

}


namespace windowing
{
	template<CallbackInputConcept InputStruct, typename... Args>
	inline void Window::register_callback(const InputStruct& input, std::function<void(Args...)>& cb, std::function<void(std::function<void(Args...)>)>& updater, const std::string& name)
	{
		_inputManager.register_callback<InputStruct, Args...>(input, std::move(cb), name, std::move(updater)); // ERR: Check
	}


	template<CallbackInputConcept InputStruct>
	inline void Window::register_callback(const InputStruct& input, std::function<void()>& cb, const std::string& name)
	{
		_inputManager.register_callback<InputStruct>(input, std::move(cb), name);
	}

	template<CallbackInputConcept InputStruct>
	inline void Window::emit(const InputStruct& input)
	{
		_inputManager.emit<InputStruct>(input);
	}
	template<CallbackInputConcept InputStruct>
	inline void Window::emit(const InputStruct& input, const std::string& name)
	{
		_inputManager.emit<InputStruct>(name);
	}
}



//------------------------------------------------


//PLANNING TO EXTEND
//void SetAsyncPollEvents(const ThreadControlInfo& cv);

//void AllowWindowToContinueAndWait();
//void WaitInitallyForSignal();
//template<class ... Args>
//void AddMouseClick(Mouse mouse, std::function<bool(Args...)> function);
//void DelMouseClick(Mouse mouse);
//MouseButton& FindMouseButtonName(Mouse mouse);
//Planning to add a function to find the mouse button soon...
//struct Async
//{
//public:
//	Async();
//	Async(const ThreadControlInfo& condition);
//	Async(Async&& other) noexcept;
//	Async& operator=(Async&& other) noexcept;

//	void notify_window();
//	void notify_change(bool needsChange);

//	void wait_for_window();
//	void wait_for_change();

//	bool get_state() const;

//	void wait_for_window();
//	void wait_for_change();

//private:
//	std::shared_ptr <ConditionalVariuble> windowInputWait;
//	std::shared_ptr <ConditionalVariuble> changingParamsWait;
//	std::shared_ptr <std::mutex> lock;
//	bool state;
//} _oneInputCurentlyActive;

//std::array<MouseButtonB, SIZET(Mouse::Count)> _mouseButtons; 

//void setMouseBeforeX(double posX);
//void setMouseBeforeY(double posY);

//void setMouseAfterX(double posX);
//void setMouseAfterY(double posY);
//WindowTButton& FindWindowTButtonName(const std::string& name);