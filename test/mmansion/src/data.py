rooms = dict()
items = dict()
strings = dict()
tag_to_id = dict()

inventory = {
    'dave': []
}


doormat = 'closed'
maindoor = 'closed'
maindoor_unlocked = False


def getDoormatSize():
    return [0, 64 if doormat =='open' else 88, 0, 8]