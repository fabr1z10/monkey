import monkey
from . import settings
from . import scripts

def platform(x, y, **item):
  #print(' -- building platform: ', item)
  z = item.get('z', 0)
  size = item['size']
  pal = item.get('pal', 0)
  node = monkey.Node()
  node.set_position(x * settings.tile_size , y * settings.tile_size, z)
  quad = item.get('quad', None)
  solid = item.get('solid', True)
  if quad:
    model = monkey.models.Quad('tiles')
    model.add(item['quad'], size=(quad[2] * size[0], quad[3] * size[1]), repeat=(size[0], size[1]), pal=pal)
    node.set_model(model)
  if solid:
    node.add_component(monkey.components.Collider(
      shape=monkey.shapes.AABB(0, size[0]*settings.tile_size, 0, size[1]*settings.tile_size), flag=2, mask=0, tag=0, batch='lines'))
  return node

def brick(x, y, **item):
  node = _brick_common(x, y, **item)
  node.add_component(monkey.components.Platform(on_bump=scripts.bump_brick))
  return node

def mushroom(x, y, **item):
  def on_touch_mushroom(mushroom, player):
    mushroom.remove()
    if settings.state == 0:
      settings.state = 1
      scripts.update_mario(player)

  data = {**item, **{'model': 'tiles/mushroom'}}
  node = _generic_bonus(x, y, **data)#'tiles/mushroom', on_touch_mushroom)
  node.getLabelledComponent('collider').setResponse(settings.Tags.PLAYER, on_enter=on_touch_mushroom)
  return node

def _brick_common(x, y, **item):
  flag = item.get('flag', 2)
  sprite = item['sprite']
  node = monkey.get_sprite(sprite)
  z = item.get('z', 0)
  node.set_position(x * settings.tile_size, y * settings.tile_size, z)
  node.add_component(monkey.components.Collider(
    shape=monkey.shapes.AABB(0, settings.tile_size, 0, settings.tile_size), flag=flag, mask=0, tag=0,
    batch='lines', label='collider'))
  node.user_data['y'] = y*settings.tile_size
  return node
  #f(item, b)
  #  parent.add(b)

def _generic_bonus(x, y, **item):
  sprite = item['model']
  pal = item.get('pal', 'default')
  dir = item.get('dir', -1)
  node = monkey.get_sprite(sprite)
  node.set_position(x * settings.tile_size, y * settings.tile_size, 0)
  node.setPalette(pal)
  node.add_component(monkey.components.Controller2D(size=[10, 10, 0], batch='lines'))
  w = monkey.components.FoeWalk2D(tag='walker', max_speed=10, flip_h=False,
    acceleration=0.1, jump_height=settings.jump_height, dir=dir,
    time_to_jump_apex=settings.time_to_jump_apex)
  node.add_component(w)
  collider = monkey.components.SpriteCollider(settings.Flags.FOE, settings.Flags.PLAYER,
    settings.Tags.FOE, batch='lines', label='collider')
  node.add_component(collider)
  def f():
    w.setState(monkey.NodeState.ACTIVE)
  node.addBehavior('walk', f)
  return node

