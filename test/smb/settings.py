import monkey

device_size = (256, 240)
room = 'world1_1'
title = 'Super Mario Bros'
shaders = [monkey.SHADER_BATCH_QUAD_PALETTE, monkey.SHADER_BATCH_LINES]
asset_directories = [ '../assets/spritesheets/smb']
tile_size = 16
jump_height = 80

class Tiles:
    block1 = [224,8,16,16]
    block2 = [288,8,16,16]

class Keys:
    restart = 299
    enter = 257

class CollisionFlags:
    player = 1
    platform = 2
    foe = 4

class CollisionTags:
    player = 0
    spawn = 1
    foe = 2

