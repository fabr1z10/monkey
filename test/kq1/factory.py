import monkey
import settings
import scripts
import game_state
import inventory
import engine

link_width = 5
link_up = 120

link_aabb = {
    'w': [0, 0, link_width,0, link_width, 166, 0, 166],
    'e': [316 - link_width, 0, 316 ,0, 316, 166, 316 - link_width, 166],
    'n': [0, link_up, 316, link_up, 316,link_up + link_width,0, link_up + link_width],
    's': [0,0,316,0,316,link_width,0,link_width]
    #'w': [0, 5, 0, 166],
    #'e': [311, 316, 0, 166],
    #'s': [0, 316, 0, 1],
    #'n': [0, 316, 120, 125]
}



def graham(sprite, x, y, scale):
    b = monkey.get_sprite('sprites/' + sprite)
    b.add_component(monkey.components.PlayerSierraController(half_width=2, speed=100,z_func=settings.z_func, dir=settings.dir, skinWidth=1))
    b.add_component(monkey.components.Collider(settings.CollisionFlags.player, settings.CollisionFlags.foe | settings.CollisionFlags.foe_hotspot, 0, monkey.shapes.Point()))
    b.set_position(x, y, 0)
    b.scale=scale
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
    n = monkey.Node()
    pos = ciao.get('pos', [0, 0, 0])
    auto_depth = ciao.get('auto_depth', False)
    n.set_position(pos[0], pos[1], pos[2] if not auto_depth else 1 - pos[1] / 166.0)
    if 'batch' in ciao:
        a = monkey.models.Quad(ciao['batch'])
        a.add(ciao['quad'])
        n.set_model(a)


    if auto_depth:
        n.add_component(monkey.components.SierraController(z_func=settings.z_func))





    # wall = ciao.get('wall')
    # if wall:
    #     baseline = wall.get('baseline', False)
    #     shape = None
    #     if 'poly' in wall:
    #         pl =wall['poly']
    #         print('sucalo',pl)
    #         shape = monkey.shapes.Polygon(wall['poly'])
    #         if baseline:
    #             zw = wall['zw']
    #             abs_polyline = [zw[i] + pos[i%2] for i in range(0,len(zw))]
    #             game_state.wallz.append({'baseline': abs_polyline, 'id': n.id})
    #     elif 'polyline' in wall:
    #         pl =wall['polyline']
    #         shape = monkey.shapes.PolyLine(points=pl)
    #         if baseline:
    #             abs_polyline = [pl[i] + pos[i%2] for i in range(0,len(pl))]
    #             game_state.wallz.append({'baseline': abs_polyline, 'id': n.id})
    #     else:
    #         print('wall must have poly or polyline')
    #         exit(1)
    #     n.add_component(monkey.components.Collider(2, 0, 0, shape, batch='lines'))
    #     game_state.walkArea.addDynamic(n)




    #
    #
    # if baseline:
    #     if ciao.get('solid', True):
    #         m = [baseline[i] - pos[i % 2] for i in range(0, len(baseline))]
    #         n.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=m), batch='lines'))
    # if wall:
    #     #m = [wall[i] - pos[i % 2] for i in range(0, len(wall))]
    #     n.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.Polygon(wall), batch='lines'))
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
    elif 'poly' in ciao:
        shape = monkey.shapes.Polygon(ciao['poly'])
    else:
        shape = monkey.shapes.PolyLine(points=ciao['polyline'])
    flag = ciao.get('flag', settings.CollisionFlags.foe_hotspot)
    mask = ciao.get('mask', settings.CollisionFlags.player)
    tag = ciao.get('tag', 0)
    print(' -- added hotspot (', flag, ', ', mask, ', ', tag, ')')
    h.add_component(monkey.components.Collider(flag, mask, tag, shape, batch='lines'))
    h.user_data = {
        'on_enter': ciao.get('on_enter', None),
        'on_leave': ciao.get('on_leave', None)
    }
    return h

def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')
    settings.items = monkey.read_data_file('items.yaml')
    settings.strings = monkey.read_data_file('strings.yaml')
    #settings.actions = monkey.read_data_file('scripts.yaml')


# this is called for every node.
# every node can have a <area> section which defines a polygonal area
# you can define: who can access the area: no one, the player only, player and npcs

def getFunc(f):
    if isinstance(f, str):
        return getattr(scripts, f)
    elif isinstance(f, list):
        return getattr(scripts, f[0])(*f[1:])
    else:
        return f


def area(node, ciao):
    if 'baseline' in ciao:
        baseline = ciao['baseline']
        game_state.wallz.append({'baseline': baseline, 'id': node.id})
    if not 'area' in ciao:
        return
    a = ciao['area']
    access = a['access']
    assert(access in ['player', 'all', 'none'])
    shape = None
    if access == 'none':
        if 'poly' in a:
            poly = a['poly']
            game_state.walkArea.addPolyWall(poly)
            shape = monkey.shapes.Polygon(poly)
        elif 'polyline' in a:
            polyline = a['polyline']
            game_state.walkArea.addLinearWall(polyline)
            shape = monkey.shapes.PolyLine(points=polyline)
        print('FIIIFIF')
        node.add_component(monkey.components.Collider(2, 0, 0, shape, batch='lines'))
    else:
        bb = monkey.polyLineToPolygon(a['polyline'], 2) if 'polyline' in a else a['poly']
        shape = monkey.shapes.Polygon(bb)
        if access == 'player':
            # if only player can access here, we need to add the poly to the walkarea
            game_state.walkArea.addPolyWall(bb)
        if 'collide' in a:
            character = a.get('character', 'player')
            assert (character in ['player', 'npc'])
            mask = settings.CollisionFlags.player if character=='player' else settings.CollisionFlags.foe
            collider = monkey.components.Collider(settings.CollisionFlags.foe_hotspot, mask, 0,
                shape, batch='lines')
            for tag, r in a['collide'].items():
                collider.setResponse(tag,
                                     on_enter=getFunc(r.get('on_enter')),
                                     on_exit=getFunc(r.get('on_exit')),
                                     on_continue=getFunc(r.get('on_continue')))
            node.add_component(collider)




