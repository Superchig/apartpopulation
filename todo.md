# TODO
- Dynamically render characters in a font into one large texture for the size,
  rather than using individual faces
- Use a class specifically for rectangles of text
  - Centered or aligned to the left
  - Maintains a font pixel size and a scale
  - Has width and height and center coordinates
  - Automatically pushes words to a new line
- Decide how to implement custom button functionality
- Display months by name
- Rewrite tables to be rendered in world space
- Render a console in OpenGL with output
- Allocate memory for nobles on the heap

## Finished todos
[x] Show passing of a month
[x] Show and update the ages of nobles
[x] Generate random numbers better

# Current Goal
- Give nobles marriage partners
- Allow nobles to give birth and have kids, as well as to die of old age

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

# Future Simulation Ideas
- People celebrate their birthdays as social events
- "Elite overproduction" (too many nobles/people) causes mass civil war, new
  wars to start, or increased political infighting, which stops populations
  from becoming too massive
- Race that reproduces by "cellular division" (members of the race are clones,
  with minor mutation of a previous member)
- Polygamy?