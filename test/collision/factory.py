import monkey
import settings



def key_up():
    player = monkey.get_node(settings.player_id)
    player.set_position(player.x, player.y+1, player.z)

def key_down():
    player = monkey.get_node(settings.player_id)
    player.set_position(player.x, player.y-1, player.z)

def key_left():
    player = monkey.get_node(settings.player_id)
    player.set_position(player.x-1, player.y, player.z)

def key_right():
    player = monkey.get_node(settings.player_id)
    player.set_position(player.x+1, player.y, player.z)


def on_enter(a,b,c):
    for key,value in b.get_children().items():
        value.get_model().set_color((1,0,0,1))

def on_leave(a,b):
    for key,value in b.get_children().items():
        value.get_model().set_color((1,1,1,1))

def aabb(x, y, w, h):
    node = monkey.Node()
    node.add_component(monkey.components.Collider(1, 0, 1, monkey.shapes.AABB(0, w, 0, h), batch='lines'))
    node.set_position(x,y,0)
    return node

def seg(x0, y0, x1, y1):
    node = monkey.Node()
    node.add_component(monkey.components.Collider(1, 0, 1, monkey.shapes.Segment(x0,y0,x1,y1), batch='lines'))
    return node

def convex(x, y, points):
    node = monkey.Node()
    node.add_component(monkey.components.Collider(1, 0, 1, monkey.shapes.ConvexPoly(points), batch='lines'))
    node.set_position(x,y,0)
    return node




def create_room(room):
    cam = monkey.CamOrtho(256, 240,
                          viewport=(0,0,256,240),
                          bounds_x=(128, 128), bounds_y=(120, 120))
    room.add_camera(cam)
    room.add_batch('lines', monkey.LineBatch(max_elements=2000, cam=0))

    ce = monkey.CollisionEngine2D(80, 80)
    ce.add_response(0, 1, on_start=on_enter, on_end=on_leave)
    room.add_runner(ce)
    root = room.root()
    player = monkey.Node()
    player.add_component(monkey.components.Collider(0, 1, 0, monkey.shapes.AABB(0,5,0,10), batch='lines'))
    player.set_position(128,120,0)
    settings.player_id = player.id
    root.add(player)

    root.add(aabb(30, 40, 50, 80))
    root.add(seg(100,10,150,70))
    root.add(convex(170,90,[0,0,10,-10,50,20,30,60]))

    kb = monkey.components.Keyboard()
    kb.add(265, 1, 0, key_up)
    kb.add(265, 2, 0, key_up)
    kb.add(264, 1, 0, key_down)
    kb.add(264, 2, 0, key_down)
    kb.add(263, 1, 0, key_left)
    kb.add(263, 2, 0, key_left)
    kb.add(262, 1, 0, key_right)
    kb.add(262, 2, 0, key_right)
    root.add_component(kb)
