#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include<fstream>
#include<vector>
#include <map>
#include<string.h>
#include<stdio.h>
#include "./AiEngines/AIBenchmark/benchmark.hpp"
using namespace std;

std::string GetFileName(const std::string& s) {

    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    std::string s1;
    if (i != std::string::npos) 
    {
        s1 = s.substr(i+1, s.length() - i);
    }

    sep = '.';
    i = s1.rfind(sep, s1.length());
    if (i != std::string::npos) 
    {
        return (s1.substr(0, i));
    }

    return("");
}

std::vector<std::string> split(const std::string &str,
                                          const std::string &delim) {
  std::vector<std::string> res;
  if ("" == str)
    return res;
  int strlen = str.length() + 1;
  char *strs = new char[strlen];
  strcpy(strs, str.c_str());

  int delimlen = delim.length() + 1;
  char *d = new char[delimlen];
  strcpy(d, delim.c_str());

  char *p = strtok(strs, d);
  while (p) {
    std::string s = p;
    res.push_back(s);
    p = strtok(NULL, d);
  }

  delete[] strs;
  delete[] d;
  return res;
}

std::vector<std::string> ReadDict(const std::string &path) {
  std::ifstream in(path);
  std::string line;
  std::vector<std::string> m_vec;
  if (in) {
    while (getline(in, line)) {
      m_vec.push_back(line);
    }
  } else {
    std::cout << "no such label file: " << path << ", exit the program..."
              << std::endl;
    exit(1);
  }
  return m_vec;
}

std::map<std::string, std::string> LoadConfig(const std::string &config_path) {
  auto config = ReadDict(config_path);
  
  std::map<std::string, std::string> dict;
  for (int i = 0; i < config.size(); i++) {
    // pass for empty line or comment
    if (config[i].size() <= 1 || config[i][0] == '#') {
      continue;
    }

    std::vector<std::string> res = split(config[i], " ");
    dict[res[0]] = res[1];
  }
  return dict;
}
int main()
{
  std::cout << "Main " << std::endl;
    std::map<std::string, std::string> config_map_ = LoadConfig("../test-config.txt");
    std::string framework, objectTagget, classFile, imagesPath, groundTruthBox, threshold;
    float f_threshold;
    framework.assign(config_map_["framework"]);
    objectTagget.assign(config_map_["objectTagget"]);
    classFile.assign(config_map_["classFile"]);
    imagesPath.assign(config_map_["imagesPath"]);
    groundTruthBox.assign(config_map_["groundTruthBox"]);
    threshold.assign(config_map_["threshold"]);

    Benchmark* bm = new Benchmark();
    bm->Init(framework, objectTagget, classFile, imagesPath, groundTruthBox);
    f_threshold = stof(threshold);
    bm->Run(f_threshold);

    return 0;
}