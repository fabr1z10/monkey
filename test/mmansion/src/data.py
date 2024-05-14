rooms = dict()
items = dict()
strings = dict()
tag_to_id = dict()

inventory = {
    'dave': [],
    'bernard': []
}

delayed_funcs = {
    'dave': None,
    'bernard': None
}


doormat = 'closed'
door_main = 'closed'
door_kitchen = 'closed'
door_dining = 'closed'
door_pantry = 'closed'
door_reactor='closed'
fridge = 'closed'
faucet = 'closed'
maindoor_unlocked = True

light_reactor = False

baselines = []

def getDoormatSize():
    return [0, 64 if doormat =='open' else 88, 0, 8]