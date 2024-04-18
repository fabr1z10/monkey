import monkey
#from .. import settings


device_size = (320, 200)
room = 'wolf'
previous_room=''
pos = [250, 50]
dir = 's'
title = 'Hello world!'
shaders = [monkey.SHADER_BATCH_QUAD_PALETTE, monkey.SHADER_BATCH_LINES]
asset_directories = [ '../assets/spritesheets/kq1']
actions = dict()

class Keys:
    restart = 299
    enter = 257
    inventory = 258
    view_item = 293 # F4
    right = 262
    left = 263
    up = 265
    down =264

class CollisionFlags:
    player = 1
    wall = 2
    foe = 4
    foe_hotspot = 8

class CollisionTags:
    player = 0
    foe = 1