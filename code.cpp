//One Lone Coder's command prompt engine for reference
//https://www.youtube.com/c/javidx9
//https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h
//https://github.com/OneLoneCoder/CommandLineFPS/blob/master/CommandLineFPS.cpp

#include <Windows.h> //https://docs.microsoft.com/en-us/previous-versions//aa383686(v=vs.85)
#include <iostream>
#include <chrono>
#include <map>

#define PI 3.14159265f
#define RAD 0.01745329f
#define DEG 57.29577951f
#define FPS60 0.01666666f
#define FPS30 0.03333333f
#define insertws swprintf
#define formatws swprintf
#define RALT 0xF001
#define LALT 0xF002
#define RCTRL 0xF003
#define LCTRL 0xF004
#define SHIFT 0xF005
#define MOUSE_DCLICK 0xF009
#define MOUSE_SCROLL 0xF010
#define MOUSE_BTN 0xF011
#define MOUSE_X 0xF058
#define MOUSE_Y 0xF059

//There are some weird issues with the 1st summary, so declaring an empty one causes the rest to work fine
///<summary>
///</summary>


namespace cmde
{
	struct VEC4F
	{
		float x;
		float y;
		float z;
		float w;

		VEC4F() { x = y = z = w = 0; }
		VEC4F(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		VEC4F operator* (const float& a) { return VEC4F(x * a, y * a, z * a, w * a); }
		VEC4F operator/ (const float& a) { return VEC4F(x / a, y / a, z / a, w / a); }
		VEC4F operator* (const VEC4F& a) { return VEC4F(x * a.x, y * a.y, z * a.z, w * a.w); }
		VEC4F operator+ (const VEC4F& a) { return VEC4F(x + a.x, y + a.y, z + a.z, w + a.w); }
		VEC4F operator- (const VEC4F& a) { return VEC4F(x - a.x, y - a.y, z - a.z, w - a.w); }
	};
	struct VEC3F : public VEC4F
	{
		VEC3F() { x = y = z = w = 0; }
		VEC3F(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			w = 0;
		}
		VEC3F(VEC4F v) { x = v.x; y = v.y; z = v.z; w = 0; }
	};
	struct VEC2F : public VEC4F
	{
		VEC2F() { x = y = z = w = 0; }
		VEC2F(float x, float y)
		{
			this->x = x;
			this->y = y;
			z = 0;
			w = 0;
		}
		VEC2F(VEC4F v) { x = v.x; y = v.y; z = 0; w = 0; }
	};

	struct FUNC
	{
		virtual VEC4F f(float x) = 0;
		virtual VEC4F derivative(float x) = 0;
	};
	struct LINEAR : public FUNC
	{
		VEC4F a;
		VEC4F b;

		LINEAR() { a = b = VEC4F(); }
		LINEAR(VEC4F a, VEC4F b)
		{
			this->a = a;
			this->b = b;
		}

		VEC4F f(float x)
		{
			return a * x + b;
		}

		VEC4F derivative(float x)
		{
			return a;
		}
	};
	struct QUADRATIC : public FUNC
	{
		VEC4F a;
		VEC4F b;
		VEC4F c;

		QUADRATIC() { a = b = c = VEC4F(); }
		QUADRATIC(VEC4F a, VEC4F b, VEC4F c)
		{
			this->a = a;
			this->b = b;
			this->c = c;
		}

		VEC4F f(float x)
		{
			return a * x * x + b * x + c;
		}

		VEC4F derivative(float x)
		{
			return a * 2 * x + b;
		}
	};


	class CMDEngine
	{
	private:
		HANDLE console;
		HANDLE consoleInput;
		HWND window;
		SMALL_RECT sr;
		bool upscale;
		wchar_t _msg[128];
		COORD _screenSize;
		int _pixelCount;
		float _deltaTime;
	public:
		const COORD& screenSize = _screenSize;
		const int& pixelCount = _pixelCount;
		const float& deltaTime = _deltaTime;
		CHAR_INFO* screen;
		bool running;
		CHAR_INFO emptyChar;
		bool autoClearScreen;
		wchar_t title[256];
		float fpsLimit;
		/// <summary>0 -> nothing ; 1 -> released ; 2 -> pressed ; 3+ -> held (for n-2 frames)</summary>
		std::map<wchar_t, short> inputs;

