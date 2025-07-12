
#include "keys.h"

#include "window_input.h"
#include "window.h"
#include "timer.h"	




namespace windowing {

	uint32_t Window::g_numOfWindows = 0;

	bool Window::_calledBufferSize = true;

	Window::Window()
	{
		_name = "Untitled Window";

		_mainWindow = nullptr;

		_width = 800.0f;
		_height = 800.0f;

		_aspectRatio = 1.0f;

		_mouseCurrentX = _width / 2;
		_mouseCurrentY = _height / 2;
	}

	Window::Window(float windowWidth, float windowHeight, const std::string& name, bool createWindow)
	{
		init(windowWidth, windowHeight, name, createWindow);

		set_escape_button(Keys::Esc, Action::Press);
		//set_movement_callbacks();
	}

	void Window::init(float windowWidth, float windowHeight, const std::string& name, bool createWindow)
	{

		_name = name;

		_mainWindow = nullptr;

		_bufferWidth = 0;
		_bufferHeight = 0;

		_width = windowWidth;
		_height = windowHeight;

		_mouseCurrentX = _width / 2;
		_mouseCurrentY = _height / 2;

		_aspectRatio = _width / _height;

		if (createWindow)
		{
			if (!create_window(_name, false))
			{
				throw std::runtime_error("Failed to create window!");
			}
		}
	}

	Window::Window(Window&& other) noexcept
	{
		_inputManager = std::move(other._inputManager);
		_mainWindow = other._mainWindow; 
		_width = other._width;
		_height = other._height;
		_bufferWidth = other._bufferWidth;
		_bufferHeight = other._bufferHeight;
		_aspectRatio = other._aspectRatio;

		_timer = std::move(other._timer); 
		_name = std::move(other._name);
		_updated = other._updated; 

		other._mainWindow = nullptr; 
		other._width = 0.0f; 
		other._height = 0.0f;
		other._bufferWidth = 0; 
		other._bufferHeight = 0; 
		other._aspectRatio = 1.0f; 

	}

	Window& Window::operator=(Window&& other) noexcept
	{
		if (this != nullptr)
		{
			_inputManager = std::move(other._inputManager);
			_mainWindow = other._mainWindow;
			_width = other._width;
			_height = other._height;
			_bufferWidth = other._bufferWidth;
			_bufferHeight = other._bufferHeight;
			_aspectRatio = other._aspectRatio;

			_timer = std::move(other._timer);
			_name = std::move(other._name);
			_updated = other._updated;


			other._mainWindow = nullptr;
			other._width = 0.0f;
			other._height = 0.0f;
			other._bufferWidth = 0;
			other._bufferHeight = 0;
			other._aspectRatio = 1.0f;

		}
		return *this;
	}


	void Window::set_escape_button(Keys key, Action action, std::optional<Mods> mod)
	{
		std::function<void()> f = [this]()
			{
				glfwSetWindowShouldClose(_mainWindow, GLFW_TRUE);
			};
		register_callback<KeyCombInputOne>(
			KeyCombInputOne(key, action, mod.value_or(Mods::None)),
			f,
			"Exit"
		);
	}

	bool Window::is_updated()
	{
		return _updated;	
	}

	void Window::update_fps_tag()
	{
		static size_t frameCount = 0;

		static std::chrono::time_point lastTime = std::chrono::steady_clock::now();
	
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - lastTime);

