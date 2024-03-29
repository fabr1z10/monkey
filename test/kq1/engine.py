import scripts
import settings
import game_state

import random
#from scripts.utils import

phrasal_verbs = {
    'pick_up',
    'turn_on',
    'turn_off',
    'look_in'
}

verb_map = {
    'get': 'pickup'
}

def read(value):
    if isinstance(value, list) and value and isinstance(value[0], str):
        func = getattr(scripts, value[0])
        if func:
            largs = [read(x) for x in value[1:]]
            return func(*largs)
    else:
        return value

def is_valid_item(item):
    # first map the item name into item_id
    matching_item_ids = settings.items['map'].get(item, [item])
    item_id = None
    for iid in matching_item_ids:
        if iid in game_state.inventory:
            item_id = iid
            break
        desc = settings.items['items'].get(iid)
        print('kollo',desc)
        if desc and desc['room'] == settings.room:
            # check disambiguity
            if 'disambigue' not in desc or eval(desc['disambigue']):
                if 'visible' not in desc or eval(desc['visible']):
                    item_id = iid
    return item_id

def guess_name(a):
    b = a.lower().split()
    if not b:
        return

    if a == settings.strings[127]:
        scripts.msg(1)
    else:
        if game_state.guess_number == 0:
            scripts.msg(128)
        elif game_state.guess_number == 1:
            scripts.msgs(129,130)
        elif game_state.guess_number == 2:
            scripts.last_wrong_guess()
        game_state.guess_number+=1

def process_action(a):
    # this can be redirected if
    if game_state.parser_override:
        globals()[game_state.parser_override](a)
        return

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
    item_id_1 = None
    item_id_2 = None
    if item1 and not item2:
        print('check',item1)
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

    key = verb
    if item_id_1:
        print(' -- check item:', item_id_1, ', key:',key)
        cc = settings.items['items'][item_id_1].get('actions', None)
        if cc:
            cc = cc.get(key, None)
            if cc:
                read(cc)
            # func = getattr(scripts, cc[0])
            # if func:
            #     largs = [read(x) for x in cc[1:]]
            #     func(*largs)
            # else:
            #     print('function:',func,'not found!')
            #     exit(1)
        else:
            print(' -- action: <',key,'> not found for',item_id_1)

    # if action:
    #     f = getattr(scripts, action, None)
    #     if f is None:
    #         f = getattr(scripts, verb, None)
    #     if f:
    #         f(item_id_1)
    #     else:
    #         # check if I have a custom msg
    #         if item_id_1 and not item_id_2:
    #             msg = settings.items['items'][item_id_1].get('msg')
    #             if msg and verb in msg:
    #                 scripts.msg(scripts.utils.interpret(msg[verb]))


