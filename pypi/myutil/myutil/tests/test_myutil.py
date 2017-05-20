#!/usr/bin/env python

try:
    import unittest2 as unittest
except ImportError:
    import unittest
    
from myutil import getline


class MyUtilTest(unittest.TestCase):
    def setUp(self):
        pass

    def test_getline(self):
        lines = ['abc', '123']
        s = getline(lines)
        self.assertEqual(s, 'abc')


if __name__ == '__main__':
    unittest.main()
