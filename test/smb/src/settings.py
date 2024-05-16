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