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
  string _file, _splitDelim, _fdData;

  unsigned int _summarySize;
  unsigned int _maxFreq;
  unordered_set<string> _wStopWords;
  unordered_map<string, unsigned int> _wfreq;
  vector<pair<string, container_t>> _weights;

  void _calcWfreq(void);
  bool _isInteger(const string &str);
  bool _isStopWord(const string &word);

  /* Methods */
  void
  _weightFreqMethod(pair<string, container_t> &sentence);
  void _tokenize(void);

public:
  explicit Summarizer(const string &file,
                      const string &splitDelim,
                      const string &stopWordsFd,
                      unsigned int summarySize);
  void calcSentWeights(const Summarizer::Method &method);
  string getSummary(void);
  void readSentences(void);
  ~Summarizer();
};