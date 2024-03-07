import scripts
import settings
import game_state
#from scripts.utils import

phrasal_verbs = {
    'pick_up',
    'turn_on',
    'turn_off'
}

verb_map = {
    'get': 'pickup'
}

def is_valid_item(item):
    # first map the item name into item_id
    matching_item_ids = settings.items['map'].get(item, [item])
    item_id = None
    for iid in matching_item_ids:
        if iid in game_state.inventory:
            item_id = iid
            break
        desc = settings.items['items'].get(iid)
        if desc and desc['room'] == settings.room:
            # check disambiguity
            if 'disambigue' not in desc or eval(desc['disambigue']):
                if 'visible' not in desc or eval(desc['visible']):
                    item_id = iid
    return item_id


def process_action(a):
    b = a.lower().split()
    if not b:
        return
    if len(b) > 1:
        if (b[0] + '_' + b[1]) in phrasal_verbs:
            b[1] = b[0] + b[1]
            b.pop(0)
    verb = b[0]
    item1 = b[1] if len(b) > 1 else None
    item2 = b[3] if len(b) > 3 else None
    # one item action
    action = ''
    verb = verb_map.get(verb, verb)
    if item1 and not item2:
        item_id_1 = is_valid_item(item1)
        if item_id_1:
            # the item is valid (either in inventory or in room)
            action = verb + '_' + item_id_1
        else:
            scripts.msg(99, x=item1)
    elif item1 and item2:
        item_id_1 = is_valid_item(item1)
        item_id_2 = is_valid_item(item2)
        if item_id_1 and item_id_2:
            action = verb + '_' + item_id_1 + '_' + item_id_2
    else:
        # no item action (e.g. bow)
        action = verb
    print('action to perform:', action)

    f = None
    if action:
        f = getattr(scripts, action, None)
        if f is None:
            f = getattr(scripts, verb, None)
        if f:
            f(item_id_1)
