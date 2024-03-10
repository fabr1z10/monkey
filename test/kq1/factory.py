import monkey
import settings
import scripts
import game_state
import inventory
import engine


link_aabb = {
    'w': [0, 1, 0, 166],
    'e': [315, 316, 0, 166],
    's': [0, 316, 0, 1],
    'n': [0, 316, 120, 121]
}



def graham(sprite, x, y):
    b = monkey.get_sprite('sprites/' + sprite)
    b.add_component(monkey.components.PlayerSierraController(half_width=2, z_func=settings.z_func, dir=settings.dir, skinWidth=1))
    b.add_component(monkey.components.Collider(settings.CollisionFlags.player, settings.CollisionFlags.foe, 0, monkey.shapes.Point()))
    b.set_position(x, y, 0)
    game_state.Ids.player = b.id
    return b



def read(d: dict, key: str, default_value=None):
    value = d.get(key, default_value)
    if isinstance(value, str) and value and value[0] == '@':
        return getattr(scripts, value[1:])()
    return value

def make_text(id,x,y, **kwargs):
    a=monkey.Text(batch='ui', font='sierra', anchor=kwargs.get('anchor',monkey.ANCHOR_CENTER), text=settings.strings[id])
    a.set_position(x,y,5)
    return a

def make_solid_rect(x, y, w, h, color = 'FFFFFF', z = 0):
    node = monkey.Node()
    print(color,'sucalo')
    node.set_model(monkey.models.from_shape(
        'tri',
             monkey.shapes.AABB(0, w, 0, h),
             monkey.from_hex(color),
             monkey.FillType.Solid))
    node.set_position(x, y, z)
    return node

def make_outline_rect(x, y, w, h, color = 'FFFFFF', z = 0):
    node = monkey.Node()
    node.set_model(monkey.models.from_shape(
        'lines',
             monkey.shapes.AABB(0, w, 0, h),
             monkey.from_hex(color),
             monkey.FillType.Outline))
    node.set_position(x, y, z)
    return node

def make_outline2_rect(x, y, w, h, color='FFFFFF', z=0):
    node = monkey.Node()
    node.add(make_outline_rect(x, y, w, h, color, z))
    node.add(make_outline_rect(x+1, y, w-2, h, color, z))
    return node



def bg(ciao):
    a = monkey.models.Quad(ciao['batch'])
    a.add(ciao['quad'])
    auto_depth = ciao.get('auto_depth', False)
    pos = ciao.get('pos', [0,0,0])
    n = monkey.Node()
    n.set_model(a)
    n.set_position(pos[0], pos[1], pos[2] if not auto_depth else 1-2*pos[1]/166.0 )
    if auto_depth:
        n.add_component(monkey.components.SierraController(z_func=settings.z_func))
    baseline = ciao.get('baseline')
    if baseline:
        game_state.wallz.append(baseline)
    return n
    # root.add(n)

def sprite(ciao):
    b = monkey.get_sprite(ciao['sprite'])
    auto_depth = ciao.get('auto_depth', False)
    pos = read(ciao, 'pos', [0, 0, 0])
    b.set_position(pos[0], pos[1], pos[2])
    if auto_depth:
        b.add_component(monkey.components.SierraController(z_func=settings.z_func))
    return b

def empty(ciao):
    a=monkey.Node()
    pos = read(ciao, 'pos', [0, 0, 0])
    a.set_position(pos[0], pos[1], pos[2])
    return a

def hotspot(ciao):
    h = monkey.Node()
    if 'aabb' in ciao:
        aabb = ciao['aabb']
        shape = monkey.shapes.AABB(aabb[0], aabb[1], aabb[2], aabb[3])
    else:
        shape = monkey.shapes.GenericPolygon(ciao['poly'])
    flag = ciao.get('flag', settings.CollisionFlags.foe)
    mask = ciao.get('mask', settings.CollisionFlags.player)
    print('ADDING HOTSPOT with mask',mask)
    h.add_component(monkey.components.Collider(flag, mask, 1, shape, batch='lines'))
    h.user_data = {
        'on_enter': ciao.get('on_enter', None),
        'on_leave': ciao.get('on_leave', None)
    }
    return h

def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')
    settings.items = monkey.read_data_file('items.yaml')
    settings.strings = monkey.read_data_file('strings.yaml')
    settings.actions = monkey.read_data_file('scripts.yaml')


def link(room, dir):
    room_target = ''
    pos_target = None
    if isinstance(room, str):
        room_target = room
    else:
        room_target = room['room']
        pos_target = room['pos']
    h = monkey.Node()
    shape = monkey.shapes.AABB(*link_aabb[dir])
    h.add_component(monkey.components.Collider(settings.CollisionFlags.foe, settings.CollisionFlags.player, 1,
        shape, batch='lines'))
    if pos_target:
        pos = pos_target
        on_enter_func = 'goto_room'
    else:
        pos = [2 if dir == 'e' else 314 if dir == 'w' else 0, 2 if dir == 'n' else 118 if dir == 's' else 0]
        on_enter_func = 'goto_room_y' if dir == 'e' or dir == 'w' else 'goto_room_x'
    h.user_data = {
        'on_enter': [on_enter_func, room_target, pos, dir]
    }
    return h


def west(ciao):
    return link(ciao.get('room'), 'w')

def east(ciao):
    return link(ciao.get('room'), 'e')

def north(ciao):
    return link(ciao.get('room'), 'n')

def south(ciao):
    return link(ciao.get('room'), 's')


