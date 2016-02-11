#!/usr/bin/env python
#
# sqlite access program
#
import os
import sys
import sqlite3

SQLITE_DB_FILE = './testdb.sqlite3'


def select_sqlite(search_name):

    try:
        print(('try connect sqlite-db: %s' % (SQLITE_DB_FILE)))
        _conn = sqlite3.connect(SQLITE_DB_FILE)
        _cursor = _conn.cursor()

        if search_name is None:
            _q = '''SELECT id, create_date, update_date, name FROM t_test1 ORDER BY id;'''
            _cursor.execute(_q)
        else:
            _t = [search_name]
            _q = '''SELECT id, create_date, update_date, name FROM t_test1 WHERE name = ? ORDER BY id;'''
            _cursor.execute(_q, _t)

        print('fetch rows.')
        for r in _cursor:
            print(('id=%s,create_date=%s, update_date==%s, name=%s' % (
                r[0], r[1], r[2], r[3])))

    except:
        print(('error : %s (%s)' % (
            sys.exc_info()[0],
            sys.exc_info()[1])))

    return 


def main():
    if 1 < len(sys.argv):
        print('select name filter.')
        select_sqlite('name1')
    else:
        print('select all rows.')
        select_sqlite(None)

    return 0


if '__main__' == __name__:
    ret = main()
    sys.exit(ret)
