//
// Created by wx on 19-6-10.
//
#pragma  once
namespace mc{
class Timer :public Meter{

  //50
  //75
  //99
private:
  // AtomicLong count;
public:
  Timer() : Counter(0) {
  }

  /**
   * @brief with initial value
   * @param val
   */
  Timer(int64_t val) {
    count = val;
  }

  virtual  ~Counter() {
  }

  /**
   * @brief fetch and add
   * @param n to add
   * @return origin value before add.
   */
  int64_t getAndAdd(int n) {
    return count.fetch_add(n);
  }

  int64_t addAndGet(int n) {
    count.fetch_add(n);
    return count;
  }

  void inc(int n) {
    count.fetch_add(n);
  }
  void dec(int n) {
    count.fetch_add(-n);
  }
  virtual int64_t getCount() const{
    return count;
  }

  METRIC_TYPE getType() const override {
    return COUNTER;
  }


};
}