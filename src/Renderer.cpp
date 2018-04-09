#include "stdafx.h"
#include "..\include\Renderer.h"
#include <iostream>
MenticsGame::Renderer::Renderer()
	: b(5, nn::nn_make_unique<BasicTrajectory>(BasicTrajectory(0, 5000, vect3(0, 0, 0), vect3(0, 0, 0), vect3(0, 0, 0))))
     , selectionManager(nn::nn_addr(b))
	 , w(1)
{
	window = init();
	
	shaders.init("VertexShader.glsl", "FragmentShader.glsl");
	compute_handle = loadComputeShader();
	aspectRatio = viewportWidth / (float)viewportHeight;

	//// Compute Shader data ////

	cameraId = glGetUniformLocation(compute_handle, "cameraPos");
	gameTimeId = glGetUniformLocation(compute_handle, "gameTime");
	dtId = glGetUniformLocation(compute_handle, "dt");

	// CPU to ComputeShader "World" buffer

	for (int i = 0; i < numWorldObjects; i++) {
		w.createQuip(0, makeTrajRandom(10, 2, 1), "bot");
	}

	std::this_thread::sleep_for(chrono::milliseconds(2000));

	w.allAgentsData(a_data);
	world[0].pos = glm::vec4(0, 0, 0, 1);
	world[0].vel = glm::vec4(0, 0, 0, 0);
	world[0].acc = glm::vec4(0, 0, 0, 0);
	world[0].radius = 0.5f;
	for (int i = 1; i < numWorldObjects; i++) {
		world[i].pos = toGlmPoint(a_data[i].pva.pos);
		world[i].vel = toGlmVector(a_data[i].pva.vel);
		world[i].acc = toGlmVector(a_data[i].pva.acc);
		world[i].radius = 0.1f;
	}

	glGenBuffers(1, &worldId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, worldId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(WorldObject) * numWorldObjects, &world, GL_DYNAMIC_COPY);

	// Counter Buffer
	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); // unbind the buffer 

											   // ComputeShader to FragmentShader "Index" buffer

	glGenBuffers(1, &indexId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere) * numWorldObjects, &indexData, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//// Fragment Shader data ////
	Resolution = glGetUniformLocation(shaders.id(), "Resolution");
	ray00Id = glGetUniformLocation(shaders.id(), "ray00");
	ray01Id = glGetUniformLocation(shaders.id(), "ray01");
	ray10Id = glGetUniformLocation(shaders.id(), "ray10");
	ray11Id = glGetUniformLocation(shaders.id(), "ray11");

	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	frameTimes[windowSize];
	index = 0;
	startNanos = currentTimeNanos();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, worldId);

	ImGui::CreateContext();

	ImGui_ImplGlfwGL3_Init(window, true);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
}

void MenticsGame::Renderer::run()
{
	do {
		uint64_t nanos = currentTimeNanos();
		frameTimes[index] = nanos;
		uint8_t prevIndex = index - 1 < 0 ? windowSize - 1 : index - 1;
		float dt = (frameTimes[index] - frameTimes[prevIndex]) / (double)windowSize;
		if (index == 0) {
			printf("Average frame time millis: %.4f\n", dt / 1e6);
		}
		index = (index + 1) % windowSize;

		
		
		w.run();

		cam.update(aspectRatio);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();

		glUseProgram(compute_handle);
		glUniform3f(cameraId, cam.cam.Position.x, cam.cam.Position.y, cam.cam.Position.z);
		float gameTime = (float)(nanos - startNanos) / 1000000000.0;

		ImGui::Begin("test");
		ImGui::Text("Game Time: %.2f", gameTime);
		ImGui::Text("CameraPos %.2f, %.2f, %.2f", cam.cam.Position.x, cam.cam.Position.y, cam.cam.Position.z);
		ImGui::End();

		glUniform1f(gameTimeId, gameTime);
		glUniform1f(dtId, dt);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
		glInvalidateBufferData(GL_ATOMIC_COUNTER_BUFFER);
		glClearBufferData(GL_ATOMIC_COUNTER_BARRIER_BIT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, 0, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, worldId);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indexId);

		glDispatchCompute(numWorldObjects, 1, 1);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		shaders.bind();

		glViewport(0, 0, viewportWidth, viewportHeight);
		glUniform2f(Resolution, (float)viewportWidth, (float)viewportHeight);
		glUniform3f(ray00Id, cam.ray00.x, cam.ray00.y, cam.ray00.z);
		glUniform3f(ray01Id, cam.ray01.x, cam.ray01.y, cam.ray01.z);
		glUniform3f(ray10Id, cam.ray10.x, cam.ray10.y, cam.ray10.z);
		glUniform3f(ray11Id, cam.ray11.x, cam.ray11.y, cam.ray11.z);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indexId);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexId);
		Sphere *ptrToIndexData = (Sphere*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		int testItem = 0;
		std::cout << '[' << ptrToIndexData[testItem].center.x
			<< ',' << ptrToIndexData[testItem].center.y
			<< ',' << ptrToIndexData[testItem].center.z
			<< ',' << ptrToIndexData[testItem].center2
			<< ',' << ptrToIndexData[testItem].radius2
			<< ']';
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size 
			GL_FLOAT,           // type 
			GL_FALSE,           // normalized?  
			0,                  // stride 
			(void*)0            // array buffer offset  
		);

		// Draw the triangle
		glDrawArrays(GL_QUADS, 0, 4);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
	} while (!glfwGetKey(window, GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window));

	glBindVertexArray(0);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

GLuint MenticsGame::Renderer::loadComputeShader()
{
	std::string fText = textFileRead("ComputeShader.glsl");
	const char *Text = fText.c_str();
	std::cout << "\n compute shader ------------\n" << Text << std::endl;
	GLuint compute_handle = glCreateProgram();
	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, &Text, NULL);
	glCompileShader(compute_shader);
	glAttachShader(compute_handle, compute_shader);
	//glDeleteShader(compute_shader);
	glLinkProgram(compute_handle);
	validateShader(compute_shader, "ComputeShader.glsl");
	validateProgram(compute_handle);
	return compute_handle;
}

GLFWwindow * MenticsGame::Renderer::init()
{
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		return 0;
	}

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	static const int width = mode->width / 2;
	static const int height = mode->height / 2;

	window = glfwCreateWindow(width, height, u8"Test", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		fprintf(stderr, "Failed to create glfw windows\n");
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	if (glewInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 0;
	}

	//glfwSetWindowSizeCallback(window, window_size_callback);
	glViewport(0, 0, width, height);
	viewportWidth = width;
	viewportHeight = height;

	return window;
}




MenticsGame::Renderer::~Renderer()
{
}
