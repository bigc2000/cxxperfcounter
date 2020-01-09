//
// Created by wx on 19-6-10.
//

#pragma once
namespace mc {
static const int BITS_PER_LONG = 63;

class UniformSample {
  int cur;
  int count;
  int maxSize;
  std::mutex lock;
  std::vector<int64_t> values;
public:

  UniformSample(int reservoirSize) : cur(0), count(0), maxSize(reservoirSize) {
    values.resize(maxSize);
  }

  UniformSample(const UniformSample &src) : cur(src.cur), count(src.count), maxSize(maxSize), values(src.values) {
    values.resize(maxSize);
  }

  UniformSample(UniformSample &&src) : cur(src.cur), count(src.count), maxSize(maxSize) {
    values = std::move(src.values);
    values.resize(maxSize);
  }

  ~UniformSample() {
  }

  ///@brief no need to lock.
  int size() const {
    return count;
  }

  ///@brief different to java, never use random to evict policy
  void update(int64_t value) {
    std::lock_guard<decltype(lock)> guard(lock);
    if (count < maxSize) {
      count++;
    }
    values[(cur++) % maxSize] = value;
  }

  Snapshot getSnapshot() {
    lock.lock();
    std::vector<double> cowVec(count);
    std::copy(values.begin(), values.begin() + count, cowVec.begin());
    lock.unlock();
    return Snapshot(std::move(cowVec));
  }
};
}

