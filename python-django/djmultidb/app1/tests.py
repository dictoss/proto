from django.test import TestCase
from django.core.management import call_command, CommandError


class CommandSetThingTestCase(TestCase):
    def test_setthing_insert(self):
        pass

    def test_setthing_update(self):
        try:
            call_command('set_thing', '1', 'name1')
        except CommandError as e:
            self.assertTrue(False, 'CommandError')
        except SystemExit as e:
            if 0 < e.code:
                self.assertTrue(False, 'SystemExit')
        except:
            self.assertTrue(False, 'other')
