//One Lone Coder's command prompt engine for reference
//https://www.youtube.com/c/javidx9
//https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h
//https://github.com/OneLoneCoder/CommandLineFPS/blob/master/CommandLineFPS.cpp

#include <Windows.h> //https://docs.microsoft.com/en-us/previous-versions//aa383686(v=vs.85)
#include <chrono>
#include <map>

#define PI 3.14159265f
#define RAD 0.01745329f
#define DEG 57.29577951f
#define FPS60 0.01666666f
#define FPS30 0.03333333f
#define insertws swprintf
#define formatws swprintf
#define appendVector vector.insert(vector.end(), addedVector.begin(), addedVector.end())
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

//First 4 bits (0x!000)
//	(4-7 & C-F) Background takes priority
//	(0-3 & 8-B) Foreground takes priority
//Second 4 bits (0x0!00)
//	(1,3,5,7,9,B,D,F) Dithering
//	(0,2,4,6,8,A,C,E) Plain color
//Third 4 bits (0x00!0)
//	(0-F) Background color
//Last 4 bits (0x000!)
//	(0-F) Foreground color
//Colors
//	0 Black
//	1 Dark Blue
//	2 Dark Green
//	3 Dark Cyan
//	4 Dark Red
//	5 Dark Purple
//	6 Gold
//	7 Light Grey
//	8 Grey
//	9 Blue
//	A Green
//	B Cyan
//	C Red
//	D Purple
//	E Yellow
//	F White




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
		VEC4F operator* (const double& a) { return *this * (float)a; }
		VEC4F operator* (const int& a) { return *this * (float)a; }
		VEC4F operator/ (const float& a) { return VEC4F(x / a, y / a, z / a, w / a); }
		VEC4F operator* (const VEC4F& a) { return VEC4F(x * a.x, y * a.y, z * a.z, w * a.w); }
		VEC4F operator+ (const VEC4F& a) { return VEC4F(x + a.x, y + a.y, z + a.z, w + a.w); }
		VEC4F operator- (const VEC4F& a) { return VEC4F(x - a.x, y - a.y, z - a.z, w - a.w); }
		bool operator== (const VEC4F& a) { return (x == a.x && y == a.y && z == a.z && w == a.w); }
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

	struct FILE
	{
		OPENFILENAMEW fileName = { 0 };
		HANDLE fileHandle = { 0 };
		static const unsigned long CHUNK_SIZE = 4096; //4kb
		wchar_t name[128] = { 0 };
		wchar_t extension[128] = { 0 };

		FILE(){}

						/// <summary>
						/// Reads this file, splits it into lines, and then passes it to the output's processing function. (T's class should be a child of PROCESSEDFILE)
						/// </summary>
						/// <param name="output">Some type of PROCESSEDFILE where the data read will be stored afterwards. To define UTF encoding override 'static const bool utf16' from PROCESSEDFILE (true -> UTF-16; false -> UTF-8)</param>
		template <class T>
		bool ProcessFile(T* output)
		{
			LARGE_INTEGER fileSize = { 0 };
			if (GetFileSizeEx(fileHandle, &fileSize) == false)
				return false;

			byte buffer[CHUNK_SIZE] = { 0 };
			wchar_t lineBuffer[CHUNK_SIZE] = { 0 };
			unsigned long lineBufferPos = 0; //The position in lineBuffer where the last line stopped
			DWORD readCount = 0;

			long tempHigh = fileSize.HighPart;
			unsigned long tempLow = fileSize.LowPart;
			unsigned long counter = 0;
			while (tempHigh > 0 || tempLow > CHUNK_SIZE)
			{
				if (counter < (ULONG_MAX - CHUNK_SIZE) + 1 ||  //counter + CHUNK_SIZE can fit within a ulong
					tempHigh > 1 || //There's at least 2 * ULONG_MAX bytes (Meaning even if counter overflows into 1 high part, there's still enough extra)
					tempLow > CHUNK_SIZE || //There are enough bytes in the low part to cover an entire chunk (For when the high parts run out and start counting down on the low part)
					(tempHigh == 1 && CHUNK_SIZE > ((ULONG_MAX - counter) + 1) && tempLow > CHUNK_SIZE - ((ULONG_MAX - counter) + 1)) //The amount of bytes in the low part are enough to hold the overflow
					)
				{
					//There are CHUNK_SIZE bytes remaining

					//Reads CHUNK_SIZE at a time
					if (ReadFile(fileHandle, &buffer, CHUNK_SIZE, &readCount, NULL))
					{
						if (!output->utf16)
						{
							//UTF-8
							for (DWORD i = 0; i < readCount; i++)
							{
								//For every byte that was read, convert it into a wchar and store it at the end of what is currently stored in lineBuffer
								lineBuffer[lineBufferPos] = (wchar_t)buffer[i];
								lineBufferPos++;
								if (lineBuffer[lineBufferPos - 1] == L'\n')
								{
									//If reached a newline character, lineBuffer now has a full line, and store where that line ends in the wbuffer
									output->ProcessLine(lineBuffer, lineBufferPos);
									lineBufferPos = 0;
								}
								if (lineBufferPos >= CHUNK_SIZE)
								{
									//If the line that is being read is longer than the current CHUNK_SIZE can allow
									output = new T();
									return false;
								}
							}
						}
						else
						{
							//UTF-16
							char wcharBytes[3] = { 0 };
							wchar_t wchar[2] = { 0 };
							for (DWORD i = 0; i < readCount; i += 2)
							{
								//For every byte that was read, convert it into a wchar and store it at the end of what is currently stored in lineBuffer
								wcharBytes[0] = buffer[i];
								wcharBytes[1] = buffer[i + 1];
								MultiByteToWideChar(CP_UTF8, 0, wcharBytes, 3, wchar, 2);
								lineBuffer[lineBufferPos] = wchar[0];
								lineBufferPos++;
								if (lineBuffer[lineBufferPos - 1] == L'\n')
								{
									//If reached a newline character, lineBuffer now has a full line, and store where that line ends in the wbuffer
									output->ProcessLine(lineBuffer, lineBufferPos);
									lineBufferPos = 0;
								}
								if (lineBufferPos >= CHUNK_SIZE)
								{
									//If the line that is being read is longer than the current CHUNK_SIZE can allow
									output = new T();
									return false;
								}
							}
						}
					}
					else
					{
						output = new T();
						return false;
					}

					if (counter < (ULONG_MAX - CHUNK_SIZE) + 1)
					{
						//If CHUNK_SIZE can be added to counter without overflow
						counter += CHUNK_SIZE;
						if (tempHigh == 0)
						{
							//If there're no more high parts
							tempLow -= CHUNK_SIZE; //Count down on tempLow the amount of bytes left
						}
					}
					else
					{
						//If CHUNK_SIZE would make counter overflow
						tempHigh--;
						counter = CHUNK_SIZE - ((ULONG_MAX - counter) + 1); //Subtracting the leftover bytes remaining in the high part from the ones to be added
						if (tempHigh == 0)
						{
							//If there're no more high parts to take care of the overflow
							tempLow -= counter; //Start countdown of bytes left on tempLow by removing the one's taken from the last high part by counter
						}
					}
				}
				else
				{
					//CHUNK_SIZE would go over the file's size
					if (tempHigh > 0)
					{
						//If there're still bytes remaining in a high part
						tempLow += (ULONG_MAX - counter) + 1; //Adding the leftover bytes remaining in the high part
					}
					break;
				}
			}
			//tempLow bytes remain unread in the file (Less than CHUNK_SIZE)
			if (ReadFile(fileHandle, &buffer, tempLow, &readCount, NULL))
			{
				if (!output->utf16)
				{
					//UTF-8
					for (DWORD i = 0; i < readCount; i++)
					{
						//For every byte that was read, convert it into a wchar and store it at the end of what is currently stored in lineBuffer
						lineBuffer[lineBufferPos] = (wchar_t)buffer[i];
						lineBufferPos++;
						if (lineBuffer[lineBufferPos - 1] == L'\n')
						{
							//If reached a newline character, lineBuffer now has a full line, and store where that line ends in the wbuffer
							output->ProcessLine(lineBuffer, lineBufferPos);
							lineBufferPos = 0;
						}
					}
				}
				else
				{
					//UTF-16
					char wcharBytes[3] = { 0 };
					wchar_t wchar[2] = { 0 };
					for (DWORD i = 0; i < readCount; i += 2)
					{
						//For every byte that was read, convert it into a wchar and store it at the end of what is currently stored in lineBuffer
						wcharBytes[0] = buffer[i];
						wcharBytes[1] = buffer[i + 1];
						MultiByteToWideChar(CP_UTF8, 0, wcharBytes, 3, wchar, 2);
						lineBuffer[lineBufferPos] = wchar[0];
						lineBufferPos++;
						if (lineBuffer[lineBufferPos - 1] == L'\n')
						{
							//If reached a newline character, lineBuffer now has a full line, and store where that line ends in the wbuffer
							output->ProcessLine(lineBuffer, lineBufferPos);
							lineBufferPos = 0;
						}
					}
				}
			}
			else
			{
				output = new T();
				return false;
			}
			if (lineBufferPos != 0)
			{
				//If the document doesn't end with a new line character, stick one on the end and pass the line to the function
				lineBuffer[lineBufferPos] = L'\n';
				lineBufferPos++;
				output->ProcessLine(lineBuffer, lineBufferPos);
				lineBufferPos = 0;
			}
			CloseHandle(fileHandle);
			return false;
		}

						/// <summary>
						/// Compares this file's extension to the one passed to this function
						/// </summary>
						/// <param name="extension">The extension to compare against, without the starting '.' (Ex. 'HasExtension(L"png")')</param>
		bool HasExtension(const wchar_t* extension)
		{
			return wcscmp(this->extension, extension) == 0;
		}
	};
	struct PROCESSEDFILE
	{
		static const bool utf16 = false;
		virtual void ProcessLine(wchar_t*, unsigned long) = 0;
						/// <summary>Check PROCESSEDFILE struct in cmde for an example, with both UTF-8 and UTF-16 encoding verions</summary>
		template <class T>
		static bool Export(cmde::FILE exportFile, T exportData) { return false; }
		/*
		//Example
		static bool Export(cmde::FILE& exportFile, Type name)
		{
			wchar_t lineBuffer[cmde::FILE::CHUNK_SIZE] = { 0 };
			//UTF-8 Version
			//
			DWORD writeCount = swprintf(lineBuffer, cmde::FILE::CHUNK_SIZE, L"(%f)\n", name.value);
			char byteBuffer[cmde::FILE::CHUNK_SIZE] = { 0 };
			writeCount = WideCharToMultiByte(CP_UTF8, 0, lineBuffer, -1, byteBuffer, cmde::FILE::CHUNK_SIZE, NULL, NULL);
			if (!WriteFile(exportFile.fileHandle, &byteBuffer, writeCount - 1, &writeCount, NULL))
			{
				return false;
			}
			//

			//UTF-16 Version
			//
			DWORD writeCount = swprintf(lineBuffer, cmde::FILE::CHUNK_SIZE, L"(%f)\n", name.value);
			if (!WriteFile(exportFile.fileHandle, &lineBuffer, writeCount * 2, &writeCount, NULL))
			{
				return false;
			}
			return true;
			//
		}
		*/
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
		CHAR_INFO* screen;
	public:
		const COORD& screenSize = _screenSize;
		const int& pixelCount = _pixelCount;
		const float& deltaTime = _deltaTime;
		bool running;
		CHAR_INFO emptyChar;
		bool autoClearScreen;
		wchar_t title[256];
		float fpsLimit;
		float* zBuffer;
						/// <summary>0 -> nothing ; 1 -> released ; 2 -> pressed ; 3 -> held ; MOUSE_X and MOUSE_Y -> point on the command prompt</summary>
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
			zBuffer = new float[pixelCount];
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

		bool OnScreen(short x, short y) { return (x >= 0 && y >= 0 && x < screenSize.X&& y < screenSize.Y); }
		bool OnScreen(VEC2F p) { return (p.x >= 0 && p.y >= 0 && p.x < screenSize.X&& p.y < screenSize.Y); }

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
						/// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void Draw(short x, short y, short col = 0x000F, short cha = 0x2588, float depth = -1)
		{
			if (OnScreen(x, y) && depth < zBuffer[y * screenSize.X + x])
			{
				screen[y * screenSize.X + x].Char.UnicodeChar = cha;
				screen[y * screenSize.X + x].Attributes = col;
				zBuffer[y * screenSize.X + x] = depth;
			}
		}
						/// <summary>Draws to a specific point on the command prompt</summary> /// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param> /// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void Draw(int x, int y, short col = 0x000F, short cha = 0x2588, float depth = -1) { Draw((short)x, (short)y, col, cha, depth); }
		//Due to issues with numbers between 0-1 when casting, '(short)((short)(x + 2) - 2)' is needed, since numbers between 2-3 convert correctly
						/// <summary>Draws to a specific point on the command prompt</summary> /// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param> /// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void Draw(float x, float y, short col = 0x000F, short cha = 0x2588, float depth = -1) { Draw((short)((short)(x + 2) - 2), (short)((short)(y + 2) - 2), col, cha, depth); }
						/// <summary>Draws to a specific point on the command prompt</summary> /// <param name="p">The position of the point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void Draw(VEC2F p, short col = 0x000F, short cha = 0x2588, float depth = -1) { Draw((short)((short)(p.x + 2) - 2), (short)((short)(p.y + 2) - 2), col, cha, depth); }
#pragma endregion

		//Gets the step size for the reaction in 1 axis when moving 1 unit in the other
		//	by going from point 1 to point 2 like this in both axis, you can find every space through which the line crosses
		//	but because they're floats, the points may not be exactly in a space, so for the first step you have to move less than 1 in the axis
		//	after the first movement in each axis, the movements can be by 1 unit
		//If the points are whole numbers, the first step can be skipped due to knowing that the points will always be exactly in a space
		//Using relative screen space for the points is the same as normal, but with the added step of converting the coordinates first
#pragma region DrawLine
						/// <summary>
						/// Draws a line on the command prompt from a point to another point
						/// </summary>
						/// <param name="x1">The x position of the first point (Leftmost is 0; Rightmost is screenSize.X)</param>
						/// <param name="y1">The y position of the first point (Topmost is 0; Bottommost is screenSize.Y)</param>
						/// <param name="x2">The x position of the second point (Leftmost is 0; Rightmost is screenSize.X)</param>
						/// <param name="y2">The y position of the second point (Topmost is 0; Bottommost is screenSize.Y)</param>
						/// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="cha">The character with which to draw the line</param>
						/// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param>
						/// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawLine(float x1, float y1, float x2, float y2, short col = 0x000F, short cha = 0x2588, float depth1 = -1, float depth2 = -1)
		{
			short ux = (x1 < x2 ? 1 : -1), uy = (y1 < y2 ? 1 : -1);
			float tx = x2 - x1, ty = y2 - y1;
			float sx = (ty != 0 ? ux * abs(tx / ty) : 0), sy = (tx != 0 ? uy * abs(ty / tx) : 0);
			Draw(x1, y1, col, cha, depth1);
			Draw(x2, y2, col, cha, depth2);
			tx = (float)fmod(ux - fmod(x1, 1.0f), 1.0f);
			ty = (float)fmod(uy - fmod(y1, 1.0f), 1.0f);
			LINEAR depthFunc = LinearFunction(x1, depth1, x2, depth2);
			for (float x = x1 + tx, y = y1 + uy * abs(tx * sy); x * ux < x2 * ux; x += ux, y += sy)
			{
				Draw(x, y, col, cha, depthFunc.f(x).y);
			}
			depthFunc = LinearFunction(y1, depth1, y2, depth2);
			for (float y = y1 + ty, x = x1 + ux * abs(ty * sx); y * uy < y2 * uy; y += uy, x += sx)
			{
				Draw(x, y, col, cha, depthFunc.f(y).y);
			}
		}
						/// <summary>Draws a line on the command pront from a point to another point</summary> /// <param name="p1">The position of the first point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="p2">The position of the second point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawLine(VEC2F p1, VEC2F p2, short col = 0x000F, short cha = 0x2588, float depth1 = -1, float depth2 = -1) { DrawLine(p1.x, p1.y, p2.x, p2.y, col, cha, depth1, depth2); }
						/// <summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="x1">The x position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y1">The y position of the first point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="x2">The x position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y2">The y position of the second point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawLineS(float x1, float y1, float x2, float y2, short col = 0x000F, short cha = 0x2588, float depth1 = -1, float depth2 = -1) { DrawLine(ScreenPosToPoint(x1, y1), ScreenPosToPoint(x2, y2), col, cha, depth1, depth2); }
						/// <summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="p1">The position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="p2">The position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawLineS(VEC2F p1, VEC2F p2, short col = 0x000F, short cha = 0x2588, float depth1 = -1, float depth2 = -1) { DrawLine(ScreenPosToPoint(p1), ScreenPosToPoint(p2), col, cha, depth1, depth2); }
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
						/// <param name="depth">How far away from the camera the shape is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawRPoly(float cx, float cy, short edges, float rad, float rot = 0, short col = 0x000F, short cha = 0x2588, float depth = -1)
		{
			float as = (360.0f / edges) * RAD;
			float max = (360.0f + rot) * RAD;
			for (float a = rot * RAD; a < max; a += as)
			{
				DrawLine((float)cos(a) * rad + cx, (float)sin(a) * rad + cy, (float)cos(a + as) * rad + cx, (float)sin(a + as) * rad + cy, col, cha, depth, depth);
			}
		}
						/// <summary>Draws a regular polygon to the screen</summary> /// <param name="p">The position of the center of the polygon</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawRPoly(VEC2F p, short edges, float rad, float rot = 0, short col = 0x000F, short cha = 0x2588, float depth = -1) { DrawRPoly(p.x, p.y, edges, rad, rot, col, cha, depth); }
						/// <summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="cx">The x position of the center of the polygon in relative screen space</param> /// <param name="cy">The y position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawRPolyS(float cx, float cy, short edges, float rad, bool useY, float rot = 0, short col = 0x000F, short cha = 0x2588, float depth = -1) { DrawRPoly(ScreenPosToPoint(cx, cy), edges, (useY ? ScreenPosToPoint(0, rad).y : ScreenPosToPoint(rad, 0).x), rot, col, cha, depth); }
						/// <summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="p">The position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawRPolyS(VEC2F p, short edges, float rad, bool useY, float rot = 0, short col = 0x000F, short cha = 0x2588, float depth = -1) { DrawRPolyS(p.x, p.y, edges, rad, useY, rot, col, cha, depth); }
#pragma endregion

						/// <summary>
						/// Writes a line of text starting from a point on the command prompt
						/// </summary>
						/// <param name="x">The x position of the leftmost character</param>
						/// <param name="y">The y position on which to write the line</param>
						/// <param name="text">The text to write</param>
						/// <param name="length">The amount of characters in the text</param>
						/// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="depth">How far away from the camera the text is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void WriteText(short x, short y, wchar_t* text, short length, short col = 0x000F, float depth = -1)
		{
			for (short i = 0; i < length; i++)
			{
				Draw((short)(i + x), y, col, text[i], depth);
			}
		}

						/// <summary>
						/// Draws a filled in triangle to the command prompt from 3 vertices
						/// </summary>
						/// <param name="v1">The first vertice of the triangle</param>
						/// <param name="v2">The second vertice of the triangle</param>
						/// <param name="v3">The third vertice of the triangle</param>
						/// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
						/// <param name="cha">The character with which to draw to that point</param>
						/// <param name="depth1">How far away from the camera the first vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
						/// <param name="depth2">How far away from the camera the second vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
						/// <param name="depth3">How far away from the camera the third vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
		void DrawTriangle(VEC2F v1, VEC2F v2, VEC2F v3, short col = 0x000F, short cha = 0x2588, float depth1 = -1, float depth2 = -1, float depth3 = -1)
		{
			VEC3F list[3] = { VEC3F(v1.x, v1.y, depth1), VEC3F(v2.x, v2.y, depth2), VEC3F(v3.x, v3.y, depth3) };
			for (short i = 0; i < 2; i++)
			{
				if (list[i].y < list[0].y)
				{
					std::swap(list[i], list[0]);
				}
				if (list[i].y > list[2].y)
				{
					std::swap(list[i], list[2]);
				}
			}
			VEC3F b1, b2, t1;
			if ((short)list[0].y == (short)list[2].y)
			{
				//Line
				DrawLine(list[0], list[1], col, cha, list[0].z, list[1].z);
				DrawLine(list[1], list[2], col, cha, list[1].z, list[2].z);
				DrawLine(list[2], list[0], col, cha, list[2].z, list[0].z);
			}
			else
			{
				if ((short)list[0].y == (short)list[1].y)
				{
					b1 = list[0];
					b2 = list[1];
					t1 = list[2];
				}
				else
				{
					if ((short)list[1].y == (short)list[2].y)
					{
						b1 = list[1];
						b2 = list[2];
						t1 = list[0];
					}
					else
					{
						//Turn into 2 triangles with a flat side and then pass them both through this function
						b1 = list[1];
						b2 = VEC3F(LinearFunction(list[0].y, list[0].x, list[2].y, list[2].x, b1.y), b1.y, LinearFunction(list[0].y, list[0].z, list[2].y, list[2].z, b1.y));
						DrawTriangle(list[0], b1, b2, col, cha, list[0].z, b1.z, b2.z);
						DrawTriangle(b1, b2, list[2], col, cha, b1.z, b2.z, list[2].z);
						return;
					}
				}
			}
			//The 'DrawLine()' code but slightly modified so that 2 lines can be done at once
			//If you draw 2 lines to the same point, both starting at the same Y level and filling in the space between them every time the Y level changes
			//then you've drawn a triangle
			short ux1 = (b1.x < t1.x ? 1 : -1), ux2 = (b2.x < t1.x ? 1 : -1), uy = (b1.y < t1.y ? 1 : -1);
			float tx1 = t1.x - b1.x, tx2 = t1.x - b2.x, ty = t1.y - b1.y;
			float sx1 = (ty != 0 ? ux1 * abs(tx1 / ty) : 0), sy1 = (tx1 != 0 ? uy * abs(ty / tx1) : 0), sx2 = (ty != 0 ? ux2 * abs(tx2 / ty) : 0), sy2 = (tx2 != 0 ? uy * abs(ty / tx2) : 0);
			DrawLine(b1.x, b1.y, b2.x, b2.y, col, cha, b1.z, b2.z);
			Draw(t1.x, t1.y, col, cha, t1.z);
			tx1 = (float)fmod(ux1 - fmod(b1.x, 1.0f), 1.0f);
			tx2 = (float)fmod(ux2 - fmod(b2.x, 1.0f), 1.0f);
			ty = (float)fmod(uy - fmod(b1.y, 1.0f), 1.0f);

			LINEAR b1Depth = LinearFunction(b1.y, b1.z, t1.y, t1.z);
			//+1 on x for line 1 & +? on y for line 1
			for (float x = b1.x + tx1, y = b1.y + uy * abs(tx1 * sy1); x * ux1 < t1.x * ux1; x += ux1, y += sy1)
			{
				Draw(x, y, col, cha, b1Depth.f(y).y);
			}
			LINEAR b2Depth = LinearFunction(b2.y, b2.z, t1.y, t1.z);
			//+1 on x for line 2 & +? on y for line 2
			for (float x = b2.x + tx2, y = b2.y + uy * abs(tx2 * sy2); x * ux2 < t1.x * ux2; x += ux2, y += sy2)
			{
				Draw(x, y, col, cha, b2Depth.f(y).y);
			}
			//+1 on y for line 1 & 2, & +? on x for line 1 & 2
			//DEBUG
			for (float y = b1.y + ty, x1 = b1.x + ux1 * abs(ty * sx1), x2 = b2.x + ux2 * abs(ty * sx2); y * uy < t1.y * uy; y += uy, x1 += sx1, x2 += sx2)
			{
				DrawLine(x1, y, x2, y, col, cha, b1Depth.f(y).y, b2Depth.f(y).y);
			}
		}
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
				zBuffer[i] = 1;
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

						/// <summary>0 -> nothing ; 1 -> released ; 2 -> pressed ; 3 -> held ; MOUSE_X and MOUSE_Y -> point on the command prompt</summary>
		void ReadInputs()
		{
			short tx = inputs[MOUSE_X];
			short ty = inputs[MOUSE_Y];
			for (auto& o : inputs)
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

					k->second = (buf.Event.KeyEvent.bKeyDown == false ? 1 : (k->second <= 1 ? 2 : 3));

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
							inputs[MOUSE_SCROLL] = (buf.Event.MouseEvent.dwButtonState >> 31 == 0 ? 1 : -1); inputs[MOUSE_SCROLL] = -1;
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
			CloseHandle(window);
			CloseHandle(console);
			CloseHandle(consoleInput);
		}

		virtual void Setup() = 0;

		virtual void Update() = 0;

						/// <summary>
						/// Opens a Windows Explorer window, allowing the user to choose a file of the designated file type to open. Will return true if a file is chosen.
						/// </summary>
						/// <param name="openFile">The variable to which this function will write the output file to if the user picks a file</param>
						/// <param name="fileType">The file type the user will be asked for</param>
						/// <param name="fileExtensions">The internal file type info. Must be formatted correctly: L"*.txt;*.jpg;*.png"</param>
		bool PromptFileSearch(FILE* openFile, const wchar_t* fileType, const wchar_t* fileExtensions)
		{
			openFile->fileName.lStructSize = sizeof(OPENFILENAMEW);
			openFile->fileName.hwndOwner = window;
			wchar_t temp[128] = { 0 };
			swprintf(temp, 128, L"%ls (%ls)%c%ls%c%c", fileType, fileExtensions, '\0', fileExtensions, '\0', '\0');
			openFile->fileName.lpstrFilter = temp;
			wchar_t flnm[MAX_PATH] = { 0 };
			flnm[0] = L'\0';
			openFile->fileName.lpstrFile = flnm;
			openFile->fileName.nMaxFile = MAX_PATH;
			openFile->fileName.Flags = OFN_FILEMUSTEXIST;
			wchar_t temp2[128];
			swprintf(temp2, 128, L"Open %ls file to import...", fileType);
			openFile->fileName.lpstrTitle = temp2;
			if (GetOpenFileNameW(&(openFile->fileName)))
			{
				HANDLE fileHandle = CreateFileW(
					openFile->fileName.lpstrFile,	// Filename
					GENERIC_READ,					// Desired access
					0,								// Share mode
					NULL,							// Security attributes
					OPEN_EXISTING,					// Opens a file only if it already exist
					FILE_ATTRIBUTE_NORMAL,			// Flags and attributes
					NULL);							// Template file handle
				if (fileHandle != INVALID_HANDLE_VALUE)
				{
					openFile->fileHandle = fileHandle;

					wchar_t fullDirectory[MAX_PATH] = { 0 };
					int length1 = swprintf(fullDirectory, MAX_PATH, L"%ls", openFile->fileName.lpstrFile);
					int lastSlash = 0;
					for (int i = 0; i < length1; i++)
					{
						if (fullDirectory[i] == L'\\')
						{
							lastSlash = i;
						}
					}
					bool extension = false;
					int index = 0;
					for (int i = lastSlash + 1; i < length1; i++)
					{
						if (!extension)
						{
							if (fullDirectory[i] == L'.')
							{
								extension = true;
								index = -1;
							}
							else
							{
								openFile->name[index] = fullDirectory[i];
							}
						}
						else
						{
							openFile->extension[index] = fullDirectory[i];
						}
						index++;
					}
					return true;
				}
			}
			return false;
		}

						/// <summary>
						/// Opens a Windows Explorer window, allowing the user to choose the name and location of the file to save to, with the designated file type. Will return true if a file is chosen.
						/// </summary>
						/// <param name="saveFile">The variable to which this function will write the output file to if the user picks a file</param>
						/// <param name="fileType">The file type the user will be asked for</param>
						/// <param name="fileExtensions">The internal file type info. Must be formatted correctly: L"*.txt;*.jpg;*.png"</param>
		bool PromptFileSave(FILE* saveFile, const wchar_t* fileType, const wchar_t* fileExtensions)
		{
			saveFile->fileName.lStructSize = sizeof(OPENFILENAMEW);
			saveFile->fileName.hwndOwner = window;
			wchar_t temp[128] = { 0 };
			swprintf(temp, 128, L"%ls (%ls)%c%ls%c%c", fileType, fileExtensions, '\0', fileExtensions, '\0', '\0');
			saveFile->fileName.lpstrFilter = temp;
			wchar_t flnm[MAX_PATH] = { 0 };
			flnm[0] = L'\0';
			saveFile->fileName.lpstrFile = flnm;
			saveFile->fileName.nMaxFile = MAX_PATH;
			saveFile->fileName.lpstrTitle = L"Save as";
			wchar_t temp2[128] = { 0 };
			int extensionsLength = swprintf(temp2, 128, L"%ls", fileExtensions);
			for (int i = 0; i < extensionsLength; i++)
			{
				if (fileExtensions[i] == L'.')
				{
					i++;
					for (int  j = 0; j + i < extensionsLength; j++)
					{
						if (fileExtensions[j + i] == L';')
						{
							temp2[j] = L'\0';
							break;
						}
						temp2[j] = fileExtensions[j + i];
					}
					break;
				}
			}
			saveFile->fileName.lpstrDefExt = temp2;
			if (GetSaveFileNameW(&(saveFile->fileName)))
			{
				HANDLE fileHandle = CreateFileW(
					saveFile->fileName.lpstrFile,	// Filename
					GENERIC_WRITE,					// Desired access
					0,								// Share mode
					NULL,							// Security attributes
					CREATE_NEW,						// Creates a new file, only if it doesn't already exist
					FILE_ATTRIBUTE_NORMAL,			// Flags and attributes
					NULL);							// Template file handle
				if (fileHandle != INVALID_HANDLE_VALUE)
				{
					saveFile->fileHandle = fileHandle;
					return true;
				}
			}
			return false;
		}

		//General useful functions
#pragma region MiscFunctions
						/// <summary>Elevates a number to the power of 2</summary>
		static double Pow2(double value) { return value * value; }
						/// <summary>Calculates the dot product of 2 vectors</summary>
		static float DotProduct(VEC4F v1, VEC4F v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }
						/// <summary>Calculates the orthagonal vector of a plane defined by 2 vectors (Index = v1; Middle = v2; Thumb = result)</summary>
		static VEC3F CrossProduct(VEC3F v1, VEC3F v2) { return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x }; }
						/// <summary>Calculates the length of a vector</summary>
		static float Magnitude(VEC4F v) { return (float)sqrt(abs(DotProduct(v, v))); }
						/// <summary>Limits 'value' to the range between 'low' and 'high'</summary>
		static float Clamp(float value, float low, float high) { return (value < low ? low : (high < value ? high : value)); }
						/// <summary>Gives the sign of a value (Positive = 1; Negative = -1; Other = itself)</summary>
		static int Sign(int value) { return (value > 0 ? 1 : (value < 0 ? -1 : value)); }
						/// <summary>Gives the sign of a value (Positive = 1; Negative = -1; Other = itself)</summary>
		static double Sign(double value) { return (value > 0.0 ? 1.0 : (value < 0.0 ? -1.0 : value)); }
						/// <summary>Calculates the angle between 2 vectors in degrees</summary>
		static float Angle(VEC4F v1, VEC4F v2) { return (float)acos(Clamp(DotProduct(v1, v2) / (Magnitude(v2) * Magnitude(v1)), -1.0f, 1.0f)) * DEG; }
						/// <summary>Gives a vector of length 1 in the same direction</summary>
		static VEC4F Normalize(VEC4F v) { return v / Magnitude(v); }

