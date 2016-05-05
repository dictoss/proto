#!/usr/bin/env python
import os
import sys
import json
# pip install requests
import requests

ZABBIX_URL = 'http://192.168.122.201/zabbix/api_jsonrpc.php'
ZABBIX_API_USER = 'testzabapiuser'
ZABBIX_API_PASS = 'testzabapipass'


class MyZabbixApi(object):
    serverurl = ''
    username = ''
    password = ''
    token = None

    def __init__(self, _url, _user, _pass):
        self.serverurl = _url
        self.username = _user
        self.password = _pass

    def _call(self, _method, _params):
        _response = None

        print('call url: %s' % self.serverurl)
        print

        try:
            payload = {
                'jsonrpc': '2.0',
                'auth': self.token,
                'method': _method,
                'id': 1,
                'params': _params,
                }

            reqdata = json.dumps(payload)

            print('request data:')
            print(reqdata)

            headers = {'Content-Length': len(reqdata),
                       'Content-Type': 'application/json'}

            _response = requests.post(self.serverurl,
                                      headers=headers,
                                      data=reqdata)

            print
            print('response data:')
            print(_response.text)
        except:
            print('EXCEPT: %s (%s)' % (sys.exc_info()[0], sys.exc_info()[1]))
            return None

        return _response.json()

    def login(self):
        print('IN login()')

        params = {'user': self.username,
                  'password': self.password
                  }

        res = self._call('user.login', params)

        if 'result' in res:
            self.token = res['result']

            print('success login.')
            return True
        else:
            print(res['error'])
            return False

    def logout(self):
        print('IN logout()')

        params = {
            }

        res = self._call('user.logout', params)

        if 'result' in res:
            if True == res['result']:
                print('success logout.')
                return True
            else:
                print('fail logout.')
                return False
        else:
            print(res['error'])
            return False

    def history_get(self):
        print('IN history_get()')

        params = {
            # 'hostids': [],
            'itemids': [23684],
            # 'time_from': 0,
            # 'time_till': 0,
            'sortfield': 'clock',
            'sortorder': 'DESC',
            'output': 'extend',
            'limit': 1000,
            'offset': 0
        }

        res = self._call('history.get', params)

        count = len(res['result'])
        print(('history count = %d' % count))

        for i, o in enumerate(res['result']):
            print('%d,%s' % (i, o['clock']))

        return res

    def history_get_loop(self, itemids, max_count, one_limit=100):
        print('IN history_get_loop()')

        total_count = 0
        retval = None

        for api_offset in range(0, max_count, one_limit):
            if (api_offset + one_limit) < max_count:
                api_limit = one_limit
            else:
                api_limit = max_count - api_offset

            params = {
                'itemids': itemids,
                # 'time_from': 0,
                # 'time_till': 0,
                'sortfield': 'clock',
                'sortorder': 'DESC',
                'output': 'extend',
                'limit': api_limit,
                'offset': api_offset
            }

            res = self._call('history.get', params)

            count = len(res['result'])
            total_count = total_count + count

            for i, o in enumerate(res['result']):
                print('%d,%s' % (i, o['clock']))

            print(('history count = %d' % count))
            print(('history total_count = %d' % total_count))

            if retval is None:
                retval = res
            else:
                retval['result'].extend(res['result'])

        return retval


def main():
    print('---- start program ----')
    _zapi = MyZabbixApi(ZABBIX_URL,
                        ZABBIX_API_USER,
                        ZABBIX_API_PASS)

    ret = _zapi.login()

    # historydata = _zapi.history_get()
    historydata = _zapi.history_get_loop([23684], 950)

    ret = _zapi.logout()

    # print history
    for i, o in enumerate(historydata['result']):
        print('%d,%s' % (i, o['clock']))

    return 0

if '__main__' == __name__:
    ret = main()
    sys.exit(ret)
