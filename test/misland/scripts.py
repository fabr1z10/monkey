import monkey
import game_state
import settings


def restart_room():
    monkey.close_room()

def make_text(string_id):
    msg = monkey.Text(batch='sprites', font='sierra', anchor=monkey.ANCHOR_CENTER,
                      text=settings.strings[string_id],
                      width=29 * 8, pal=1)
    msg.set_position(160, 100, 1)
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



def rm_node(id):
    def f():
        print(id)
        monkey.get_node(id).remove()
        set_main_node_active(True)()
    return f

def set_main_node_active(value):
    def f():
        #monkey.get_node(settings.text_edit_node).active = value
        monkey.get_node(game_state.Ids.game_node).active=value
    return f
def message(script, id, last=True):
    script.add(monkey.actions.CallFunc(function=set_main_node_active(False)))

    msg = make_text(id)
    script.add(monkey.actions.Add(id=game_state.Ids.text_node, node=msg))
    wk = monkey.actions.WaitForKey()
    wk.add(settings.Keys.enter, func=rm_node(msg.id))
    script.add(wk)

    # msg = monkey.actions.Msg(font='kq1main/sierra', text=settings.strings[id], batch='ui', pos=(160, 100, 1.1),#(158, 83, 1.1),
    #     palette=1, parent=settings.text_node, margin=(0,0), halign=1, valign=1, max_width=230,
    #     on_create=make_text_border)
    # if last:
    #     msg.set_on_end(function=set_main_node_active(True))
    # script.add(msg)

def pippo(a, b, c):
    print('entering weppo')


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


def drown(player, other, c):
    _drown(player, player.x, 4)

def drown2(player, other, c):
    _drown(player, 188, 38)