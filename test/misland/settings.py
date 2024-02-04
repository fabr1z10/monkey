import monkey

device_size = (320, 200)
room = 'lookout'
title = 'Hello world!'
shaders = [monkey.SHADER_BATCH_QUAD_PALETTE, monkey.SHADER_BATCH_LINES]
asset_directories = [ '../assets/spritesheets/kq1']

class Keys:
    restart = 299
    enter = 257

class CollisionFlags:
    player = 1
    wall = 2
    foe = 4