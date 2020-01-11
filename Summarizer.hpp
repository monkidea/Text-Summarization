#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct Summarizer {
  enum Method { ProbDensity, WeightFreq };

private:
  typedef struct container {
    int nseq;
    double weight;
  } container_t;

  FILE *_fd;
  string _file;

  int _tWeight;
  int _maxWeight;
  unordered_set<string> _wStopWords;
  unordered_map<string, int> _wfreq;
  vector<pair<string, container_t>> _weights;

  void _calcWfreq(void);
  bool _isInteger(const string &str);
  bool _isStopWord(const string &word);

  /* Methods */
  void
  _weightFreqMethod(pair<string, container_t> &sentence);

public:
  explicit Summarizer(const string &file);
  void calcSentWeights(const Summarizer::Method &method);
  string getSummary(unsigned int picksz);
  void readSentences(void);
  ~Summarizer();
};