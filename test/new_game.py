#!/usr/bin/python3

import sys
import os

name = sys.argv[1]

print('creating new game: ' + name)


if os.path.isdir(name):
    print('game already exists.')
    exit(1)

os.mkdir(name)

f = open("misland/main.py", "w")
f.write('#!/usr/bin/python3\n\n')
f.write('import monkey\n\n')
f.write('game = monkey.engine()\n')
f.write('game.start()\n')
f.write('game.run()\n')
f.write('game.shutdown()\n')
f.close()

f = open("misland/strings.py", "w")
f.close()

f = open("misland/state.py", "w")
f.close()

f = open("misland/settings.py", "w")
f.write('import monkey\n\n')
f.write('device_size = (256, 240)\n')
f.write("room = 'hello_world'\n")
f.write("title = 'Hello world!'\n")
f.write('shaders = [monkey.SHADER_BATCH_QUAD_PALETTE]\n')
f.close()



