import monkey
import yaml
from sys import exit
from . import settings
from . import ui
from . import data

def evaluate(node):
    if isinstance(node, str) and node[0] == '@':
        return eval(node[1:])
    return node

def ciao(x, y):
    print('go to --> ',x, y)
    ui.check_delayed_func()
    script = monkey.Script(id="__player")
    script.add(monkey.actions.Walk(data.tag_to_id['player'], (x,y)))
    monkey.play(script)
    # if walkdir:
    #     script.add(monkey.actions.Turn(data.tag_to_id['player'], walkdir))
    # monkey.play(script)
    # id = data.tag_to_id.get(settings.player, None)
    # if id:
    #     monkey.get_node(id).sendMessage(id="goto", pos=(x,y))


def makeModel(info, item):
    a = None
    #print('pollo', info)
    batch = None
    anim = evaluate(info.get('anim'))
    if 'sprite' in info:
        spriteId = info['sprite']
        a = monkey.models.getSprite(spriteId)
        batch = info.get('batch', spriteId[:spriteId.find('/')])
    elif 'bg' in info:
        batch = info['bg']['batch']
        a = monkey.models.Quad(batch)
        a.add(info['bg']['quad'])
    item.set_model(a, batch=batch)
    if anim:
        item.setAnimation(anim)


def addMouseArea(info, node, item):
    im = info['mouse']
    item = info.get('item', item)
    camera = im.get('cam', 0)
    priority = im.get('priority', 1)
    if 'aabb' in im:
        shape = monkey.shapes.AABB(*evaluate(im['aabb']))
    node.add_component(monkey.components.MouseArea(shape, priority, camera,
        on_enter=ui.on_enter_item(item), on_leave=ui.on_leave_item, on_click=ui.execute_action, batch='line'))

def area(info, node):

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
    data.strings = monkey.read_data_file('strings.yaml')

def makeDebugPath(poly):
    a = monkey.Node()
    a.set_model(monkey.models.from_shape('line', monkey.shapes.Polygon(poly), (1,1,1,1), monkey.FillType.Outline))
    a.set_position(0,0,5)
    return a


def addWalkArea(room_info, room, game_node):
    wa = room_info['walkarea']
    poly = wa['poly']
    walkArea = monkey.WalkArea(poly, 2)
    game_node.add(makeDebugPath(poly))
    for hole in wa.get('holes', []):
        hp = hole['poly']
        game_node.add(makeDebugPath(hp))
        walkArea.addPolyWall(hp)
    room.add_runner(walkArea)
    data.wa = walkArea

def z_func(x, y):
    z = 1.0 - y / 136.0
    md = -1
    iwall = -1
    wall_id = -1
    for wall in data.baselines:
        wall_id += 1
        node = monkey.get_node(wall['id'])
        ba = wall['baseline']
        pos = (node.x, node.y)
        bl = [ba[i] + pos[i%2] for i in range(0, len(ba))]
        if x < bl[0] or x > bl[-2]:
            continue
        for i in range(0, len(bl) - 2, 2):
            if x >= bl[i] and x <= bl[i + 2]:
                yl = bl[i + 1] + ((bl[i + 3] - bl[i + 1]) / (bl[i + 2] - bl[i])) * (x - bl[i])
                if y < yl:
                    if md < 0 or md > (yl - y):
                        md = yl - y
                        iwall = wall_id
    if iwall != -1:
        zwall = monkey.get_node(data.baselines[iwall]['id']).z
        z += zwall + 1.0
    return z

def createItem(desc, item):
    node = monkey.Node()
    #data.tag_to_id[item] = node.id
    pos = desc.get('pos', [0, 0])
    z = desc.get('z', 0)
    auto_depth = desc.get('auto_depth', False)
    z = 1 - pos[1]/136.0 if auto_depth else z
    node.set_position(pos[0], pos[1], z)

    #if 'model' in desc:
    makeModel(desc, node)
    if 'baseline' in desc:
        baseline = desc['baseline']
        data.baselines.append({'baseline': baseline, 'id': node.id})
    if 'area' in desc:
        area(desc, node)
    if 'mouse' in desc and item != settings.characters[settings.player]:
        addMouseArea(desc, node, item)
    if desc.get('type', '') == 'character':
        dir = desc.get('direction')
        node.add_component(monkey.components.WalkableCharacter(200, z_func=z_func, direction=dir))
    if item == settings.characters[settings.player]:
        data.tag_to_id['player'] = node.id
        node.add_component(monkey.components.Follow(0))
    hasLight = getattr(data, "light_" + settings.room, True)
    if not hasLight:
        node.setPalette('dark')

    return node



def create_room(room):
    data.baselines = []
    root = room.root()
    data.tag_to_id = {}
    settings.action = settings.default_verb
    settings.item1 = None
    settings.item2 = None


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
    room.add_runner(monkey.Scheduler())


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
    for key, value in settings.verbs.items():
        t = monkey.Text('text', 'c64', data.strings[value['text']], pal='green')
        box_size = t.size
        on_click = getattr(ui, value['on_click']) if 'on_click' in value else ui.on_click_verb(key)
        t.add_component(monkey.components.MouseArea(monkey.shapes.AABB(0, box_size[0], -8, -8+box_size[1]), 0, 1,
            on_enter=ui.on_enter_verb, on_leave=ui.on_leave_verb, on_click=on_click, batch='line_ui'))
        t.set_position(value['pos'][0], value['pos'][1], 0)
        text_node.add(t)
    # first item in inventory is placed in (1, 21)

    inventory = monkey.Node()
    settings.id_inv = inventory.id
    text_node.add(inventory)
    new_kid_selector = monkey.Node()
    text_node.add(new_kid_selector)
    ui.refresh_inventory()




    # adding label for current action
    cact = monkey.Text('text', 'c64', data.strings[settings.verbs[settings.default_verb]['text']], pal='purple')
    cact.set_position(1, 53, 0)
    data.tag_to_id['label_action'] = cact.id
    text_node.add(cact)

    settings.id_game = game_node.id
    settings.id_text = text_node.id
    settings.id_newkid = new_kid_selector.id


    root.add(game_node)
    root.add(text_node)

    addWalkArea(room_info, room, game_node)

    # place static items
    for item in room_info.get('items', []):
        condition = item.get('condition', None)
        active = evaluate(item.get('active', True))

        if not active:
            continue

        if condition and not eval(condition):
            continue

        #print(item)
        game_node.add(createItem(item, None))

    # place dynamic items

    print (' -- adding dynamic items...')
    for item, desc in data.items.items():
        create = evaluate(desc.get('create', True))
        if not create:
            continue
        active = evaluate(desc.get('active', True))

        if desc['room'] == settings.room:
            node = createItem(desc, item)
            if not active:
                node.state = monkey.NodeState.ACTIVE if active else monkey.NodeState.INACTIVE
            #print('adding', item, active)
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