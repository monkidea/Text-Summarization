#include "Summarizer.hpp"
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstring>
#include <sstream>

Summarizer::Summarizer(const string &file)
    : _fd(fopen("StopWords", "r"))
    , _file(file)
    , _tWeight(0)
    , _maxWeight(INT_MIN) {

  if (!_fd)
    throw invalid_argument("Missing file: \"StopWords\"");

  char arr[1024];
  bzero((void *)arr, sizeof(arr));
  while (fgets(arr, sizeof(arr), _fd)) {
    string word = arr;
    if (word.back() == '\n')
      word.erase(--word.end());

    _wStopWords.insert(word);
  }
}

void
Summarizer::_calcWfreq(void) {
  for (const auto &p : _weights) {
    const string line = p.first;
    stringstream ss{line};
    string word;
    while (ss >> word) {
      transform(word.begin(), word.end(), word.begin(),
                ::tolower);
      if (!Summarizer::_isStopWord(word) &&
          !Summarizer::_isInteger(word))
        _wfreq[word]++;
      int val = _wfreq[word];
      _tWeight += val;
      _maxWeight = max(_maxWeight, val);
    }
  }
}

bool
Summarizer::_isInteger(const string &str) {
  if (str.empty())
    return false;
  char *ptr;
  strtod(str.c_str(), &ptr);
  return *ptr == 0;
}

bool
Summarizer::_isStopWord(const string &word) {
  return _wStopWords.find(word) != _wStopWords.end();
}

void
Summarizer::_weightFreqMethod(
    pair<string, container_t> &sentence) {
  double weight = 0;
  const string line = sentence.first;
  stringstream ss{line};
  string word;

  while (ss >> word) {
    transform(word.begin(), word.end(), word.begin(),
              ::tolower);
    assert(_maxWeight > 0);
    assert(_wfreq.find(word) != _wfreq.end());
    weight += (double)_wfreq[word] / _maxWeight;
  }

  assert(weight > (double)0);
  sentence.second.weight = weight;
}

void
Summarizer::calcSentWeights(
    const Summarizer::Method &method) {
  Summarizer::_calcWfreq();

  for (auto &p : _weights) {
    if (method == Summarizer::Method::WeightFreq)
      _weightFreqMethod(p);
    else
      throw invalid_argument(
          "Error: Invalid method specified.");
  }

  sort(_weights.begin(), _weights.end(),
       [](const auto &p1, const auto &p2) -> bool {
         return p1.second.weight > p2.second.weight;
       });
}

string
Summarizer::getSummary(unsigned int picksz) {
  string summary{};
  if (picksz > _weights.size())
    throw invalid_argument(
        "Fatal: picksz larger than _weights.");

  auto f_itr = _weights.begin();
  auto l_itr = _weights.begin() + picksz;
  vector<pair<string, container_t>> weights(f_itr, l_itr);

  sort(weights.begin(), weights.end(),
       [](const auto &p1, const auto &p2) -> bool {
         return p1.second.nseq < p2.second.nseq;
       });

  for (const auto &p : weights) {
    if (!picksz)
      return summary;

    summary += p.first + ". ";
    picksz--;
  }

  summary.erase(--summary.end());
  return summary;
}

void
Summarizer::readSentences(void) {
  FILE *fd = fopen(_file.c_str(), "r");
  if (!fd)
    throw invalid_argument(
        "Error reading file: fopen() fail.");

  char arr[1024];
  unsigned int nseq = 0;
  bzero((void *)arr, sizeof(arr));

  while (fgets(arr, sizeof(arr), fd)) {
    char *ptr = strtok(arr, ".");
    while (ptr) {
      nseq++;
      container_t c;
      c.nseq = nseq;
      c.weight = 0.0;
      _weights.push_back({ptr, c});
      ptr = strtok(NULL, ".");
    }
  }

  fclose(fd);
}

Summarizer::~Summarizer(void) {
  fclose(_fd);
}