#pragma region LinearFunction
						/// <summary>Calculates the y value of point x on the line defined by the 2 points provided (Returns y1 if x1 == x2)</summary>
		static float LinearFunction(float x1, float y1, float x2, float y2, float x)
		{
			if (y1 == y2 || x1 == x2)
				return y1;
			return y1 + ((y2 - y1) / (x2 - x1) * (x - x1));
		}
						/// <summary>Calculates the point at 'x' on the line defined by the 2 points provided (Returns p1 if p1.x == p2.x)</summary>
		static VEC4F LinearFunction(VEC4F p1, VEC4F p2, float x)
		{
			if (p1.x == p2.x)
				return p1;
			return { x, LinearFunction(p1.x, p1.y, p2.x, p2.y, x), LinearFunction(p1.x, p1.z, p2.x, p2.z, x), LinearFunction(p1.x, p1.w, p2.x, p2.w, x) };
		}
		//Warn if both points have the same X and thus can't be made into a linear function
						/// <summary>Calculates the y value of point x on the line defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
		static bool LinearFunction(float x1, float y1, float x2, float y2, float x, float* output) { if (x1 == x2) { return false; } *output = LinearFunction(x1, y1, x2, y2, x); return true; }
						/// <summary>Calculates the point at 'x' on the line defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
		static bool LinearFunction(VEC4F p1, VEC4F p2, float x, VEC4F* output) { if (p1.x == p2.x) { return false; } *output = LinearFunction(p1, p2, x); return true; }
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=0 if x1 == x2)</summary>
		static LINEAR LinearFunction(float x1, float y1, float x2, float y2)
		{
			if (x1 == x2)
				return LINEAR();
			return { VEC4F(0, (y2 - y1) / (x2 - x1), 0, 0), VEC4F(0, -(y2 - y1) / (x2 - x1) * x1 + y1, 0, 0) };
		}
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=z=w=0 if p1.x == p2.x)</summary>
		static LINEAR LinearFunction(VEC4F p1, VEC4F p2)
		{
			if (p1.x == p2.x)
				return LINEAR();
			float t = 1.0f / (p2.x - p1.x);
			return { VEC4F(1, (p2.y - p1.y) * t, (p2.z - p1.z) * t, (p2.w - p1.w) * t), VEC4F(0, -(p2.y - p1.y) * t * p1.x + p1.y, -(p2.z - p1.z) * t * p1.x + p1.z, -(p2.w - p1.w) * t * p1.x + p1.w) };
		}
		//Warn if both points have the same X and thus can't be made into a linear function
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
		static bool LinearFunction(float x1, float y1, float x2, float y2, LINEAR* output) { if (x1 == x2) { return false; } *output = LinearFunction(x1, y1, x2, y2); return true; }
						/// <summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
		static bool LinearFunction(VEC4F p1, VEC4F p2, LINEAR* output) { if (p1.x == p2.x) { return false; } *output = LinearFunction(p1, p2); return true; }
