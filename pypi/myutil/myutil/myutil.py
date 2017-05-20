#!/usr/bin/env python
#
#  my util for python
#
import sys


def getline(lines):
    return lines[0]


class LineUtil(object):
    lines = []
    line_pos = 0

    def __init__(self, lines):
        self.lines = lines

    def getline(self):
        return lines[line_pos]

    def next(self):
        line_pos += 1


if '__main__' == __name__:
    pass