						/// <summary>
						/// Constructor for the command prompt engine
						/// </summary>
						/// <param name="screenWidth">The width of the command prompt in characters</param>
						/// <param name="screenHeight">The height of the command prompt in characters</param>
						/// <param name="fontWidth">The width of the characters in pixels</param>
						/// <param name="fontHeight">The height of the characters in pixels (Less than 3 may cause lines between pixels; 1 may cause distortions for some resolutions)</param>
						/// <param name="autoUpscale">Whether to automatically scale the font up so that it occupies the most amount of screen space</param>
						/// <param name="clearScreen">Whether to automatically clear the screen at the beginning of 'Update()' (You can do so manually with the 'ClearFrame()' function)</param>
						/// <param name="maxFPS">The minimum amount of time before the next frame is drawn (1/fps). Setting this to 0 or less will allow for infinte framerate</param>
		CMDEngine(short screenWidth, short screenHeight, short fontWidth = 1, short fontHeight = 1, bool autoUpscale = true, bool clearScreen = true, float maxFPS = FPS60)
		{
			upscale = autoUpscale;
			_screenSize = { (short)screenWidth, (short)screenHeight };
			_pixelCount = screenWidth * screenHeight;
			autoClearScreen = clearScreen;
			running = true;
			if (fontWidth > 30 || fontHeight > 30)
			{
				ThrowError(L"FontTooBig (30)");
				return;
			}
			if (screenWidth < 15 || screenHeight < 2 || fontWidth < 1 || fontHeight < 1)
			{
				ThrowError(L"SmallerThanMin (15, 2, 1, 1)");
				return;
			}
			screen = new CHAR_INFO[_pixelCount];
			_deltaTime = 0;
			emptyChar.Char.UnicodeChar = 0x2588;
			emptyChar.Attributes = 0x0000;
			inputs.insert(std::pair<wchar_t, short>(RALT, 0));
			inputs.insert(std::pair<wchar_t, short>(LALT, 0));
			inputs.insert(std::pair<wchar_t, short>(RCTRL, 0));
			inputs.insert(std::pair<wchar_t, short>(LCTRL, 0));
			inputs.insert(std::pair<wchar_t, short>(SHIFT, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_DCLICK, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_SCROLL, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 0, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 1, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 2, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 3, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 4, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 5, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 6, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_BTN + 7, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_X, 0));
			inputs.insert(std::pair<wchar_t, short>(MOUSE_Y, 0));
			SetTitle(L"CMDEngine Program");
			if (maxFPS > 0)
			{
				fpsLimit = maxFPS;
			}

			//Setting up the console window (Not entirely sure about what everything here does, had to copy most of it due to the complexity)
#pragma region ConsoleWindowSetup
			//Creates an object that then basically functions as the console
			console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			//Gets a reference to the input handler
			consoleInput = GetStdHandle(STD_INPUT_HANDLE);
			//Changes some of the input handler's settings (Prevents highlighting and permits reading mouse input)
			SetConsoleMode(consoleInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT);
			//Set the window size to the smallest possible
			sr = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(console, true, &sr);
			//Set the buffer size to the wanted size
			if (!SetConsoleScreenBufferSize(console, _screenSize))
			{
				ThrowError(L"SetConsoleScreenBufferSize");
				return;
			}
			//Assign the console object to the actual console (From now on changes to the console object will affect the actual console)
			if (!SetConsoleActiveScreenBuffer(console))
			{
				ThrowError(L"SetConsoleActiveScreenBuffer");
				return;
			}

			//Setting the cursor visibility
			CONSOLE_CURSOR_INFO cci = CONSOLE_CURSOR_INFO();
			cci.bVisible = false;
			cci.dwSize = 3;
			SetConsoleCursorInfo(console, &cci);

			//Setting the character size and font
			CONSOLE_FONT_INFOEX cfi = CONSOLE_FONT_INFOEX();
			cfi.cbSize = sizeof(cfi);
			cfi.dwFontSize.X = fontWidth;
			cfi.dwFontSize.Y = fontHeight;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;
			//Don't know why, but  ' cfi.FaceName = L"Liberation Mono"; '  doesn't work, so instead 'wcscpy_s' has to be used
			//	this copies the wide character string in the 2nd parameter into the variable in the 1st parameter
			//wcscpy_s(cfi.FaceName, L"Liberation Mono");
			if (!SetCurrentConsoleFontEx(console, false, &cfi))
			{
				ThrowError(L"SetConsoleActiveScreenBuffer");
				return;
			}

			//Check to see if the wanted size is too big for the screen
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(console, &csbi))
			{
				ThrowError(L"GetConsoleScreenBufferInfo");
				return;
			}
			if (_screenSize.Y > csbi.dwMaximumWindowSize.Y)
			{
				swprintf(_msg, 128, L"Screen Height / Font Height Too Big (%d * %d)", csbi.dwMaximumWindowSize.Y, fontHeight);
				ThrowError(_msg);
				return;
			}
			if (_screenSize.X > csbi.dwMaximumWindowSize.X)
			{
				swprintf(_msg, 128, L"Screen Width / Font Width Too Big (%d * %d)", csbi.dwMaximumWindowSize.X, fontWidth);
				ThrowError(_msg);
				return;
			}

			//Tries to scale the font up to make the window occupy the most screen space possible
			if (autoUpscale)
			{
				float fontRatio = ((float)fontWidth) / ((float)fontHeight);
				short tFW = fontWidth;
				short tFH = fontHeight;
				short i = 1;
				while (true)
				{
					if ((fontRatio * i) == (trunc(fontRatio * i)))
					{
						cfi.dwFontSize.X = fontWidth + (short)(fontRatio * i);
						cfi.dwFontSize.Y = fontHeight + i;
						SetCurrentConsoleFontEx(console, false, &cfi);
						GetConsoleScreenBufferInfo(console, &csbi);
						if (_screenSize.X > csbi.dwMaximumWindowSize.X || _screenSize.Y > csbi.dwMaximumWindowSize.Y)
						{
							break;
						}
						tFW = fontWidth + (short)(fontRatio * i);
						tFH = fontHeight + i;
					}
					i++;
				}
				cfi.dwFontSize.X = tFW;
				cfi.dwFontSize.Y = tFH;
				SetCurrentConsoleFontEx(console, false, &cfi);
				GetConsoleScreenBufferInfo(console, &csbi);
			}

			//Set the window size to the wanted screen size
			sr = { 0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
			if (!SetConsoleWindowInfo(console, true, &sr))
			{
				ThrowError(L"SetConsoleWindowInfo");
				return;
			}
#pragma endregion
			
			//Gets a reference to the program window and changes some settings
#pragma region WindowSettings
			//Sets the title of the program to something random in order to distinguish it from the other programs on the computer
			int temp = rand();
			swprintf(_msg, 128, L"%d", temp);
			SetConsoleTitle(_msg);
			Sleep(40);
			//Finds the window with that random name (Don't have to set it back to normal cause the 'UpdateTitle()' is run every frame anyways)
			window = FindWindowW(NULL, _msg);
			//Changes the window's settings (Prevents maximizing, minimizing, and resizing)
			SetWindowLongPtrW(window, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX));
			::SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOREPOSITION);
#pragma endregion
		}

						/// <summary>
						/// Prints an error message to the console (Returns 0 so that you can just do 'return ThrowError(...);')
						/// </summary>
						/// <param name="msg">The message to print to the console</param>
		int ThrowError(const wchar_t* msg)
		{
			wchar_t _error[128];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), _error, 128, NULL);
			wprintf(L"ERROR: %s\n\t%s\n", msg, _error);
			running = false;
			return 0;
		}

		bool OnScreen(short x, short y) { return (x >= 0 && y >= 0 && x < screenSize.X && y < screenSize.Y); }
			bool OnScreen(VEC2F p) { return (p.x >= 0 && p.y >= 0 && p.x < screenSize.X && p.y < screenSize.Y); }

#pragma region DrawFunctions
		//Simply modifies the value of the specified position in the screen array
#pragma region Draw
						/// <summary>
						/// Draws to a specific point on the command prompt
						/// </summary>
						/// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param>
						/// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param>
						/// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="cha">The character with which to draw to that point</param>
		void Draw(short x, short y, short col = 0x000F, short cha = 0x2588)
		{
			if (OnScreen(x, y))
			{
				screen[y * screenSize.X + x].Char.UnicodeChar = cha;
				screen[y * screenSize.X + x].Attributes = col;
			}
		}
							/// <summary>Draws to a specific point on the command prompt</summary> /// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param> /// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param>
			void Draw(float x, float y, short col = 0x000F, short cha = 0x2588) { Draw((short)x, (short)y, col, cha); }
							/// <summary>Draws to a specific point on the command prompt</summary> /// <param name="p">The position of the point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param>
			void Draw(VEC2F p, short col = 0x000F, short cha = 0x2588) { Draw((short)p.x, (short)p.y, col, cha); }
