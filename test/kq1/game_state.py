class Ids:
    game_node = None
    text_node = None
    score_label = None

score = 0
max_score = 158
sound='on'
troll_gone = 0
rock_moved = 0
protective_spell = 0
goat_east = 0
bowl_taken = 0
note_taken = 0
cheese_taken=0
walnut_opened = 0
goat_follow = 0
gate_open = 0
parser_override = None
guess_number = 0

inventory = set()
collected = set()




inventory_selected = 0
inventory_nodes = []
wallz = []

# items = {
#
#     'dagger': {
#         'id': 22,
#         'desc': 24,
#         'sprite': 'sprites/item_dagger'
#     },
#     'carrot': {
#         'id': 35,
#         'desc': 36,
#         'sprite': 'sprites/item_carrot'
#     },
#     'pouch': {
#         'id': 46,
#         'desc': 45,
#         'sprite': 'sprites/item_pouch'
#     },
#     'bowl': {
#         'id': 53,
#         'desc': 54,
#         'sprite': 'sprites/item_bowl'
#     },
#     'pebbles': {
#         'id': 58,
#         'desc': 59,
#         'sprite': 'sprites/item_pebbles'
#     },
#     'walnut': {
#         'id': 63,
#         'desc': 64,
#         'sprite': 'sprites/item_walnut'
#     },
#     'note': {
#         'id': 82,
#         'desc': 83,
#         'sprite': 'sprites/item_note'
#
#     },
#     'cheese': {
#         'id': 90,
#         'desc': 89,
#         'sprite': 'sprites/item_cheese'
#
#     }
#
# }






nodes = {}