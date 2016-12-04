#!/usr/bin/python3
#
# cassandra client test program
#
import os
import sys
import datetime
import json
import cassandra


def main():
    print('cassandra client test')
    print('cassandra.__version__ = {}'.format(cassandra.__version__))
    return

if '__main__' == __name__:
    ret = main()
    sys.exit(ret)
               
