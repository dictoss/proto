#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# load and dump json in japanese.
#
# ex)
#   $ python2 ./pyjson.py
#   $ python3 ./pyjson.py
#

import os
import sys
import io
import json


JSON_FILE_IN_PATH = './load.json'
JSON_FILE_OUT_PATH = './dump.json'


def main():
    _loaddata = ''

    _curdir = os.path.abspath(os.path.dirname(sys.argv[0]))
    _infile = '%s/%s' % (_curdir, JSON_FILE_IN_PATH)
    _outfile = '%s/%s' % (_curdir, JSON_FILE_OUT_PATH)

    # load json
    try:
        with io.open(_infile, 'r', encoding='utf-8') as f:
            _loaddata = json.load(f)

        print('success load json file. (%s)' % (JSON_FILE_IN_PATH))
        print('load data:')
        print('%s' % (_loaddata))
    except:
        print('EXCEPT load: %s (%s)' % (sys.exc_info()[0], sys.exc_info()[1]))

    # dump json
    try:
        with io.open(_outfile, 'w', encoding='utf-8') as f:
            # must unicode str.
            # How do you know to serialize a large json object ?
            _u_str = json.dumps(_loaddata, ensure_ascii=False)
            f.write(_u_str)

        print('success dump. see %s.' % (JSON_FILE_OUT_PATH))
    except:
        print('EXCEPT dump: %s (%s)' % (sys.exc_info()[0], sys.exc_info()[1]))


if '__main__' == __name__:
    main()
