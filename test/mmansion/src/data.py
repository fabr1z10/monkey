rooms = dict()
items = dict()
strings = dict()
tag_to_id = dict()

inventory = {
    'dave': []
}


doormat = 'closed'
door_main = 'closed'
door_kitchen = 'closed'
door_dining = 'closed'
fridge = 'closed'
faucet = 'closed'
maindoor_unlocked = True

baselines = []

def getDoormatSize():
    return [0, 64 if doormat =='open' else 88, 0, 8]