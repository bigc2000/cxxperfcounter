//
// Created by wx on 18-8-11.
//
#pragma  once

#include <memory>
#include <vector>
#include <cinttypes>
#include <algorithm>
#include <string>
#include <sstream>
#include <numeric>

namespace mc {
class Snapshot;

class Snapshot {
private:
  std::vector<double> values;
public:

  Snapshot(Snapshot &&src) {
    values = std::move(src.values);
  }
  Snapshot(const Snapshot &src) {
    values = std::move(src.values);
  }

  template<typename T>
  Snapshot(const std::vector<T> &src) {
    values.resize(src.size());
    std::copy(values.begin(), values.end(), values.begin());
    std::sort(values.begin(), values.end());
  }

  Snapshot(const std::vector<double> &src)
  {
    values.resize(src.size());
    std::copy(src.begin(), src.end(), values.begin());
    std::sort(values.begin(), values.end());
  }

  Snapshot(std::vector<double> &&src) {
    values = src;
    std::sort(values.begin(), values.end());
    src.clear();
  }
  
  ~Snapshot() = default;

  double getValue(double percent) const {
    if (percent >= 0.0 && percent <= 1.0) {
      if (this->values.size() == 0) {
        return 0.0;
      }
      else {
        double pos = percent * (double)(this->values.size() + 1);
        if (pos < 1.0) {
          return this->values[0];
        }
        else if (pos >= (double)this->values.size()) {
          return values.back();
        }
        else {
          double lower = this->values[(int)pos - 1];
          double upper = this->values[(int)pos];
          return lower + (pos - (int)(pos)) * (upper - lower);
        }
      }
    }
    else {
      throw std::invalid_argument("quantile is not in [0..1]");
    }
  }

  int size() const {
    return static_cast<int>(values.size());
  }

  double getMedian() const {
    return getValue(0.5);
  }


  double get75thPercentile() const {
    return getValue(0.75);
  }


  double get95thPercentile() const {
    return getValue(0.95);
  }

  double get99thPercentile() const {
    return getValue(0.99);
  }


  double get999thPercentile() const {
    return this->getValue(0.999);
  }

  const std::vector<double> &getValues() const {
    return values;
  }

  std::vector<double> getValues() {
    return values;
  }

  /// @return 统计量（[n,min,max,sum,avg,var])
  std::vector<double> stat() const {
    std::vector<double> mathStat(6);
    if (!values.empty()) {
      int n = values.size();
      mathStat[0] = n;
      mathStat[1] = values[0];
      mathStat[2] = values[values.size() - 1];
      double sum = std::accumulate(values.begin(), values.end(), 0);
      mathStat[3] = sum;
      double avg = sum / values.size();
      mathStat[4] = avg;
      double squareSum = 0.0;
      if (n == 1) {
        mathStat[5] = 1.0;
      }
      else {}
      for (size_t i = 0; i < values.size(); i++) {
        squareSum += (values[i] - avg) * (values[i] - avg);
      }
      mathStat[5] = squareSum / (n - 1);
    }
    return mathStat;
  }

  void dump(FILE *output) const {
    if (output != nullptr) {
      fprintf(output, "%lu", values.size());
      for (auto &value : values) {
        fprintf(output, ",%f", value);
      }
    }
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "[" << getMedian() << "," << get75thPercentile() << "," << get95thPercentile() << "," << get99thPercentile()
      << "]";
    return ss.str();
  }
};

typedef Snapshot *SnapshotPtr;
}



