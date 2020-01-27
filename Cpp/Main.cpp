#include "Summarizer.hpp"
#include <iostream>

using namespace std;

int
main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Invoke: Main file\n";
    return 1;
  }

  const string arg = argv[1];
  if (arg.empty()) {
    cerr << "Fatal: Invalid file\n";
    return 1;
  }

  Summarizer summarizer{arg};
  summarizer.readSentences();
  summarizer.calcSentWeights(Summarizer::WeightFreq);
  cout << summarizer.getSummary(2) << "\n";

  return 0;
}