#pragma endregion

		//Gets the step size for the reaction in 1 axis when moving 1 unit in the other
		//	by going from point 1 to point 2 like this in both axis, you can find every space through which the line crosses
		//	but because they're floats, the points may not be exactly in a space, so for the first step you have to move less than 1 in the axis
		//	after the first movement in each axis, the movements can be by 1 unit
		//If the points are whole numbers, the first step can be skipped due to knowing that the points will always be exactly in a space
		//Using relative screen space for the points is the same as normal, but with the added step of converting the coordinates first
#pragma region DrawLine
						/// <summary>
						/// Draws a line on the command pront from a point to another point
						/// </summary>
						/// <param name="x1">The x position of the first point (Leftmost is 0; Rightmost is screenSize.X)</param>
						/// <param name="y1">The y position of the first point (Topmost is 0; Bottommost is screenSize.Y)</param>
						/// <param name="x2">The x position of the second point (Leftmost is 0; Rightmost is screenSize.X)</param>
						/// <param name="y2">The y position of the second point (Topmost is 0; Bottommost is screenSize.Y)</param>
						/// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="cha">The character with which to draw the line</param>
		void DrawLine(float x1, float y1, float x2, float y2, short col = 0x000F, short cha = 0x2588)
		{
			short ux = (x1 < x2 ? 1 : -1), uy = (y1 < y2 ? 1 : -1);
			float tx = x2 - x1, ty = y2 - y1;
			float sx = (ty != 0 ? ux * abs(tx / ty) : 0), sy = (tx != 0 ? uy * abs(ty / tx) : 0);
			Draw(x1, y1, col, cha);
			Draw(x2, y2, col, cha);
			tx = fmod(ux - fmod(x1, 1.0f), 1.0f);
			ty = fmod(uy - fmod(y1, 1.0f), 1.0f);
			for (float x = x1 + tx, y = y1 + uy * abs(tx * sy); x * ux < x2 * ux; x += ux, y += sy)
			{
				Draw(x, y, col, cha);
			}
			for (float y = y1 + ty, x = x1 + ux * abs(ty * sx); y * uy < y2 * uy; y += uy, x += sx)
			{
				Draw(x, y, col, cha);
			}
		}
							/// <summary>Draws a line on the command pront from a point to another point</summary> /// <param name="p1">The position of the first point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="p2">The position of the second point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param>
			void DrawLine(VEC2F p1, VEC2F p2, short col = 0x000F, short cha = 0x2588) { DrawLine(p1.x, p1.y, p2.x, p2.y, col, cha); }
							/// <summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="x1">The x position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y1">The y position of the first point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="x2">The x position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y2">The y position of the second point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param>
			void DrawLineS(float x1, float y1, float x2, float y2, short col = 0x000F, short cha = 0x2588) { DrawLine(ScreenPosToPoint(x1, y1), ScreenPosToPoint(x2, y2), col, cha); }
							/// <summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="p1">The position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="p2">The position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param>
			void DrawLineS(VEC2F p1, VEC2F p2, short col = 0x000F, short cha = 0x2588) { DrawLine(ScreenPosToPoint(p1), ScreenPosToPoint(p2), col, cha); }
#pragma endregion

		//Uses trigonometry to get the vertices of a regular polygon of 'edges' sides and then connects them with lines
		//Using relative screen space for the polygon requires the conversion of the center point and the radius
		//	but due to the radius not being a point, it must be defined relative to the width or height of the screen
#pragma region DrawRPoly
						/// <summary>
						/// Draws a regular polygon to the screen
						/// </summary>
						/// <param name="cx">The x position of the center of the polygon</param>
						/// <param name="cy">The y position of the center of the polygon</param>
						/// <param name="edges">The amount of edges the polygon has</param>
						/// <param name="rad">The distance from the center point to each of the vertices</param>
						/// <param name="rot">The angle at which to draw the polygon in degrees</param>
						/// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="cha">The character with which to draw the polygon</param>
		void DrawRPoly(float cx, float cy, short edges, float rad, float rot = 0, short col = 0x000F, short cha = 0x2588)
			{
				float as = (360.0f / edges) * RAD;
				float max = (360.0f + rot) * RAD;
				for (float a = rot * RAD; a < max; a += as)
				{
					DrawLine(cos(a) * rad + cx, sin(a) * rad + cy, cos(a + as) * rad + cx, sin(a + as) * rad + cy, col, cha);
				}
			}
							/// <summary>Draws a regular polygon to the screen</summary> /// <param name="p">The position of the center of the polygon</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param>
			void DrawRPoly(VEC2F p, short edges, float rad, float rot = 0, short col = 0x000F, short cha = 0x2588) { DrawRPoly(p.x, p.y, edges, rad, rot, col, cha); }
							/// <summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="cx">The x position of the center of the polygon in relative screen space</param> /// <param name="cy">The y position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param>
			void DrawRPolyS(float cx, float cy, short edges, float rad, bool useY, float rot = 0, short col = 0x000F, short cha = 0x2588) { DrawRPoly(ScreenPosToPoint(cx, cy), edges, (useY ? ScreenPosToPoint(0, rad).y : ScreenPosToPoint(rad, 0).x), rot, col, cha); }
							/// <summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="p">The position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param>
			void DrawRPolyS(VEC2F p, short edges, float rad, bool useY, float rot = 0, short col = 0x000F, short cha = 0x2588) { DrawRPolyS(p.x, p.y, edges, rad, useY, rot, col, cha); }
