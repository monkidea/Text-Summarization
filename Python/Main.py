#!/usr/bin/python3

import sys
import argparse
from pprint import pprint
from TextSummarizer import Exhaustive, TextRank


if __name__ == "__main__":
    args = argparse.ArgumentParser()
    args.add_argument("file", help="File to read", type=str)
    args.add_argument(
        "method", help="Method to use for summarization", type=str)
    p_args = args.parse_args()

    text = list()
    with open(p_args.file) as f:
        text = f.read()

    text = text.split(".")
    text = [x.lstrip() for x in text]
    text = [x.rstrip() for x in text]

    if p_args.method == "textrank":
        textrank = TextRank(nodes=len(text), text=text)
        ktop = textrank.KTopRanks(k=2)
        for line in ktop:
            print(line)
    elif p_args.method == "wtfreq":
        exhm = Exhaustive(tokens=text)
        ktop = exhm.KTopRanks(k=2)
        for s, wt in ktop:
            print(s)
    else:
        sys.exit("Invalid method specified: \"{0}\"".format(p_args.method))
