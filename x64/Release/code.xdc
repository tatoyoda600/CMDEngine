<?xml version="1.0"?><doc>
<members>
<member name="M:cmde.FILE.HasExtension(System.Char!System.Runtime.CompilerServices.IsConst*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="353">
<summary>
Compares this file's extension to the one passed to this function
</summary>
<param name="extension">The extension to compare against, without the starting '.' (Ex. 'HasExtension(L"png")')</param>
</member>
<member name="F:cmde.CMDEngine.inputs" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="425">
<summary>0 -&gt; nothing ; 1 -&gt; released ; 2 -&gt; pressed ; 3 -&gt; held ; MOUSE_X and MOUSE_Y -&gt; point on the command prompt</summary>
</member>
<member name="M:cmde.CMDEngine.#ctor(System.Int16,System.Int16,System.Int16,System.Int16,System.Boolean,System.Boolean,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="428">
<summary>
Constructor for the command prompt engine
</summary>
<param name="screenWidth">The width of the command prompt in characters</param>
<param name="screenHeight">The height of the command prompt in characters</param>
<param name="fontWidth">The width of the characters in pixels</param>
<param name="fontHeight">The height of the characters in pixels (Less than 3 may cause lines between pixels; 1 may cause distortions for some resolutions)</param>
<param name="autoUpscale">Whether to automatically scale the font up so that it occupies the most amount of screen space</param>
<param name="clearScreen">Whether to automatically clear the screen at the beginning of 'Update()' (You can do so manually with the 'ClearFrame()' function)</param>
<param name="maxFPS">The minimum amount of time before the next frame is drawn (1/fps). Setting this to 0 or less will allow for infinte framerate</param>
</member>
<member name="M:cmde.CMDEngine.ThrowError(System.Char!System.Runtime.CompilerServices.IsConst*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="603">
<summary>
Prints an error message to the console (Returns 0 so that you can just do 'return ThrowError(...);')
</summary>
<param name="msg">The message to print to the console</param>
</member>
<member name="M:cmde.CMDEngine.Draw(System.Int16,System.Int16,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="622">
<summary>
Draws to a specific point on the command prompt
</summary>
<param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param>
<param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param>
<param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
<param name="cha">The character with which to draw to that point</param>
<param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.Draw(System.Int32,System.Int32,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="639">
<summary>Draws to a specific point on the command prompt</summary> /// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param> /// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.Draw(System.Single,System.Single,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="642">
<summary>Draws to a specific point on the command prompt</summary> /// <param name="x">The x position of the point (Leftmost is 0; Rightmost is screenSize.X)</param> /// <param name="y">The y position of the point (Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.Draw(cmde.VEC2F,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="644">
<summary>Draws to a specific point on the command prompt</summary> /// <param name="p">The position of the point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw to that point</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawLine(System.Single,System.Single,System.Single,System.Single,System.Int16,System.Int16,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="655">
<summary>
Draws a line on the command prompt from a point to another point
</summary>
<param name="x1">The x position of the first point (Leftmost is 0; Rightmost is screenSize.X)</param>
<param name="y1">The y position of the first point (Topmost is 0; Bottommost is screenSize.Y)</param>
<param name="x2">The x position of the second point (Leftmost is 0; Rightmost is screenSize.X)</param>
<param name="y2">The y position of the second point (Topmost is 0; Bottommost is screenSize.Y)</param>
<param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
<param name="cha">The character with which to draw the line</param>
<param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param>
<param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawLine(cmde.VEC2F,cmde.VEC2F,System.Int16,System.Int16,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="686">
<summary>Draws a line on the command pront from a point to another point</summary> /// <param name="p1">The position of the first point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="p2">The position of the second point (Leftmost is 0; Rightmost is screenSize.X; Topmost is 0; Bottommost is screenSize.Y)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawLineS(System.Single,System.Single,System.Single,System.Single,System.Int16,System.Int16,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="688">
<summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="x1">The x position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y1">The y position of the first point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="x2">The x position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0)</param> /// <param name="y2">The y position of the second point in relative screen space (Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawLineS(cmde.VEC2F,cmde.VEC2F,System.Int16,System.Int16,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="690">
<summary>Draws a line on the command pront from a point in relative screen space to another point in relative screen space</summary> /// <param name="p1">The position of the first point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="p2">The position of the second point in relative screen space (Leftmost is 0.0; Rightmost is 1.0; Topmost is 0.0; Bottommost is 1.0)</param> /// <param name="col">The color with which to draw the line (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the line</param> /// <param name="depth1">How far away from the camera the first point is (For rendering things on top of each other) (Negative values are always drawn)</param> /// <param name="depth2">How far away from the camera the second point is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawRPoly(System.Single,System.Single,System.Int16,System.Single,System.Single,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="698">
<summary>
Draws a regular polygon to the screen
</summary>
<param name="cx">The x position of the center of the polygon</param>
<param name="cy">The y position of the center of the polygon</param>
<param name="edges">The amount of edges the polygon has</param>
<param name="rad">The distance from the center point to each of the vertices</param>
<param name="rot">The angle at which to draw the polygon in degrees</param>
<param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
<param name="cha">The character with which to draw the polygon</param>
<param name="depth">How far away from the camera the shape is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawRPoly(cmde.VEC2F,System.Int16,System.Single,System.Single,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="718">
<summary>Draws a regular polygon to the screen</summary> /// <param name="p">The position of the center of the polygon</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawRPolyS(System.Single,System.Single,System.Int16,System.Single,System.Boolean,System.Single,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="720">
<summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="cx">The x position of the center of the polygon in relative screen space</param> /// <param name="cy">The y position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawRPolyS(cmde.VEC2F,System.Int16,System.Single,System.Boolean,System.Single,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="722">
<summary>Draws a regular polygon to the screen using relative screen space</summary> /// <param name="p">The position of the center of the polygon in relative screen space</param> /// <param name="edges">The amount of edges the polygon has</param> /// <param name="rad">The distance from the center point to each of the vertices in relative screen space</param> /// <param name="useY">Whether the radius' size is defined by the screen's height or width ('true' means height is used; 'false' for width)</param> /// <param name="rot">The angle at which to draw the polygon in degrees</param> /// <param name="col">The color with which to draw the polygon (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param> /// <param name="cha">The character with which to draw the polygon</param> /// <param name="depth">How far away from the camera the pixel is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.WriteText(System.Int16,System.Int16,System.Char*,System.Int16,System.Int16,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="726">
<summary>
Writes a line of text starting from a point on the command prompt
</summary>
<param name="x">The x position of the leftmost character</param>
<param name="y">The y position on which to write the line</param>
<param name="text">The text to write</param>
<param name="length">The amount of characters in the text</param>
<param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
<param name="depth">How far away from the camera the text is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.DrawTriangle(cmde.VEC2F,cmde.VEC2F,cmde.VEC2F,System.Int16,System.Int16,System.Single,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="743">
<summary>
Draws a filled in triangle to the command prompt from 3 vertices
</summary>
<param name="v1">The first vertice of the triangle</param>
<param name="v2">The second vertice of the triangle</param>
<param name="v3">The third vertice of the triangle</param>
<param name="col">The color with which to draw to that point (16 available colors (0-F); Must be inputted as Hex 0x0000; The last 2 zeros determine the background and foreground colors respectively (0x00BF))</param>
<param name="cha">The character with which to draw to that point</param>
<param name="depth1">How far away from the camera the first vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
<param name="depth2">How far away from the camera the second vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
<param name="depth3">How far away from the camera the third vertice is (For rendering things on top of each other) (Negative values are always drawn)</param>
</member>
<member name="M:cmde.CMDEngine.ScreenPosToPoint(System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="836">
<summary>
Converts a relative screen space position ('0.0 - 1.0' is 'Left - Right' or 'Top - Bottom') to a point in the pixel grid ('0.0 - screenSize.X' is 'Left - Right' and '0.0 - screenSize.Y' is 'Top - Bottom')
</summary>
<param name="x">The x value (If outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
<param name="y">The y value (If outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
</member>
<member name="M:cmde.CMDEngine.ScreenPosToPoint(cmde.VEC2F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="845">
<summary>Converts a relative screen space position ('0.0 - 1.0' is 'Left - Right' or 'Top - Bottom') to a point in the pixel grid ('0.0 - screenSize.X' is 'Left - Right' and '0.0 - screenSize.Y' is 'Top - Bottom')</summary> /// <param name="p">The value (If x or y is outside of range [0.0; 1.0] the resulting pixel will be out of the screen)</param>
</member>
<member name="M:cmde.CMDEngine.ClearFrame" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="848">
<summary>Fills the entire console with the character and color in the 'empty' variable</summary>
</member>
<member name="M:cmde.CMDEngine.SetWindowPos(System.Int16,System.Int16)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="983">
<summary>
Moves the command prompt window to a specific point on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::SetWindowPos')
</summary>
<param name="x">Moves the window so that there are this many pixels between the edge of the screen and the left side of the window (0 pixels seems to actually be -7)</param>
<param name="y">Moves the window so that there are this many pixels between the edge of the screen and the top side of the window (In order to hide the Windows top bar use -29)</param>
</member>
<member name="M:cmde.CMDEngine.SetWindowPos(cmde.VEC2F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="992">
<summary>Moves the command prompt window to a specific point on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::SetWindowPos')</summary> /// <param name="x">Moves the window so that there are this many pixels between the edge of the screen and the left side of the window (0 pixels seems to actually be -7)</param> /// <param name="y">Moves the window so that there are this many pixels between the edge of the screen and the top side of the window (In order to hide the Windows top bar use -29)</param>
</member>
<member name="M:cmde.CMDEngine.MoveWindow(System.Int16,System.Int16)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="996">
<summary>
Moves the command prompt window by a number of pixels on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::MoveWindow')
</summary>
<param name="x">Moves the window this many pixels to the right (Negative numbers go to the left)</param>
<param name="y">Moves the window this many pixels down (Negative numbers go up)</param>
</member>
<member name="M:cmde.CMDEngine.MoveWindow(cmde.VEC2F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1007">
<summary>Moves the command prompt window by a number of pixels on the screen (This function has the same name as one in 'Windows.h', if by any chance you meant to use that one, use '::MoveWindow')</summary> /// <param name="x">Moves the window this many pixels to the right (Negative numbers go to the left)</param> /// <param name="y">Moves the window this many pixels down (Negative numbers go up)</param>
</member>
<member name="M:cmde.CMDEngine.ReadInputs" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1023">
<summary>0 -&gt; nothing ; 1 -&gt; released ; 2 -&gt; pressed ; 3 -&gt; held ; MOUSE_X and MOUSE_Y -&gt; point on the command prompt</summary>
</member>
<member name="M:cmde.CMDEngine.Start" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1106">
<summary>Starts the game loop (Clears the frame; Runs Setup(); Runs Update() in a loop while 'running' is true)</summary>
</member>
<member name="M:cmde.CMDEngine.PromptFileSearch(cmde.FILE*,System.Char!System.Runtime.CompilerServices.IsConst*,System.Char!System.Runtime.CompilerServices.IsConst*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1161">
<summary>
Opens a Windows Explorer window, allowing the user to choose a file of the designated file type to open. Will return true if a file is chosen.
</summary>
<param name="openFile">The variable to which this function will write the output file to if the user picks a file</param>
<param name="fileType">The file type the user will be asked for</param>
<param name="fileExtensions">The internal file type info. Must be formatted correctly: L"*.txt;*.jpg;*.png"</param>
</member>
<member name="M:cmde.CMDEngine.PromptFileSave(cmde.FILE*,System.Char!System.Runtime.CompilerServices.IsConst*,System.Char!System.Runtime.CompilerServices.IsConst*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1234">
<summary>
Opens a Windows Explorer window, allowing the user to choose the name and location of the file to save to, with the designated file type. Will return true if a file is chosen.
</summary>
<param name="saveFile">The variable to which this function will write the output file to if the user picks a file</param>
<param name="fileType">The file type the user will be asked for</param>
<param name="fileExtensions">The internal file type info. Must be formatted correctly: L"*.txt;*.jpg;*.png"</param>
</member>
<member name="M:cmde.CMDEngine.Pow2(System.Double)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1307">
<summary>Elevates a number to the power of 2</summary>
</member>
<member name="M:cmde.CMDEngine.DotProduct(cmde.VEC4F,cmde.VEC4F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1309">
<summary>Calculates the dot product of 2 vectors</summary>
</member>
<member name="M:cmde.CMDEngine.CrossProduct(cmde.VEC3F,cmde.VEC3F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1311">
<summary>Calculates the orthagonal vector of a plane defined by 2 vectors (Index = v1; Middle = v2; Thumb = result)</summary>
</member>
<member name="M:cmde.CMDEngine.Magnitude(cmde.VEC4F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1313">
<summary>Calculates the length of a vector</summary>
</member>
<member name="M:cmde.CMDEngine.Clamp(System.Single,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1315">
<summary>Limits 'value' to the range between 'low' and 'high'</summary>
</member>
<member name="M:cmde.CMDEngine.Sign(System.Int32)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1317">
<summary>Gives the sign of a value (Positive = 1; Negative = -1; Other = itself)</summary>
</member>
<member name="M:cmde.CMDEngine.Sign(System.Double)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1319">
<summary>Gives the sign of a value (Positive = 1; Negative = -1; Other = itself)</summary>
</member>
<member name="M:cmde.CMDEngine.Angle(cmde.VEC4F,cmde.VEC4F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1321">
<summary>Calculates the angle between 2 vectors in degrees</summary>
</member>
<member name="M:cmde.CMDEngine.Normalize(cmde.VEC4F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1323">
<summary>Gives a vector of length 1 in the same direction</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(System.Single,System.Single,System.Single,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1327">
<summary>Calculates the y value of point x on the line defined by the 2 points provided (Returns y1 if x1 == x2)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(cmde.VEC4F,cmde.VEC4F,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1334">
<summary>Calculates the point at 'x' on the line defined by the 2 points provided (Returns p1 if p1.x == p2.x)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(System.Single,System.Single,System.Single,System.Single,System.Single,System.Single*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1342">
<summary>Calculates the y value of point x on the line defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(cmde.VEC4F,cmde.VEC4F,System.Single,cmde.VEC4F*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1344">
<summary>Calculates the point at 'x' on the line defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(System.Single,System.Single,System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1346">
<summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=0 if x1 == x2)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(cmde.VEC4F,cmde.VEC4F)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1353">
<summary>Generates the linear function based on 'x' defined by the 2 points provided (Returns y=z=w=0 if p1.x == p2.x)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(System.Single,System.Single,System.Single,System.Single,cmde.LINEAR*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1362">
<summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if x1 == x2)</summary>
</member>
<member name="M:cmde.CMDEngine.LinearFunction(cmde.VEC4F,cmde.VEC4F,cmde.LINEAR*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1364">
<summary>Generates the linear function based on 'x' defined by the 2 points provided and inserts it into 'output' (Returns false if p1.x == p2.x)</summary>
</member>
<member name="M:cmde.CMDEngine.QuadraticFunction(cmde.VEC2F,cmde.VEC2F,cmde.VEC2F,System.Single,cmde.VEC2F*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1368">
<summary>Calculates the point at 'x' on the line defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)</summary>
</member>
<member name="M:cmde.CMDEngine.QuadraticFunction(cmde.VEC4F,cmde.VEC4F,cmde.VEC4F,cmde.QUADRATIC*)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1386">
<summary>Generates the quadratic function based on 'x' defined by the 3 points provided and inserts it into 'output' (Returns false if there is any repeat value in p1.x, p2.x, and p3.x)</summary>
</member>
<member name="M:cmde.CMDEngine.VectorFromAngles(System.Single,System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1405">
<summary>Assumes the XYZ axis are set up correctly (X+ is left when Z+ is forwards and Y+ is up)</summary>
</member>
<member name="M:Test3D.RaycastHit.Raycast(cmde.VEC3F,cmde.VEC3F,std.vector&lt;Test3D.Object,std.allocator&lt;Test3D.Object&gt;&gt;*!System.Runtime.CompilerServices.IsImplicitlyDereferenced,std.vector&lt;Test3D.Object**,std.allocator&lt;Test3D.Object**&gt;&gt;*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1779">
<summary>
Casts a ray and detects the first plane of the objects with which it intersects
</summary>
<param name="origin">The point from ray starts at</param>
<param name="direction">The direction the ray goes</param>
<param name="objects">The objects to test against</param>
</member>
<member name="M:Test3D.RaycastHit.Raycast(cmde.VEC3F,cmde.VEC3F,Test3D.Object*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1806">
<summary>
Casts a ray and detects the first plane of the objects with which it intersects
</summary>
<param name="origin">The point from ray starts at</param>
<param name="direction">The direction the ray goes</param>
<param name="objects">The objects to test against</param>
</member>
<member name="M:Test3D.Camera.#ctor" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="1875">
<summary>There must be a default constructor or computer gets mad (This should be unusable though)</summary>
</member>
<member name="M:Test3D.DepthConversion(System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="2116">
<summary>A lot of projection matrices output depth in a weird format, which differs from the one used in this program. This function converts depth from the linear type used in this program to that weird one</summary>
</member>
<member name="M:Test3D.DenormalizeDepth(System.Single)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="2118">
<summary>Takes a relative depth value where 0 is the near plane and 1 is the far plane, and returns an actual depth value</summary>
</member>
<member name="M:Test3D.RaycastAll(cmde.VEC3F,cmde.VEC3F,std.vector&lt;Test3D.Object,std.allocator&lt;Test3D.Object&gt;&gt;*!System.Runtime.CompilerServices.IsImplicitlyDereferenced,std.vector&lt;Test3D.RaycastHit,std.allocator&lt;Test3D.RaycastHit&gt;&gt;*,std.vector&lt;Test3D.Object**,std.allocator&lt;Test3D.Object**&gt;&gt;*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="C:\Users\tatoy\source\repos\Engine\code.cpp" line="2121">
<summary>
Casts a ray and detects every plane of the objects with which it intersects
</summary>
<param name="origin">The point from ray starts at</param>
<param name="direction">The direction the ray goes</param>
<param name="objects">The objects to test against</param>
<param name="output">The vector in which to store the data of every hit</param>
</member>
</members>
</doc>