#pragma endregion
#pragma endregion

						/// <summary>
						/// Converts a relative screen space position ('0.0 - 1.0' is 'Left - Right' or 'Top - Bottom') to a point in the pixel grid ('0.0 - screenSize.X' is 'Left - Right' and '0.0 - screenSize.Y' is 'Top - Bottom')
						/// </summary>
						/// <param name="x">The x value (If outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
						/// <param name="y">The y value (If outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
		VEC2F ScreenPosToPoint(float x, float y)
		{
			return { x * screenSize.X, y * screenSize.Y };
		}
							///<summary>Converts a relative screen space position ('0.0 - 1.0' is 'Left - Right' or 'Top - Bottom') to a point in the pixel grid ('0.0 - screenSize.X' is 'Left - Right' and '0.0 - screenSize.Y' is 'Top - Bottom')</summary> /// <param name="p">The value (If x or y is outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
			VEC2F ScreenPosToPoint(VEC2F p) { return { p.x * screenSize.X, p.y * screenSize.Y }; }

						/// <summary>Fills the entire console with the character and color in the 'empty' variable</summary>
		void ClearFrame()
		{
			for (int i = 0; i < pixelCount; i++)
			{
				screen[i] = emptyChar;
			}
		}

		void DrawFrame()
		{
			WriteConsoleOutput(console, screen, { screenSize.X, screenSize.Y }, { 0, 0 }, &sr);
		}

		void ResizeWindow(short screenWidth, short screenHeight, short fontWidth = 1, short fontHeight = 1)
		{
			if (fontWidth > 30 || fontHeight > 30)
			{
				ThrowError(L"FontTooBig (30)");
				return;
			}
			if (screenWidth < 15 || screenHeight < 2 || fontWidth < 1 || fontHeight < 1)
			{
				ThrowError(L"SmallerThanMin (15, 2, 1, 1)");
				return;
			}
			_screenSize = { (short)screenWidth, (short)screenHeight };
			_pixelCount = screenWidth * screenHeight;
			screen = new CHAR_INFO[pixelCount];
			//Setting up the console window (Not entirely sure about what everything here does, had to copy most of it due to the complexity)
#pragma region ConsoleWindowSetup
			//Creates an object that then basically functions as the console
			console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			//Set the window size to the smallest possible
			sr = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(console, true, &sr);
			//Set the buffer size to the wanted size
			if (!SetConsoleScreenBufferSize(console, _screenSize))
			{
				ThrowError(L"SetConsoleScreenBufferSize");
				return;
			}
			//Assign the console object to the actual console (From now on changes to the console object will affect the actual console)
			if (!SetConsoleActiveScreenBuffer(console))
			{
				ThrowError(L"SetConsoleActiveScreenBuffer");
				return;
			}

			//Setting the cursor visibility
			CONSOLE_CURSOR_INFO cci = CONSOLE_CURSOR_INFO();
			cci.bVisible = false;
			cci.dwSize = 3;
			SetConsoleCursorInfo(console, &cci);

			//Setting the character size and font
			CONSOLE_FONT_INFOEX cfi = CONSOLE_FONT_INFOEX();
			cfi.cbSize = sizeof(cfi);
			cfi.dwFontSize.X = fontWidth;
			cfi.dwFontSize.Y = fontHeight;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;
			//Don't know why, but  ' cfi.FaceName = L"Liberation Mono"; '  doesn't work, so instead 'wcscpy_s' has to be used
			//	this copies the wide character string in the 2nd parameter into the variable in the 1st parameter
			//wcscpy_s(cfi.FaceName, L"Liberation Mono");
			if (!SetCurrentConsoleFontEx(console, false, &cfi))
			{
				ThrowError(L"SetConsoleActiveScreenBuffer");
				return;
			}

			//Check to see if the wanted size is too big for the screen
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(console, &csbi))
			{
				ThrowError(L"GetConsoleScreenBufferInfo");
				return;
			}
			if (_screenSize.Y > csbi.dwMaximumWindowSize.Y)
			{
				wchar_t msg[128];
				swprintf(msg, 128, L"Screen Height / Font Height Too Big (%d * %d)", csbi.dwMaximumWindowSize.Y, fontHeight);
				ThrowError(msg);
				return;
			}
			if (_screenSize.X > csbi.dwMaximumWindowSize.X)
			{
				wchar_t msg[128];
				swprintf(msg, 128, L"Screen Width / Font Width Too Big (%d * %d)", csbi.dwMaximumWindowSize.X, fontWidth);
				ThrowError(msg);
				return;
			}

			//Tries to scale the font up to make the window occupy the most screen space possible
			if (upscale)
			{
				float fontRatio = ((float)fontWidth) / ((float)fontHeight);
				short tFW = fontWidth;
				short tFH = fontHeight;
				short i = 1;
				while (true)
				{
					if ((fontRatio * i) == (trunc(fontRatio * i)))
					{
						cfi.dwFontSize.X = fontWidth + (short)(fontRatio * i);
						cfi.dwFontSize.Y = fontHeight + i;
						SetCurrentConsoleFontEx(console, false, &cfi);
						GetConsoleScreenBufferInfo(console, &csbi);
						if (_screenSize.X > csbi.dwMaximumWindowSize.X || _screenSize.Y > csbi.dwMaximumWindowSize.Y)
						{
							break;
						}
						tFW = fontWidth + (short)(fontRatio * i);
						tFH = fontHeight + i;
					}
					i++;
				}
				cfi.dwFontSize.X = tFW;
				cfi.dwFontSize.Y = tFH;
				SetCurrentConsoleFontEx(console, false, &cfi);
				GetConsoleScreenBufferInfo(console, &csbi);
			}

			//Set the window size to the wanted screen size
			sr = { 0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
			if (!SetConsoleWindowInfo(console, true, &sr))
			{
				ThrowError(L"SetConsoleWindowInfo");
				return;
			}
#pragma endregion
		}

		//Used the same name as a function in 'Windows.h', this is horrible practice
						/// <summary>
						/// Moves the command prompt window to a specific point on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::SetWindowPos')
						/// </summary>
						/// <param name="x">Moves the window so that there are this many pixels between the edge of the screen and the left side of the window (0 pixels seems to actually be -7)</param>
						/// <param name="y">Moves the window so that there are this many pixels between the edge of the screen and the top side of the window (In order to hide the Windows top bar use -29)</param>
		void SetWindowPos(short x, short y)
		{
			::SetWindowPos(window, NULL, x, y, NULL, NULL, SWP_NOSIZE);
		}
							/// <summary>Moves the command prompt window to a specific point on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::SetWindowPos')</summary> /// <param name="x">Moves the window so that there are this many pixels between the edge of the screen and the left side of the window (0 pixels seems to actually be -7)</param> /// <param name="y">Moves the window so that there are this many pixels between the edge of the screen and the top side of the window (In order to hide the Windows top bar use -29)</param>
			void SetWindowPos(VEC2F v) { ::SetWindowPos(window, NULL, (int)v.x, (int)v.y, NULL, NULL, SWP_NOSIZE); }

		//Used the same name as a function in 'Windows.h', this is horrible practice
						/// <summary>
						/// Moves the command prompt window by a number of pixels on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::MoveWindow')
						/// </summary>
						/// <param name="x">Moves the window this many pixels to the right (Negative numbers go to the left)</param>
						/// <param name="y">Moves the window this many pixels down (Negative numbers go up)</param>
		void MoveWindow(short x, short y)
		{
			WINDOWINFO winInfo = WINDOWINFO();
			GetWindowInfo(window, &winInfo);
			::SetWindowPos(window, NULL, winInfo.rcWindow.left + x, winInfo.rcWindow.top + y, NULL, NULL, SWP_NOSIZE);
		}
							/// <summary>Moves the command prompt window by a number of pixels on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::MoveWindow')</summary> /// <param name="x">Moves the window this many pixels to the right (Negative numbers go to the left)</param> /// <param name="y">Moves the window this many pixels down (Negative numbers go up)</param>
			void MoveWindow(VEC2F v) { MoveWindow((short)v.x, (short)v.y); }

		void SetTitle(const wchar_t* msg)
		{
			swprintf(title, 256, msg);
		}

		void UpdateTitle()
		{
			wchar_t tmp[256];
			swprintf(tmp, 256, L"%s  |  FPS:%f", title, 1.0f / deltaTime);
			SetConsoleTitle(tmp);
		}

						/// <summary>0 -> nothing ; 1 -> released ; 2 -> pressed ; 3+ -> held (for n-2 frames)</summary>
		void ReadInputs()
		{
			short tx = inputs[MOUSE_X];
			short ty = inputs[MOUSE_Y];
			for (auto &o : inputs)
			{
				switch (o.second)
				{
				case 2:
					o.second = 3;
					break;
				case 1:
					o.second = 0;
					break;
				case -1:
					o.second = 0;
					break;
				}
			}
			inputs[MOUSE_X] = tx;
			inputs[MOUSE_Y] = ty;
			INPUT_RECORD buf;
			DWORD count = 0;
			DWORD trash;

			GetNumberOfConsoleInputEvents(consoleInput, &count);
			for (DWORD c = 0; c < count; c++)
			{
				if (!ReadConsoleInputW(consoleInput, &buf, 1, &trash))
				{
					ThrowError(L"FailedToReadInput");
					return;
				}

				if (buf.EventType == KEY_EVENT)
				{
					wchar_t t = buf.Event.KeyEvent.uChar.UnicodeChar;
					std::map<wchar_t, short>::iterator k = inputs.find(t);
					if (k == inputs.end())
					{
						continue;
					}

					k->second++;
					if (buf.Event.KeyEvent.bKeyDown == false)
					{
						k->second = 1;
					}
					else
					{
						if (k->second == 1)
						{
							k->second = 2;
						}
					}

					if (buf.Event.KeyEvent.dwControlKeyState > 0)
					{
						inputs[LALT] = (inputs[LALT] == 0 && (buf.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) == 0 ? 0 : 1);
						inputs[RALT] = (inputs[RALT] == 0 && (buf.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) == 0 ? 0 : 1);
						inputs[LCTRL] = (inputs[LCTRL] == 0 && (buf.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) == 0 ? 0 : 1);
						inputs[RCTRL] = (inputs[RCTRL] == 0 && (buf.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) == 0 ? 0 : 1);
						inputs[SHIFT] = (inputs[SHIFT] == 0 && (buf.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) == 0 ? 0 : 1);
					}
				}
				else
				{
					if (buf.EventType == MOUSE_EVENT)
					{
						switch (buf.Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
							inputs[MOUSE_X] = buf.Event.MouseEvent.dwMousePosition.X;
							inputs[MOUSE_Y] = buf.Event.MouseEvent.dwMousePosition.Y;
							break;
						case DOUBLE_CLICK:
							inputs[MOUSE_DCLICK] = 1;
							break;
						case MOUSE_WHEELED:
							inputs[MOUSE_SCROLL] = (buf.Event.MouseEvent.dwButtonState >> 31 == 0 ? 1: -1);inputs[MOUSE_SCROLL] = -1;
							break;
						case 0: //Means a button was clicked
							for (int i = 0; i < 8; i++)
							{
								inputs[MOUSE_BTN + i] += ((buf.Event.MouseEvent.dwButtonState & (1 << i)) > 0 ? 1 : 1 - inputs[MOUSE_BTN + i]);
							}
							break;
						}
					}
				}
			}
		}

						/// <summary>Starts the game loop (Clears the frame; Runs Setup(); Runs Update() in a loop while 'running' is true)</summary>
		void Start()
		{
			auto curT = std::chrono::system_clock::now();
			auto preT = curT;
			std::chrono::duration<float> dT;

			ClearFrame();
			Setup();
			WriteConsoleOutput(console, screen, { screenSize.X, screenSize.Y }, { 0, 0 }, &sr);
			if (autoClearScreen)
			{
				while (running)
				{
					curT = std::chrono::system_clock::now();
					dT = curT - preT;
					_deltaTime = dT.count();
					if (deltaTime < fpsLimit)
						continue;
					preT = curT;
					UpdateTitle();
					ClearFrame();
					ReadInputs();
					Update();
					//Draw Frame
					WriteConsoleOutput(console, screen, { screenSize.X, screenSize.Y }, { 0, 0 }, &sr);
				}
			}
			else
			{
				while (running)
				{
					curT = std::chrono::system_clock::now();
					dT = curT - preT;
					_deltaTime = dT.count();
					if (deltaTime < fpsLimit)
						continue;
					preT = curT;
					UpdateTitle();
					ReadInputs();
					Update();
					//Draw frame
					WriteConsoleOutput(console, screen, { screenSize.X, screenSize.Y }, { 0, 0 }, &sr);
				}
			}
		}

		virtual void Setup() = 0;

		virtual void Update() = 0;


		//General useful functions
