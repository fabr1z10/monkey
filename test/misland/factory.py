import monkey
import settings




def init():
    settings.rooms = monkey.read_data_file('rooms.yaml')

def create_room(room):
    room.add_runner(monkey.CollisionEngine2D(80, 80))

    room_info = settings.rooms[settings.room]

    size = room_info['size']
    print(' -- size:', size)
    #game_area = (316, 166)
    viewport = (2, 25, 316, 166)
    #mid_y = game_area[1] // 2
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
                       width = 29*8)
    test.set_position(160, 100, 0)
    root.add(test)

    # display some images
    a = monkey.models.Quad(batch='001')
    a.add([0, 0, 316, 166])
    n = monkey.Node()
    n.set_model(a)
    #root.add(n)

    # display a sprite
    b = monkey.get_sprite('sprites/graham')
    b.add_component(monkey.SierraController(half_width=2, y_front=0, y_back=166))
    root.add(b)

    # display some lines
    # c = monkey.Node()
    # c.set_model(monkey.models.from_shape('lines', monkey.shapes.Segment(0, 0, 10, 10), monkey.from_hex('ffffff'), monkey.FillType.Outline))
    # root.add(c)

    d = monkey.Node()
    d.set_model(monkey.models.from_shape('lines', monkey.shapes.PolyLine(points=[10,0,10,50,110,60]), monkey.from_hex('ff0000'), monkey.FillType.Outline))
    root.add(d)

    l1 = monkey.Node()
    l1.add_component(monkey.components.Collider(2, 0, 0, monkey.shapes.PolyLine(points=[20,0,20,90]), batch='lines'))
    root.add(l1)
    #a.prova([1,2,3])
    #root.add(a)
    #test = monkey.Text('sprites/sierra',
    #                   "We, at Sierra, wish to thank you for playing King's Quest. We are very sorry that you did not succeed and hope you will fare better next time.\nGood luck.",
    #                   width = 29*8)