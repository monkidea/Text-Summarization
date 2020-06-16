#include "Summarizer.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

int
main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Invoke: Main <text file> <delim> <stopwords> "
            "<summary size>\n";
    return 1;
  }

  const string arg = argv[1];
  const string splitDelim = argv[2];
  const string stopWordsFd = argv[3];
  const unsigned int summarySize = atoi(argv[4]);

  Summarizer summarizer{arg, splitDelim, stopWordsFd,
                        summarySize};
  summarizer.readSentences();
  summarizer.calcSentWeights(Summarizer::WeightFreq);
  cout << summarizer.getSummary() << "\n";

  return 0;
}