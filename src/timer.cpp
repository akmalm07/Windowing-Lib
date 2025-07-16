
#include "timer.h"

namespace windowing
{

	Timer::Timer()
	{
		_timeStarted = std::chrono::steady_clock::now();

		_isStopped = false;
	}

	Timer::Timer(bool start)
	{
		if (start)
		{
			_state = true;
			_timeStarted = std::chrono::steady_clock::now();
		}
		else
		{
			_state = false;
			_timeStarted = std::chrono::steady_clock::time_point();
		}

		_isStopped = false;
	}



	float Timer::stop_time(bool debug)
	{
		_state = true;
		_isStopped = true;

		_timeEnded = std::chrono::steady_clock::now();

		std::chrono::duration duration = _timeEnded - _timeStarted;

		float milliseconds = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

		if (debug)
		{
			std::cout << "Timer stops at " << milliseconds << " ms \n";
		}

		return milliseconds;
	}

	void Timer::start_time()
	{
		_state = true;
		_isStopped = false;

		_timeStarted = std::chrono::steady_clock::now();
	}

	double Timer::current_time_ms() const 
	{
		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _timeStarted).count());
	}

	double Timer::current_time_s() const
	{
		return current_time_ms() / 1000.0;
	}

	double Timer::reset_ms(bool debug)
	{
		if (_isStopped)
		{
			if (debug)
			{
				std::cout << "Timer is stopped\n";
			}
			return 0.0f;
		}
		_timeEnded = std::chrono::steady_clock::now();

		std::chrono::duration duration = _timeEnded - _timeStarted;

		_timeStarted = std::chrono::steady_clock::now();

		double milliseconds = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

		if (debug)
		{
			std::cout << milliseconds << " ms \n";
		}

		return milliseconds;
	}

	double Timer::reset_s(bool debug)
	{
		return reset_ms(debug) / 1000.0;
	}

	bool Timer::get_state() const
	{
		return _state;
	}

	double Timer::get_delta_time_ms(bool debug)
	{
		double time = reset_ms(debug);
		if (time < 0.005)
		{
			time = 0.005;
		}
		return time;
	}

	double Timer::get_delta_time_s(bool debug)
	{
		return get_delta_time_ms(debug) / 1000.0;
	}


	Timer::~Timer()
	{
		_state = false;

		if (!_isStopped)
		{
			stop_time(false);
		}
	}


} // namespace windowing
//
//
//#include "windowing.h"
//
//int main()
//{
//	windowing::Window window(1280, 720, "ImGui Test", false, true);
//
//	// Main loop
//	while (!window.get_should_close()) 
//	{
//		window.update();		
//
//		// ImGui window
//		ImGui::Begin("Hello, world!");
//		ImGui::Text("This is an ImGui + OpenGL window.");
//		ImGui::End();
//
//		// Rendering
//		windowing::clear_color();
//		windowing::render();
//
//	}
//
//	// Cleanup
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(window.get_window());
//	glfwTerminate();
//
//	return 0;
//}


/*
#include "windowing.h"

int main()
{
	// Initialize GLFW
	if (!glfwInit())
		return -1;

	// Setup OpenGL version (3.3 Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Test", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW (must be after context is current)
	if (glewInit() != GLEW_OK) {
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui window
		ImGui::Begin("Hello, world!");
		ImGui::Text("This is an ImGui + OpenGL window.");
		if (ImGui::Button("Click Me"))
		{
		}
		ImGui::End();


		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
*/