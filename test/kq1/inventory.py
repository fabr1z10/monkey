import monkey
import game_state
import factory
import settings
import scripts

def show_inventory():
    _show_inventory(False)

def show_view_item():
    _show_inventory(True)

def _show_inventory(allow_keys: bool):
    monkey.get_node(game_state.Ids.game_node).state = monkey.NodeState.INACTIVE
    monkey.get_node(game_state.Ids.text_node).state = monkey.NodeState.INACTIVE
    inv = monkey.Node()
    game_state.inventory_selected = 0
    game_state.Ids.inventory_node = inv.id
    #inv.active=True
    inv.add(factory.make_text(19, 160, 196))
    inv.add(factory.make_text(20,160,4))
    kb = monkey.Node()
    k1 = monkey.components.Keyboard()
    if allow_keys:
        k1.add(settings.Keys.up, 1, 0, inventory_row_up)
        k1.add(settings.Keys.down, 1, 0, inventory_row_down)
        k1.add(settings.Keys.right, 1, 0, inventory_next)
        k1.add(settings.Keys.left, 1, 0, inventory_previous)
        k1.add(settings.Keys.enter, 1, 0, inventory_show_item)
    k1.add_fallback(inventory_exit)
    #k1.add(settings.Keys.inventory, 1, 0, scripts.show_inventory)
    kb.add_component(k1)
    inv.add(kb)
    y = 180
    if game_state.inventory:
        i = 0
        for item in game_state.inventory:
            inode = factory.make_text(settings.items['items'][item]['name'], 8 if i % 2 == 0 else 312, y, anchor=monkey.ANCHOR_TOPLEFT if i % 2 == 0 else monkey.ANCHOR_TOPRIGHT)
            inode.user_data = {'item_id': item}
            inv.add(inode)
            game_state.inventory_nodes.append(inode.id)
            i += 1
            if i % 2 == 0:
                y -= 8
        if allow_keys:
            monkey.get_node(game_state.inventory_nodes[0]).get_model().set_palette(2)
    else:
        inv.add(factory.make_text(21, 160, y))
    monkey.get_node(game_state.Ids.root).add(inv)

def inventory_row_up():
    if game_state.inventory_selected > 1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected -= 2
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_row_down():
    if game_state.inventory_selected < len(game_state.inventory)-1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected = min(len(game_state.inventory)-1, game_state.inventory_selected + 2)
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_next():
    print(game_state.inventory_selected, len(game_state.inventory))
    if game_state.inventory_selected < len(game_state.inventory)-1:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected += 1
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_previous():
    if game_state.inventory_selected > 0:
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(0)
        game_state.inventory_selected -= 1
        monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).get_model().set_palette(2)

def inventory_exit():
    game_state.inventory_nodes = []
    inv = monkey.get_node(game_state.Ids.inventory_node).remove()
    monkey.get_node(game_state.Ids.game_node).state = monkey.NodeState.ACTIVE
    monkey.get_node(game_state.Ids.text_node).state = monkey.NodeState.ACTIVE


def inventory_show_item():
    # get selected item
    item_id = monkey.get_node(game_state.inventory_nodes[game_state.inventory_selected]).user_data['item_id']
    inventory_exit()
    s = monkey.Script()
    #print (item_id)
    #print( game_state.items[item_id])
    scripts.show_item_detail(s, item_id)
    monkey.play(s)
