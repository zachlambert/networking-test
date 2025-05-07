#pragma once

#include <cstdint>
#include <vector>

enum class Pixel : std::uint8_t {
  Clear,
  Red,
  Green,
  Blue,
  Magenta,
  Cyan,
  Yellow,
  White,
  Black
};

struct Grid {
public:
  Grid(std::size_t size = 0) : size_(size), data_(size * size) {
    std::fill(data_.begin(), data_.end(), Pixel::Clear);
  }

  void resize(std::size_t size) {
    this->size_ = size;
    data_.resize(size * size);
    std::fill(data_.begin(), data_.end(), Pixel::Clear);
  }

  Pixel &operator()(std::size_t x, std::size_t y) {
    return data_[y * size_ + x];
  }
  const Pixel &operator()(std::size_t x, std::size_t y) const {
    return data_[y * size_ + x];
  }

  std::size_t size() const { return size_; }

private:
  std::size_t size_;
  std::vector<Pixel> data_;
};

void render(const Grid &grid, char marker = '#');
