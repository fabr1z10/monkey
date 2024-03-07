import monkey
import game_state
import settings
import random
import math







    # check if items are in room

    exit(1)
    b=a.lower().strip()
    b = '_'.join(b.split())
    print(b)
    exit(1)
    #f = getattr(scripts, settings.room +'_' + b, None)
    script_list = settings.rooms[settings.room].get('scripts', None)
    if not script_list or b not in script_list:
        script_list = settings.actions
    aa = script_list.get(b, None)
    print('fortimi', aa, b)
    if aa:
        script_name = aa[0]
        script_args = aa[1] if len(aa) > 1 else {}
        if 'rect' in script_args:
            if not scripts.player_in(*script_args['rect']):
                scripts.msg(id=75)
                return
        s = getattr(scripts, script_name)(**script_args)
    else:
        print('nofound')


def player_in(x0, y0, w, h):
    player = monkey.get_node(game_state.Ids.player)
    return player.x >= x0 and player.x <= x0+w and player.y >= y0 and player.y <= y0+h




def zfunc1(x, y):
    za = 1.0 if y < (108 + 0.593*(x-184)) and y<107 else 0
    return 1.0 - y / 166.0 + za

def zfunc2(x, y):
    za = 1.0 if y < (205.727 - 0.607*x) and x > 161 else 0
    print (x, y, 205.727 - 0.607*x, za)
    return 1.0 - y / 166.0 + za

def z_witch(x, y):
    za = 1.0 if (x < 42) else 0.0
    print(x,y,za)
    return 1.0 - y / 166.0 + za









def look_inventory_item(item, m1, m2):
    if item in game_state.inventory:
        s = monkey.Script()
        message_item(s, item)
        monkey.play(s)
    else:
        # check if item in room
        taken = getattr(game_state, item + '_taken', 0)
        msg(m2 if taken else m1)

def look_item(**kwargs):
    print('suuu')
    item = kwargs['item']
    if item in game_state.inventory:
        s = monkey.Script()
        message_item(s, item)
        monkey.play(s)
    else:
        # check if item in room
        if item in game_state.nodes:
            msg(id=game_state.items[item]['desc'])
        else:
            msg(id=68)

def read_note():
    msg(id=68 if 'note' not in game_state.inventory else 84)

def open_walnut():
    if 'walnut' in game_state.inventory:
        if game_state.walnut_opened == 0:
            game_state.walnut_opened = 1
            game_state.items['walnut']['id'] = 69
            game_state.items['walnut']['desc'] = 67
            game_state.items['walnut']['sprite'] = 'sprites/item_gold_walnut'
            msg(66)
        else:
            msg(70)
    else:
        msg(68)


def caa():
    exit(1)
def open_witch_door(**kwargs):
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes['door'], anim='open', sync=True))
    s.add(monkey.actions.CallFunc(_goto_room('witchous', [100,5], 's')))
    monkey.play(s)

def open(**kwargs):
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes[kwargs['item']], anim='open', sync=True))
    monkey.play(s)

def close(**kwargs):
    s = monkey.Script()
    s.add(monkey.actions.Animate(id=game_state.nodes[kwargs['item']], anim='open', sync=True, backwards=True))
    monkey.play(s)

def look_witch_table(**kwargs):
    x = monkey.get_node(game_state.Ids.player).x
    if x < 200:
        msg (id = 77)
    else:
        if game_state.note_taken == 0:
            msg(id = 80)
        else:
            msg(id=76)






    # msg = monkey.actions.Msg(font='kq1main/sierra', text=settings.strings[id], batch='ui', pos=(160, 100, 1.1),#(158, 83, 1.1),
    #     palette=1, parent=settings.text_node, margin=(0,0), halign=1, valign=1, max_width=230,
    #     on_create=make_text_border)
    # if last:
    #     msg.set_on_end(function=set_main_node_active(True))
    # script.add(msg)

def pippo(a, b, c):
    print('entering weppo')

def caught_by_wolf(a,b):
    msg(38)
    c = monkey.get_sprite('007/wolf_fight')
    c.set_position(a.x,a.y,a.z)
    a.remove()
    b.remove()
    monkey.get_node(game_state.Ids.game_node).add(c)
    s = monkey.Script()
    s.add(monkey.actions.Delay(5))
    s.add(monkey.actions.Remove(c.id))
    message(s, 0)
    monkey.play(s)

def goat_east(goat, a):
    game_state.goat_east = 1
    goat.remove()