#pragma endregion

						/// <summary>Calculates the point at 'x' on the line defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)</summary>
		static bool QuadraticFunction(VEC2F p1, VEC2F p2, VEC2F p3, float x, VEC2F* output)
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
						/// <summary>Generates the quadratic function based on 'x' defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)</summary>
		static bool QuadraticFunction(VEC4F p1, VEC4F p2, VEC4F p3, QUADRATIC* output)
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
		static VEC3F VectorFromAngles(float h, float v)
		{
			return { (float)(sin(h * RAD) * cos(v * RAD)), (float)sin(v * RAD), (float)(cos(h * RAD) * cos(v * RAD)) };
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
	struct PLANE
	{
		cmde::VEC3F point;
		cmde::VEC3F normal;

		PLANE() { point = normal = cmde::VEC3F(); }
		PLANE(cmde::VEC3F p, cmde::VEC3F n) { point = p; normal = Normalize(n); }
	};

	struct Triangle
	{
		cmde::VEC3F vertices[3];
		short color;
		cmde::VEC3F normal;
		//Used for wireframe rendering, so that only the actual triangles' lines get drawn and not the clipped triangles' lines
		bool visibleSides[3];

		Triangle(cmde::VEC3F p1, cmde::VEC3F p2, cmde::VEC3F p3, short col = 0x00FF)
		{
			vertices[0] = p1;
			vertices[1] = p2;
			vertices[2] = p3;
			visibleSides[0] = true;
			visibleSides[1] = true;
			visibleSides[2] = true;
			color = col;
			normal = Normalize(CrossProduct(vertices[0] - vertices[1], vertices[2] - vertices[1]));
		}

		Triangle(cmde::VEC3F p1, cmde::VEC3F p2, cmde::VEC3F p3, short col, bool s1, bool s2, bool s3)
		{
			vertices[0] = p1;
			vertices[1] = p2;
			vertices[2] = p3;
			visibleSides[0] = s1;
			visibleSides[1] = s2;
			visibleSides[2] = s3;
			color = col;
			normal = Normalize(CrossProduct(vertices[0] - vertices[1], vertices[2] - vertices[1]));
		}

		Triangle GetWithOffset(cmde::VEC3F offset)
		{
			return Triangle(vertices[0] + offset, vertices[1] + offset, vertices[2] + offset, color, visibleSides[0], visibleSides[1], visibleSides[2]);
		}
	};

	struct Mesh
	{
		std::vector<Triangle> triangles;
		float radius;

		Mesh(std::vector<Triangle>& triangles = *new std::vector<Triangle>())
		{
			this->triangles = triangles;
			radius = 0;
			for (Triangle& t : triangles)
			{
				radius = max(max(radius, Magnitude(t.vertices[0])), max(Magnitude(t.vertices[1]), Magnitude(t.vertices[2])));
			}
		}

		void ChangeColor(short col)
		{
			for (Triangle& t : triangles)
			{
				t.color = col;
			}
		}
	};

	struct Object
	{
		Mesh mesh;
		cmde::VEC3F position;

		Object(Mesh& m = *new Mesh(), cmde::VEC3F pos = { 0, 0, 0 })
		{
			mesh = m;
			position = pos;
		}
	};

	struct ObjFile : public cmde::PROCESSEDFILE
	{
		static const bool utf16 = false;
		std::vector<cmde::VEC3F> points;
		std::vector<Triangle> triangles;
		wchar_t* ptr;
		cmde::VEC3F tempV;
		std::vector<cmde::VEC3F> tempVs;
		double trash;
		int index;

		ObjFile()
		{
			points = std::vector<cmde::VEC3F>();
			triangles = std::vector<Triangle>();
		}

		void ProcessLine(wchar_t* line, unsigned long length)
		{
			if (length > 1)
			{
				ptr = &line[0];
				tempVs = std::vector<cmde::VEC3F>();
				switch (line[0])
				{
				case L'v':
					if (line[1] != ' ')
					{
						return;
					}
					while (isdigit(*ptr) == false && *ptr != L'-')
					{
						ptr++;
					}
					tempV.x = (float)std::wcstod(ptr, &ptr);

					while (isdigit(*ptr) == false && *ptr != L'-')
					{
						ptr++;
					}
					tempV.y = (float)std::wcstod(ptr, &ptr);

					while (isdigit(*ptr) == false && *ptr != L'-')
					{
						ptr++;
					}
					tempV.z = (float)std::wcstod(ptr, &ptr);

					points.push_back(tempV);
					break;
				case L'f':
					//Faces can be made up of any amount of points, not limited to triangles
					// Fan made up of (v[0], v[i], v[i+1])
					//The format is '[v]/[vn]/[vt]' with all but [v] being optional (Though both '/' must be there)
					// [v] is the index
					// [vn] is the normal
					// [vt] is the uv
					if (line[1] != ' ')
					{
						return;
					}
					index = 0;
					while (ptr < &line[length - 1])
					{
						while (isdigit(*ptr) == false && *ptr != L'-' && ptr < &line[length - 1])
						{
							ptr++;
						}
						if (ptr >= &line[length - 1])
						{
							break;
						}
						index = (int)(std::wcstod(ptr, &ptr) - 1.0);
						tempVs.push_back(points[index]);
						if (*ptr == L'/')
						{
							ptr++;
							trash = std::wcstod(ptr, &ptr); //[vn]
						}
						if (*ptr == L'/')
						{
							ptr++;
							trash = std::wcstod(ptr, &ptr); //[vt]
						}
					}
					for (int i = 1; i < tempVs.size() - 1; i++)
					{
						triangles.push_back(Triangle(tempVs[0], tempVs[i], tempVs[i + 1]));
					}
					break;
				}
			}
		}
	};

	struct CMDE3DFile : public cmde::PROCESSEDFILE
	{
		static const int utf16 = false;
		std::vector<Triangle> triangles;
		wchar_t* ptr;
		cmde::VEC3F tempVs[3];
		short color;
		CMDEngine* ce;

		CMDE3DFile()
		{
			triangles = std::vector<Triangle>();
		}

		void ProcessLine(wchar_t* line, unsigned long length)
		{
			if (length > 1)
			{
				ptr = &line[0];
				//(x1 y1 z1) ; (x2 y2 z2) ; (x3 y3 z3) #col
				ptr++; //'('
				tempVs[0].x = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[0].y = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[0].z = (float)std::wcstod(ptr, &ptr);
				ptr++; //')'
				ptr++; //' '
				ptr++; //';'
				ptr++; //' '
				ptr++; //'('
				tempVs[1].x = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[1].y = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[1].z = (float)std::wcstod(ptr, &ptr);
				ptr++; //')'
				ptr++; //' '
				ptr++; //';'
				ptr++; //' '
				ptr++; //'('
				tempVs[2].x = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[2].y = (float)std::wcstod(ptr, &ptr);
				ptr++; //' '
				tempVs[2].z = (float)std::wcstod(ptr, &ptr);
				ptr++; //')'
				ptr++; //' '
				ptr++; //'#'
				color = (short)std::wcstol(ptr, &ptr, 16);


				triangles.push_back(Triangle(tempVs[0], tempVs[1], tempVs[2], color));
			}
		}

		static bool Export(cmde::FILE& exportFile, Mesh mesh)
		{
			wchar_t lineBuffer[cmde::FILE::CHUNK_SIZE] = { 0 };
			for (Triangle& t : mesh.triangles)
			{
				//UTF-8 Version
				DWORD writeCount = swprintf(lineBuffer, cmde::FILE::CHUNK_SIZE,
					L"(%c%011f %c%011f %c%011f) ; (%c%011f %c%011f %c%011f) ; (%c%011f %c%011f %c%011f) #0x%04hx\n",
					(t.vertices[0].x < 0 ? L'-' : L'+'), abs(t.vertices[0].x),
					(t.vertices[0].y < 0 ? L'-' : L'+'), abs(t.vertices[0].y),
					(t.vertices[0].z < 0 ? L'-' : L'+'), abs(t.vertices[0].z),
					(t.vertices[1].x < 0 ? L'-' : L'+'), abs(t.vertices[1].x),
					(t.vertices[1].y < 0 ? L'-' : L'+'), abs(t.vertices[1].y),
					(t.vertices[1].z < 0 ? L'-' : L'+'), abs(t.vertices[1].z),
					(t.vertices[2].x < 0 ? L'-' : L'+'), abs(t.vertices[2].x),
					(t.vertices[2].y < 0 ? L'-' : L'+'), abs(t.vertices[2].y),
					(t.vertices[2].z < 0 ? L'-' : L'+'), abs(t.vertices[2].z),
					t.color);
				char byteBuffer[cmde::FILE::CHUNK_SIZE] = { 0 };
				writeCount = WideCharToMultiByte(CP_UTF8, 0, lineBuffer, -1, byteBuffer, cmde::FILE::CHUNK_SIZE, NULL, NULL);
				if (!WriteFile(exportFile.fileHandle, &byteBuffer, writeCount - 1, &writeCount, NULL))
				{
					return false;
				}

				//UTF-16 Version
				/*
				DWORD writeCount = swprintf(lineBuffer, cmde::FILE::CHUNK_SIZE,
					L"(%c%011f %c%011f %c%011f) ; (%c%011f %c%011f %c%011f) ; (%c%011f %c%011f %c%011f) #0x%04hx\n",
					(t.vertices[0].x < 0 ? L'-' : L'+'), abs(t.vertices[0].x),
					(t.vertices[0].y < 0 ? L'-' : L'+'), abs(t.vertices[0].y),
					(t.vertices[0].z < 0 ? L'-' : L'+'), abs(t.vertices[0].z),
					(t.vertices[1].x < 0 ? L'-' : L'+'), abs(t.vertices[1].x),
					(t.vertices[1].y < 0 ? L'-' : L'+'), abs(t.vertices[1].y),
					(t.vertices[1].z < 0 ? L'-' : L'+'), abs(t.vertices[1].z),
					(t.vertices[2].x < 0 ? L'-' : L'+'), abs(t.vertices[2].x),
					(t.vertices[2].y < 0 ? L'-' : L'+'), abs(t.vertices[2].y),
					(t.vertices[2].z < 0 ? L'-' : L'+'), abs(t.vertices[2].z),
					t.color);
				if (!WriteFile(exportFile.fileHandle, &lineBuffer, writeCount * 2, &writeCount, NULL))
				{
					return false;
				}
				*/
			}
			return true;
		}
	};

	struct RaycastHit
	{
		PLANE plane;
		Object* object;
		Triangle* triangle;

		RaycastHit() { object = new Object(); triangle = new Triangle({ cmde::VEC3F(), cmde::VEC3F(), cmde::VEC3F() }); }

		RaycastHit(PLANE p, Object* o, Triangle* t) { plane = p; object = o; triangle = t; }

						/// <summary>
						/// Casts a ray and detects the first plane of the objects with which it intersects
						/// </summary>
						/// <param name="origin">The point from ray starts at</param>
						/// <param name="direction">The direction the ray goes</param>
						/// <param name="objects">The objects to test against</param>
		bool Raycast(cmde::VEC3F origin, cmde::VEC3F direction, std::vector<Object>& objects)
		{
			std::vector<RaycastHit> hits = std::vector<RaycastHit>();
			RaycastAll(origin, direction, objects, &hits);
			float distance = -1;
			for (RaycastHit& p : hits)
			{
				float tempDist = Magnitude(p.plane.point - origin);
				if (tempDist < distance || distance < 0)
				{
					distance = tempDist;

					plane = p.plane;
					object = p.object;
					triangle = p.triangle;
				}
			}
			return hits.size() > 0;
		}
	};

	struct Camera
	{
		cmde::VEC3F position;
		cmde::VEC2F facing;
		cmde::VEC3F left;
		cmde::VEC3F forwards;
		cmde::VEC3F up;
		cmde::VEC3F sightLimitL;
		cmde::VEC3F sightLimitT;
		cmde::VEC2F fov;
		float nearPlane;
		float farPlane;
		PLANE* inBounds;
		COORD screenSize;
		CMDEngine* engine;

						///<summary>There must be a default constructor or computer gets mad (This should be unusable though)</summary>
		Camera()
		{
			farPlane = -1;
			nearPlane = -1;
			screenSize = COORD();
			UpdateInBounds();
		}

		Camera(cmde::VEC3F position, cmde::VEC2F facing, cmde::VEC2F fov, float nearPlane, float farPlane, CMDEngine* engine)
		{
			this->position = position;
			this->left = left;
			this->forwards = forwards;
			this->up = up;
			this->sightLimitL = sightLimitL;
			this->sightLimitT = sightLimitT;
			this->fov = fov;
			this->nearPlane = nearPlane;
			this->farPlane = farPlane;
			this->screenSize = engine->screenSize;
			this->engine = engine;
			UpdateInBounds();
			UpdateRotation();
		}

		void UpdateInBounds()
		{
			cmde::VEC3F slr = left * -sin(fov.x * 0.5f * RAD) + forwards * cos(fov.x * 0.5f * RAD);
			cmde::VEC3F slb = up * -sin(fov.x * 0.5f * RAD) + forwards * cos(fov.x * 0.5f * RAD);
			inBounds = new PLANE[]{ { position, CrossProduct(sightLimitL, up) }, { position, CrossProduct(left, sightLimitT) }, { position, CrossProduct(up, slr) }, { position, CrossProduct(slb, left) }, { forwards * nearPlane + position, forwards }, { forwards * farPlane + position, forwards * -1 } };
		}

		void RenderShapeSpherical(Object& obj, bool wireframe = false)
		{
			UpdateInBounds();
			std::vector<Triangle> newTriangles = ClipTriangles(obj, position, inBounds);
			for (Triangle& t : newTriangles)
			{
				if (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), t.vertices[0] - position) > 0)
				{
					//Backface culling (This isn't a TODO, skipping the triangle like this is the backface culling)
					continue;
				}
				cmde::VEC3F vertices[3];
				for (int i = 0; i < 3; i++)
				{
					engine->DrawLine({ 0, 0 }, { 2, 0 });
					engine->DrawLine({ 0, 2 }, { 2, 2 });
					engine->DrawLine({ 0, 4 }, { 2, 4 });
					engine->Draw(0.0f, 1.0f);
					engine->Draw(2.0f, 3.0f);
					cmde::VEC3F temp = t.vertices[i] - position;
					cmde::VEC3F hTemp = forwards * DotProduct(temp, forwards);
					cmde::VEC3F vTemp = hTemp + up * DotProduct(temp, up);
					hTemp = hTemp + left * DotProduct(temp, left);
					float hAngle = Angle(hTemp, sightLimitL);
					float vAngle = Angle(vTemp, sightLimitT);

					vertices[i] = engine->ScreenPosToPoint((hAngle / fov.x), (vAngle / fov.y));
					vertices[i].z = (Magnitude(temp) - nearPlane) / (farPlane - nearPlane);
				}
				if (wireframe == true)
				{
					short color = (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), forwards) < 0 ? 0x00EE : 0x00BB);
					if (t.visibleSides[0] == true)
						engine->DrawLine(vertices[0], vertices[1], color, 0x2588, vertices[0].z, vertices[1].z);
					if (t.visibleSides[1] == true)
						engine->DrawLine(vertices[1], vertices[2], color, 0x2588, vertices[1].z, vertices[2].z);
					if (t.visibleSides[2] == true)
						engine->DrawLine(vertices[2], vertices[0], color, 0x2588, vertices[2].z, vertices[0].z);
				}
				else
				{
					engine->DrawTriangle(vertices[0], vertices[1], vertices[2], t.color, 0x2588, vertices[0].z, vertices[1].z, vertices[2].z);
				}
			}
		}

		void RenderShapeProjection(Object& obj, bool wireframe = false)
		{
			UpdateInBounds();
			std::vector<Triangle> newTriangles = ClipTriangles(obj, position, inBounds);
			for (Triangle t : newTriangles)
			{
				if (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), t.vertices[0] - position) >= 0)
				{
					//Backface culling (This isn't a TODO, skipping the triangle like this is the backface culling)
					continue;
				}
				cmde::VEC3F vertices[3];
				for (int i = 0; i < 3; i++)
				{
					engine->DrawLine({ 0, 0 }, { 2, 0 });
					engine->DrawLine({ 0, 2 }, { 2, 2 });
					engine->DrawLine({ 0, 0 }, { 0, 4 });
					engine->Draw(2.0f, 1.0f);
					cmde::VEC3F temp = t.vertices[i] - position;
					temp = { DotProduct(temp, left), DotProduct(temp, up), DotProduct(temp, forwards) };
					vertices[i] = (ProjectionMatrixify(temp) + cmde::VEC2F(1, 1)) * 0.5f * cmde::VEC2F(screenSize.X, screenSize.Y);
					vertices[i].z = (temp.z - nearPlane) / (farPlane - nearPlane);
				}
				if (wireframe == true)
				{
					short color = (DotProduct(CrossProduct(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]), forwards) < 0 ? 0x00EE : 0x00BB);
					if (t.visibleSides[0] == true)
						engine->DrawLine(vertices[0], vertices[1], color, 0x2588, vertices[0].z, vertices[1].z);
					if (t.visibleSides[1] == true)
						engine->DrawLine(vertices[1], vertices[2], color, 0x2588, vertices[1].z, vertices[2].z);
					if (t.visibleSides[2] == true)
						engine->DrawLine(vertices[2], vertices[0], color, 0x2588, vertices[2].z, vertices[0].z);
				}
				else
				{
					engine->DrawTriangle(vertices[0], vertices[1], vertices[2], t.color, 0x2588, vertices[0].z, vertices[1].z, vertices[2].z);
				}
			}
		}

		void UpdateRotation()
		{
			//X+ is left when Z+ is forwards and Y+ is up
			forwards = VectorFromAngles(facing.x, facing.y);	//  0,  0,  1
			left = VectorFromAngles(facing.x + 90, 0);			//  1,  0,  0
			up = VectorFromAngles(facing.x, facing.y + 90);		//  0,  1,  0
			sightLimitL = left * sin(fov.x * 0.5f * RAD) + forwards * cos(fov.x * 0.5f * RAD);
			sightLimitT = up * sin(fov.x * 0.5f * RAD) + forwards * cos(fov.x * 0.5f * RAD);
		}

		cmde::VEC2F ProjectionMatrixify(cmde::VEC3F v)
		{
			float f1 = 1.0f / tanf(fov.x * 0.5f * RAD);
			float f2 = 1.0f / tanf(fov.y * 0.5f * RAD);
			if (v.z != 0)
				return { (((float)screenSize.Y / (float)screenSize.X) * f1 * v.x) / -v.z, (f2 * v.y) / -v.z };
			return { (((float)screenSize.Y / (float)screenSize.X) * f1 * v.x), (f2 * v.y) };
		}

		//TODO, MAYBE
		void PostProcessing(std::vector<COORD>* buffer)
		{
			for (COORD& b : *buffer)
			{
				//POST PROCESSING GOES HERE
			}
		}
	};

	static bool RayPlaneIntersection(PLANE plane, cmde::VEC3F origin, cmde::VEC3F direction, cmde::VEC3F* output)
	{
		float nd = DotProduct(plane.normal, direction);
		if (nd == 0)
			return false;
		float t = (DotProduct(plane.normal, plane.point - origin) / nd);
		if (t < 0)
			return false;
		*output = origin + direction * t;
		return true;
	}

	static bool PointInTriangle(cmde::VEC3F point, Triangle& triangle)
	{
		return (DotProduct(point - triangle.vertices[0], CrossProduct(triangle.vertices[2] - triangle.vertices[0], triangle.normal)) < 0) &&
			(DotProduct(point - triangle.vertices[1], CrossProduct(triangle.vertices[0] - triangle.vertices[1], triangle.normal)) < 0) &&
			(DotProduct(point - triangle.vertices[2], CrossProduct(triangle.vertices[1] - triangle.vertices[2], triangle.normal)) < 0);
	}

	static std::vector<Triangle> ClipTriangles(Object& obj, cmde::VEC3F cameraPos, PLANE inBounds[6])
	{
		std::vector<Triangle> output = std::vector<Triangle>();
		for (Triangle& t : obj.mesh.triangles)
		{
			output.push_back(t.GetWithOffset(obj.position));
		}
		ClipTriangles(&output, cameraPos, inBounds);
		return output;
	}

	static void ClipTriangles(std::vector<Triangle>* ts, cmde::VEC3F cameraPos, PLANE inBounds[6])
	{
		for (short i = 0; i < 6; i++)
		{
			std::vector<Triangle> temp;
			for (Triangle& t : *ts)
			{
				std::vector<Triangle> ct = ClipTriangle(t, inBounds[i]);
				temp.insert(temp.end(), ct.begin(), ct.end());
			}
			*ts = temp;
		}
	}

	static std::vector<Triangle> ClipTriangle(Triangle& t, PLANE inBounds)
	{
		bool oob[3] = { false };
		short c = 0;
		std::vector<Triangle> output = std::vector<Triangle>();
		for (short i = 0; i < 3; i++)
		{
			if (DotProduct(t.vertices[i] - inBounds.point, inBounds.normal) < 0)
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
		short o = 0;
		for (short i = 0; i < 3; i++)
		{
			if (oob[i] == true)
			{
				o = i;
				g = t.vertices[i];
				g1 = t.vertices[(i + 1) % 3];
				g2 = t.vertices[(i + 2) % 3];
				break;
			}
		}
		RayPlaneIntersection(inBounds, g, g1 - g, &new1);
		RayPlaneIntersection(inBounds, g, g2 - g, &new2);

		switch (c)
		{
		case 0:
			output.push_back(Triangle(t));
			break;
		case 3:
			//nothing
			break;
		case 1:
			output.push_back(Triangle(new1, g1, g2, t.color, true && t.visibleSides[o], true && t.visibleSides[(o + 1) % 3], false));
			output.push_back(Triangle(new1, g2, new2, t.color, false, true && t.visibleSides[(o + 2) % 3], false));
			break;
		case 2:
			output.push_back(Triangle(g, new1, new2, t.color, true && t.visibleSides[o], false, true && t.visibleSides[(o + 2) % 3]));
			break;
		}
		return output;
	}

					///<summary>A lot of projection matrices output depth in a weird format, which differs from the one used in this program. This function converts depth from the linear type used in this program to that weird one</summary>
	float DepthConversion(float depth) { return (depth * camera.farPlane) / (depth * (camera.farPlane - camera.nearPlane) + camera.nearPlane); }
					///<summary>Takes a relative depth value where 0 is the near plane and 1 is the far plane, and returns an actual depth value</summary>
	float DenormalizeDepth(float depth) { return depth * (camera.farPlane - camera.nearPlane) + camera.nearPlane; }

					/// <summary>
					/// Casts a ray and detects every plane of the objects with which it intersects
					/// </summary>
					/// <param name="origin">The point from ray starts at</param>
					/// <param name="direction">The direction the ray goes</param>
					/// <param name="objects">The objects to test against</param>
					/// <param name="output">The vector in which to store the data of every hit</param>
	static bool RaycastAll(cmde::VEC3F origin, cmde::VEC3F direction, std::vector<Object>& objects, std::vector<RaycastHit>* output)
	{
		direction = Normalize(direction);
		for (Object& o : objects)
		{
			cmde::VEC3F nearest = origin + direction * DotProduct(o.position - origin, direction);
			if (o.mesh.radius >= Magnitude(nearest - o.position))
			{
				//Ray passes through this object's bounding sphere (Could possibly collide)
				for (Triangle& t : o.mesh.triangles)
				{
					if (DotProduct(t.normal, direction) > 0)
					{
						Triangle tOff = t.GetWithOffset(o.position);
						cmde::VEC3F point = cmde::VEC3F();
						if (RayPlaneIntersection(PLANE(tOff.vertices[0], tOff.normal), origin, direction, &point) == true)
						{
							if (PointInTriangle(point, tOff) == true)
							{
								output->push_back(RaycastHit(PLANE(point, tOff.normal), &o, &t));
							}
						}
					}
				}
			}
		}
		return output->size() > 0;
	}

public:
	Object obj1;
	Object obj2;
	Object obj3;
	Camera camera;
	bool myRenderingSystem;
	bool wireframe;
	std::vector<COORD> postProcessBuffer = {}; //TODO, MAYBE
	std::vector<Object> objects = {};
	short selCol = 0x0000;

	/*
	Mesh defaultRaycast;
	Triangle* resetTriangleColor = new Triangle(cmde::VEC3F(), cmde::VEC3F(), cmde::VEC3F());
	short resetColor = 0x0FF1;
	*/


	Test3D(short screenWidth, short screenHeight, short fontWidth, short fontHeight) : cmde::CMDEngine(screenWidth, screenHeight, fontWidth, fontHeight, true, true, FPS60)
	{
		//1x1x1 Cube
		///*
		Mesh cube1 = Mesh(*new std::vector<Triangle>{
			{ { 0, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 }, 0x00AA },
			{ { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, 0x00AA },
			{ { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, 0x00AA },
			{ { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, 0x00AA },
			{ { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 1 }, 0x00AA },
			{ { 1, 1, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, 0x00AA },
			{ { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 }, 0x00AA },
			{ { 0, 1, 0 }, { 1, 1, 1 }, { 1, 1, 0 }, 0x00AA },
			{ { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, 0x00AA },
			{ { 0, 0, 0 }, { 0, 1, 1 }, { 0, 1, 0 }, 0x00AA },
			{ { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, 0x00AA },
			{ { 0, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 }, 0x00AA }
			});
		//*/

		//2x2x2 Cube (With Z+2 offset)
		/*
		Mesh cube2 = Mesh(*new std::vector<Triangle> {
			{ { 0, 0, 2 }, { 0, 2, 2 }, { 2, 0, 2 }, 0x0099 },
			{ { 0, 2, 2 }, { 2, 2, 2 }, { 2, 0, 2 }, 0x0099 },
			{ { 0, 0, 2 }, { 2, 0, 2 }, { 0, 0, 4 }, 0x0099 },
			{ { 2, 0, 2 }, { 2, 0, 4 }, { 0, 0, 4 }, 0x0099 },
			{ { 2, 0, 2 }, { 2, 2, 2 }, { 2, 0, 4 }, 0x0099 },
			{ { 2, 2, 2 }, { 2, 2, 4 }, { 2, 0, 4 }, 0x0099 },
			{ { 0, 2, 2 }, { 0, 2, 4 }, { 2, 2, 4 }, 0x0099 },
			{ { 0, 2, 2 }, { 2, 2, 4 }, { 2, 2, 2 }, 0x0099 },
			{ { 0, 0, 2 }, { 0, 0, 4 }, { 0, 2, 4 }, 0x0099 },
			{ { 0, 0, 2 }, { 0, 2, 4 }, { 0, 2, 2 }, 0x0099 },
			{ { 0, 0, 4 }, { 2, 0, 4 }, { 2, 2, 4 }, 0x0099 },
			{ { 0, 0, 4 }, { 2, 2, 4 }, { 0, 2, 4 }, 0x0099 },
		});
		*/

		//1x20x1 Pole (Rainbow)
		/*
		Mesh cube2 = Mesh(*new std::vector<Triangle> {
			{ { 0, -10, 0 }, { 0, 10, 0 }, { 1, -10, 0 }, 0x0066 },
			{ { 0, 10, 0 }, { 1, 10, 0 }, { 1, -10, 0 }, 0x0077 },
			{ { 0, -10, 0 }, { 1, -10, 0 }, { 0, -10, 1 }, 0x0055 },
			{ { 1, -10, 0 }, { 1, -10, 1 }, { 0, -10, 1 }, 0x0044 },
			{ { 1, -10, 0 }, { 1, 10, 0 }, { 1, -10, 1 }, 0x0033 },
			{ { 1, 10, 0 }, { 1, 10, 1 }, { 1, -10, 1 }, 0x0022 },
			{ { 0, 10, 0 }, { 0, 10, 1 }, { 1, 10, 1 }, 0x0011 },
			{ { 0, 10, 0 }, { 1, 10, 1 }, { 1, 10, 0 }, 0x0000 },
			{ { 0, -10, 0 }, { 0, -10, 1 }, { 0, 10, 1 }, 0x0099 },
			{ { 0, -10, 0 }, { 0, 10, 1 }, { 0, 10, 0 }, 0x00AA },
			{ { 0, -10, 1 }, { 1, -10, 1 }, { 1, 10, 1 }, 0x00BB },
			{ { 0, -10, 1 }, { 1, 10, 1 }, { 0, 10, 1 }, 0x00CC }
		});
		*/

		//1x20x1 Pole
		/*
		Mesh pole20 = Mesh(*new std::vector<Triangle> {
			{ { 0, -10, 0 }, { 0, 10, 0 }, { 1, -10, 0 }, 0x0077 },
			{ { 0, 10, 0 }, { 1, 10, 0 }, { 1, -10, 0 }, 0x0077 },
			{ { 0, -10, 0 }, { 1, -10, 0 }, { 0, -10, 1 }, 0x0077 },
			{ { 1, -10, 0 }, { 1, -10, 1 }, { 0, -10, 1 }, 0x0077 },
			{ { 1, -10, 0 }, { 1, 10, 0 }, { 1, -10, 1 }, 0x0077 },
			{ { 1, 10, 0 }, { 1, 10, 1 }, { 1, -10, 1 }, 0x0077 },
			{ { 0, 10, 0 }, { 0, 10, 1 }, { 1, 10, 1 }, 0x0077 },
			{ { 0, 10, 0 }, { 1, 10, 1 }, { 1, 10, 0 }, 0x0077 },
			{ { 0, -10, 0 }, { 0, -10, 1 }, { 0, 10, 1 }, 0x0077 },
			{ { 0, -10, 0 }, { 0, 10, 1 }, { 0, 10, 0 }, 0x0077 },
			{ { 0, -10, 1 }, { 1, -10, 1 }, { 1, 10, 1 }, 0x0077 },
			{ { 0, -10, 1 }, { 1, 10, 1 }, { 0, 10, 1 }, 0x0077 }
		});
		*/

		//20x 1x1x1 Cubes (In shape of 1x20x1 pole)
		/*
		Mesh cube1x20 = Mesh(*new std::vector<Triangle> {
			{ { 0, -10, 0 }, { 0, -9, 0 }, { 1, -10, 0 }, 0x0066 }, { { 0, -9, 0 }, { 1, -9, 0 }, { 1, -10, 0 }, 0x0077 }, { { 0, -10, 0 }, { 1, -10, 0 }, { 0, -10, 1 }, 0x0055 }, { { 1, -10, 0 }, { 1, -10, 1 }, { 0, -10, 1 }, 0x0044 }, { { 1, -10, 0 }, { 1, -9, 0 }, { 1, -10, 1 }, 0x0033 }, { { 1, -9, 0 }, { 1, -9, 1 }, { 1, -10, 1 }, 0x0022 }, { { 0, -9, 0 }, { 0, -9, 1 }, { 1, -9, 1 }, 0x0011 }, { { 0, -9, 0 }, { 1, -9, 1 }, { 1, -9, 0 }, 0x0000 }, { { 0, -10, 0 }, { 0, -10, 1 }, { 0, -9, 1 }, 0x0099 }, { { 0, -10, 0 }, { 0, -9, 1 }, { 0, -9, 0 }, 0x00AA }, { { 0, -10, 1 }, { 1, -10, 1 }, { 1, -9, 1 }, 0x00BB }, { { 0, -10, 1 }, { 1, -9, 1 }, { 0, -9, 1 }, 0x00CC },
			{ { 0, -9, 0 }, { 0, -8, 0 }, { 1, -9, 0 }, 0x0066 }, { { 0, -8, 0 }, { 1, -8, 0 }, { 1, -9, 0 }, 0x0077 }, { { 0, -9, 0 }, { 1, -9, 0 }, { 0, -9, 1 }, 0x0055 }, { { 1, -9, 0 }, { 1, -9, 1 }, { 0, -9, 1 }, 0x0044 }, { { 1, -9, 0 }, { 1, -8, 0 }, { 1, -9, 1 }, 0x0033 }, { { 1, -8, 0 }, { 1, -8, 1 }, { 1, -9, 1 }, 0x0022 }, { { 0, -8, 0 }, { 0, -8, 1 }, { 1, -8, 1 }, 0x0011 }, { { 0, -8, 0 }, { 1, -8, 1 }, { 1, -8, 0 }, 0x0000 }, { { 0, -9, 0 }, { 0, -9, 1 }, { 0, -8, 1 }, 0x0099 }, { { 0, -9, 0 }, { 0, -8, 1 }, { 0, -8, 0 }, 0x00AA }, { { 0, -9, 1 }, { 1, -9, 1 }, { 1, -8, 1 }, 0x00BB }, { { 0, -9, 1 }, { 1, -8, 1 }, { 0, -8, 1 }, 0x00CC },
			{ { 0, -8, 0 }, { 0, -7, 0 }, { 1, -8, 0 }, 0x0066 }, { { 0, -7, 0 }, { 1, -7, 0 }, { 1, -8, 0 }, 0x0077 }, { { 0, -8, 0 }, { 1, -8, 0 }, { 0, -8, 1 }, 0x0055 }, { { 1, -8, 0 }, { 1, -8, 1 }, { 0, -8, 1 }, 0x0044 }, { { 1, -8, 0 }, { 1, -7, 0 }, { 1, -8, 1 }, 0x0033 }, { { 1, -7, 0 }, { 1, -7, 1 }, { 1, -8, 1 }, 0x0022 }, { { 0, -7, 0 }, { 0, -7, 1 }, { 1, -7, 1 }, 0x0011 }, { { 0, -7, 0 }, { 1, -7, 1 }, { 1, -7, 0 }, 0x0000 }, { { 0, -8, 0 }, { 0, -8, 1 }, { 0, -7, 1 }, 0x0099 }, { { 0, -8, 0 }, { 0, -7, 1 }, { 0, -7, 0 }, 0x00AA }, { { 0, -8, 1 }, { 1, -8, 1 }, { 1, -7, 1 }, 0x00BB }, { { 0, -8, 1 }, { 1, -7, 1 }, { 0, -7, 1 }, 0x00CC },
			{ { 0, -7, 0 }, { 0, -6, 0 }, { 1, -7, 0 }, 0x0066 }, { { 0, -6, 0 }, { 1, -6, 0 }, { 1, -7, 0 }, 0x0077 }, { { 0, -7, 0 }, { 1, -7, 0 }, { 0, -7, 1 }, 0x0055 }, { { 1, -7, 0 }, { 1, -7, 1 }, { 0, -7, 1 }, 0x0044 }, { { 1, -7, 0 }, { 1, -6, 0 }, { 1, -7, 1 }, 0x0033 }, { { 1, -6, 0 }, { 1, -6, 1 }, { 1, -7, 1 }, 0x0022 }, { { 0, -6, 0 }, { 0, -6, 1 }, { 1, -6, 1 }, 0x0011 }, { { 0, -6, 0 }, { 1, -6, 1 }, { 1, -6, 0 }, 0x0000 }, { { 0, -7, 0 }, { 0, -7, 1 }, { 0, -6, 1 }, 0x0099 }, { { 0, -7, 0 }, { 0, -6, 1 }, { 0, -6, 0 }, 0x00AA }, { { 0, -7, 1 }, { 1, -7, 1 }, { 1, -6, 1 }, 0x00BB }, { { 0, -7, 1 }, { 1, -6, 1 }, { 0, -6, 1 }, 0x00CC },
			{ { 0, -6, 0 }, { 0, -5, 0 }, { 1, -6, 0 }, 0x0066 }, { { 0, -5, 0 }, { 1, -5, 0 }, { 1, -6, 0 }, 0x0077 }, { { 0, -6, 0 }, { 1, -6, 0 }, { 0, -6, 1 }, 0x0055 }, { { 1, -6, 0 }, { 1, -6, 1 }, { 0, -6, 1 }, 0x0044 }, { { 1, -6, 0 }, { 1, -5, 0 }, { 1, -6, 1 }, 0x0033 }, { { 1, -5, 0 }, { 1, -5, 1 }, { 1, -6, 1 }, 0x0022 }, { { 0, -5, 0 }, { 0, -5, 1 }, { 1, -5, 1 }, 0x0011 }, { { 0, -5, 0 }, { 1, -5, 1 }, { 1, -5, 0 }, 0x0000 }, { { 0, -6, 0 }, { 0, -6, 1 }, { 0, -5, 1 }, 0x0099 }, { { 0, -6, 0 }, { 0, -5, 1 }, { 0, -5, 0 }, 0x00AA }, { { 0, -6, 1 }, { 1, -6, 1 }, { 1, -5, 1 }, 0x00BB }, { { 0, -6, 1 }, { 1, -5, 1 }, { 0, -5, 1 }, 0x00CC },
			{ { 0, -5, 0 }, { 0, -4, 0 }, { 1, -5, 0 }, 0x0066 }, { { 0, -4, 0 }, { 1, -4, 0 }, { 1, -5, 0 }, 0x0077 }, { { 0, -5, 0 }, { 1, -5, 0 }, { 0, -5, 1 }, 0x0055 }, { { 1, -5, 0 }, { 1, -5, 1 }, { 0, -5, 1 }, 0x0044 }, { { 1, -5, 0 }, { 1, -4, 0 }, { 1, -5, 1 }, 0x0033 }, { { 1, -4, 0 }, { 1, -4, 1 }, { 1, -5, 1 }, 0x0022 }, { { 0, -4, 0 }, { 0, -4, 1 }, { 1, -4, 1 }, 0x0011 }, { { 0, -4, 0 }, { 1, -4, 1 }, { 1, -4, 0 }, 0x0000 }, { { 0, -5, 0 }, { 0, -5, 1 }, { 0, -4, 1 }, 0x0099 }, { { 0, -5, 0 }, { 0, -4, 1 }, { 0, -4, 0 }, 0x00AA }, { { 0, -5, 1 }, { 1, -5, 1 }, { 1, -4, 1 }, 0x00BB }, { { 0, -5, 1 }, { 1, -4, 1 }, { 0, -4, 1 }, 0x00CC },
			{ { 0, -4, 0 }, { 0, -3, 0 }, { 1, -4, 0 }, 0x0066 }, { { 0, -3, 0 }, { 1, -3, 0 }, { 1, -4, 0 }, 0x0077 }, { { 0, -4, 0 }, { 1, -4, 0 }, { 0, -4, 1 }, 0x0055 }, { { 1, -4, 0 }, { 1, -4, 1 }, { 0, -4, 1 }, 0x0044 }, { { 1, -4, 0 }, { 1, -3, 0 }, { 1, -4, 1 }, 0x0033 }, { { 1, -3, 0 }, { 1, -3, 1 }, { 1, -4, 1 }, 0x0022 }, { { 0, -3, 0 }, { 0, -3, 1 }, { 1, -3, 1 }, 0x0011 }, { { 0, -3, 0 }, { 1, -3, 1 }, { 1, -3, 0 }, 0x0000 }, { { 0, -4, 0 }, { 0, -4, 1 }, { 0, -3, 1 }, 0x0099 }, { { 0, -4, 0 }, { 0, -3, 1 }, { 0, -3, 0 }, 0x00AA }, { { 0, -4, 1 }, { 1, -4, 1 }, { 1, -3, 1 }, 0x00BB }, { { 0, -4, 1 }, { 1, -3, 1 }, { 0, -3, 1 }, 0x00CC },
			{ { 0, -3, 0 }, { 0, -2, 0 }, { 1, -3, 0 }, 0x0066 }, { { 0, -2, 0 }, { 1, -2, 0 }, { 1, -3, 0 }, 0x0077 }, { { 0, -3, 0 }, { 1, -3, 0 }, { 0, -3, 1 }, 0x0055 }, { { 1, -3, 0 }, { 1, -3, 1 }, { 0, -3, 1 }, 0x0044 }, { { 1, -3, 0 }, { 1, -2, 0 }, { 1, -3, 1 }, 0x0033 }, { { 1, -2, 0 }, { 1, -2, 1 }, { 1, -3, 1 }, 0x0022 }, { { 0, -2, 0 }, { 0, -2, 1 }, { 1, -2, 1 }, 0x0011 }, { { 0, -2, 0 }, { 1, -2, 1 }, { 1, -2, 0 }, 0x0000 }, { { 0, -3, 0 }, { 0, -3, 1 }, { 0, -2, 1 }, 0x0099 }, { { 0, -3, 0 }, { 0, -2, 1 }, { 0, -2, 0 }, 0x00AA }, { { 0, -3, 1 }, { 1, -3, 1 }, { 1, -2, 1 }, 0x00BB }, { { 0, -3, 1 }, { 1, -2, 1 }, { 0, -2, 1 }, 0x00CC },
			{ { 0, -2, 0 }, { 0, -1, 0 }, { 1, -2, 0 }, 0x0066 }, { { 0, -1, 0 }, { 1, -1, 0 }, { 1, -2, 0 }, 0x0077 }, { { 0, -2, 0 }, { 1, -2, 0 }, { 0, -2, 1 }, 0x0055 }, { { 1, -2, 0 }, { 1, -2, 1 }, { 0, -2, 1 }, 0x0044 }, { { 1, -2, 0 }, { 1, -1, 0 }, { 1, -2, 1 }, 0x0033 }, { { 1, -1, 0 }, { 1, -1, 1 }, { 1, -2, 1 }, 0x0022 }, { { 0, -1, 0 }, { 0, -1, 1 }, { 1, -1, 1 }, 0x0011 }, { { 0, -1, 0 }, { 1, -1, 1 }, { 1, -1, 0 }, 0x0000 }, { { 0, -2, 0 }, { 0, -2, 1 }, { 0, -1, 1 }, 0x0099 }, { { 0, -2, 0 }, { 0, -1, 1 }, { 0, -1, 0 }, 0x00AA }, { { 0, -2, 1 }, { 1, -2, 1 }, { 1, -1, 1 }, 0x00BB }, { { 0, -2, 1 }, { 1, -1, 1 }, { 0, -1, 1 }, 0x00CC },
			{ { 0, -1, 0 }, { 0, 0, 0 }, { 1, -1, 0 }, 0x0066 }, { { 0, 0, 0 }, { 1, 0, 0 }, { 1, -1, 0 }, 0x0077 }, { { 0, -1, 0 }, { 1, -1, 0 }, { 0, -1, 1 }, 0x0055 }, { { 1, -1, 0 }, { 1, -1, 1 }, { 0, -1, 1 }, 0x0044 }, { { 1, -1, 0 }, { 1, 0, 0 }, { 1, -1, 1 }, 0x0033 }, { { 1, 0, 0 }, { 1, 0, 1 }, { 1, -1, 1 }, 0x0022 }, { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, 0x0011 }, { { 0, 0, 0 }, { 1, 0, 1 }, { 1, 0, 0 }, 0x0000 }, { { 0, -1, 0 }, { 0, -1, 1 }, { 0, 0, 1 }, 0x0099 }, { { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, 0 }, 0x00AA }, { { 0, -1, 1 }, { 1, -1, 1 }, { 1, 0, 1 }, 0x00BB }, { { 0, -1, 1 }, { 1, 0, 1 }, { 0, 0, 1 }, 0x00CC },
			{ { 0, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 }, 0x0066 }, { { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, 0x0077 }, { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, 0x0055 }, { { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, 0x0044 }, { { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 1 }, 0x0033 }, { { 1, 1, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, 0x0022 }, { { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 }, 0x0011 }, { { 0, 1, 0 }, { 1, 1, 1 }, { 1, 1, 0 }, 0x0000 }, { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, 0x0099 }, { { 0, 0, 0 }, { 0, 1, 1 }, { 0, 1, 0 }, 0x00AA }, { { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, 0x00BB }, { { 0, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 }, 0x00CC },
			{ { 0, 1, 0 }, { 0, 2, 0 }, { 1, 1, 0 }, 0x0066 }, { { 0, 2, 0 }, { 1, 2, 0 }, { 1, 1, 0 }, 0x0077 }, { { 0, 1, 0 }, { 1, 1, 0 }, { 0, 1, 1 }, 0x0055 }, { { 1, 1, 0 }, { 1, 1, 1 }, { 0, 1, 1 }, 0x0044 }, { { 1, 1, 0 }, { 1, 2, 0 }, { 1, 1, 1 }, 0x0033 }, { { 1, 2, 0 }, { 1, 2, 1 }, { 1, 1, 1 }, 0x0022 }, { { 0, 2, 0 }, { 0, 2, 1 }, { 1, 2, 1 }, 0x0011 }, { { 0, 2, 0 }, { 1, 2, 1 }, { 1, 2, 0 }, 0x0000 }, { { 0, 1, 0 }, { 0, 1, 1 }, { 0, 2, 1 }, 0x0099 }, { { 0, 1, 0 }, { 0, 2, 1 }, { 0, 2, 0 }, 0x00AA }, { { 0, 1, 1 }, { 1, 1, 1 }, { 1, 2, 1 }, 0x00BB }, { { 0, 1, 1 }, { 1, 2, 1 }, { 0, 2, 1 }, 0x00CC },
			{ { 0, 2, 0 }, { 0, 3, 0 }, { 1, 2, 0 }, 0x0066 }, { { 0, 3, 0 }, { 1, 3, 0 }, { 1, 2, 0 }, 0x0077 }, { { 0, 2, 0 }, { 1, 2, 0 }, { 0, 2, 1 }, 0x0055 }, { { 1, 2, 0 }, { 1, 2, 1 }, { 0, 2, 1 }, 0x0044 }, { { 1, 2, 0 }, { 1, 3, 0 }, { 1, 2, 1 }, 0x0033 }, { { 1, 3, 0 }, { 1, 3, 1 }, { 1, 2, 1 }, 0x0022 }, { { 0, 3, 0 }, { 0, 3, 1 }, { 1, 3, 1 }, 0x0011 }, { { 0, 3, 0 }, { 1, 3, 1 }, { 1, 3, 0 }, 0x0000 }, { { 0, 2, 0 }, { 0, 2, 1 }, { 0, 3, 1 }, 0x0099 }, { { 0, 2, 0 }, { 0, 3, 1 }, { 0, 3, 0 }, 0x00AA }, { { 0, 2, 1 }, { 1, 2, 1 }, { 1, 3, 1 }, 0x00BB }, { { 0, 2, 1 }, { 1, 3, 1 }, { 0, 3, 1 }, 0x00CC },
			{ { 0, 3, 0 }, { 0, 4, 0 }, { 1, 3, 0 }, 0x0066 }, { { 0, 4, 0 }, { 1, 4, 0 }, { 1, 3, 0 }, 0x0077 }, { { 0, 3, 0 }, { 1, 3, 0 }, { 0, 3, 1 }, 0x0055 }, { { 1, 3, 0 }, { 1, 3, 1 }, { 0, 3, 1 }, 0x0044 }, { { 1, 3, 0 }, { 1, 4, 0 }, { 1, 3, 1 }, 0x0033 }, { { 1, 4, 0 }, { 1, 4, 1 }, { 1, 3, 1 }, 0x0022 }, { { 0, 4, 0 }, { 0, 4, 1 }, { 1, 4, 1 }, 0x0011 }, { { 0, 4, 0 }, { 1, 4, 1 }, { 1, 4, 0 }, 0x0000 }, { { 0, 3, 0 }, { 0, 3, 1 }, { 0, 4, 1 }, 0x0099 }, { { 0, 3, 0 }, { 0, 4, 1 }, { 0, 4, 0 }, 0x00AA }, { { 0, 3, 1 }, { 1, 3, 1 }, { 1, 4, 1 }, 0x00BB }, { { 0, 3, 1 }, { 1, 4, 1 }, { 0, 4, 1 }, 0x00CC },
			{ { 0, 4, 0 }, { 0, 5, 0 }, { 1, 4, 0 }, 0x0066 }, { { 0, 5, 0 }, { 1, 5, 0 }, { 1, 4, 0 }, 0x0077 }, { { 0, 4, 0 }, { 1, 4, 0 }, { 0, 4, 1 }, 0x0055 }, { { 1, 4, 0 }, { 1, 4, 1 }, { 0, 4, 1 }, 0x0044 }, { { 1, 4, 0 }, { 1, 5, 0 }, { 1, 4, 1 }, 0x0033 }, { { 1, 5, 0 }, { 1, 5, 1 }, { 1, 4, 1 }, 0x0022 }, { { 0, 5, 0 }, { 0, 5, 1 }, { 1, 5, 1 }, 0x0011 }, { { 0, 5, 0 }, { 1, 5, 1 }, { 1, 5, 0 }, 0x0000 }, { { 0, 4, 0 }, { 0, 4, 1 }, { 0, 5, 1 }, 0x0099 }, { { 0, 4, 0 }, { 0, 5, 1 }, { 0, 5, 0 }, 0x00AA }, { { 0, 4, 1 }, { 1, 4, 1 }, { 1, 5, 1 }, 0x00BB }, { { 0, 4, 1 }, { 1, 5, 1 }, { 0, 5, 1 }, 0x00CC },
			{ { 0, 5, 0 }, { 0, 6, 0 }, { 1, 5, 0 }, 0x0066 }, { { 0, 6, 0 }, { 1, 6, 0 }, { 1, 5, 0 }, 0x0077 }, { { 0, 5, 0 }, { 1, 5, 0 }, { 0, 5, 1 }, 0x0055 }, { { 1, 5, 0 }, { 1, 5, 1 }, { 0, 5, 1 }, 0x0044 }, { { 1, 5, 0 }, { 1, 6, 0 }, { 1, 5, 1 }, 0x0033 }, { { 1, 6, 0 }, { 1, 6, 1 }, { 1, 5, 1 }, 0x0022 }, { { 0, 6, 0 }, { 0, 6, 1 }, { 1, 6, 1 }, 0x0011 }, { { 0, 6, 0 }, { 1, 6, 1 }, { 1, 6, 0 }, 0x0000 }, { { 0, 5, 0 }, { 0, 5, 1 }, { 0, 6, 1 }, 0x0099 }, { { 0, 5, 0 }, { 0, 6, 1 }, { 0, 6, 0 }, 0x00AA }, { { 0, 5, 1 }, { 1, 5, 1 }, { 1, 6, 1 }, 0x00BB }, { { 0, 5, 1 }, { 1, 6, 1 }, { 0, 6, 1 }, 0x00CC },
			{ { 0, 6, 0 }, { 0, 7, 0 }, { 1, 6, 0 }, 0x0066 }, { { 0, 7, 0 }, { 1, 7, 0 }, { 1, 6, 0 }, 0x0077 }, { { 0, 6, 0 }, { 1, 6, 0 }, { 0, 6, 1 }, 0x0055 }, { { 1, 6, 0 }, { 1, 6, 1 }, { 0, 6, 1 }, 0x0044 }, { { 1, 6, 0 }, { 1, 7, 0 }, { 1, 6, 1 }, 0x0033 }, { { 1, 7, 0 }, { 1, 7, 1 }, { 1, 6, 1 }, 0x0022 }, { { 0, 7, 0 }, { 0, 7, 1 }, { 1, 7, 1 }, 0x0011 }, { { 0, 7, 0 }, { 1, 7, 1 }, { 1, 7, 0 }, 0x0000 }, { { 0, 6, 0 }, { 0, 6, 1 }, { 0, 7, 1 }, 0x0099 }, { { 0, 6, 0 }, { 0, 7, 1 }, { 0, 7, 0 }, 0x00AA }, { { 0, 6, 1 }, { 1, 6, 1 }, { 1, 7, 1 }, 0x00BB }, { { 0, 6, 1 }, { 1, 7, 1 }, { 0, 7, 1 }, 0x00CC },
			{ { 0, 7, 0 }, { 0, 8, 0 }, { 1, 7, 0 }, 0x0066 }, { { 0, 8, 0 }, { 1, 8, 0 }, { 1, 7, 0 }, 0x0077 }, { { 0, 7, 0 }, { 1, 7, 0 }, { 0, 7, 1 }, 0x0055 }, { { 1, 7, 0 }, { 1, 7, 1 }, { 0, 7, 1 }, 0x0044 }, { { 1, 7, 0 }, { 1, 8, 0 }, { 1, 7, 1 }, 0x0033 }, { { 1, 8, 0 }, { 1, 8, 1 }, { 1, 7, 1 }, 0x0022 }, { { 0, 8, 0 }, { 0, 8, 1 }, { 1, 8, 1 }, 0x0011 }, { { 0, 8, 0 }, { 1, 8, 1 }, { 1, 8, 0 }, 0x0000 }, { { 0, 7, 0 }, { 0, 7, 1 }, { 0, 8, 1 }, 0x0099 }, { { 0, 7, 0 }, { 0, 8, 1 }, { 0, 8, 0 }, 0x00AA }, { { 0, 7, 1 }, { 1, 7, 1 }, { 1, 8, 1 }, 0x00BB }, { { 0, 7, 1 }, { 1, 8, 1 }, { 0, 8, 1 }, 0x00CC },
			{ { 0, 8, 0 }, { 0, 9, 0 }, { 1, 8, 0 }, 0x0066 }, { { 0, 9, 0 }, { 1, 9, 0 }, { 1, 8, 0 }, 0x0077 }, { { 0, 8, 0 }, { 1, 8, 0 }, { 0, 8, 1 }, 0x0055 }, { { 1, 8, 0 }, { 1, 8, 1 }, { 0, 8, 1 }, 0x0044 }, { { 1, 8, 0 }, { 1, 9, 0 }, { 1, 8, 1 }, 0x0033 }, { { 1, 9, 0 }, { 1, 9, 1 }, { 1, 8, 1 }, 0x0022 }, { { 0, 9, 0 }, { 0, 9, 1 }, { 1, 9, 1 }, 0x0011 }, { { 0, 9, 0 }, { 1, 9, 1 }, { 1, 9, 0 }, 0x0000 }, { { 0, 8, 0 }, { 0, 8, 1 }, { 0, 9, 1 }, 0x0099 }, { { 0, 8, 0 }, { 0, 9, 1 }, { 0, 9, 0 }, 0x00AA }, { { 0, 8, 1 }, { 1, 8, 1 }, { 1, 9, 1 }, 0x00BB }, { { 0, 8, 1 }, { 1, 9, 1 }, { 0, 9, 1 }, 0x00CC },
			{ { 0, 9, 0 }, { 0, 10, 0 }, { 1, 9, 0 }, 0x0066 }, { { 0, 10, 0 }, { 1, 10, 0 }, { 1, 9, 0 }, 0x0077 }, { { 0, 9, 0 }, { 1, 9, 0 }, { 0, 9, 1 }, 0x0055 }, { { 1, 9, 0 }, { 1, 9, 1 }, { 0, 9, 1 }, 0x0044 }, { { 1, 9, 0 }, { 1, 10, 0 }, { 1, 9, 1 }, 0x0033 }, { { 1, 10, 0 }, { 1, 10, 1 }, { 1, 9, 1 }, 0x0022 }, { { 0, 10, 0 }, { 0, 10, 1 }, { 1, 10, 1 }, 0x0011 }, { { 0, 10, 0 }, { 1, 10, 1 }, { 1, 10, 0 }, 0x0000 }, { { 0, 9, 0 }, { 0, 9, 1 }, { 0, 10, 1 }, 0x0099 }, { { 0, 9, 0 }, { 0, 10, 1 }, { 0, 10, 0 }, 0x00AA }, { { 0, 9, 1 }, { 1, 9, 1 }, { 1, 10, 1 }, 0x00BB }, { { 0, 9, 1 }, { 1, 10, 1 }, { 0, 10, 1 }, 0x00CC }
		});
		*/

		//2x 1x10x1 Pole
		/*
		Mesh pole10x2 = Mesh(*new std::vector<Triangle> {
			{ { 0, 0, 0 }, { 0, 10, 0 }, { 1, 0, 0 }, 0x0077 },
			{ { 0, 10, 0 }, { 1, 10, 0 }, { 1, 0, 0 }, 0x0077 },
			{ { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, 0x0077 },
			{ { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, 0x0077 },
			{ { 1, 0, 0 }, { 1, 10, 0 }, { 1, 0, 1 }, 0x0077 },
			{ { 1, 10, 0 }, { 1, 10, 1 }, { 1, 0, 1 }, 0x0077 },
			{ { 0, 10, 0 }, { 0, 10, 1 }, { 1, 10, 1 }, 0x0077 },
			{ { 0, 10, 0 }, { 1, 10, 1 }, { 1, 10, 0 }, 0x0077 },
			{ { 0, 0, 0 }, { 0, 0, 1 }, { 0, 10, 1 }, 0x0077 },
			{ { 0, 0, 0 }, { 0, 10, 1 }, { 0, 10, 0 }, 0x0077 },
			{ { 0, 0, 1 }, { 1, 0, 1 }, { 1, 10, 1 }, 0x0077 },
			{ { 0, 0, 1 }, { 1, 10, 1 }, { 0, 10, 1 }, 0x0077 },

			{ { 0, -10, 0 }, { 0, 0, 0 }, { 1, -10, 0 }, 0x0066 },
			{ { 0, 0, 0 }, { 1, 0, 0 }, { 1, -10, 0 }, 0x0066 },
			{ { 0, -10, 0 }, { 1, -10, 0 }, { 0, -10, 1 }, 0x0066 },
			{ { 1, -10, 0 }, { 1, -10, 1 }, { 0, -10, 1 }, 0x0066 },
			{ { 1, -10, 0 }, { 1, 0, 0 }, { 1, -10, 1 }, 0x0066 },
			{ { 1, 0, 0 }, { 1, 0, 1 }, { 1, -10, 1 }, 0x0066 },
			{ { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, 0x0066 },
			{ { 0, 0, 0 }, { 1, 0, 1 }, { 1, 0, 0 }, 0x0066 },
			{ { 0, -10, 0 }, { 0, -10, 1 }, { 0, 0, 1 }, 0x0066 },
			{ { 0, -10, 0 }, { 0, 0, 1 }, { 0, 0, 0 }, 0x0066 },
			{ { 0, -10, 1 }, { 1, -10, 1 }, { 1, 0, 1 }, 0x0066 },
			{ { 0, -10, 1 }, { 1, 0, 1 }, { 0, 0, 1 }, 0x0066 }
		});
		*/

		//Donut
		/*
		Mesh donut = Mesh(*new std::vector<Triangle> {
			{ { 5.2, -1.0, 0 }, { 5.8, -1.0, 0 }, { 5.3, -1.8, 0 }, 0x0055 },
			{ { 5.8, -1.0, 0 }, { 5.7, -1.8, 0 }, { 5.3, -1.8, 0 }, 0x0055 },
			{ { 5.8, -1.0, 0 }, { 6.2, -1.2, 0 }, { 5.7, -1.8, 0 }, 0x0055 },
			{ { 6.2, -1.2, 0 }, { 6.0, -2.0, 0 }, { 5.7, -1.8, 0 }, 0x0055 },
			{ { 6.2, -1.2, 0 }, { 6.5, -1.4, 0 }, { 6.0, -2.0, 0 }, 0x0055 },
			{ { 6.5, -1.4, 0 }, { 6.8, -1.8, 0 }, { 6.0, -2.0, 0 }, 0x0055 },
			{ { 6.8, -1.8, 0 }, { 6.2, -2.4, 0 }, { 6.0, -2.0, 0 }, 0x0055 },
			{ { 6.8, -1.8, 0 }, { 7.0, -2.6, 0 }, { 6.2, -2.4, 0 }, 0x0055 },
			{ { 7.0, -2.6, 0 }, { 6.2, -3.6, 0 }, { 6.2, -2.4, 0 }, 0x0055 },
			{ { 7.0, -2.6, 0 }, { 7.0, -3.4, 0 }, { 6.2, -3.6, 0 }, 0x0055 },
			{ { 7.0, -3.4, 0 }, { 6.8, -4.2, 0 }, { 6.2, -3.6, 0 }, 0x0055 },
			{ { 6.8, -4.2, 0 }, { 6.5, -4.6, 0 }, { 6.2, -3.6, 0 }, 0x0055 },
			{ { 6.5, -4.6, 0 }, { 6.0, -4.0, 0 }, { 6.2, -3.6, 0 }, 0x0055 },
			{ { 6.5, -4.6, 0 }, { 6.2, -4.8, 0 }, { 6.0, -4.0, 0 }, 0x0055 },
			{ { 6.2, -4.8, 0 }, { 5.7, -4.2, 0 }, { 6.0, -4.0, 0 }, 0x0055 },
			{ { 6.2, -4.8, 0 }, { 5.8, -5.0, 0 }, { 5.7, -4.2, 0 }, 0x0055 },
			{ { 5.8, -5.0, 0 }, { 5.2, -5.0, 0 }, { 5.7, -4.2, 0 }, 0x0055 },
			{ { 5.2, -5.0, 0 }, { 5.3, -4.2, 0 }, { 5.7, -4.2, 0 }, 0x0055 },
			{ { 5.2, -5.0, 0 }, { 4.8, -4.8, 0 }, { 5.3, -4.2, 0 }, 0x0055 },
			{ { 4.8, -4.8, 0 }, { 5.0, -4.0, 0 }, { 5.3, -4.2, 0 }, 0x0055 },
			{ { 4.8, -4.8, 0 }, { 4.5, -4.6, 0 }, { 5.0, -4.0, 0 }, 0x0055 },
			{ { 4.5, -4.6, 0 }, { 4.2, -4.2, 0 }, { 5.0, -4.0, 0 }, 0x0055 },
			{ { 4.2, -4.2, 0 }, { 4.8, -3.6, 0 }, { 5.0, -4.0, 0 }, 0x0055 },
			{ { 4.2, -4.2, 0 }, { 4.0, -3.4, 0 }, { 4.8, -3.6, 0 }, 0x0055 },
			{ { 4.0, -3.4, 0 }, { 4.8, -2.4, 0 }, { 4.8, -3.6, 0 }, 0x0055 },
			{ { 4.0, -3.4, 0 }, { 4.0, -2.6, 0 }, { 4.8, -2.4, 0 }, 0x0055 },
			{ { 4.0, -2.6, 0 }, { 4.2, -1.8, 0 }, { 4.8, -2.4, 0 }, 0x0055 },
			{ { 4.2, -1.8, 0 }, { 5.0, -2.0, 0 }, { 4.8, -2.4, 0 }, 0x0055 },
			{ { 4.2, -1.8, 0 }, { 4.5, -1.4, 0 }, { 5.0, -2.0, 0 }, 0x0055 },
			{ { 4.5, -1.4, 0 }, { 4.8, -1.2, 0 }, { 5.0, -2.0, 0 }, 0x0055 },
			{ { 4.8, -1.2, 0 }, { 5.3, -1.8, 0 }, { 5.0, -2.0, 0 }, 0x0055 },
			{ { 4.8, -1.2, 0 }, { 5.2, -1.0, 0 }, { 5.3, -1.8, 0 }, 0x0055 },
			{ { 5.3, -1.8, 1 }, { 5.8, -1.0, 1 }, { 5.2, -1.0, 1 }, 0x0055 },
			{ { 5.3, -1.8, 1 }, { 5.7, -1.8, 1 }, { 5.8, -1.0, 1 }, 0x0055 },
			{ { 5.7, -1.8, 1 }, { 6.2, -1.2, 1 }, { 5.8, -1.0, 1 }, 0x0055 },
			{ { 5.7, -1.8, 1 }, { 6.0, -2.0, 1 }, { 6.2, -1.2, 1 }, 0x0055 },
			{ { 6.0, -2.0, 1 }, { 6.5, -1.4, 1 }, { 6.2, -1.2, 1 }, 0x0055 },
			{ { 6.0, -2.0, 1 }, { 6.8, -1.8, 1 }, { 6.5, -1.4, 1 }, 0x0055 },
			{ { 6.0, -2.0, 1 }, { 6.2, -2.4, 1 }, { 6.8, -1.8, 1 }, 0x0055 },
			{ { 6.2, -2.4, 1 }, { 7.0, -2.6, 1 }, { 6.8, -1.8, 1 }, 0x0055 },
			{ { 6.2, -2.4, 1 }, { 6.2, -3.6, 1 }, { 7.0, -2.6, 1 }, 0x0055 },
			{ { 6.2, -3.6, 1 }, { 7.0, -3.4, 1 }, { 7.0, -2.6, 1 }, 0x0055 },
			{ { 6.2, -3.6, 1 }, { 6.8, -4.2, 1 }, { 7.0, -3.4, 1 }, 0x0055 },
			{ { 6.2, -3.6, 1 }, { 6.5, -4.6, 1 }, { 6.8, -4.2, 1 }, 0x0055 },
			{ { 6.2, -3.6, 1 }, { 6.0, -4.0, 1 }, { 6.5, -4.6, 1 }, 0x0055 },
			{ { 6.0, -4.0, 1 }, { 6.2, -4.8, 1 }, { 6.5, -4.6, 1 }, 0x0055 },
			{ { 6.0, -4.0, 1 }, { 5.7, -4.2, 1 }, { 6.2, -4.8, 1 }, 0x0055 },
			{ { 5.7, -4.2, 1 }, { 5.8, -5.0, 1 }, { 6.2, -4.8, 1 }, 0x0055 },
			{ { 5.7, -4.2, 1 }, { 5.2, -5.0, 1 }, { 5.8, -5.0, 1 }, 0x0055 },
			{ { 5.7, -4.2, 1 }, { 5.3, -4.2, 1 }, { 5.2, -5.0, 1 }, 0x0055 },
			{ { 5.3, -4.2, 1 }, { 4.8, -4.8, 1 }, { 5.2, -5.0, 1 }, 0x0055 },
			{ { 5.3, -4.2, 1 }, { 5.0, -4.0, 1 }, { 4.8, -4.8, 1 }, 0x0055 },
			{ { 5.0, -4.0, 1 }, { 4.5, -4.6, 1 }, { 4.8, -4.8, 1 }, 0x0055 },
			{ { 5.0, -4.0, 1 }, { 4.2, -4.2, 1 }, { 4.5, -4.6, 1 }, 0x0055 },
			{ { 5.0, -4.0, 1 }, { 4.8, -3.6, 1 }, { 4.2, -4.2, 1 }, 0x0055 },
			{ { 4.8, -3.6, 1 }, { 4.0, -3.4, 1 }, { 4.2, -4.2, 1 }, 0x0055 },
			{ { 4.8, -3.6, 1 }, { 4.8, -2.4, 1 }, { 4.0, -3.4, 1 }, 0x0055 },
			{ { 4.8, -2.4, 1 }, { 4.0, -2.6, 1 }, { 4.0, -3.4, 1 }, 0x0055 },
			{ { 4.8, -2.4, 1 }, { 4.2, -1.8, 1 }, { 4.0, -2.6, 1 }, 0x0055 },
			{ { 4.8, -2.4, 1 }, { 5.0, -2.0, 1 }, { 4.2, -1.8, 1 }, 0x0055 },
			{ { 5.0, -2.0, 1 }, { 4.5, -1.4, 1 }, { 4.2, -1.8, 1 }, 0x0055 },
			{ { 5.0, -2.0, 1 }, { 4.8, -1.2, 1 }, { 4.5, -1.4, 1 }, 0x0055 },
			{ { 5.0, -2.0, 1 }, { 5.3, -1.8, 1 }, { 4.8, -1.2, 1 }, 0x0055 },
			{ { 5.3, -1.8, 1 }, { 5.2, -1.0, 1 }, { 4.8, -1.2, 1 }, 0x0055 },
			{ { 4.0, -2.6, 1 }, { 4.2, -1.8, 1 }, { 4.2, -1.8, 0 }, 0x0055 },
			{ { 4.0, -2.6, 0 }, { 4.0, -2.6, 1 }, { 4.2, -1.8, 0 }, 0x0055 },
			{ { 4.2, -1.8, 1 }, { 4.5, -1.4, 1 }, { 4.5, -1.4, 0 }, 0x0055 },
			{ { 4.2, -1.8, 0 }, { 4.2, -1.8, 1 }, { 4.5, -1.4, 0 }, 0x0055 },
			{ { 4.5, -1.4, 1 }, { 4.8, -1.2, 1 }, { 4.8, -1.2, 0 }, 0x0055 },
			{ { 4.5, -1.4, 0 }, { 4.5, -1.4, 1 }, { 4.8, -1.2, 0 }, 0x0055 },
			{ { 4.8, -1.2, 1 }, { 5.2, -1.0, 1 }, { 5.2, -1.0, 0 }, 0x0055 },
			{ { 4.8, -1.2, 0 }, { 4.8, -1.2, 1 }, { 5.2, -1.0, 0 }, 0x0055 },
			{ { 5.2, -1.0, 1 }, { 5.8, -1.0, 1 }, { 5.8, -1.0, 0 }, 0x0055 },
			{ { 5.2, -1.0, 0 }, { 5.2, -1.0, 1 }, { 5.8, -1.0, 0 }, 0x0055 },
			{ { 5.8, -1.0, 1 }, { 6.2, -1.2, 1 }, { 6.2, -1.2, 0 }, 0x0055 },
			{ { 5.8, -1.0, 0 }, { 5.8, -1.0, 1 }, { 6.2, -1.2, 0 }, 0x0055 },
			{ { 6.2, -1.2, 1 }, { 6.5, -1.4, 1 }, { 6.5, -1.4, 0 }, 0x0055 },
			{ { 6.2, -1.2, 0 }, { 6.2, -1.2, 1 }, { 6.5, -1.4, 0 }, 0x0055 },
			{ { 6.5, -1.4, 1 }, { 6.8, -1.8, 1 }, { 6.8, -1.8, 0 }, 0x0055 },
			{ { 6.5, -1.4, 0 }, { 6.5, -1.4, 1 }, { 6.8, -1.8, 0 }, 0x0055 },
			{ { 6.8, -1.8, 1 }, { 7.0, -2.6, 1 }, { 7.0, -2.6, 0 }, 0x0055 },
			{ { 6.8, -1.8, 0 }, { 6.8, -1.8, 1 }, { 7.0, -2.6, 0 }, 0x0055 },
			{ { 7.0, -2.6, 1 }, { 7.0, -3.4, 1 }, { 7.0, -3.4, 0 }, 0x0055 },
			{ { 7.0, -2.6, 0 }, { 7.0, -2.6, 1 }, { 7.0, -3.4, 0 }, 0x0055 },
			{ { 7.0, -3.4, 1 }, { 6.8, -4.2, 1 }, { 6.8, -4.2, 0 }, 0x0055 },
			{ { 7.0, -3.4, 0 }, { 7.0, -3.4, 1 }, { 6.8, -4.2, 0 }, 0x0055 },
			{ { 6.8, -4.2, 1 }, { 6.5, -4.6, 1 }, { 6.5, -4.6, 0 }, 0x0055 },
			{ { 6.8, -4.2, 0 }, { 6.8, -4.2, 1 }, { 6.5, -4.6, 0 }, 0x0055 },
			{ { 6.5, -4.6, 1 }, { 6.2, -4.8, 1 }, { 6.2, -4.8, 0 }, 0x0055 },
			{ { 6.5, -4.6, 0 }, { 6.5, -4.6, 1 }, { 6.2, -4.8, 0 }, 0x0055 },
			{ { 6.2, -4.8, 1 }, { 5.8, -5.0, 1 }, { 5.8, -5.0, 0 }, 0x0055 },
			{ { 6.2, -4.8, 0 }, { 6.2, -4.8, 1 }, { 5.8, -5.0, 0 }, 0x0055 },
			{ { 5.8, -5.0, 1 }, { 5.2, -5.0, 1 }, { 5.2, -5.0, 0 }, 0x0055 },
			{ { 5.8, -5.0, 0 }, { 5.8, -5.0, 1 }, { 5.2, -5.0, 0 }, 0x0055 },
			{ { 5.2, -5.0, 1 }, { 4.8, -4.8, 1 }, { 4.8, -4.8, 0 }, 0x0055 },
			{ { 5.2, -5.0, 0 }, { 5.2, -5.0, 1 }, { 4.8, -4.8, 0 }, 0x0055 },
			{ { 4.8, -4.8, 1 }, { 4.5, -4.6, 1 }, { 4.5, -4.6, 0 }, 0x0055 },
			{ { 4.8, -4.8, 0 }, { 4.8, -4.8, 1 }, { 4.5, -4.6, 0 }, 0x0055 },
			{ { 4.5, -4.6, 1 }, { 4.2, -4.2, 1 }, { 4.2, -4.2, 0 }, 0x0055 },
			{ { 4.5, -4.6, 0 }, { 4.5, -4.6, 1 }, { 4.2, -4.2, 0 }, 0x0055 },
			{ { 4.2, -4.2, 1 }, { 4.0, -3.4, 1 }, { 4.0, -3.4, 0 }, 0x0055 },
			{ { 4.2, -4.2, 0 }, { 4.2, -4.2, 1 }, { 4.0, -3.4, 0 }, 0x0055 },
			{ { 4.0, -3.4, 1 }, { 4.0, -2.6, 1 }, { 4.0, -2.6, 0 }, 0x0055 },
			{ { 4.0, -3.4, 0 }, { 4.0, -3.4, 1 }, { 4.0, -2.6, 0 }, 0x0055 },
			{ { 5.3, -1.8, 0 }, { 5.7, -1.8, 0 }, { 5.7, -1.8, 1 }, 0x0055 },
			{ { 5.3, -1.8, 1 }, { 5.3, -1.8, 0 }, { 5.7, -1.8, 1 }, 0x0055 },
			{ { 5.7, -1.8, 0 }, { 6.0, -2.0, 0 }, { 6.0, -2.0, 1 }, 0x0055 },
			{ { 5.7, -1.8, 1 }, { 5.7, -1.8, 0 }, { 6.0, -2.0, 1 }, 0x0055 },
			{ { 6.0, -2.0, 0 }, { 6.2, -2.4, 0 }, { 6.2, -2.4, 1 }, 0x0055 },
			{ { 6.0, -2.0, 1 }, { 6.0, -2.0, 0 }, { 6.2, -2.4, 1 }, 0x0055 },
			{ { 6.2, -2.4, 0 }, { 6.2, -3.6, 0 }, { 6.2, -3.6, 1 }, 0x0055 },
			{ { 6.2, -2.4, 1 }, { 6.2, -2.4, 0 }, { 6.2, -3.6, 1 }, 0x0055 },
			{ { 6.2, -3.6, 0 }, { 6.0, -4.0, 0 }, { 6.0, -4.0, 1 }, 0x0055 },
			{ { 6.2, -3.6, 1 }, { 6.2, -3.6, 0 }, { 6.0, -4.0, 1 }, 0x0055 },
			{ { 6.0, -4.0, 0 }, { 5.7, -4.2, 0 }, { 5.7, -4.2, 1 }, 0x0055 },
			{ { 6.0, -4.0, 1 }, { 6.0, -4.0, 0 }, { 5.7, -4.2, 1 }, 0x0055 },
			{ { 5.7, -4.2, 0 }, { 5.3, -4.2, 0 }, { 5.3, -4.2, 1 }, 0x0055 },
			{ { 5.7, -4.2, 1 }, { 5.7, -4.2, 0 }, { 5.3, -4.2, 1 }, 0x0055 },
			{ { 5.3, -4.2, 0 }, { 5.0, -4.0, 0 }, { 5.0, -4.0, 1 }, 0x0055 },
			{ { 5.3, -4.2, 1 }, { 5.3, -4.2, 0 }, { 5.0, -4.0, 1 }, 0x0055 },
			{ { 5.0, -4.0, 0 }, { 4.8, -3.6, 0 }, { 4.8, -3.6, 1 }, 0x0055 },
			{ { 5.0, -4.0, 1 }, { 5.0, -4.0, 0 }, { 4.8, -3.6, 1 }, 0x0055 },
			{ { 4.8, -3.6, 0 }, { 4.8, -2.4, 0 }, { 4.8, -2.4, 1 }, 0x0055 },
			{ { 4.8, -3.6, 1 }, { 4.8, -3.6, 0 }, { 4.8, -2.4, 1 }, 0x0055 },
			{ { 4.8, -2.4, 0 }, { 5.0, -2.0, 0 }, { 5.0, -2.0, 1 }, 0x0055 },
			{ { 4.8, -2.4, 1 }, { 4.8, -2.4, 0 }, { 5.0, -2.0, 1 }, 0x0055 },
			{ { 5.0, -2.0, 0 }, { 5.3, -1.8, 0 }, { 5.3, -1.8, 1 }, 0x0055 },
			{ { 5.0, -2.0, 1 }, { 5.0, -2.0, 0 }, { 5.3, -1.8, 1 }, 0x0055 }
		});
		*/

		obj1 = Object(cube1, cmde::VEC3F(0, 0, 0));
		obj1.mesh.ChangeColor(0x00AA);
		objects.push_back(obj1);
		obj2 = Object(cube1, cmde::VEC3F(0, 1, 0));
		obj2.mesh.ChangeColor(0x00BB);
		objects.push_back(obj2);
		//defaultRaycast = *new Mesh(*new std::vector<Triangle>{ { { 0.1f, 0, 0 }, { 0, 0.1f, 0 }, { 0.1f, 0.1f, 0 }, 0x0003 }, { { 0, 0, 0 }, { 0, 0.1f, 0 }, { 0.1f, 0, 0 }, 0x0003 } });
		//obj3 = Object(defaultRaycast, { 0, 0, 0 });
		//objects.push_back(obj3);
		myRenderingSystem = false;
		wireframe = false;
		emptyChar.Attributes = 0x0088;
		camera = Camera(
			{ 0.5f, 0.5f, -2 }, //pos
			{ 0, 0 }, //facing
			{ 90, 90 }, //fov (173 is the last perfectly functional value; 180 counts as 0, adding counts up from 0 but the image is reversed)
			0.1f, //nearPlane
			200.0f, //farPlane
			this //engine (For drawing to the screen)
		);
	}

	void Setup()
	{

	}

	void Update()
	{
		wchar_t print[128] = {};
		int printLength = 0;

		camera.UpdateInBounds();

		//Raycast crosshair
		/*
		if (resetColor != 0x0FF1)
		{
			resetTriangleColor->color = resetColor;
		}
		objects.at(2).mesh = defaultRaycast;
		RaycastHit hit;
		std::vector<Triangle> tempTriangles = std::vector<Triangle>();
		if (hit.Raycast(camera.position, camera.forwards, objects))
		{
			if (resetTriangleColor != hit.triangle)
			{
				resetTriangleColor = hit.triangle;
				resetColor = hit.triangle->color;
			}

			hit.triangle->color = 0x0001;

			cmde::VEC3F tempV1 = Normalize(CrossProduct(hit.plane.normal, hit.plane.normal + camera.up));
			cmde::VEC3F tempV2 = Normalize(CrossProduct(hit.plane.normal, tempV1));
			tempTriangles.push_back(Triangle(tempV1 * 0.1f, tempV1 * -1 * 0.1f, tempV2 * 0.1f, 0x0003).GetWithOffset(hit.plane.point + camera.forwards * -0.1f));
			tempTriangles.push_back(Triangle(tempV1 * 0.1f, tempV2 * -1 * 0.1f, tempV1 * -1 * 0.1f, 0x0003).GetWithOffset(hit.plane.point + camera.forwards * -0.1f));
			tempTriangles.push_back(Triangle(tempV1 * 0.1f, tempV2 * 0.1f, tempV1 * -1 * 0.1f, 0x0003).GetWithOffset(hit.plane.point + camera.forwards * -0.1f));
			tempTriangles.push_back(Triangle(tempV1 * 0.1f, tempV1 * -1 * 0.1f, tempV2 * -1 * 0.1f, 0x0003).GetWithOffset(hit.plane.point + camera.forwards * -0.1f));
		}
		objects.at(2).mesh = Mesh(tempTriangles);
		*/

		//Move object
		objects.at(0).position = objects.at(0).position + cmde::VEC3F(0.1f, 0, 0) * deltaTime;

		if (myRenderingSystem == true)
		{
			for (Object& o : objects)
			{
				camera.RenderShapeSpherical(o, wireframe);
			}
		}
		else
		{
			for (Object& o : objects)
			{
				camera.RenderShapeProjection(o, wireframe);
			}
		}

		DrawLineS(cmde::VEC2F(0.495f, 0.5f), cmde::VEC2F(0.51f, 0.5f), 0x00FF);
		DrawLineS(cmde::VEC2F(0.5f, 0.495f), cmde::VEC2F(0.5f, 0.51f), 0x00FF);

		Inputs();
		DrawPallet(3, 0, 25, 10);

		camera.UpdateRotation();

		//Debug UI
		/*
		for (short i = 0; i < screenSize.Y; i++)
		{
			printLength = swprintf(print, 128, L"D: %f", zBuffer[i * screenSize.X + 10]);
			WriteText(10, i, print, printLength);
		}
		printLength = swprintf(print, 128, L"X: %f | Y: %f | Z: %f", camera.position.x, camera.position.y, camera.position.z);
		WriteText(0, 0, print, printLength);
		printLength = swprintf(print, 128, L"H: %f | V: %f", camera.facing.x, camera.facing.y);
		WriteText(0, 1, print, printLength);
		printLength = swprintf(print, 128, L"forwards: (%f, %f, %f)", camera.forwards.x, camera.forwards.y, camera.forwards.z);
		WriteText(0, 2, print, printLength);
		printLength = swprintf(print, 128, L"left: (%f, %f, %f)", camera.left.x, camera.left.y, camera.left.z);
		WriteText(0, 3, print, printLength);
		printLength = swprintf(print, 128, L"up: (%f, %f, %f)", camera.up.x, camera.up.y, camera.up.z);
		WriteText(0, 4, print, printLength);

		printLength = swprintf(print, 128, L"sll: (%f, %f, %f)", camera.sightLimitL.x, camera.sightLimitL.y, camera.sightLimitL.z);
		WriteText(0, 5, print, printLength);
		cmde::VEC3F slr = camera.forwards * DotProduct(camera.sightLimitL, camera.forwards) - camera.left * DotProduct(camera.sightLimitL, camera.left);
		printLength = swprintf(print, 128, L"slr: (%f, %f, %f)", slr.x, slr.y, slr.z);
		WriteText(0, 6, print, printLength);
		*/
	}

	void DrawPallet(short topLeftX, short topLeftY, short bottomRightX, short bottomRightY)
	{
		short sizeX = bottomRightX - topLeftX - 1;
		short offsetX = ceilf(sizeX * 0.20f);
		sizeX -= 2 * offsetX;
		short sizeY = bottomRightY - topLeftY - 1;
		DrawLine(topLeftX, topLeftY, bottomRightX, topLeftY, 0x0000, 0x2588, -1, -1);
		DrawLine(topLeftX, topLeftY, topLeftX, bottomRightY, 0x0000, 0x2588, -1, -1);
		DrawLine(bottomRightX, topLeftY, bottomRightX, bottomRightY, 0x0000, 0x2588, -1, -1);
		DrawLine(topLeftX, bottomRightY, bottomRightX, bottomRightY, 0x0000, 0x2588, -1, -1);
		for (short i = 0; i < sizeX; i++)
		{
			for (short j = 0; j < sizeY; j++)
			{
				Draw(topLeftX + 1 + 0			+ i,	topLeftY + 1 + j, 0x0080 + (selCol + 15) % 16,	0x2592);
				Draw(topLeftX + 1 + offsetX		+ i,	topLeftY + 1 + j, selCol * 17,					0x2588, -2);
				Draw(topLeftX + 1 + 2 * offsetX + i,	topLeftY + 1 + j, 0x0080 + (selCol + 1) % 16,	0x2592);
			}
		}
	}
	void Inputs()
	{
		//Moving
		camera.position = camera.position + camera.forwards * (inputs[L'w'] >= 2 ? 0.05f : 0);
		camera.position = camera.position + camera.left * (inputs[L'a'] >= 2 ? 0.05f : 0);
		camera.position = camera.position + camera.forwards * (inputs[L's'] >= 2 ? -0.05f : 0);
		camera.position = camera.position + camera.left * (inputs[L'd'] >= 2 ? -0.05f : 0);
		camera.position = camera.position + camera.up * (inputs[L'q'] >= 2 ? -0.05f : 0);
		camera.position = camera.position + camera.up * (inputs[L'e'] >= 2 ? 0.05f : 0);

		//Looking
		camera.facing.x += (inputs[MOUSE_X] > 200 ? -0.4f : 0); // right
		camera.facing.x += (inputs[MOUSE_X] < 50 ? 0.4f : 0); // left
		camera.facing.y += (inputs[MOUSE_Y] > 200 ? -0.4f : 0); // bottom
		camera.facing.y += (inputs[MOUSE_Y] < 50 ? 0.4f : 0); // top

		//Rendering
		if (inputs[L'r'] == 2)
		{
			myRenderingSystem = !myRenderingSystem;
		}

		//Importing/Exporting
		if (inputs[L'f'] == 2)
		{
			cmde::FILE file;
			if (PromptFileSearch(&file, L"OBJ", L"*.obj;*.3D.cmde"))
			{
				if (file.HasExtension(L"obj"))
				{
					ObjFile output = ObjFile();
					file.ProcessFile<ObjFile>(&output);
					objects.at(1).mesh = Mesh(output.triangles);
				}
				else
				{
					if (file.HasExtension(L"3D.cmde"))
					{
						CMDE3DFile output = CMDE3DFile();
						file.ProcessFile<CMDE3DFile>(&output);
						objects.at(1).mesh = Mesh(output.triangles);
					}
				}
			}
		}
		if (inputs[L'g'] == 2)
		{
			cmde::FILE file;
			if (PromptFileSave(&file, L"CMDE 3D", L"*.3D.cmde"))
			{
				CMDE3DFile::Export(file, objects.at(1).mesh);
			}
		}

		//Painting
		selCol = ((inputs[L'.'] == 2 ? selCol + 1 : (inputs[L','] == 2 ? selCol - 1 : selCol)) + 16) % 16;
		if (inputs[L'c'] >= 2)
		{
			RaycastHit hit;
			std::vector<Triangle> tempTriangles = std::vector<Triangle>();
			if (hit.Raycast(camera.position, camera.forwards, objects))
			{
				hit.triangle->color = selCol;
			}
		}
	}
};

int main()
{
	//RotateShape game(120, 120, 1, 1);
	//RotateShape game(400, 400, 2, 2);
	Test3D game(250, 250, 3, 3);
	game.Start();
}