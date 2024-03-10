import monkey
import game_state
import settings
import random
import regex as re
import shapely


def id_to_string(string_id, **kwargs):
    message = settings.strings[string_id]
    aa = dict(kwargs, random=random, msg=id_to_string)
    expr = set(re.findall('(\#\#[^\#]*\#\#)', message))
    for ex in expr:
        message = message.replace(ex, str(eval(ex[2:-2], aa)))
    return message


def make_text(string_id, **kwargs):
    # allow for dynamic strings
    message = id_to_string(string_id, **kwargs)

    msg = monkey.Text(batch='sprites', font='sierra', anchor=monkey.ANCHOR_CENTER,
                      text=message,
                      width=29 * 8, pal=1)
    msg.set_position(160, 100, 2)
    border = monkey.Node()
    border.set_model(monkey.models.from_shape('tri',
                                              monkey.shapes.AABB(-10, msg.size[0] + 10, -msg.size[1] - 5, 5),
                                              (1, 1, 1, 1),
                                              monkey.FillType.Solid))
    border.set_position(0, 0, -0.1)
    border2 = monkey.Node()
    border2.set_model(monkey.models.from_shape('lines',
                                               monkey.shapes.AABB(-5, msg.size[0] + 5, -msg.size[1] - 3, 3),
                                               monkey.from_hex('AA0000'),
                                               monkey.FillType.Outline))
    border2.set_position(0, 0, -0.01)
    border3 = monkey.Node()
    border3.set_model(monkey.models.from_shape('lines',
                                               monkey.shapes.AABB(-6, msg.size[0] + 6, -msg.size[1] - 3, 3),
                                               monkey.from_hex('AA0000'),
                                               monkey.FillType.Outline))
    border3.set_position(0, 0, -0.01)
    print('ccc',border.id)
    print('ccd',border2.id)
    print('cce',border3.id)
    msg.add(border)
    msg.add(border2)
    msg.add(border3)
    return msg


def set_main_node_active(value):
    def f():
        #monkey.get_node(settings.text_edit_node).active = value
        monkey.get_node(game_state.Ids.game_node).state = value
    return f

def rm_node(*args):
    def f():
        for id in args:
            monkey.get_node(id).remove()
        set_main_node_active(monkey.NodeState.ACTIVE)()
    return f


# Returns true if player is within item bounds
def is_within_bounds(item):
    item_desc = settings.items['items'][item]
    bounds = item_desc.get('bounds')
    if bounds:
        player = monkey.get_node(game_state.Ids.player)
        p = shapely.geometry.Point(player.x, player.y)
        ppoints = []
        assert len(bounds) % 2 == 0, "bounds must be made of a even number of floats"
        for i in range(0, len(bounds), 2):
            ppoints.append([bounds[i], bounds[i+1]])
        poly = shapely.geometry.Polygon(ppoints)
        return poly.contains(p)
    else:
        return True

def move_item_by(item, delta):
    idesc = settings.items['items'][item]
    p0 = idesc['pos']
    idesc['pos'] = [p0[0] + delta[0], p0[1] + delta[1], p0[2] + delta[2]]

def _goto_room(room, pos, dir):
    def f():
        settings.room = room
        settings.pos = pos
        settings.dir = dir
        monkey.close_room()
    return f


def interpret(s):
    if isinstance(s, list):
        if s[0] == 'if':
            return s[2] if eval(s[1]) else s[3]
    return s