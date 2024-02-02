import monkey
import settings
import scripts



def bg(ciao):
    a = monkey.models.Quad(batch='001')
    a.add(ciao['quad'])
    auto_depth = ciao.get('auto_depth', False)
    pos = ciao.get('pos', [0,0,0])
    n = monkey.Node()
    n.set_model(a)
    n.set_position(pos[0], pos[1], pos[2] if not auto_depth else 1-2*pos[1]/166.0 )
    return n
    # root.add(n)

def sprite(ciao):
    b = monkey.get_sprite(ciao['sprite'])
    auto_depth = ciao.get('auto_depth', False)
    pos = ciao.get('pos', [0,0,0])
    b.set_position(pos[0], pos[1], pos[2] if not auto_depth else 1-2*pos[1]/166.0 )
    return b

def hotspot(ciao):
    h = monkey.Node()

    if 'aabb' in ciao:
        aabb = ciao['aabb']
        shape = monkey.shapes.AABB(aabb[0], aabb[1], aabb[2], aabb[3])
    else:
        shape = monkey.shapes.Polygon(ciao['poly'])
    h.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.player, 1,
                                               shape, batch='lines'))
    h.user_data = {
        'on_enter': ciao.get('on_enter', None),
        'on_leave': ciao.get('on_leave', None)
    }
    return h

def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')

def on_enter_hotspot(a, b, c):
    on_enter = b.user_data.get('on_enter')
    if on_enter:
        getattr(scripts, on_enter)(a,b,c)


def on_leave_hotspot(a,b):
    on_leave = b.user_data.get('on_leave')
    if on_leave:
        getattr(scripts, on_leave)(a,b)

def create_room(room):
    ce = monkey.CollisionEngine2D(80, 80)
    ce.add_response(0, 1, on_start=on_enter_hotspot, on_end=on_leave_hotspot)
    room.add_runner(ce)
    room.add_runner(monkey.Scheduler())

    room_info = settings.rooms[settings.room]

    size = room_info['size']
    print(' -- size:', size)
    # game_area = (316, 166)
    viewport = (2, 25, 316, 166)
    # mid_y = game_area[1] // 2
    cam = monkey.CamOrtho(316, 166,
                          viewport=viewport,
                          bounds_x=(158, 158), bounds_y=(83, 83))
    room.add_camera(cam)
    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='sprites'))
    for batch in room_info.get('batches', []):
        room.add_batch(batch, monkey.SpriteBatch(max_elements=10000, cam=0, sheet=batch))
    room.add_batch('lines', monkey.LineBatch(max_elements=200, cam=0))

    root = room.root()

    # display some text
    test = monkey.Text(batch='sprites', font='sierra', anchor=monkey.ANCHOR_CENTER,
                       text="Misfortune strikes and you have fallen into the moat. Your struggles and cries have attracted hungry alligators. They do not want to let you go.",
                       width=29 * 8)
    test.set_position(160, 100, 0)
    root.add(test)

    # display some images


    # display a sprite
    b = monkey.get_sprite('sprites/graham')
    b.add_component(monkey.SierraController(half_width=2, y_front=0, y_back=166))
    b.add_component(monkey.components.Collider(settings.CollisionFlags.player, settings.CollisionFlags.foe, 0, monkey.shapes.Point()))
    root.add(b)

    # display some lines
    # c = monkey.Node()
    # c.set_model(monkey.models.from_shape('lines', monkey.shapes.Segment(0, 0, 10, 10), monkey.from_hex('ffffff'), monkey.FillType.Outline))
    # root.add(c)

    d = monkey.Node()
    d.set_model(monkey.models.from_shape('lines', monkey.shapes.PolyLine(points=[10, 0, 10, 50, 110, 60]),
                                         monkey.from_hex('ff0000'), monkey.FillType.Outline))
    root.add(d)

    for wall in room_info.get('walls', []):
        w = monkey.Node()
        w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=wall), batch='lines'))
        root.add(w)

    for item in room_info.get('items', []):
        f = globals().get(item['type'])
        if f:
            root.add(f(item))
    # a.prova([1,2,3])
    # root.add(a)
    # test = monkey.Text('sprites/sierra',
    #                   "We, at Sierra, wish to thank you for playing King's Quest. We are very sorry that you did not succeed and hope you will fare better next time.\nGood luck.",
    #                   width = 29*8)