def on_enter_hotspot(a, b, c):
    on_enter = b.user_data.get('on_enter')
    if on_enter:
        getattr(scripts, on_enter[0])(a, b, *on_enter[1:])


def on_leave_hotspot(a,b):
    on_leave = b.user_data.get('on_leave')
    if on_leave:
        getattr(scripts, on_leave[0])(a, b, *on_leave[1:])



def create_room(room):

    ce = monkey.CollisionEngine2D(80, 80)
    ce.add_response(0, 1, on_start=on_enter_hotspot, on_end=on_leave_hotspot)
    room.add_runner(ce)
    room.add_runner(monkey.Scheduler())
    room.add_runner(monkey.Clock())

    room_info = settings.rooms[settings.room]
    on_start = room_info.get('on_start')
    if on_start:
        room.on_start = getattr(scripts, on_start)

    zfunc = room_info.get('z_func', 'zfunc_default')
    settings.z_func = getattr(scripts, zfunc)
    #size = room_info['size']
    #print(' -- size:', size)
    # game_area = (316, 166)
    viewport = (2, 25, 316, 166)
    # mid_y = game_area[1] // 2
    cam = monkey.CamOrtho(316, 166,
                          viewport=viewport,
                          bounds_x=(158, 158), bounds_y=(83, 83))
    room.add_camera(cam)

    ui_cam = monkey.CamOrtho(320,200, viewport=(0,0,320,200), bounds_x=(160,160), bounds_y=(100,100))
    room.add_camera(ui_cam)

    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='sprites'))
    room.add_batch('ui', monkey.SpriteBatch(max_elements=10000, cam=1, sheet='sprites'))
    for batch in room_info.get('batches', []):
        room.add_batch(batch, monkey.SpriteBatch(max_elements=10000, cam=0, sheet=batch))
    room.add_batch('lines', monkey.LineBatch(max_elements=200, cam=0))
    room.add_batch('tri', monkey.TriangleBatch(max_elements=1000, cam=0))


    root = room.root()
    game_state.Ids.root = root.id


    game_node = monkey.Node()
    text_node = monkey.Node()
    #inventory_node = monkey.Node()
    game_state.Ids.game_node = game_node.id
    game_state.Ids.text_node = text_node.id
    #game_state.Ids.inventory_node = inventory_node.id
    root.add(game_node)
    root.add(text_node)
    #root.add(inventory_node)

    kb = monkey.components.Keyboard()
    kb.add(settings.Keys.restart, 1, 0, scripts.restart_room)
    kb.add(settings.Keys.inventory, 1, 0, inventory.show_inventory)
    kb.add(settings.Keys.view_item, 1, 0, inventory.show_view_item)
    game_node.add_component(kb)


    # display some text
    # test = monkey.Text(batch='ui', font='sierra', text="Ciao")
    # test.set_position(0, 24, 0)
    # text_node.add(test)

    # display some images


    # display a sprite
    # b = monkey.get_sprite('sprites/graham')
    # b.add_component(monkey.components.PlayerSierraController(half_width=2, z_func=settings.z_func, dir=settings.dir))
    # b.add_component(monkey.components.Collider(settings.CollisionFlags.player, settings.CollisionFlags.foe, 0, monkey.shapes.Point()))
    # b.set_position(settings.pos[0], settings.pos[1], 0)
    # game_state.Ids.player = b.id
    game_node.add(graham('graham', settings.pos[0], settings.pos[1]))

    # create parser
    parser = monkey.TextEdit(batch='ui', font='sierra', prompt='>', cursor='_', width=2000,pal=0, on_enter=engine.process_action)
    parser.set_position(0,24,0)
    text_node.add(parser)
    #settings.text_edit_node = parser.id

    # display some lines
    # c = monkey.Node()
    # c.set_model(monkey.models.from_shape('lines', monkey.shapes.Segment(0, 0, 10, 10), monkey.from_hex('ffffff'), monkey.FillType.Outline))
    # root.add(c)

    # d = monkey.Node()
    # d.set_model(monkey.models.from_shape('lines', monkey.shapes.PolyLine(points=[10, 0, 10, 50, 110, 60]),
    #                                      monkey.from_hex('ff0000'), monkey.FillType.Outline))
    # root.add(d)


    # make room
    for wall in room_info.get('walls', []):
        w = monkey.Node()
        w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=wall), batch='lines'))
        game_node.add(w)

    game_state.wallz = []
    for wall in room_info.get('walls_z', []):
        w = monkey.Node()
        w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=wall), batch='lines'))
        game_state.wallz.append(wall)
        game_node.add(w)


    if 'west' in room_info:
        game_node.add(link(room_info['west'], 'w'))
    if 'east' in room_info:
        game_node.add(link(room_info['east'], 'e'))
    if 'north' in room_info:
        game_node.add(link(room_info['north'], 'n'))
    if 'south' in room_info:
        game_node.add(link(room_info['south'], 's'))



    for item in room_info.get('items', []):
        f = globals().get(item['type'])
        condition = item.get('condition', None)
        if condition and not eval(condition):
            continue
        if f:
            node = f(item)
            if 'tag' in item:
                game_state.nodes[item['tag']] = node.id
            game_node.add(node)

    # place dynamic items
    print (' --- adding dynamic items...')
    for item, desc in settings.items['items'].items():
        if desc['room'] == settings.room:
            item_type = desc.get('type')
            if item_type:
                f = globals().get(item_type)
                if f:
                    node = f(desc)
                    game_node.add(node)
                    game_state.nodes[item] = node.id


