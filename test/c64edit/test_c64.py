import unittest
from src.tokenizer import Tokenizer
class MyTestCase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.t = Tokenizer('assets/token.yaml')

    def _helper(self, instruction, result):
        print(' -- testing: ',instruction)
        inst=self.t.tokenize(instruction)
        self.assertEqual(inst, result)

    def test_tokenizer(self):
        self._helper('PRINT"♡"', bytearray(b'\x99\x22\x93\x22'))
        self._helper('IFSC>HSTHENHS=SC', bytearray(b'\x8b\x53\x43\xb1\x48\x53\xa7\x48\x53\xb2\x53\x43'))





