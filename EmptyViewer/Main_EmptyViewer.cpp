#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 1280;
int Height = 720;
std::vector<float> OutputImage;
// -------------------------------------------------

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Sphere {
	vec3 center;
	float radius;
};

struct Plane {
	vec3 point;
	vec3 normal;
};

std::vector<Sphere> spheres = {
	{{-4, 0, -7}, 1},
	{{0, 0, -7}, 2},
	{{4, 0, -7}, 1}
};

Plane plane = { {0, -1, -5}, {0, 1, 0} }; 

// Light source
vec3 light_position = vec3(-10, 10, -10);  // Directional light source

// Phong shading constants
float ambient_strength = 0.1f;
float diffuse_strength = 0.7f;
float specular_strength = 0.5f;
float shininess = 32.0f;

// Intersection for Sphere
bool intersectSphere(const Ray& ray, const Sphere& sphere, float& t) {
	vec3 oc = ray.origin - sphere.center;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant > 0) {
		t = (-b - glm::sqrt(discriminant)) / (2.0f * a);  // Use glm::sqrt
		return true;
	}
	return false;
}

bool intersectPlane(const Ray& ray, const Plane& plane, float& t) {
	float denom = dot(plane.normal, ray.direction);
	if (abs(denom) > 1e-6f) {
		vec3 p0l0 = plane.point - ray.origin;
		t = dot(p0l0, plane.normal) / denom;
		return (t >= 0);
	}
	return false;
}

// Phong Shading Model
vec3 phongShading(const vec3& point, const vec3& normal, const vec3& viewDir) {
	// Ambient
	vec3 ambient = ambient_strength * vec3(1.0f, 1.0f, 1.0f);

	// Diffuse
	vec3 lightDir = normalize(light_position - point);
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diffuse_strength * diff * vec3(1.0f, 1.0f, 1.0f);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
	vec3 specular = specular_strength * spec * vec3(1.0f, 1.0f, 1.0f);

	return ambient + diffuse + specular;
}

void render() {  // Function to render the scene
	OutputImage.clear(); // Clear the output image buffer
	vec3 eye = vec3(0, 0, 0); // Camera pos
	float aspectRatio = (float)Width / (float)Height;
	float viewportHeight = 0.2f;
	float viewportWidth = viewportHeight * aspectRatio;
	float focalLength = 0.1f;

	vec3 horizontal = vec3(viewportWidth, 0, 0);
	vec3 vertical = vec3(0, viewportHeight, 0);
	vec3 lowerLeftCorner = eye - horizontal / 2.0f - vertical / 2.0f - vec3(0, 0, focalLength);
	// Loop over pixel
	for (int j = 0; j < Height; ++j) {
		for (int i = 0; i < Width; ++i) {
			float u = (float)i / (Width - 1);
			float v = (float)j / (Height - 1);
			Ray ray = { eye, normalize(lowerLeftCorner + u * horizontal + v * vertical - eye) };

			vec3 color = vec3(0);
			float t_min = FLT_MAX;

			// Check intersection with spheres
			for (const auto& sphere : spheres) {
				float t;
				if (intersectSphere(ray, sphere, t) && t < t_min) {
					t_min = t;
					vec3 intersectionPoint = ray.origin + t * ray.direction;
					vec3 normal = normalize(intersectionPoint - sphere.center);
					vec3 viewDir = normalize(eye - intersectionPoint);
					color = phongShading(intersectionPoint, normal, viewDir);
					break;
				}
			}

			// Check intersection with plane
			if (t_min == FLT_MAX) {
				float t;
				if (intersectPlane(ray, plane, t) && t < t_min) {
					t_min = t;
					vec3 intersectionPoint = ray.origin + t * ray.direction;
					vec3 normal = plane.normal;
					vec3 viewDir = normalize(eye - intersectionPoint);
					color = phongShading(intersectionPoint, normal, viewDir);
				}
			}

			OutputImage.push_back(color.x);
			OutputImage.push_back(color.y);
			OutputImage.push_back(color.z);
		}
	}
}


void resize_callback(GLFWwindow*, int nw, int nh)
{
	//This is called in response to the window resizing.
	//The new width and height are passed in so we make 
	//any necessary changes:
	Width = nw;
	Height = nh;
	//Tell the viewport to use all of our screen estate
	glViewport(0, 0, nw, nh);

	//This is not necessary, we're just working in 2d so
	//why not let our spaces reflect it?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, static_cast<double>(Width)
		, 0.0, static_cast<double>(Height)
		, 1.0, -1.0);

	//Reserve memory for our render so that we don't do 
	//excessive allocations and render the image
	OutputImage.reserve(Width * Height * 3);
	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//We have an opengl context now. Everything from here on out 
	//is just managing our window or opengl directly.

	//Tell the opengl state machine we don't want it to make 
	//any assumptions about how pixels are aligned in memory 
	//during transfers between host and device (like glDrawPixels(...) )
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//We call our resize function once to set everything up initially
	//after registering it as a callback with glfw
	glfwSetFramebufferSizeCallback(window, resize_callback);
	resize_callback(NULL, Width, Height);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// -------------------------------------------------------------
		//Rendering begins!
		glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
		//and ends.
		// -------------------------------------------------------------

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Close when the user hits 'q' or escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}