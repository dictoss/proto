#!/usr/bin/python3
#
# cassandra client test program
#
import os
import sys
import datetime
import json
import cassandra
from cassandra.cluster import Cluster
from cassandra.query import tuple_factory, named_tuple_factory, dict_factory, BatchStatement, PreparedStatement

CASSANDRA_CLUSTER = ['192.168.22.112']


def _get_client_version():
    print('cassandra.__version__ = {}'.format(cassandra.__version__))


def _select_users(keyspace):
    try:
        _cluster = Cluster(contact_points=CASSANDRA_CLUSTER, port=9042)
        _session = _cluster.connect()

        _session.set_keyspace(keyspace)
        _session.row_factory = dict_factory

        # fetch all record
        _rows = _session.execute('''SELECT * FROM users;''')
        for r in _rows:
            print(r)

        # fetch prepare record
        _user_lookup_stmt = _session.prepare(
            "SELECT * FROM users WHERE userid=?")

        _lookup_result = []
        for o in ['dictoss']:
            _user = _session.execute(_user_lookup_stmt, [o])
            print(_user)
            print(list(_user))
            _lookup_result.append(_user)
    except:
        print('EXCEPT: {}({})'.format(sys.exc_info()[0], sys.exc_info()[1]))
    else:
        _session.shutdown()


def _select2_users(keyspace):
    try:
        _cluster = Cluster(contact_points=CASSANDRA_CLUSTER, port=9042)
        with _cluster.connect() as _session:
            _session.set_keyspace(keyspace)
            _session.row_factory = dict_factory
        
            # fetch all record
            _rows = _session.execute('''SELECT * FROM users;''')
            for r in _rows:
                print(r)
                
            # fetch prepare record
            _user_lookup_stmt = _session.prepare(
                "SELECT * FROM users WHERE userid=?")

            _lookup_result = []
            for o in ['dictoss']:
                _user = _session.execute(_user_lookup_stmt, [o])
                print(_user)
                print(list(_user))
                _lookup_result.append(_user)
    except:
        print('EXCEPT: {}({})'.format(sys.exc_info()[0], sys.exc_info()[1]))

        
def _upsert_users(keyspace):
    try:
        _cluster = Cluster(contact_points=CASSANDRA_CLUSTER, port=9042)
        _session = _cluster.connect()

        _session.set_keyspace(keyspace)
        _session.row_factory = dict_factory

        _add_users = [
            {'userid': 'dummy1', 'first_name': '11', 'last_name': 'dummy', 'emails': set('a')},
            {'userid': 'dummy2', 'first_name': '22', 'last_name': 'dummy', 'emails': set('b')}
        ]
        
        _prepare_insert = _session.prepare(
            "INSERT INTO users (userid, first_name, last_name, emails) VALUES (?, ?, ?, ?)")
            # "INSERT INTO users (userid, first_name, emails) VALUES (?, ?, ?)")

        _batch = BatchStatement(consistency_level=0)
        for user in _add_users:
            print(user)
            _batch.add(_prepare_insert, [ user['userid'], user['first_name'], user['last_name'], user['emails'] ])
            # _batch.add(_prepare_insert, [ user['userid'], user['first_name'], user['emails'] ])
        _session.execute(_batch)
        
    except:
        print('EXCEPT insert: {}({})'.format(sys.exc_info()[0], sys.exc_info()[1]))
    else:
        _session.shutdown()


def main():
    print('cassandra client test')
    _get_client_version()
    _select2_users('mykeyspace')
    _upsert_users('mykeyspace')
    return

if '__main__' == __name__:
    ret = main()
    sys.exit(ret)
