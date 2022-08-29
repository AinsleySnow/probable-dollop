class node:
    def __init__(self) -> None:
        self.value = None
        self.left = None
        self.right = None
        self.parent = None

class splay:
    def __init__(self) -> None:
        self.root = None
    

def right_rotate(t: splay, y: node) -> None:
    x = y.left
    y.left = x.right

    if x.right:
        x.right.parent = y

    x.parent = y.parent
    if y.parent:
        if y == y.parent.right:
            y.parent.right = x
        else:
            y.parent.left = x
    else:
        t.root = x

    x.right = y
    y.parent = x

def left_rotate(t: splay, x: node) -> None:
    y = x.right
    x.right = y.left

    if y.left:
        y.left.parent = x

    y.parent = x.parent
    if x.parent:
        if x == x.parent.left:
            x.parent.left = y
        else:
            x.parent.right = y
    else:
        t.root = y

    y.left = x
    x.parent = y


def splay_node(t: splay, x: node) -> None:
    while x.parent:
        if x == x.parent.left:
            if not x.parent.parent:
                # zig 的情况
                right_rotate(t, x.parent)
            elif x.parent == x.parent.parent.left:
                # zig-zig 的情况
                right_rotate(t, x.parent.parent)
                right_rotate(t, x.parent)
            else:
                # zig-zag 的情况
                right_rotate(t, x.parent)
                left_rotate(t, x.parent)
        else:
            if not x.parent.parent:
                # zig 的情况
                left_rotate(t, x.parent)
            elif x.parent == x.parent.parent.right:
                # zig-zig 的情况
                left_rotate(t, x.parent.parent)
                left_rotate(t, x.parent)
            else:
                # zig-zag 的情况
                left_rotate(t, x.parent)
                right_rotate(t, x.parent)


def insert(t: splay, v: int) -> None:
    if not t.root:
        t.root = node()
        t.root.value = v
        return
    
    current = t.root

    while current:
        trailing = current
        if current.value > v:
            current = current.left
        else:
            current = current.right
    
    new_node = node()
    new_node.value = v
    
    if trailing.value > v:
        trailing.left = new_node
        new_node.parent = trailing
    else:
        trailing.right = new_node
        new_node.parent = trailing
    
    splay_node(t, new_node)


def search(t: splay, v: int) -> node:
    current = t.root
    while current:
        if current.value > v:
            current = current.left
        elif current.value < v:
            current = current.right
        else:
            break
    else:
        return None
    
    splay_node(t, current)
    return current


def min_node(x: node) -> node:
    current = x
    while current.left:
        current = current.left
    return current


def delete(t: splay, x: node) -> None:
    def transplant(t: splay, u: node, v: node) -> None:
        if u.parent == None:
            t.root = v
        elif u == u.parent.left:
            u.parent.left = v
        else:
            u.parent.right = v
        if v:
            v.parent = u.parent

    if not x.left:
        y = x.right
        transplant(t, x, x.right)
    elif not x.right:
        y = x.left
        transplant(t, x, x.left)
    else:
        y = min_node(x.right)
        if y.parent != x:
            transplant(t, y, y.right)
            y.right = x.right
            y.right.parent = y

        transplant(t, x, y)
        y.left = x.left
        y.left.parent = y
    
    splay_node(t, y)


def inorder_walk(n: node, depth: int = 0) -> None:
    if n:
        inorder_walk(n.left, depth + 1)
        print('%d, %d' % (n.value, depth))
        inorder_walk(n.right, depth + 1)


if __name__ == '__main__':
    nums = [5, 3, 1, 2, 4, 8, 9, 10, 7, 6]
    t = splay()
    for n in nums:
        insert(t, n)
    inorder_walk(t.root)
    n9 = search(t, 9)
    delete(t, n9)
    print()
    inorder_walk(t.root)
