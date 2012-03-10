
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

#include <gl3/gl3w.h>
#include <GL/glext.h>
#include <GL/glfw.h>

#include <cstdio>
#include <cstdlib>

#include <ui.h>
#include <glUtils.h>
#include <root.h>


int main(int argc, char *argv[])
{
	if ( !UI::init(640,480) || isGLError() )
	{
		fprintf(stderr, "ERROR: Could not initialize UI.\n");
		UI::shutdown();
		return EXIT_FAILURE;
	}

	// Print out some info on the runtime state
	fprintf(stdout, "GL VERSION: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(stdout, "Window settings:\n\trgba bits: (%d,%d,%d,%d)\n"
			"\tdepth bits: %d\n"
			"\tstencil bits: %d\n",
			glfwGetWindowParam(GLFW_RED_BITS),
			glfwGetWindowParam(GLFW_GREEN_BITS),
			glfwGetWindowParam(GLFW_BLUE_BITS),
			glfwGetWindowParam(GLFW_ALPHA_BITS),
			glfwGetWindowParam(GLFW_DEPTH_BITS),
			glfwGetWindowParam(GLFW_STENCIL_BITS));

	// Create & initialize the program object
	UI::Callbacks *program;

	program = new Root();
	if ( ! ((Root*)program)->init() )
	{
		fprintf(stderr, "Failed to initialize program\n");
		UI::shutdown();
		return EXIT_FAILURE;
	}
	// Set the window's title
	UI::setWindowTitle("CMPT829 - Multi-Light Deferred Cascaded Smoothed ShadowMap - Faham Negini");

	UI::setCallbacks(program);

	// Event loop
	UI::mainLoop();

	// Clean up the UI before exit
	UI::shutdown();

	delete program;

	return EXIT_SUCCESS;
}
