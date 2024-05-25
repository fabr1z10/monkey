from . import settings
import monkey


class Line:
    def __init__(self, text=''):
        node = monkey.Text('text', 'c64', text.ljust(settings.COL_COUNT, ' '))
        monkey.get_node(settings.editor_id).add(node)
        self.id = node.id
        self.text = text
        # if breaks is true, continues on next line
        self.breaks = len(text) == settings.COL_COUNT  # False

    def __del__(self):
        print('cia', self.id)
        if monkey and self.id:
            monkey.get_node(self.id).remove()

    # corpo del distruttore

    def update(self, text):
        self.text = text
        line = monkey.get_node(self.id)
        line.updateText(self.text.rstrip().ljust(settings.COL_COUNT, ' '))
        self.breaks = len(text) == settings.COL_COUNT  # False
        print('QUI ', len(text), settings.COL_COUNT)


class TokenNode:
    def __init__(self):
        self.token = -1
        self.children = dict()

    def add(self, character, token):
        self.children[character] = token






def parseLine(line):
    pass
# def tokenize(s):
#     i = 0
#     tokens = []
#     in_quotes = False
#     verbatim = False
#     while i < len(s):
#         if verbatim:
#             verbatim &= s[i] != ":"
#             i += 1
#             continue
#         if s[i] == "\"":
#             in_quotes = not in_quotes
#             i += 1
#             continue
#         start = i
#         print('trying token starting @',start)
#         token = -1
#         node = settings.tokenRoot
#         while not in_quotes and i < len(s):
#             print('examine',s[i])
#             if s[i] not in node.children:
#                 break
#             node = node.children[s[i]]
#             token = node.token
#             i += 1
#         if token != -1:
#             tokens.append((start, i - start, token))
#             print('found token @', start, (i - start), ':', token)
#             if token == 131:        # data
#                 verbatim = True
#         else:
#             i = start+1
#         #    i += 1
#     i = 0
#     instruction = bytearray()
#     while i < len(s):
#         if tokens and tokens[0][0] == i:
#             instruction.append(tokens[0][2])
#             i += tokens[0][1]
#             tokens.pop(0)
#         else:
#             instruction.append(settings.tokens['ord'].get(s[i], ord(s[i])))
#             i += 1
#     return instruction


def save(file):
    print(' -- file: ', file)
    # address of next instruction
    address = 0x0801
    instructions = dict()

    m = bytearray()
    m += address.to_bytes(2, 'little')
    # f.write(m)
    i = 0
    while i < len(settings.lines):
        cl = settings.lines[i]
        l = cl.text
        i += 1
        if not l:
            continue
        if cl.breaks:
            l += settings.lines[i].text
            i += 1
        lc = 0
        while l[lc].isnumeric():
            lc += 1
        print('line number ends @ ', lc, l[:lc])
        inst = settings.tokenizer.tokenize(l[lc:].lstrip())
        print(' # bytes:', len(inst))
        # 4 bytes are occupied to hold address of next BASIC instruction and the line number
        # 1 byte is the 0 at the end of inst
        address += len(inst) + 4 + 1
        ln = int(l[:lc])
        line_number = ln.to_bytes(2, 'little')
        next_inst = address.to_bytes(2, 'little')
        instruction = bytearray()
        instruction += next_inst
        instruction += line_number
        instruction += inst
        instruction.append(0)
        print(' '.join(f'{x:02x}' for x in instruction))
        instructions[ln] = instruction
        # f.write(instruction)
    sorted_instructions = sorted(instructions)  # [x[1] for x in sorted(instructions.items())]
    for s in sorted_instructions:
        print(int(s), ': ', ' '.join(f'{x:02x}' for x in instructions[s]))
    # print(sorted_instructions)
    # do the proper writing
    with open(file, 'wb') as f:
        address = 0x0801
        m = address.to_bytes(2, 'little')
        f.write(m)
        for s in sorted_instructions:
            f.write(instructions[s])
        m = bytearray()
        m.append(0)
        m.append(0)
        f.write(m)



def openf(file):
    settings.lines.clear()
    settings.working_file = file
    settings.cx = 0
    settings.cy = 0
    li = settings.tokenizer.readBasicFile(file)
    for ln, inst in li.items():
        for j in range(0, 1 + len(inst) // settings.COL_COUNT):
            settings.lines.append(Line(inst[(40 * j):(40 * (j + 1))]))
    #print(settings.invtoken)
    # with open(file, 'rb') as f:
    #     settings.working_file = file
    #     address = f.read(2)
    #     a0 = int.from_bytes(address, 'little')
    #     if int(address[0]) == 0x01 and int(address[1]) == 0x08:
    #         print(' -- found BASIC Program.')
    #     while True:
    #         address = f.read(2)
    #         a1 = int.from_bytes(address, 'little')
    #         if a1 == 0:
    #             break
    #         length = a1 - a0
    #         # read line number
    #         ln = int.from_bytes(f.read(2), 'little')
    #         print('---')
    #         print('line number:', ln)
    #         print('length of current inst: ', length)
    #         data = f.read(length - 4)[:-1]  # drop last byte
    #         m = [str(ln), ' ']
    #         in_quotes = False
    #         for b in data:
    #             if int(b) == ord('"'):
    #                 in_quotes = not in_quotes
    #             used_map = settings.invord if in_quotes else settings.invtokens
    #             m.append(used_map.get(int(b), chr(b)))
    #         inst = ''.join(m)
    #         print(inst)
    #         a0 = a1
    #         for j in range(0, 1 + len(inst) // settings.COL_COUNT):
    #             settings.lines.append(Line(inst[(40 * j):(40 * (j + 1))]))
    reposition()
    # settings.line_count = len(settings.lines)
    # monkey.get_node(settings.editor_id).clear()
    print('# lines:', len(settings.lines))
    # for i in range(0, settings.LINE_COUNT):
    #     row_index = settings.doc_top + i
    #     if row_index >= len(settings.lines):
    #         break
    #     #print('ECCO:',settings.lines[row_index][0])
    #     merd=settings.lines[row_index][0].ljust(settings.col_count, '*')
    #     #print('eCCO:', merd +']')
    #     line = monkey.Text('text', 'c64', settings.lines[row_index][0].ljust(settings.col_count, ' '))
    #     settings.lines[row_index][1] = line.id
    #     line.set_position(0, settings.device_size[1]-8 - i*8, 0)
    #     monkey.get_node(settings.editor_id).add(line)
    # for a in settings.lines:
    #     print(a[0])


def reposition():
    for l in settings.lines:
        monkey.get_node(l.id).set_position(0, -1000, 0)

    for i in range(0, settings.COL_COUNT):
        row_index = settings.doc_top + i
        if row_index >= len(settings.lines):
            break
        node = monkey.get_node(settings.lines[row_index].id)
        node.set_position(0, settings.device_size[1] - 8 * (i + 1), 0)
