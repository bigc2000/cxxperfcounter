#pragma once
namespace mc{
class FalconAgentUtil{

public:
  static std::string tags;
public:
  static void initDefaultTags(const std::string& xboxTag){
    tags = "group=cc";
  }

};
}