#include "Summarizer.hpp"
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstring>
#include <sstream>

Summarizer::Summarizer(const string &file,
                       const string &splitDelim,
                       const string &stopWordsFd,
                       unsigned int summarySize)
    : _fd(fopen(stopWordsFd.c_str(), "r"))
    , _file(file)
    , _summarySize(summarySize)
    , _maxFreq(INT_MIN) {

  if (!_fd)
    throw invalid_argument(
        "Fatal: Could not open StopWords file.");

  _splitDelim = splitDelim;

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
      if (_splitDelim == ".")
        transform(word.begin(), word.end(), word.begin(),
                  ::tolower);
      if (!Summarizer::_isStopWord(word) &&
          !Summarizer::_isInteger(word) &&
          !(word.size() == 1 && !isalpha(word[0])))
        _wfreq[word]++;
      unsigned int val = _wfreq[word];
      _maxFreq = max(_maxFreq, val);
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
Summarizer::_tokenize(void) {
  const auto ltrim = [](string &s) {
    while (isspace(s.front())) {
      s.erase(s.begin());
    }
  };

  unsigned int nseq = 0;
  string lines = _fdData;

  size_t pos = 0;
  while ((pos = lines.find(_splitDelim)) != string::npos) {
    auto line = lines.substr(0, pos);
    ltrim(line);

    nseq++;
    container_t c;
    c.weight = (double)0;
    c.nseq = nseq;

    _weights.push_back({line, c});
    lines.erase(0, pos + 1);
  }
}

void
Summarizer::_weightFreqMethod(
    pair<string, container_t> &sentence) {
  double weight = 0;
  const string line = sentence.first;
  stringstream ss{line};
  string word;

  while (ss >> word) {
    if (_splitDelim == ".")
      transform(word.begin(), word.end(), word.begin(),
                ::tolower);
    assert(_maxFreq > 0);
    assert(_wfreq.find(word) != _wfreq.end());
    weight += (double)_wfreq[word] / _maxFreq;
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
          "Fatal: Invalid method specified.");
  }

  sort(_weights.begin(), _weights.end(),
       [](const auto &p1, const auto &p2) -> bool {
         return p1.second.weight > p2.second.weight;
       });
}

string
Summarizer::getSummary(void) {
  string summary{};
  if (_summarySize > _weights.size())
    throw invalid_argument(
        "Fatal: _summarySize > _weights.");

  auto summarySize = _summarySize;

  auto f_itr = _weights.begin();
  auto l_itr = _weights.begin() + summarySize;
  vector<pair<string, container_t>> weights(f_itr, l_itr);

  sort(weights.begin(), weights.end(),
       [](const auto &p1, const auto &p2) -> bool {
         return p1.second.nseq < p2.second.nseq;
       });

  for (const auto &p : weights) {
    if (!summarySize)
      return summary;

    summary += p.first + ". ";
    summarySize--;
  }

  summary.erase(--summary.end());
  return summary;
}

void
Summarizer::readSentences(void) {
  FILE *fd = fopen(_file.c_str(), "r");
  if (!fd)
    throw invalid_argument(
        "Fatal: Could not read input (text) file.");

  char arr[1024];
  bzero((void *)arr, sizeof(arr));

  while (fgets(arr, sizeof(arr), fd)) {
    auto line = string(arr);
    replace(line.begin(), line.end(), '\n', ' ');
    _fdData += line;
  }

  _tokenize();
}

Summarizer::~Summarizer(void) {
  fclose(_fd);
}