		if (duration.count() >= 1)
		{
			std::string fpsTitle = _name + " | FPS: " + std::to_string(frameCount);
			glfwSetWindowTitle(_mainWindow, fpsTitle.c_str());

			lastTime = std::chrono::steady_clock::now();
			frameCount = 0;
		}
		else
		{
			frameCount++;
		}

	}

	void Window::update()
	{
		reset_delta_time();
		swap_buffers();
		poll_events();
		update_fps_tag();
	}

	void Window::reset_delta_time()
	{
		_timer.get_delta_time_ms();
	}

	float Window::get_delta_time_sec()
	{
		return _timer.get_delta_time_s();
	}

	float Window::get_delta_time_ms() 
	{
		return _timer.get_delta_time_ms();
	}

	bool Window::create_window(const std::string& name, bool disableCursor)
	{
		std::cout << "Window count: " << g_numOfWindows << "\n";

		_name = name;

		if (g_numOfWindows == 0)
		{
			if (glfwInit() == GLFW_FALSE)
			{
				std::cerr << "Error initializing GLFW! \n";
				return false;
			}

			glfwSetErrorCallback([](int error, const char* description) {
				fprintf(stderr, "GLFW Error: %s\n", description);
				});

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		_mainWindow = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
		if (!_mainWindow)
		{
			throw std::runtime_error("GLFW window creation failed!");
		}

		std::cout << "Successfully created GLFW window \"" << _name << "\", "
			<< "width: " << _width << ", height: " << _height << "\n";

		if (g_numOfWindows == 0)
		{
			glfwMakeContextCurrent(_mainWindow);
			glewExperimental = GL_TRUE;

			if (glewInit() != GLEW_OK)
			{
				std::cerr << "Error initializing GLEW! \n";
				return false;
			}
			
			
			//glEnable(GL_CULL_FACE); //DEBUG
			//glCullFace(GL_BACK);
			//glFrontFace(GL_CCW);

			//glEnable(GL_DEPTH_TEST);
			//glDepthFunc(GL_LESS);
			

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			glEnable(GL_DEPTH_TEST);

			glViewport(0, 0, static_cast<int>(_width), static_cast<int>(_height));

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		g_numOfWindows++;

		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight);

		int theWidth = static_cast<int>(_width);
		int theHeight = static_cast<int>(_height);
		glfwGetWindowSize(_mainWindow, &theWidth, &theHeight);

		glfwSetCursorPos(_mainWindow, theWidth / 2, theHeight / 2);

		if (disableCursor)
		{
			glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		CreateCallbacks();

		glfwSetWindowUserPointer(_mainWindow, this);

		return true;
	}



	void Window::set_disable_cursor(bool disableCursor)
	{
		if (disableCursor)
		{
			glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	float Window::get_aspect_ratio() const
	{
		return _aspectRatio;
	}


	void Window::set_should_close(bool trueOrFalse)
	{
		return glfwSetWindowShouldClose(_mainWindow, (trueOrFalse ? GLFW_TRUE : GLFW_FALSE));
	}

	void Window::swap_buffers() const
	{
		glfwSwapBuffers(_mainWindow);
	}


	/*
	void Window::set_movement_callbacks()
	{
		std::array<Direction, 6> dirs =
		{
			Direction::Up,
			Direction::Down,
			Direction::Left,
			Direction::Right,
			Direction::Forward,
			Direction::Backward

		};

		std::array<std::string, dirs.size()> dirNames =
		{
			"MoveUp",
			"MoveDown",
			"MoveLeft",
			"MoveRight",
			"MoveForward",
			"MoveBackward"
		};

		std::array<KeyCombInputOne, dirs.size()> input =
		{
			KeyCombInputOne(Keys::W, Action::Press | Action::Repeat),
			KeyCombInputOne(Keys::S, Action::Press | Action::Repeat),
			KeyCombInputOne(Keys::A, Action::Press | Action::Repeat),
			KeyCombInputOne(Keys::D, Action::Press | Action::Repeat),
			KeyCombInputOne(Keys::Q, Action::Press | Action::Repeat),
			KeyCombInputOne(Keys::E, Action::Press | Action::Repeat)
		};



		std::array<std::function<void()>, dirs.size()> keyMoveFuncs;

		for (size_t i = 0; i < dirs.size(); i++)
		{
			keyMoveFuncs[i] = [this, dir = dirs[i]]() -> void
				{
					_camera->event_key(dir, _timer.current_time_s());
				};

			register_callback<KeyCombInputOne>(
				input[i],
				keyMoveFuncs[i],
				dirNames[i]
			);
		}

		std::function<void()> mouseFuncs = [this]()
			{
				_camera->event_key(_timer.current_time_s(), -_mouseChangeX, -_mouseChangeY);
			};
		
		register_callback<MouseMoveInput>(
			MouseMoveInput(MouseChange::MoveXY),
			mouseFuncs,
			"MouseMove"
		);

		std::function<void()> scrollFunc = [this]()
			{
				_camera->event_scroll(_scrollOffsetY);
			};

		register_callback<MouseScrollInput>(
			MouseScrollInput(MouseChange::MoveY),
			scrollFunc,
			"MouseScroll"
		);

	}
	*/

	bool Window::is_key_active(Keys key, Action action) const
	{
		return glfwGetKey(_mainWindow, static_cast<int>(key)) == convert_to_glfw(action);
	}

	std::string Window::get_name() const
	{
		return _name;
	}

	void Window::set_name(const std::string& name)
	{
		glfwSetWindowTitle(_mainWindow, name.c_str());
		_name = std::string(name);
	}

	bool Window::get_should_close() const
	{
		return glfwWindowShouldClose(_mainWindow);
	}


	int Window::get_buffer_width()
	{ 
		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight);

		return _bufferWidth; 
	}


	int Window::get_buffer_height()
	{ 
		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight); 

		return _bufferHeight; 
	}

	const int* Window::get_buffer_width_p()
	{
		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight);

		return &_bufferWidth;
	}

	const int* Window::get_buffer_height_p()
	{
		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight);

		return &_bufferHeight;
	}

	GLFWwindow* Window::get_window() const
	{
		return _mainWindow;
	}

	bool Window::set_window(GLFWwindow* window)
	{ 
		if (!window)
		{
			fprintf(stderr, "\nThe Window provided was nullptr!\n");
			return false;
		}
		_mainWindow = std::move(window); 


		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); 


		glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight); 

		int width;
		int height;

		glfwGetWindowSize(_mainWindow, &width, &height); 

		glfwSetCursorPos(_mainWindow, width / 2, height / 2); 

		_width = (float)width;
		_height = (float)height;

		glfwMakeContextCurrent(_mainWindow); 
		  
		CreateCallbacks(); 

		//glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetWindowUserPointer(_mainWindow, this); 

		_aspectRatio = _width / _height;

		return true;

	}


	float Window::get_width() const
	{
		return _width;
	}

	float Window::get_height() const
	{ 
		return _height; 
	}


	void Window::make_window_context_current()
	{
		glfwMakeContextCurrent(_mainWindow);
	}


	void Window::clear_window()
	{

		if (!_mainWindow)
		{
			_mainWindow = nullptr;
			glfwDestroyWindow(_mainWindow);
		}
		if (g_numOfWindows > 0)
		{
			g_numOfWindows--;
		}

		//GraphicsHandlerAPI::DeleteWindowBase(_mainWindow);
	}

	void Window::poll_events() const
	{
		glfwPollEvents();
	}

	void Window::wait_events() const
	{
		glfwWaitEvents();
	}

	Window::~Window()
	{
		clear_window();
	}


	//Private Functions
	void Window::CreateCallbacks()
	{
		//glfwSetWindowUserPointer(_mainWindow, this); 
		glfwSetKeyCallback(_mainWindow, m_HandleKeys);
		glfwSetCursorPosCallback(_mainWindow, m_HandleMouseCursor);
		glfwSetMouseButtonCallback(_mainWindow, m_HandleMouseButtons);
		glfwSetScrollCallback(_mainWindow, m_HandleMouseScroll);


	}


	void Window::m_HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
	{
		Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

		instance->HandleKeys(key, code, action, mode);
	}


	void Window::m_HandleMouseCursor(GLFWwindow* window, double posX, double posY)
	{
		Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

		instance->HandleMouseCursor(posX, posY);
	}

	void Window::m_HandleMouseScroll(GLFWwindow* window, double posX, double posY)
	{
		Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		instance->HandleMouseScroll(posX, posY);
	}


	void Window::m_HandleMouseButtons(GLFWwindow* window, int button, int action, int mods)
	{
		Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));

		instance->HandleMouseButtons(button, action, mods);
	}

	int Window::convert_to_glfw(Action action) const
	{
		switch (action)
		{
		case Action::Press:
			return GLFW_PRESS;
		case Action::Release:
			return GLFW_RELEASE;
		case Action::Repeat:
			return GLFW_REPEAT;
		default:
			return GLFW_RELEASE; // Default to release if unknown action
		}
	}


	void Window::HandleKeys(int key, int code, int action, int mode)
	{
		Mods mod = Mods::None;
		if (mode == 0)
		{
			mod = MODS(mode);
		}

		const auto keys = _inputManager.list_entires_const<KeyCombInputOne>();

		switch (action)
		{
		case GLFW_PRESS:
		{
			for (const auto& ky : keys)
			{
				if (ky->matches(KeyCombInputOne(KEYS(key), Action::Press, mod)))
				{
					_updated = true;
					_keys[key] = true;
					std::cout << "Key pressed: " << key << "\n";
					_inputManager.emit(KeyCombInputOne(KEYS(key), Action::Press, mod));
				}
			}
			
		}
		break;
		case GLFW_RELEASE:
		{
			for (const auto& ky : keys)
			{
				if (ky->matches(KeyCombInputOne(KEYS(key), Action::Release, mod)))
				{
					_updated = false;
					_keys[key] = false;

					_inputManager.emit(KeyCombInputOne(KEYS(key), Action::Release, mod));
				}
			}
			
		}
		break;
		case GLFW_REPEAT:
		{
			for (const auto& ky : keys)
			{
				if (ky->matches(KeyCombInputOne(KEYS(key), Action::Repeat, mod)))
				{
					_updated = true;
					_keys[key] = false;

					_inputManager.emit(KeyCombInputOne(KEYS(key), Action::Repeat, mod));
				}
			}
		
		}
		break;
		}//switch statement

		const auto keyPoly = _inputManager.list_entires_const<KeyCombInputPoly>();

		if (keyPoly.empty())
		{
			return;
		}

		for (const auto& ky : keyPoly)
		{
			bool success = true;
			for (const auto& number : ky->input.numbers)// Spacialize function to further the ease of use
			{
				if (!_keys[static_cast<int>(number)])
				{
					success = false;
				}
				if (success)
				{
					ky->emit_and_update();
				}
			}
		}
		
	}

	


	void Window::HandleMouseCursor(double posX, double posY)
	{
		_mouseChangeX = posX - _mouseCurrentX;
		_mouseChangeY = _mouseCurrentY - posY;

		_mouseCurrentX = posX;
		_mouseCurrentY = posY;
		
		const auto mice = _inputManager.list_entires_const<MouseMoveInput>();
		if (!mice.empty())
		{
			for (const auto& mouse : mice)
			{
				mouse->emit_and_update();
			}
		}
	}

	void Window::HandleMouseScroll(double xOffset, double yOffset)
	{
		_scrollOffsetY = yOffset;
		_scrollOffsetX = xOffset;

		const auto scrolls = _inputManager.list_entires_const<MouseScrollInput>();
		if (!scrolls.empty())
		{
			for (const auto& scroll : scrolls)
			{
				scroll->emit_and_update();
			}
		}
	}


	void Window::HandleMouseButtons(int mouseButton, int action, int mods)
	{

		const auto keys = _inputManager.list_entires_const<MouseButtonInput>();
		const auto buttons = _inputManager.list_entires_const<AABButtonInput>();

		switch (action)
		{
		case GLFW_PRESS: 
		{
			Mouse mouse = (mouseButton == GLFW_MOUSE_BUTTON_LEFT ? Mouse::Left : Mouse::Right);

			for (const auto& ky : keys)
			{
				if (ky->matches(MouseButtonInput(mouse, Action::Press)))
				{
					_inputManager.emit(MouseButtonInput(mouse, Action::Press));
				}
			}

			for (const auto& ky : buttons)
			{
				if (ky->is_hovering(_mouseCurrentX, _mouseCurrentY, Action::Press, mouse))
				{
					ky->emit_and_update();
				}
			}

			break;
		}

		case GLFW_RELEASE:
		{

			Mouse mouse = (mouseButton == GLFW_MOUSE_BUTTON_LEFT ? Mouse::Left : Mouse::Right);

			for (const auto& ky : keys)
			{
				if (ky->matches(MouseButtonInput(mouse, Action::Release)))
				{
					_inputManager.emit(MouseButtonInput(mouse, Action::Release));
				}
			}

			for (const auto& ky : buttons)
			{
				if (ky->input.is_touching(_mouseCurrentX, _mouseCurrentY, Action::Release, mouse))
				{
					ky->emit_and_update();
				}
			}

			break;
		}

		case GLFW_REPEAT:
		{

			Mouse mouse = (mouseButton == GLFW_MOUSE_BUTTON_LEFT ? Mouse::Left : Mouse::Right);

			for (const auto& ky : keys)
			{
				if (ky->matches(MouseButtonInput(mouse, Action::Repeat)))
				{
					_inputManager.emit(MouseButtonInput(mouse, Action::Repeat));
				}
			}

			for (const auto& ky : buttons)
			{
				if (ky->input.is_touching(_mouseCurrentX, _mouseCurrentY, Action::Repeat, mouse))
				{
					ky->emit_and_update();
				}
			}

			break;
		}
		}//switch statement

	}

	// TO BE ADDED

	//Window::Async::Async() = default;

	//Window::Async::Async(const ThreadControlInfo& condition)
	//	: windowInputWait(condition.windowInputWait), changingParamsWait(condition.changingParamsWait), lock(condition.lock), state(false)
	//{}

	//Window::Async::Async(Async&& other) noexcept = default;

	//Window::Async& Window::Async::operator=(Async&& other) noexcept = default;

	//void Window::Async::notify_change(bool needsChange)
	//{
	//	if (needsChange)
	//	{
	//		state = true;
	//		changingParamsWait->notify_one();
	//		windowInputWait->wait_for_condition(*lock);

	//	}
	//}

	//void Window::Async::wait_for_window()
	//{
	//	windowInputWait->wait_for_condition(*lock);
	//}

	//void Window::Async::wait_for_change()
	//{

	//	changingParamsWait->wait_for_condition(*lock);
	//}

	//bool Window::Async::get_state() const
	//{
	//	return state;
	//}

	//void Window::Async::notify_window()
	//{
	//	state = false;
	//	
	//	windowInputWait->notify_one();
	//	changingParamsWait->wait_for_condition(*lock);
	//}
	//void Window::AllowWindowoContinueAndWait()
	//{
	//	_oneInputCurentlyActive.notify_window("I am from the camera logic");
	//}

	//void Window::WaitInitallyForSignal()
	//{
	//	_oneInputCurentlyActive.wait_for_change();
	//}
	//void Window::SetAsyncPollEvents(const ThreadControlInfo& cv)
	//{
	//	_oneInputCurentlyActive = cv;
	//}
	/*

	ClassicWindow::ClassicWindow(float windowWidth, float windowHeight, const std::string& name, bool createWindow)
		: Window(windowWidth, windowHeight, name, createWindow)
	{
	}

	ClassicWindow::ClassicWindow(ClassicWindow&& other) noexcept
		: Window(std::move(other))
	{
		_camera = std::move(other._camera);
	}

	ClassicWindow& ClassicWindow::operator=(ClassicWindow&& other) noexcept
	{
		if (this != &other)
		{
			Window::operator=(std::move(other));
			_camera = std::move(other._camera);
		}
		return *this;
	}

	inline QuatWindow::QuatWindow(float windowWidth, float windowHeight, const std::string& name, bool createWindow)
		: Window(windowWidth, windowHeight, name, createWindow)
	{
	}

	inline QuatWindow::QuatWindow(QuatWindow&& other) noexcept
		: Window(std::move(other))
	{
		_camera = std::move(other._camera);
	}

	inline QuatWindow& QuatWindow::operator=(QuatWindow&& other) noexcept
	{
		if (this == &other)
		{
			Window::operator=(std::move(other));
			_camera = std::move(other._camera);
		}
		return *this;
	}
	*/


} //Namespace windowing