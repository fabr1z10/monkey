import monkey

from . import data
from . import settings
from . import scripts
from . import builders


def init():
  data.worlds = monkey.read_data_file('worlds.yaml')
  data.strings = monkey.read_data_file('strings.yaml')

def text(text, x, y):
  t = monkey.Text('ui', 'mario', text)
  t.set_position(x, y, 0)
  return t

def player(x, y, speed, acceleration, jh, tja):
  ms = settings.mario_states[settings.state]
  node = monkey.get_sprite(ms['model'])
  node.set_position(x * settings.tile_size, y * settings.tile_size, 0)
  node.add_component(monkey.components.Controller2D(size=ms['size'], batch='lines', label='controller2d', mask_up=2|16))
  node.add_component(monkey.components.SpriteCollider(settings.Flags.PLAYER, settings.Flags.FOE,
    settings.Tags.PLAYER, batch='lines'))
  cl = monkey.components.PlayerWalk2D(max_speed=speed,
    acceleration=acceleration, jump_height=jh, time_to_jump_apex=tja)
  node.add_component(cl)
  node.add_component(monkey.components.Follow(0))

  def f():
    cl.setState(monkey.NodeState.INACTIVE)
  node.addBehavior('warp', f)
  settings.player_id = node.id
  return node

def base(item, x, y):
  z = item.get('z', 0)
  node = monkey.Node()
  node.set_position(x * settings.tile_size, y * settings.tile_size, z)
  return node
# solid = item.get('solid', None)
# if 'quad' in item:

#   for i in range(0, len(loc), 4):


def coin(item, parent):
  loc = item['loc']
  z = item.get('z', 0)
  def on_get_coin(coin, player):
    coin.remove()
  for i in range(0, len(loc), 2):
    node = monkey.get_sprite('tiles/coin_collect')
    node.set_position(loc[i] * settings.tile_size, loc[i+1] * settings.tile_size, z)
    collider = monkey.components.SpriteCollider(settings.Flags.FOE, settings.Flags.PLAYER,
                                                settings.Tags.FOE, batch='lines')
    collider.setResponse(settings.Tags.PLAYER, on_enter=on_get_coin)
    node.add_component(collider)
    parent.add(node)






def invisible_brick(item, parent):
  def f(item, b):
    bonus = item.get('bonus')
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_invisible_brick))
    b.user_data['bonus'] = bonus
  brick_common(item, parent, f, flag=16)





def brick_bonus(item, parent):
  def f(item, b):
    bonus = item.get('bonus')
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform_bonus))
    b.user_data['bonus'] = bonus
    b.user_data['hit'] = 1
  brick_common(item, parent, f)


def brick_coin(item, parent):
  def f(item, b):
    hits = item.get('hits', 1)
    b.add_component(monkey.components.Platform(on_bump=scripts.bump_platform))
    b.user_data['hit'] = hits
  brick_common(item, parent, f)

def tiled(item, parent):
   model = monkey.get_tiled(item['tiled'], **item.get('args', {}))
   solid = item.get('solid', None)
   z = item.get('z',0)
   loc = item['loc']
   for i in range(0, len(loc), 2):
     node = monkey.Node()
     node.set_position(loc[i] * 16, loc[i + 1] * 16, z)
     node.set_model(model)
     if solid:
       node.add_component(monkey.components.Collider(
         shape=monkey.shapes.AABB(0, solid[0]*16, 0, solid[1]*16), flag=2, mask=0, tag=0, batch='lines'))
     parent.add(node)

def warp(item, parent):
  loc =item['loc']
  for i in range(0, len(loc), 3):
    node = monkey.Node()
    node.set_position(loc[i] * 16, loc[i + 1] * 16, 0)
    collider = monkey.components.Collider(
      shape=monkey.shapes.AABB(0, 16, 0, 2), flag=settings.Flags.FOE, mask=settings.Flags.PLAYER, tag=0, batch='lines')
    collider.setResponse(settings.Tags.PLAYER, on_enter=scripts.on_enter_warp(loc[i+2]), on_exit=scripts.on_leave_warp)
    node.add_component(collider)
    parent.add(node)


def create_room(room):
  world = data.worlds[settings.room]
  bg_color = world.get('bg_color', [0, 0, 0])
  world_size = world['size']
  name = world['name']
  room.set_clear_color(*bg_color)
  device_width = settings.device_size[0]
  device_height = settings.device_size[1]
  hdw = device_width // 2
  hdh = device_height // 2
  cam = monkey.CamOrtho(device_width, device_height,
    viewport=(0, 0, device_width, device_height),
    bounds_x=(hdw, world_size[0] - hdw), bounds_y=(hdh, world_size[1] - hdh))
  cam_ui = monkey.CamOrtho(device_width, device_height,
    viewport=(0, 0, device_width, device_height),
    bounds_x=(hdw, hdw), bounds_y=(hdh, hdh))
  room.add_camera(cam)
  room.add_camera(cam_ui)
  ce = monkey.CollisionEngine2D(80, 80)
  #ce.add_response(settings.Tags.PLAYER, settings.Tags.FOE, scripts.playerVsFoeCallback)
  room.add_runner(ce)
  room.add_runner(monkey.Scheduler())
  root = room.root()
  kb = monkey.components.Keyboard()
  kb.add(settings.restart_key, 1, 0, lambda: monkey.close_room())
  kb.add(264, 1, 0, scripts.enter_warp)
  root.add_component(kb)
  settings.main_node = root.id



  room.add_batch('tiles', monkey.SpriteBatch(max_elements=10000, cam=0, sheet='tiles'))
  room.add_batch('lines', monkey.LineBatch(max_elements=2000, cam=0))
  room.add_batch('ui', monkey.SpriteBatch(max_elements=1000, cam=1, sheet='tiles'))
  root.add(text(data.strings['score'], 24, 232))
  root.add(text(str(data.score).zfill(6), 24, 224))
  root.add(text(data.strings['world'], 144, 232))
  root.add(text(data.strings['time'], 200, 232))
  root.add(text('*' + str(data.coins).zfill(2), 96, 224))
  root.add(text(name, 152, 224))
  items = world.get('items', [])
  root.add(player(settings.start_pos[0], settings.start_pos[1], settings.speed, 0.1,
    settings.jump_height, settings.time_to_jump_apex))
  for item in items:
    tipo = item['type']
    loc = item['loc']

    i = 0
    while i < len(loc):
      x = loc[i]
      y = loc[i+1]
      l = dict()
      if i+2 < len(loc) and isinstance(loc[i+2], dict):
        l = loc[i+2]
        i += 3
      else:
        i += 2
      a = {**item, **l}
      del a['loc']
      node = getattr(builders, tipo)(x, y, **(a))
      root.add(node)




    # z = item.get('z', 0)
    # solid = item.get('solid', None)
    # if 'quad' in item:
    #   loc = item['loc']
    #   for i in range(0, len(loc), 4):
    #     node = monkey.Node()
    #     node.set_position(loc[i] * 16, loc[i+1] * 16, z)
    #     repx = loc[i+2]
    #     repy = loc[i+3]
    #     quad = item.get('quad', [])
    #     if quad:
    #       model = monkey.models.Quad('tiles')
    #       model.add(item['quad'], size=(quad[2] * repx, quad[3] * repy), repeat=(repx, repy))
    #       node.set_model(model)
    #     if solid:
    #       node.add_component(monkey.components.Collider(
    #         shape=monkey.shapes.AABB(0, repx*16, 0, repy*16), flag=2, mask=0, tag=0, batch='lines'))
    #     root.add(node)