#pragma region MiscFunctions
		float Pow2(float f)
		{
			return f * f;
		}
		float DotProduct(VEC4F v1, VEC4F v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
		}
		VEC3F CrossProduct(VEC3F v1, VEC3F v2)
		{
			return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
		}
		float Magnitude(VEC4F v)
		{
			return sqrt(abs(DotProduct(v, v)));
		}
						/// <summary>Calculates the angle between 2 vectors in degrees</summary>
		float Angle(VEC4F v1, VEC4F v2)
		{
			return acos(DotProduct(v1, v2) / (Magnitude(v2) * Magnitude(v1))) * DEG;
			
		}
		VEC4F Normalize(VEC4F v)
		{
			return v / Magnitude(v);
		}
#pragma region LinearFunction
						/// <summary>Calculates the y value of point x on the line defined by the 2 points provided (Returns y1 if x1 == x2)</summary>
		float LinearFunction(float x1, float y1, float x2, float y2, float x)
		{
			if (y1 == y2 || x1 == x2)
				return y1;
			return y1 + ((y2 - y1) / (x2 - x1) * (x - x1));
		}
						/// <summary>Calculates the point at 'x' on the line defined by the 2 points provided (Returns p1 if p1.x == p2.x)</summary>
		VEC4F LinearFunction(VEC4F p1, VEC4F p2, float x)
		{
			if (p1.x == p2.x)
				return p1;
			return { x, LinearFunction(p1.x, p1.y, p2.x, p2.y, x), LinearFunction(p1.x, p1.z, p2.x, p2.z, x), LinearFunction(p1.x, p1.w, p2.x, p2.w, x) };
		}
		//Warn if both points have the same X and thus can't be made into a linear function
							/// <summary>Calculates the y value of point x on the line defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
			bool LinearFunction(float x1, float y1, float x2, float y2, float x, float* output) { if (x1 == x2) { return false; } *output = LinearFunction(x1, y1, x2, y2, x); return true; }
							/// <summary>Calculates the point at 'x' on the line defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
			bool LinearFunction(VEC4F p1, VEC4F p2, float x, VEC4F* output) { if (p1.x == p2.x) { return false; } *output = LinearFunction(p1, p2, x); return true; }
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=0 if x1 == x2)</summary>
		LINEAR LinearFunction(float x1, float y1, float x2, float y2)
		{
			if (x1 == x2)
				return LINEAR();
			return { VEC4F(0, (y2 - y1) / (x2 - x1), 0, 0), VEC4F(0, -(y2 - y1) / (x2 - x1) * x1 + y1, 0, 0) };
		}
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=z=w=0 if p1.x == p2.x)</summary>
		LINEAR LinearFunction(VEC4F p1, VEC4F p2)
		{
			if (p1.x == p2.x)
				return LINEAR();
			float t = 1.0f / (p2.x - p1.x);
			return { VEC4F(1, (p2.y - p1.y) * t, (p2.z - p1.z) * t, (p2.w - p1.w) * t), VEC4F(0, -(p2.y - p1.y) * t * p1.x + p1.y, -(p2.z - p1.z) * t * p1.x + p1.z, -(p2.w - p1.w) * t * p1.x + p1.w) };
		}
		//Warn if both points have the same X and thus can't be made into a linear function
							/// <summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
			bool LinearFunction(float x1, float y1, float x2, float y2, LINEAR* output) { if (x1 == x2) { return false; } *output = LinearFunction(x1, y1, x2, y2); return true; }
							/// <summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
			bool LinearFunction(VEC4F p1, VEC4F p2, LINEAR* output) { if (p1.x == p2.x) { return false; } *output = LinearFunction(p1, p2); return true; }
