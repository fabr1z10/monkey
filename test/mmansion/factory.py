import monkey
import settings

def graham(sprite, x, y, scale):
    b = monkey.get_sprite('sprites/' + sprite)
    #b.add_component(monkey.components.PlayerSierraController(half_width=2, dir='s', skinWidth=1))
    b.add_component(monkey.components.WalkableCharacter(200))
    #b.add_component(monkey.components.Collider(settings.CollisionFlags.player, settings.CollisionFlags.foe, 0, monkey.shapes.Point()))
    b.set_position(x, y, 0)
    b.scale=scale
    settings.player_id = b.id
    return b

def dave(x,y):
    a = monkey.Node()
    b = monkey.models.MultiSprite()
    b.addSprite(monkey.models.getSprite('sprites/dave_legs'))       # root
    b.addSprite(monkey.models.getSprite('sprites/dave_body'), parent=0, link=0, z=0.1)
    b.addSprite(monkey.models.getSprite('sprites/dave_head'), parent=1, link=0)
    a.set_model(b, batch='sprites')
    a.set_position(x,y,0)
    a.add_component(monkey.components.WalkableCharacter(200))
    settings.player_id = a.id
    #a.setAnimation('walk_n')
    return a


def on_enter_hotspot(a, b, c):
    pass


def on_leave_hotspot(a,b):
    pass


def ciao(x, y):
    print('go to --> ',x, y)
    monkey.get_node(settings.player_id).sendMessage(id="goto", pos=(x,y))


def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')


def create_poly(p):
    a = monkey.Node()
    a.set_model(monkey.models.from_shape('lines', monkey.shapes.Polygon(p), (1,1,1,1), monkey.FillType.Outline))
    return a

def bg(ciao,wa):
    n = monkey.Node()
    pos = ciao.get('pos', [0, 0, 0])
    n.set_position(pos[0], pos[1], 0);
    wall = ciao.get('wall')
    if wall:
        shape = None
        if 'poly' in wall:
            shape = monkey.shapes.Polygon(wall['poly'])
        elif 'polyline' in wall:
            pl = points=wall['polyline']
            shape = monkey.shapes.PolyLine(points=pl)
        else:
            print('wall must have poly or polyline')
            exit(1)
        wa.addDynamic(n)
        n.add_component(monkey.components.Collider(2, 0, 0, shape, batch='lines'))
    return n


def create_room(room):
    root = room.root()
    ce = monkey.CollisionEngine2D(80, 80)
    ce.add_response(0, 1, on_start=on_enter_hotspot, on_end=on_leave_hotspot)
    room.add_runner(ce)
    mm = monkey.MouseManager()
    mm.setFunc(0, ciao)
    room.add_runner(mm)
    cam = monkey.CamOrtho(320, 200,
                          viewport=(0,0,320,200),
                          bounds_x=(160, 160), bounds_y=(100, 100))
    room.add_camera(cam)

    room_info = settings.rooms[settings.room]

    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='kq1/sprites'))
    room.add_batch('lines', monkey.LineBatch(max_elements=200, cam=0))

    game_node = monkey.Node()
    text_node = monkey.Node()
    #inventory_node = monkey.Node()
    root.add(game_node)

    game_node.add(graham('graham', 10,10,1))
    #game_node.add(dave(50,50))
    wa = room_info['walkarea']
    poly = wa['poly']
    walkArea = monkey.WalkArea(poly, 2)
    room.add_runner(walkArea)
    game_node.add(create_poly(poly))
    if 'holes' in wa:
        for hole in wa['holes']:
            if 'poly' in hole:
                walkArea.addPolyWall(hole['poly'])
                game_node.add(create_poly(hole['poly']))


    for item in room_info.get('items', []):
        f = globals().get(item['type'])
        condition = item.get('condition', None)
        if condition and not eval(condition):
            continue
        if f:
            node = f(item,walkArea)
            game_node.add(node)