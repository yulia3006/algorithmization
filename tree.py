class TreeNode:
    def __init__(self, value=0, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right

def max_path_sum(root):
    # вычисление максимального пути
    def max_gain(node):
        if not node:
            return 0

        # вычисление максимальные пути для левого и правого поддеревьев с помощью рекурсии
        left_gain = max(max_gain(node.left), 0)
        right_gain = max(max_gain(node.right), 0)

        # обновлние суммы максимального пути
        current_max = node.value + left_gain + right_gain
        max_sum[0] = max(max_sum[0], current_max)

        # возращение максимального пути, который можно получить
        return node.value + max(left_gain, right_gain)

    max_sum = [float('-inf')]
    max_gain(root)
    return max_sum[0]

#пример работы
root = TreeNode(2)
root.left = TreeNode(3)
root.right = TreeNode(4)
print(max_path_sum(root))