#pragma endregion

						/// <summary>Calculates the point at 'x' on the line defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)<summary>
		bool QuadraticFunction(VEC2F p1, VEC2F p2, VEC2F p3, float x, VEC2F* output)
		{
			if (p1.x == p2.x || p1.x == p3.x || p2.x == p3.x)
			{
				return false;
			}
			float d = (p1.x * (p3.x * p3.x - p2.x * p2.x) + (p3.x * p2.x + p1.x * p1.x) * (p2.x - p3.x));
			if (d == 0)
			{
				return false;
			}
			float a = (-p1.y * (p3.x - p2.x) - p2.y * (p1.x - p3.x) + p3.y * (p1.x + p2.x)) / d;
			float b = (p2.y - p3.y + a * p3.x * p3.x - a * p2.x * p2.x) / (p2.x - p3.x);
			float c = p3.y - b * p3.x - a * p3.x * p3.x;
			*output = { x, a * x * x + b * x + c };
			return true;
		}
						/// <summary>Generates the quadratic function based on 'x' defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)<summary>
		bool QuadraticFunction(VEC4F p1, VEC4F p2, VEC4F p3, QUADRATIC* output)
		{
			if (p1.x == p2.x || p1.x == p3.x || p2.x == p3.x)
			{
				return false;
			}
			float d = (p1.x * (p3.x * p3.x - p2.x * p2.x) + (p3.x * p2.x + p1.x * p1.x) * (p2.x - p3.x));
			if (d == 0)
			{
				return false;
			}
			VEC4F a = { 0, (-p1.y * (p3.x - p2.x) - p2.y * (p1.x - p3.x) + p3.y * (p1.x + p2.x)) / d, (-p1.z * (p3.x - p2.x) - p2.z * (p1.x - p3.x) + p3.z * (p1.x + p2.x)) / d, (-p1.w * (p3.x - p2.x) - p2.w * (p1.x - p3.x) + p3.w * (p1.x + p2.x)) / d };
			VEC4F b = { 1, (p2.y - p3.y + a.y * p3.x * p3.x - a.y * p2.x * p2.x) / (p2.x - p3.x), (p2.z - p3.z + a.z * p3.x * p3.x - a.z * p2.x * p2.x) / (p2.x - p3.x), (p2.w - p3.w + a.w * p3.x * p3.x - a.w * p2.x * p2.x) / (p2.x - p3.x) };
			VEC4F c = { 0, p3.y - b.y * p3.x - a.y * p3.x * p3.x, p3.z - b.z * p3.x - a.z * p3.x * p3.x, p3.w - b.w * p3.x - a.w * p3.x * p3.x };
			*output = { a, b, c };
			return true;
		}

						/// <summary>Assumes the XYZ axis are set up correctly (X+ is left when Z+ is forwards and Y+ is up)</summary>
		VEC3F VectorFromAngles(float h, float v)
		{
			return { sin(h * RAD) * cos(v * RAD), sin(v * RAD), cos(h * RAD) * cos(v * RAD) };
		}
#pragma endregion
	};
}

class RotateShape : public cmde::CMDEngine
{
public:
	int angle;
	int edges;
	float radius;
	cmde::VEC2F c;
	float r;
	wchar_t msg[128];
	float t;

	RotateShape(short screenWidth, short screenHeight, short fontWidth, short fontHeight) : cmde::CMDEngine(screenWidth, screenHeight, fontWidth, fontHeight, false, true, FPS60)
	{
		swprintf(msg, 128, L"");
		radius = 38;
		angle = 360;
		edges = 3;
		c = { 0.5f, 0.5f };
		r = 0.49f;
		t = 0;
	}

	void Setup()
	{
		SetTitle(L"Sketchy Program");
		c = ScreenPosToPoint(c.x, c.y);
		r = ScreenPosToPoint(0, r).y;
		SetWindowPos(600, 40);
	}

	void Update()
	{	
		DrawRPoly(c, edges, r, (float)angle);
		angle++;

		if (angle % 60 == 0)
		{
			edges++;
		}

		MoveWindow((short)cos(angle * 9 * RAD) * 18, (short)sin(angle * 0.8f * RAD) * 10);
	}
};

#include <vector>

class Test3D : public cmde::CMDEngine
{

	struct Triangle
	{
		cmde::VEC3F vertices[3];

		Triangle(cmde::VEC3F p1, cmde::VEC3F p2, cmde::VEC3F p3)
		{
			vertices[0] = p1;
			vertices[1] = p2;
			vertices[2] = p3;
		}
	};

	struct Mesh
	{
		std::vector<Triangle> triangles;
		cmde::VEC3F aabb[2];
		cmde::VEC3F center;

