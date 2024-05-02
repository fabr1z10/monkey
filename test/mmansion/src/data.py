rooms = dict()
items = dict()
strings = dict()
tag_to_id = dict()

doormat = 'open'

def getDoormatSize():
    return [0, 64 if doormat =='open' else 88, 0, 8]