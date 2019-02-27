#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

/*
	excerpt from olcPixelGameEngine.h
*/

/*
	Object Oriented Mode
	~~~~~~~~~~~~~~~~~~~~

	If the olcPixelGameEngine.h is called from several sources it can cause
	multiple definitions of objects. To prevent this, ONLY ONE of the pathways
	to including this file must have OLC_PGE_APPLICATION defined before it. This prevents
	the definitions being duplicated.

	Consider the following project structure:

	Class1.h	- Includes olcPixelGameEngine.h, overrides olc::PixelGameEngine
	Class1.cpp	- #define OLC_PGE_APPLICATION #include "Class1.h"
	Class2.h	- Includes Class1.h, which includes olcPixelGameEngine.h
	Class2.cpp	- #define OLC_PGE_APPLICATION #include "Class2.h"
	main.cpp	- Includes Class1.h and Class2.h

	If all else fails, create a file called "olcPixelGameEngine.cpp" with the following
	two lines. Then you can just #include "olcPixelGameEngine.h" as normal without worrying
	about defining things. Dont forget to include that cpp file as part of your build!

	#define OLC_PGE_APPLICATION
	#include "olcPixelGameEngine.h"

*/