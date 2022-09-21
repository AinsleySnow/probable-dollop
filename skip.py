from copy import deepcopy
from random import randint, random


class node:
    key: int
    value: int
    level: int
    forward: list
    
    def __init__(self, level: int, key: int, value: int) -> None:
        self.key = key
        self.value = value
        self.level = level
        self.forward = []


class skip:
    header: node
    level: int
    max_level: int

    def __init__(self) -> None:
        self.header = None
        self.level = None
        self.max_level = None


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
    update = [None] * s.level
    x = s.header
    for i in range(s.level - 1, -1, -1):
        while x.forward[i].key < key:
            x = x.forward[i]
        update[i] = deepcopy(x)
    
    x = x.forward[0]
    if x.key == key:
        x.value = new_value
    else:
        new_level = random_level(0.5, s.max_level)
        if new_level > s.level:
            for i in range(s.level + 1, new_level + 1):
                update[i] = s.header
            s.level = new_level
        x = node(new_level, key, new_value)
        for i in range(s.level):
            x.forward[i] = update[i].forward[i]
            update[i].forward[i] = x


def delete(s: skip, key: int, new_value: int) -> None:
    update = [None] * s.level
    x = s.header
    for i in range(s.level - 1, -1, -1):
        while x.forward[i].key < key:
            x = x.forward[i]
        update[i] = deepcopy(x)
    
    x = x.forward[0]
    if x.key == new_value:
        for i in range(s.level):
            if update[i].forward[i] != x:
                break
            update[i].forward[i] = x.forward[i]
        while s.level > 1 and (not s.header.forward[s.level]):
            s.level -= 1


if __name__ == '__main__':
    data = [(randint(0, 100)) for i in range(10)]
    skip_list = skip()
    skip_list.header = node(-1, -1, -1)
    skip_list.max_level = 16
    skip_list.level = 1

    for (i, d) in enumerate(data):
        insert(skip_list, i, d)
    x = skip_list.header
    while x.forward.get(0):
        print(f'{x.key}: {x.value}')
        x = x.forward[0]
    