#pragma  once

#include <string>
#include <map>
#include "Clock.h"

namespace mc {
enum METRIC_TYPE {
  COUNTER,
  GAUGE,
  HIST,//not support yet
};
constexpr const static int TICK_INTERVAL =
static_cast<int> (std::chrono::duration_cast<TIME_DURATION>(std::chrono::seconds(5)).count());
constexpr const static int TIME_UNIT_ONE_SECOND =
static_cast<int> (std::chrono::duration_cast<TIME_DURATION>(std::chrono::seconds(1)).count()); // 1 second=? time unit

decltype(TIME_DURATION(1).count()) GetNowTimeCount();
class Metric {
  std::string _name;
  std::map<std::string, std::string> tagMap;//@note NoteThreadSafe,should init_once

public:
  virtual int64_t getCount() const = 0;

  virtual METRIC_TYPE getType() const =0;

  void name(std::string &&name_) {
    this->_name = name_;
  }

  void name(const std::string &name_) {
    this->_name = name_;
  }

  const std::string &name() const {
    return _name;
  }

  /// @note NotThreadSafe,just init
  void addTags(std::map<std::string, std::string> tag) {
    tagMap.insert(tag.begin(), tag.end());
  }

  const std::map<std::string, std::string>& tags() const {
    return tagMap;
  };


  static std::string formatTags(const std::map<std::string, std::string> &tagMap) {
    std::string _tags;
    if (!_tags.empty()) {
      _tags.append(",");
    }
    for (const auto &it:tagMap) {
      _tags.append(it.first).append("=").append(it.second);
    };
    return _tags;
  }
};
}