def _wolf():
    msg(37)
    a = monkey.get_sprite('007/wolf')
    a.set_position(180,10,0)
    a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 50,  z_func=settings.z_func))
    a.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.player, 1,
                                               monkey.shapes.AABB(-5, 5, -1, 1), batch='lines'))
    a.user_data = {
        'on_enter': ['caught_by_wolf']
    }
    monkey.get_node(game_state.Ids.game_node).add(a)

def spellEnd():
    game_state.protective_spell = 0
    msg(41)

def spellStart():
    print ('************************************************************************************')
    game_state.protective_spell = 1
    monkey.getClock().addEvent(True, True, 15, spellEnd)


def _fairy():
    a = monkey.get_sprite('fairy/fairy')
    a.set_position(126,86,0)
    a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 10, z_func=settings.z_func,
                                                          walk_e='walk', walk_n='walk', walk_s='walk'))
    monkey.get_node(game_state.Ids.game_node).add(a)
    spell_script = monkey.Script()
    message(spell_script, 39)
    spell_script.add(monkey.actions.CallFunc(spellStart))
    spell_script.add(monkey.actions.Delay(2))
    message(spell_script, 40)
    spell_script.add(monkey.actions.Remove(a.id))
    monkey.play(spell_script)



def create_wolf():
    script = monkey.Script()
    script.add(monkey.actions.Delay(random.randint(1, 10)))
    script.add(monkey.actions.CallFunc(_wolf))
    monkey.play(script)

def create_fairy():
    script = monkey.Script()
    script.add(monkey.actions.Delay(random.randint(1, 10)))
    script.add(monkey.actions.CallFunc(_fairy))
    monkey.play(script)


def create_goat():
    if game_state.goat_east == 0:
        a = monkey.get_sprite('sprites/goat')
        a.set_position(226,78,0)
        angle = random.uniform(0, 2.0 * math.pi)
        vector = [math.cos(angle), math.sin(angle)]
        a.add_component(monkey.components.NPCSierraController(game_state.Ids.player, 60, 1000, 10, z_func=settings.z_func,
                                                              walk_e='walk', walk_n='walk', walk_s='walk', flip=1, direction=vector))
        a.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.foe, 0,
                                                   monkey.shapes.AABB(-5, 5, -1, 1), batch='lines'))
        monkey.get_node(game_state.Ids.game_node).add(a)




def pippa(a, b):
    print('leaving jiko')

def _drown(player, x, y):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Move(id=player.id, position=(x, y, player.z), speed=0))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    message(script, 1)
    script.add(monkey.actions.Delay(time=2))
    message(script, 0)
    monkey.play(script)


def drown(player, other):
    _drown(player, player.x, 4)

def drown3(player, other):
    script = monkey.Script()
    script.add(monkey.actions.SierraEnable(id=player.id, value=False))
    script.add(monkey.actions.Animate(id=player.id, anim='drown'))
    message(script, 50)
    script.add(monkey.actions.Delay(time=2))
    message(script, 0)
    monkey.play(script)


def start_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham_swim'), batch='sprites')

def end_swim(player, other):
    player.set_model(monkey.models.getSprite('sprites/graham'), batch='sprites')

def drown2(player, other):
    _drown(player, 188, 38)


def pickup(**kwargs):
    item = kwargs['item']
    if item in game_state.inventory:
        msg_if_have = kwargs.get('msg_if_have', 26)
        msg(id=msg_if_have)
    else:
        game_state.inventory.add(item)
        if getattr(game_state, item + '_taken', None) is not None:
            setattr(game_state, item + '_taken', 1)
        if item in game_state.nodes:
            monkey.get_node(game_state.nodes[item]).remove()
        msg(id =kwargs['msg'])


def switch(condition, a1, a2):
    a = a1 if eval(condition) else a2
    s = globals()[a[0]](*a[1:])


def rock_pos():
    if game_state.rock_moved == 1:
        return [236, 21, 0]
    return [236, 33, 0]


#lookout_look_tree = _msg(7)
#lookout_look_castle = _msg(4)





def _goto_room(room, pos, dir):
    def f():
        settings.room = room
        settings.pos = pos
        settings.dir = dir
        monkey.close_room()
    return f


def climb_tree():
    script = monkey.Script()
    message(script, 28)
    script.add(monkey.actions.CallFunc(_goto_room('treetop', [80,3], 'n')))
    monkey.play(script)

def get_dagger():
    if 'dagger' in game_state.inventory:
        msg(26)
    else:
        if game_state.rock_moved:
            game_state.inventory.add('dagger')
            msg(25)
        else:
            msg(27)