def link(room, dir):
    node = monkey.Node()
    x = None
    y = None
    x_bounds = [0, 316]
    y_bounds = [0, 166]
    if isinstance(room, str):
        room_target = room
    else:
        room_target = room['room']
        x = room.get('x', None)
        y = room.get('y', None)
        x_bounds = room.get('x_bounds', None)
        y_bounds = room.get('y_bounds', None)
    if not x:
        x = link_width + 1 if dir == 'e' else (316 - link_width - 1 if dir == 'w' else None)
    if not y:
        y = link_width + 1 if dir == 'n' else (link_up - link_width - 1 if dir == 's' else None)
    area(node, {'area': {'poly': link_aabb[dir], 'access': 'all',
                         'collide': {0: {'on_enter': ['goto_room', room_target, [x, y], dir, x_bounds, y_bounds]}}}})
    return node




# def on_enter_hotspot(c1, c2, c):
#     on_enter = c1.user_data.get('on_enter')
#     if on_enter
#         getattr(scripts, on_enter[0])(hotspot, character, *on_enter[1:])
#
#     on_enter = c2.user_data.get('on_enter')
#     if on_enter:
#         getattr(scripts, on_enter[0])(hotspot, character, *on_enter[1:])
#
# def on_leave_hotspot(hotspot, character):
#     on_leave = hotspot.user_data.get('on_leave')
#     if on_leave:
#         getattr(scripts, on_leave[0])(hotspot, character, *on_leave[1:])


def make_wall(points):
    a = []
    a.extend(points)
    a.extend(points[:2])
    w = monkey.Node()
    w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=a), batch='lines'))
    return w


def create_room(room):
    game_state.wallz = []
    game_state.nodes = dict()
    ce = monkey.CollisionEngine2D(80, 80)
    #ce.add_response(0, 0, on_start=on_enter_hotspot, on_end=on_leave_hotspot)
    room.add_runner(ce)
    room.add_runner(monkey.Scheduler())
    room.add_runner(monkey.Clock())
    root = room.root()

    room_info = settings.rooms[settings.room]
    wa = room_info['walkarea']

    poly = wa['poly']
    walkArea = monkey.WalkArea(poly, 2)

    if wa.get('solid', True):
        root.add(make_wall(poly))
    for hole in wa.get('holes', []):
        hp = hole['poly']
        walkArea.addPolyWall(hp)
        if hole.get('solid', True):
            root.add(make_wall(hp))
        else:
            root.add(hotspot(hole))
    game_state.walkArea = walkArea

    room.add_runner(walkArea)


    on_start = room_info.get('on_start')
    room.addOnStart(scripts.setup3d)
    if on_start:
        room.addOnStart(getattr(scripts, on_start))

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
    room.add_batch('tri2', monkey.TriangleBatch(max_elements=1000, cam=1))



    game_state.Ids.root = root.id


    game_node = monkey.Node()
    text_node = monkey.Node()

    menu_node = monkey.Node()
    menu_bar = monkey.Node()
    menu_bar.set_model(monkey.models.from_shape('tri2', monkey.shapes.AABB(0,320,0,8), (1,1,1,1), monkey.FillType.Solid))
    menu_bar.set_position(0,192,0)
    menu_node.add(menu_bar)
    score_label = monkey.Text(batch='ui', font='sierra', text='Ciao', anchor=monkey.ANCHOR_BOTTOMLEFT, pal=1)
    score_label.set_position(8, 0, 0)
    sound_label = monkey.Text(batch='ui', font='sierra', text='Ciao', anchor=monkey.ANCHOR_BOTTOMLEFT, pal=1)
    sound_label.set_position(240, 0, 0)
    menu_bar.add(score_label)
    menu_bar.add(sound_label)
    root.add(menu_node)
    game_state.Ids.score_label = score_label.id
    game_state.Ids.sound_label = sound_label.id



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
    scale_player = room_info.get('scale_player', 1.0)
    game_node.add(graham('graham', settings.pos[0], settings.pos[1], scale_player))

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
    # for wall in room_info.get('walls', []):
    #     w = monkey.Node()
    #     w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=wall), batch='lines'))
    #     wa.addLinearWall(wall)
    #     game_node.add(w)
    #
    # game_state.wallz = []
    # for wall in room_info.get('walls_z', []):
    #     w = monkey.Node()
    #     w.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=wall), batch='lines'))
    #     game_state.wallz.append(wall)
    #     game_node.add(w)


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
                #print('adding tag ',item['tag'], node.id)
                game_state.nodes[item['tag']] = node.id
            area(node, item)
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
                    area(node, desc)
                    game_state.nodes[item] = node.id