		Mesh(std::vector<Triangle> triangles = std::vector<Triangle>())
		{
			this->triangles = triangles;

			for (Triangle& t : triangles)
			{
				for (short i = 0; i < 3; i++)
				{
					RecalculateAABB(t.vertices[i]);
				}
			}
		}

		void RecalculateAABB(cmde::VEC3F p)
		{
			aabb[0].x = min(p.x, aabb[0].x);
			aabb[0].y = min(p.y, aabb[0].y);
			aabb[0].z = min(p.z, aabb[0].z);
			aabb[1].x = max(p.x, aabb[1].x);
			aabb[1].y = max(p.y, aabb[1].y);
			aabb[1].z = max(p.z, aabb[1].z);
			center = aabb[0] + (aabb[1] - aabb[0]) * 0.5f;
		}
		void AddTriangle(Triangle t) { triangles.push_back(t); }
		void AddTriangles(std::vector<Triangle> triangles) { this->triangles.insert(this->triangles.end(), triangles.begin(), triangles.end()); }
	};

	bool LinePlaneIntersection(cmde::VEC3F planePoint, cmde::VEC3F planeNormal, cmde::VEC3F lineOrigin, cmde::VEC3F lineDirection, cmde::VEC3F* output)
	{
		planeNormal = Normalize(planeNormal);
		float nd = DotProduct(planeNormal, lineDirection);
		if (nd == 0)
			return false;
		//t = (N * a - N * O) / nd
		float t = (DotProduct(planeNormal, planePoint) - DotProduct(planeNormal, lineOrigin)) / nd;
		*output = lineOrigin + lineDirection * t;
		return true;
	}

	std::vector<Triangle> ClipTriangles(short tempInBoundsCount, std::vector<Triangle> ts, cmde::VEC3F cameraPos, cmde::VEC3F inBounds[6])
	{
		//'i' SHOULD BE THE AMOUNT OF 'inBounds' THERE ARE (6), BUT I'VE ONLY GOT 4 SO FAR
		for (short i = 0; i < tempInBoundsCount; i++)
		{
			std::vector<Triangle> temp;
			for (Triangle& t : ts)
			{
				std::vector<Triangle> o = ClipTriangle(t, cameraPos, inBounds[i]);
				temp.insert(temp.end(), o.begin(), o.end());
			}
			ts = temp;
		}
		return ts;
	}

	std::vector<Triangle> ClipTriangle(Triangle t, cmde::VEC3F cameraPos, cmde::VEC3F inBounds)
	{
		bool oob[3] = { false };
		short c = 0;
		std::vector<Triangle> output = std::vector<Triangle>();
		for (short i = 0; i < 3; i++)
		{
			if (DotProduct(t.vertices[i] - pos, inBounds) < 0)
			{
				oob[i] = true;
				c++;
			}
		}

		//Apparently the inside of a switch is all considered the same scope, so you can't have 2 variables with the same name in different cases
		cmde::VEC3F g;
		cmde::VEC3F g1;
		cmde::VEC3F g2;
		cmde::VEC3F new1;
		cmde::VEC3F new2;
		//Must flip the booleans around if c is 2 because then the one that is inside the bounds is the one that's needed
		if (c == 2)
		{
			for (short i = 0; i < 3; i++)
			{
				oob[i] = !oob[i];
			}
		}
		for (short i = 0; i < 3; i++)
		{
			if (oob[i] == true)
			{
				g = t.vertices[i];
				g1 = t.vertices[(i + 1) % 3];
				g2 = t.vertices[(i + 2) % 3];
				break;
			}
		}
		LinePlaneIntersection(cameraPos, inBounds, g, g1 - g, &new1);
		LinePlaneIntersection(cameraPos, inBounds, g, g2 - g, &new2);

		switch (c)
		{
		case 0:
			output.push_back(t);
			break;
		case 3:
			//nothing
			break;
		case 1:
			output.push_back(Triangle(new1, g1, g2));
			output.push_back(Triangle(new1, g2, new2));
			break;
		case 2:
			output.push_back(Triangle(g, new1, new2));
			break;
		}
		return output;
	}

	//-------------------------------------------------------------------------------------
	//OLC projection matrix thing
	cmde::VEC3F ProjectionMatrixify(cmde::VEC3F v)
	{
		float fov = 90.0f;
		float f = 1.0f / tanf(fov * 0.5f * RAD);
		float zf = 1000.0f; // far plane
		float zn = 0.1f; // near plane
		v.z = -v.z; //This thing uses - distance for depth, but since I use + distance everywhere else, this tiny change is necessary
		if (v.z != 0)
			return { (((float)screenSize.Y / (float)screenSize.X) * f * v.x) / v.z, (f * v.y) / v.z, ((v.z - zn) * (zf / (zf - zn))) / v.z };
		return { (((float)screenSize.Y / (float)screenSize.X) * f * v.x), (f * v.y), ((v.z - zn) * (zf / (zf - zn))) };
	}
	//-------------------------------------------------------------------------------------

public:
	Mesh shape;
	cmde::VEC3F pos;
	cmde::VEC2F facing;
	cmde::VEC2F fov;
	cmde::VEC3F sightLimitL;
	cmde::VEC3F sightLimitT;
	cmde::VEC3F forwards;
	cmde::VEC3F left;
	cmde::VEC3F up;
	bool myRenderingSystem;

	Test3D(short screenWidth, short screenHeight, short fontWidth, short fontHeight) : cmde::CMDEngine(screenWidth, screenHeight, fontWidth, fontHeight, true, true, FPS60)
	{
		shape = Mesh();
		pos = { 0.5f, 0.5f, -2 };
		fov = { 90, 90 };
		facing = { 0, 0 };
		//X+ is left when Z+ is forwards and Y+ is up
		sightLimitL = VectorFromAngles(facing.x + fov.x * 0.5f, facing.y);
		sightLimitT = VectorFromAngles(facing.x, facing.y + fov.y * 0.5f);
		forwards = VectorFromAngles(facing.x, facing.y);   //  0,  0,  1
		left = VectorFromAngles(facing.x + 90, facing.y);  //  1,  0,  0
		up = VectorFromAngles(facing.x, facing.y + 90);    //  0,  1,  0
		myRenderingSystem = false;
	}

	void Setup()
	{
		shape.AddTriangle({ { 0, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 } });
		shape.AddTriangle({ { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } });
		shape.AddTriangle({ { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 } });
		shape.AddTriangle({ { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 } });
		shape.AddTriangle({ { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 1 } });
		shape.AddTriangle({ { 1, 1, 0 }, { 1, 1, 1 }, { 1, 0, 1 } });
		shape.AddTriangle({ { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 } });
		shape.AddTriangle({ { 0, 1, 0 }, { 1, 1, 1 }, { 1, 1, 0 } });
		shape.AddTriangle({ { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 } });
		shape.AddTriangle({ { 0, 0, 0 }, { 0, 1, 1 }, { 0, 1, 0 } });
		shape.AddTriangle({ { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 } });
		shape.AddTriangle({ { 0, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } });
	}

