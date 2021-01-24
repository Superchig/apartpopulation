# TODO

## "Trivial" TODOs
- Display months by name
- Rewrite tables to be rendered in world space
- Use enum classes rather than enums

## "Trivial"-medium TODOs
- Implement table scrolling via mouse up-or-down
- Implement a GLFW [key callback](https://gamedev.stackexchange.com/questions/150157/how-to-improve-my-input-handling-in-glfw) for input handling, rather than querying
  the key states every frame

## Broad, handy TODOs
- Render a console in OpenGL with output
- Implement basic sprite editor that can import sprites from sprite sheets
  - May need a new representation for sprites that record parts of a texture
    atlas
  - May need a disk-saveable text representation (YAML) for sprite assets

## Finished todos
- [x] Show passing of a month
- [x] Show and update the ages of nobles
- [x] Generate random numbers better
- [x] Allocate memory for nobles on the heap
- [x] Give nobles spouses
- [x] Allow nobles to give birth and have kids, as well as to die of old age (tentatively)
- [x] Dynamically render font glyphs into a texture atlas and use that for
  rendering text
- [x] Implement batch rendering (at least for quads)
- [x] Render sprites and text with batch renderer
- [x] Extend quad renderer to support "unlimited" textures, based off of texture
  index (tentatively)
- [x] Render all text of the same font and size in one draw call
- [x] Render lines in table with tiny quads
- [x] Mark off some old TODOs
- [x] Cull parts of table that aren't on screen
- [x] Add actual table-scrolling functionality (that mostly works) to scrollbar
- [x] Make white texture accessible as a Texture2D object

# Current Goal
- Move the pass buttons and noble table with the camera
- Implement basic sprite grid to represent controlled land using the white
  texture (but colored accordingly)
  - Could have the land be colored according to its controller (state) and
    have a letter based off whether it's a village vs. city
- Render text that's been constrained to a box, which can be centered or
  left-aligned (or right-aligned?)

# Future Programming Ideas
- Modern C++ (C++11, C++14, C++17, C++20)
- [More advanced engine capabilities](https://learnopengl.com/In-Practice/2D-Game/Final-thoughts)
  - Sprite sheet/texture atlas
  - Instanced rendering
  - Triangle strips
  - Space partitioning
  - Minimize state changes
- Consider using a class specifically for rectangles of text
  - Centered or aligned to the left
  - Maintains a font pixel size and a scale
  - Has width and height and center coordinates
  - Automatically pushes words to a new line
- Consider using pages rather than a scrollbar for noble table?
- How do you benchmark C++ code?
- Use [bitmasks](https://stackoverflow.com/questions/1406554/why-use-flagsbitmasks-rather-than-a-series-of-booleans) if you want
- How would we store historical figures to be more cache-friendly? With SOA or
  look-up indices?
- For audio, consider using [SoLoud](https://sol.gfxile.net/soloud/)
- Implement some kind of frame vs. render [game loop](http://gameprogrammingpatterns.com/game-loop.html), with possible semi-fixed [delta times](https://gafferongames.com/post/fix_your_timestep/)
- Do we need performance optimizations for checking button presses? 
  (put in contiguous array as with DOD, use ECS with DOD, or binary-space
  partitioning?)

# Future Simulation Ideas
- People celebrate their birthdays as social events
- "Elite overproduction" (too many nobles/people) causes mass civil war, new
  wars to start, or increased political infighting, which stops populations
  from becoming too massive
- Race that reproduces by "cellular division" (members of the race are clones,
  with minor mutation of a previous member)
- Polygamy?
- Natural selection-esque personality traits
	- Recklessness
		- Likeliness to launch coups or attempt to seize a position
	- FactionLoyalty
		- How hard/easy it is to get someone to leave a faction
	- Clemency
		- How likely someone is to punish someone after they try something against them
		- Showing clemency should have certain benefits, while destroying your enemies
		  will probably have obvious benefits of their own.
	- Studiousness vs Activity (different name)
		- How often someone will try to improve their skills vs do things
