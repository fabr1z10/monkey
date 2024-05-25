#!/usr/bin/python3


import argparse

from src.tokenizer import Tokenizer


parser = argparse.ArgumentParser(
                    prog='compare',
                    description='Compare two C64 programs, returns lines that differ',
                    epilog='Text at the bottom of help')

parser.add_argument('filename1')           # positional argument
parser.add_argument('filename2')           # positional argument
args = parser.parse_args()

t = Tokenizer('assets/token.yaml')

i1 = t.readBasicFile(args.filename1)
i2 = t.readBasicFile(args.filename2)

for l1, s1 in i1.items():
    if l1 not in i2:
        print ('-- Line: ', l1, ' not in ', args.filename2)
    else:
        if s1 == i2[l1]:
            pass
            #print (' -- Line: ', l1, ' OK')
        else:
            print(' -- Line: ', l1, ' difference:')
            print('    ', s1)
            print('    ', i2[l1])
            print(' ***')



