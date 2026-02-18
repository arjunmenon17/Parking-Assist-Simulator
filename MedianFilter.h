#pragma once
#include <Arduino.h>

template <typename T>
class MedianFilter5 {
public:
  MedianFilter5() : count_(0), idx_(0), hasCurrent_(false), current_(0) {
    for (int i = 0; i < 5; i++) buf_[i] = 0;
  }

  T push(T v) {
    buf_[idx_] = v;
    idx_ = (idx_ + 1) % 5;
    if (count_ < 5) count_++;

    // Compute median of the filled values
    T tmp[5];
    for (int i = 0; i < count_; i++) tmp[i] = buf_[i];

    // insertion sort (count_ <= 5)
    for (int i = 1; i < count_; i++) {
      T key = tmp[i];
      int j = i - 1;
      while (j >= 0 && tmp[j] > key) {
        tmp[j + 1] = tmp[j];
        j--;
      }
      tmp[j + 1] = key;
    }

    T med = tmp[count_ / 2];
    current_ = med;
    hasCurrent_ = true;
    return med;
  }

  bool hasCurrent() const { return hasCurrent_; }

  T currentOr(T fallback) const {
    return hasCurrent_ ? current_ : fallback;
  }

private:
  T buf_[5];
  int count_;
  int idx_;
  bool hasCurrent_;
  T current_;
};