	void Update()
	{
		cmde::VEC3F offset = shape.center - pos;
		if (DotProduct(forwards, offset) > 0)
		{
			/*
			for (Triangle t : shape.triangles)
			{
				cmde::VEC2F vertices[3];
				for (int i = 0; i < 3; i++)
				{
					cmde::VEC3F temp = t.vertices[i] - pos;
					cmde::VEC3F hTemp = forwards * DotProduct(temp, forwards);
					cmde::VEC3F vTemp = hTemp + up * DotProduct(temp, up);
					hTemp = hTemp + left * DotProduct(temp, left);
					float hAngle = Angle(hTemp, sightLimitL);
					float vAngle = Angle(vTemp, sightLimitT);
					vertices[i] = ScreenPosToPoint((hAngle / fov.x) * (DotProduct(CrossProduct(hTemp, sightLimitL), up) > 0.2f ? 1 : -1), (vAngle / fov.y) * (-DotProduct(CrossProduct(vTemp, sightLimitT), left) > 0 ? 1 : -1));
				}
				short color = (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), forwards) < 0 ? 0x00EE : 0x00BB);
				if (OnScreen(vertices[0]) && OnScreen(vertices[1]))
					DrawLine(vertices[0], vertices[1], color);
				if (OnScreen(vertices[1]) && OnScreen(vertices[2]))
					DrawLine(vertices[1], vertices[2], color);
				if (OnScreen(vertices[2]) && OnScreen(vertices[0]))
					DrawLine(vertices[2], vertices[0], color);
			}
			*/
			

			cmde::VEC3F slr = forwards * DotProduct(sightLimitL, forwards) - left * DotProduct(sightLimitL, left);
			cmde::VEC3F slb = forwards * DotProduct(sightLimitT, forwards) - up * DotProduct(sightLimitT, up);
			//cmde::VEC3F inBounds[] = { CrossProduct(sightLimitL, up), CrossProduct(left, sightLimitT), CrossProduct(up, slr), CrossProduct(slb, left) };
			cmde::VEC3F inBounds[] = { CrossProduct(sightLimitL, up), CrossProduct(up, slr) };
			//Near and far planes too

			//std::vector<Triangle> newTriangles = shape.triangles;
			std::vector<Triangle> newTriangles = ClipTriangles(2, shape.triangles, pos, inBounds);
			for (Triangle t : newTriangles)
			{
				cmde::VEC2F vertices[3];
				for (int i = 0; i < 3; i++)
				{
					if (myRenderingSystem == true)
					{
						DrawLine({ 0, 0 }, { 2, 0 });
						DrawLine({ 0, 2 }, { 2, 2 });
						DrawLine({ 0, 4 }, { 2, 4 });
						Draw(0.0f, 1.0f);
						Draw(2.0f, 3.0f);
						cmde::VEC3F temp = t.vertices[i] - pos;
						cmde::VEC3F hTemp = forwards * DotProduct(temp, forwards);
						cmde::VEC3F vTemp = hTemp + up * DotProduct(temp, up);
						hTemp = hTemp + left * DotProduct(temp, left);
						float hAngle = Angle(hTemp, sightLimitL);
						float vAngle = Angle(vTemp, sightLimitT);
						vertices[i] = ScreenPosToPoint((hAngle / fov.x) * (DotProduct(CrossProduct(hTemp, sightLimitL), up) > 0 ? 1 : -1), (vAngle / fov.y) * (-DotProduct(CrossProduct(vTemp, sightLimitT), left) > 0 ? 1 : -1));
					}
					else
					{
						DrawLine({ 0, 0 }, { 2, 0 });
						DrawLine({ 0, 2 }, { 2, 2 });
						DrawLine({ 0, 0 }, { 0, 4 });
						Draw(2.0f, 1.0f);
						cmde::VEC3F temp = t.vertices[i] - pos;
						temp = { DotProduct(temp, left), DotProduct(temp, up), DotProduct(temp, forwards) };
						vertices[i] = (ProjectionMatrixify(temp) + cmde::VEC3F(1, 1, 1)) * 0.5f * cmde::VEC2F(screenSize.X, screenSize.Y);
					}
				}
				short color = (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), forwards) < 0 ? 0x00EE : 0x00BB);
				//if (OnScreen(vertices[0]) && OnScreen(vertices[1]))
					DrawLine(vertices[0], vertices[1], color);
				//if (OnScreen(vertices[1]) && OnScreen(vertices[2]))
					DrawLine(vertices[1], vertices[2], color);
				//if (OnScreen(vertices[2]) && OnScreen(vertices[0]))
					DrawLine(vertices[2], vertices[0], color);
			}

		}

		pos = pos + forwards * (inputs[L'w'] >= 2 ? 0.005f : 0);
		pos = pos + left * (inputs[L'a'] >= 2 ? 0.005f : 0);
		pos = pos + forwards * (inputs[L's'] >= 2 ? -0.005f : 0);
		pos = pos + left * (inputs[L'd'] >= 2 ? -0.005f : 0);
		pos = pos + up * (inputs[L'q'] >= 2 ? 0.005f : 0);
		pos = pos + up * (inputs[L'e'] >= 2 ? -0.005f : 0);
		if (inputs[L'r'] == 2)
		{
			myRenderingSystem = !myRenderingSystem;
		}

		facing.x += (inputs[MOUSE_X] > 200 ? -0.2f : 0); // right
		facing.x += (inputs[MOUSE_X] < 50 ? 0.2f : 0); // left
		facing.y += (inputs[MOUSE_Y] > 200 ? -0.2f : 0); // bottom
		facing.y += (inputs[MOUSE_Y] < 50 ? 0.2f : 0); // top

		//X+ is left when Z+ is forwards and Y+ is up
		sightLimitL = VectorFromAngles(facing.x + fov.x * 0.5f, facing.y);
		sightLimitT = VectorFromAngles(facing.x, facing.y + fov.y * 0.5f);
		forwards = VectorFromAngles(facing.x, facing.y);   //  0,  0,  1
		left = VectorFromAngles(facing.x + 90, facing.y);  //  1,  0,  0
		up = VectorFromAngles(facing.x, facing.y + 90);    //  0,  1,  0
	}
};

int main()
{
	//RotateShape game(120, 120, 1, 1);
	//RotateShape game(400, 400, 2, 2);
	Test3D game(250, 250, 3, 3);
	game.Start();
}