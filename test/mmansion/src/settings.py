import monkey

device_size = (320, 200)
main_view_height = 136
main_view_y = 54
room = 'library'
title = 'Maniac Mansion'
enable_mouse = True
shaders = [
    monkey.SHADER_BATCH_QUAD_PALETTE,
    monkey.SHADER_BATCH_LINES
]
# identifies the item which is the current player

characters = ['dave', 'bernard']#, 'michael']

inv_x = [1, 178]
inv_y = [21, 13]
inventory_start_index = 0
inventory_max_items = 4

player = 1

id_game = None
id_text = None
id_inv = None
id_newkid = None


default_verb = 'walkto'
action = default_verb
item1 = None
item2 = None
preposition = None


verbs = {
    'push': {
        'text': 0,
        'pos': [1, 45]
    },
    'pull': {
        'text': 1,
        'pos': [1, 37]
    },
    'give': {
        'text': 2,
        'pos': [1, 29],
        'objects': 2,
        'preposition': 15
    },
    'open': {
        'text': 3,
        'pos': [65, 45]
    },
    'close': {
        'text': 4,
        'pos': [65, 37]
    },
    'read': {
        'text': 5,
        'pos': [65, 29]
    },
    'walkto': {
        'text': 6,
        'pos': [121, 45]
    },
    'pickup': {
        'text': 7,
        'pos': [121, 37]
    },
    'whatis': {
        'text': 8,
        'pos': [121, 29]
    },
    'newkid': {
        'text': 9,
        'pos': [193, 45],
        'on_click': 'newkid'
    },
    'unlock': {
        'text': 10,
        'pos': [193, 37]
    },
    'use': {
        'text': 11,
        'pos': [193, 29],
        'objects': 2,
        'preposition': 30
    },
    'turnon': {
        'text': 12,
        'pos': [257, 45]
    },
    'turnoff': {
        'text': 13,
        'pos': [257, 37]
    },
    'fix': {
        'text': 14,
        'pos': [257, 29]
    },
}
