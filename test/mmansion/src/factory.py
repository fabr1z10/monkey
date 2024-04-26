import monkey
import yaml
from sys import exit
from . import settings
from . import ui
from . import data

def ciao(x, y):
    print('go to --> ',x, y)
    id = data.tag_to_id.get(settings.player, None)
    if id:
        monkey.get_node(id).sendMessage(id="goto", pos=(x,y))


def makeModel(info, item):
    a = None
    batch = info['batch']
    if info['type'] == 'multisprite':
        a = monkey.models.MultiSprite()
        for node in info['nodes']:
            parent = node.get('parent', None)
            link = node.get('link', None)
            z = node.get('z', 0)
            a.addSprite(monkey.models.getSprite(batch+'/'+node['sprite']), parent=parent, link=link, z=z)
    elif info['type'] == 'bg':
        a = monkey.models.Quad(batch)
        a.add(info['quad'])

    item.set_model(a, batch=batch)

def area(info, node):
    if 'baseline' in info:
        baseline = ciao['baseline']
        data.wallz.append({'baseline': baseline, 'id': node.id})
    a = info['area']
    shape = None
    pos = info['pos']
    dynamic = a.get('dynamic', False)
    if 'poly' in a:
        poly = a['poly']
        # valid for static stuff!
        if not dynamic:
            world_poly = [poly[i] + pos[i % 2] for i in range(0, len(poly))]
            data.wa.addPolyWall(world_poly)
        shape = monkey.shapes.Polygon(poly)
    elif 'polyline' in a:
        polyline = a['polyline']
        if not dynamic:
            world_polyline = [polyline[i] + pos[i % 2] for i in range(0, len(polyline))]
            data.w.addLinearWall(world_polyline)
        shape = monkey.shapes.PolyLine(points=polyline)
    if dynamic:
        data.wa.addDynamic(node)
    a = monkey.Node()
    a.set_model(monkey.models.from_shape('line', shape, (1,1,1,1), monkey.FillType.Outline))
    a.set_position(0,0,5)
    node.add(a)

def init():
    data.rooms = monkey.read_data_file('rooms.yaml')
    data.items = monkey.read_data_file('items.yaml')

def addWalkArea(room_info, room, game_node):
    wa = room_info['walkarea']
    poly = wa['poly']
    walkArea = monkey.WalkArea(poly, 2)
    a = monkey.Node()
    a.set_model(monkey.models.from_shape('line', monkey.shapes.Polygon(poly), (1,1,1,1), monkey.FillType.Outline))
    a.set_position(0,0,5)
    game_node.add(a)
    room.add_runner(walkArea)
    data.wa = walkArea

def z_func(x, y):
    z = 1.0 - y / 136.0
    return z

def createItem(desc):
    node = monkey.Node()
    #data.tag_to_id[item] = node.id
    pos = desc.get('pos', [0, 0])
    auto_depth = desc.get('auto_depth', False)
    z = 1 - pos[1]/136.0 if auto_depth else 0
    node.set_position(pos[0], pos[1], z)

    if 'model' in desc:
        makeModel(desc['model'], node)
    if 'area' in desc:
        area(desc, node)
    if desc.get('type', '') == 'character':
        node.add_component(monkey.components.WalkableCharacter(200, z_func=z_func))
    return node



def create_room(room):
    root = room.root()

    if settings.room not in data.rooms:
        print(' -- Error! Cannot find room: ',settings.room)
        exit(1)
    room_info = data.rooms[settings.room]

    dw = settings.device_size[0]
    dh = settings.device_size[1]
    room_width = room_info['size'][0]
    room_height = settings.main_view_height

    hw = dw // 2
    hh = dh // 2
    vhh = settings.main_view_height // 2

    cam = monkey.CamOrtho(dw, settings.main_view_height,
                          viewport=(0, settings.main_view_y, dw, settings.main_view_height),
                          bounds_x=(hw, room_width - hw), bounds_y=(vhh, room_height - vhh))
    cam_ui = monkey.CamOrtho(dw, dh, viewport=(0, 0, dw, dh), bounds_x=(hw, hw), bounds_y=(hh, hh))

    room.add_camera(cam)
    room.add_camera(cam_ui)

    mm = monkey.MouseManager()
    mm.setFunc(0, ciao)
    mm.addCamera(0)
    mm.addCamera(1)
    room.add_runner(mm)

    game_node = monkey.Node()
    text_node = monkey.Node()

    room.add_batch('text', monkey.SpriteBatch(max_elements=10000, cam=1, sheet='petscii'))
    room.add_batch('sprites', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='sprites'))
    batch = room_info.get('batch', None)
    if batch:
        room.add_batch(batch, monkey.SpriteBatch(max_elements=10000, cam=0, sheet=batch))
    room.add_batch('line', monkey.LineBatch(max_elements=1000, cam=0))
    room.add_batch('line_ui', monkey.LineBatch(max_elements=1000, cam=1))

    # adding verbs
    for v in settings.verbs:
        t = monkey.Text('text', 'c64', v[0], pal=1)
        box_size = t.size
        t.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0, box_size[0], -8, -8+box_size[1]), 0, 1,
            on_enter=ui.on_enter_verb, on_leave=ui.on_leave_verb, batch='line_ui'))
        t.set_position(v[1], v[2], 0)
        text_node.add(t)

    root.add(game_node)
    root.add(text_node)

    addWalkArea(room_info, room, game_node)

    # place static items
    for item in room_info.get('items', []):
        condition = item.get('condition', None)
        if condition and not eval(condition):
            continue
        print(item)
        game_node.add(createItem(item))

    # place dynamic items
    data.tag_to_id= {}
    print (' -- adding dynamic items...')
    for item, desc in data.items.items():
        if desc['room'] == settings.room:
            print('adding', item)
            node = createItem(desc)
            game_node.add(node)
            data.tag_to_id[item] = node.id

            #item_type = desc.get('type')
            #if item_type:
            #    f = globals().get(item_type)
            #    if f:
            #        node = f(desc)
            #        game_node.add(node)
            #        area(node, desc)
            #        game_state.nodes[item] = node.id