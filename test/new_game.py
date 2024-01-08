#!/usr/bin/python3

import sys
import os


print('creatint new game: ' + sys.argv[1])

if os.path.isdir(sys.argv[1]):
    print('game already exists.')
    exit(1)
