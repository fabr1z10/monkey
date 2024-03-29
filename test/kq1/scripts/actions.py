import monkey
import game_state


def removeNode(id: str):
    return monkey.actions.CallFunc(lambda: monkey.get_node(game_state.nodes[id]).remove())
