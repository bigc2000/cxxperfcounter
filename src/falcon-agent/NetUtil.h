//
// Created by wx on 19-6-6.
//

#pragma once

#include <event2/http.h>
#include <unistd.h>

namespace mc {
class NetUtil {
  static std::string host;

public:
  static std::string getHost() {
    if (host.empty()) {
      char name[64];
      int errcode = gethostname(name, sizeof(name));
      if (errcode < 0) {
        return "";
      } else {
        host = name;
      }
    } else {
      return host;
    }
//    hostent ent;
//    gethostent_r(&ent,sizeof(hostent));
  }
};
}