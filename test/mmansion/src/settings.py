import monkey

device_size = (320, 200)
main_view_height = 136
main_view_y = 54
room = 'start'
title = 'Maniac Mansion'
enable_mouse = True
shaders = [
    monkey.SHADER_BATCH_QUAD_PALETTE,
    monkey.SHADER_BATCH_LINES
]
# identifies the item which is the current player
player='dave'

verbs = [
    ('Push', 1, 45), ('Pull', 1, 37), ('Give', 1, 29),
    ('Open', 65, 45), ('Close', 65, 37), ('Read', 65, 29),
    ('Walk to', 121, 45), ('Pick up', 121, 37), ('What is', 121, 29),
    ('New Kid', 193, 45), ('Unlock', 193, 37), ('Use', 193, 29),
    ('Turn on', 257, 45), ('Turn off', 257, 37), ('Fix', 257, 29)]