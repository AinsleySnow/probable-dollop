from random import randint, random


class dyn_table:
    max_size: int
    data: list

    def __init__(self, msz: int) -> None:
        self.max_size = msz
        self.data = []
    
    def __getitem__(self, i: int):
        if len(self.data) - 1 <= i and i < self.max_size:
            self.data.extend([None] * (i - len(self.data) + 1))
            return self.data[i]
        elif i <= len(self.data) - 1:
            return self.data[i]
        raise KeyError()

    def __setitem__(self, i: int, v) -> None:
        if len(self.data) - 1 <= i and i < self.max_size:
            self.data.extend([None] * (i - len(self.data) + 1))
            self.data[i] = v
            return
        elif i <= len(self.data) - 1:
            self.data[i] = v
            return
        raise KeyError()


class node:
    key: int
    value: int
    level: int
    forward: dyn_table
    
    def __init__(self, level: int, key: int, value: int, max_level: int) -> None:
        self.key = key
        self.value = value
        self.level = level
        self.forward = dyn_table(max_level)


class skip:
    header: node
    level: int
    max_level: int

    def __init__(self, h: node, l: int, ml: int) -> None:
        self.header = h
        self.level = l
        self.max_level = ml


def search(s: skip, key: int) -> node:
    x = s.header
    for i in range(s.level, 0, -1):
        while x.forward[i].key < key:
            x = x.forward[i]
    
    x = x.forward[0]
    if x.key == key:
        return x
    return None


def random_level(p: float, max_level: int) -> int:
    v = 1
    while random() < p and v < max_level:
        v += 1
    return v


def insert(s: skip, key: int, new_value: int) -> None:
    update = dyn_table(s.max_level)
    x = s.header
    for i in range(s.level - 1, -1, -1):
        while x.forward[i] and x.forward[i].key < key:
            x = x.forward[i]
        update[i] = x
    
    x = x.forward[0]
    if x and x.key == key:
        x.value = new_value
    else:
        new_level = random_level(0.5, s.max_level)
        if new_level > s.level:
            for i in range(s.level, new_level):
                update[i] = s.header
            s.level = new_level
        x = node(new_level, key, new_value, s.max_level)
        for i in range(s.level):
            if update[i]:
                x.forward[i] = update[i].forward[i]
                update[i].forward[i] = x


def delete(s: skip, key: int) -> None:
    update = dyn_table(s.max_level)
    x = s.header
    for i in range(s.level - 1, -1, -1):
        while x.forward[i] and x.forward[i].key < key:
            x = x.forward[i]
        update[i] = x
    
    x = x.forward[0]
    if x and x.key == key:
        for i in range(s.level):
            if update[i].forward[i] != x:
                break
            update[i].forward[i] = x.forward[i]
        while s.level > 1 and (not s.header.forward[s.level]):
            s.level -= 1


if __name__ == '__main__':
    data = [(randint(0, 100)) for i in range(10)]
    skip_list = skip(node(-1, -1, -1, 16), 1, 16)

    for (i, d) in enumerate(data):
        insert(skip_list, i, d)
    x = skip_list.header
    while x:
        print(f'{x.key}: {x.value}')
        x = x.forward[0]

    print()

    delete(skip_list, 2)
    delete(skip_list, 5)
    x = skip_list.header
    while x:
        print(f'{x.key}: {x.value}')
        x = x.forward[0]