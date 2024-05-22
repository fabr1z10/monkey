import monkey



device_size = (320, 200)
LINE_COUNT = device_size[1] // 8 - 2
COL_COUNT = device_size[0] // 8
doc_lines = 1
doc_top = 0
enable_mouse = True
input_string=''
menu_id = None
editor_id = None
menu_type = ''
question_id = None
question_func = ''
# program lines. these can be much more than line_count! But only <line_count> lines
# can be visible at any time on the screen, starting from doc_top
# so visible lines go
lines = []
cx = 0
cy = 0
qcx = 0
in_string = False
title = 'New game'
working_file=None
id_main= None
shaders = [
    monkey.SHADER_BATCH_QUAD_PALETTE,
    monkey.SHADER_BATCH_LINES
]
tokens = None
tokenRoot = None