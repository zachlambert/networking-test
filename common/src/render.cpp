#include "render.hpp"
#include <cstdio>

void render(const Grid &grid, char marker) {
  for (std::size_t y = 0; y < grid.size(); y++) {
    for (std::size_t x = 0; x < grid.size(); x++) {
      switch (grid(x, y)) {
      case Pixel::Clear:
        printf(" ");
        break;
      default:
        printf("\033[0m%c", marker);
        break;
#if 0
        case Red:
        case Green:
        case Blue:
        case Magenta:
        case Cyan:
        case Yellow:
        case White:
        case Black:
#endif
      }
    }
  }
}
