#!/usr/bin/python2
import os
import sys
import json
#
import requests

ZABBIX_URL = 'http://127.0.0.1/zabbix/api_jsonrpc.php'
ZABBIX_API_USER = 'testzabapiuser'
ZABBIX_API_PASS = 'testzabapipass'

IS_VERBOSE = True


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

        return json.loads(_response.text)

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


def main():
    print('---- start program ----')
    _zapi = MyZabbixApi(ZABBIX_URL,
                        ZABBIX_API_USER,
                        ZABBIX_API_PASS)

    ret = _zapi.login()
    ret = _zapi.logout()

    return 0

if '__main__' == __name__:
    ret = main()
    sys.exit(ret)
