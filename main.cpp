#define USE_THIS
#ifdef USE_THIS
#include "vis.h"

#include <cstring> // stoi
#include <chrono>

int main (int argc, char* argv[])
{
	std::cout << "SPG assignment \"marching cubes\"\n=============================\n" << std::endl;

	// read program arguments
	// TODO: care a little more <3 --> use getopt
	/*for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-v") == 0) vis = true;
	}*/
	
	Vis vis;
	vis.display();
}
#endif // USE_THIS