# TODO
- Use a class specifically for rectangles of text
  - Centered or aligned to the left
  - Maintains a font pixel size and a scale
  - Has width and height and center coordinates
  - Automatically pushes words to a new line
- Display months by name
- Rewrite tables to be rendered in world space
- Render a console in OpenGL with output

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

# Current Goal
- Add actual table-scrolling functionality to scrollbar
- Add scrollbars/render text or tables more efficiently
- Use enum classes rather than enums
- Make white texture accessible as a Texture2D object

# Future Programming Ideas
- Modern C++ (C++11, C++14, C++17, C++20)
- [More advanced engine capabilities](https://learnopengl.com/In-Practice/2D-Game/Final-thoughts)
  - Sprite sheet/texture atlas
  - Instanced rendering
  - Triangle strips
  - Space partitioning
  - Minimize state changes
- How do you benchmark C++ code?
- Use [bitmasks](https://stackoverflow.com/questions/1406554/why-use-flagsbitmasks-rather-than-a-series-of-booleans) if you want
- How would we store historical figures to be more cache-friendly? With SOA or
  look-up indices?
- For audio, consider using [SoLoud](https://sol.gfxile.net/soloud/)

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
