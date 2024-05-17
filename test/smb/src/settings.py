import monkey

room ='1-1'
tile_size=16
device_size = (256, 240)
title = 'New game'
shaders = [
    monkey.SHADER_BATCH_QUAD_PALETTE,
    monkey.SHADER_BATCH_LINES
]
restart_key = 299
speed = 200
jump_height = 96 #64
time_to_jump_apex = 0.5
start_pos = (3, 5)
main_node = None

state = 0
mario_states = [
    {
        'model': 'tiles/mario',
        'size': [10, 10, 0],
        'center': [5, 0, 0]
    },
    {
        'model': 'tiles/supermario',
        'size': [10, 30, 0],
        'center': [5, 0, 0]
    }
]

class Flags:
    PLAYER = 1
    FOE = 4

class Tags:
    PLAYER = 0
    FOE = 1
