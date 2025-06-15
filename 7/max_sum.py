def max_subarray_sum(arr, left, right):
    #если массив состоит из одного элемента
    if left == right:
        return arr[left]
    #находим середину массива
    mid = (left + right) // 2
    # рекурсивно находим максимальные суммы в левой и правой частях
    left_max = max_subarray_sum(arr, left, mid)
    right_max = max_subarray_sum(arr, mid + 1, right)
    # находим максимальную сумму, пересекающую середину
    left_cross = float('-inf')
    current_sum = 0
    for i in range(mid, left - 1, -1):
        current_sum += arr[i]
        left_cross = max(left_cross, current_sum)
    right_cross = float('-inf')
    current_sum = 0
    for i in range(mid + 1, right + 1):
        current_sum += arr[i]
        right_cross = max(right_cross, current_sum)

    cross_max = left_cross + right_cross
    #возвращаем максимальную сумму из трех вариантов
    return max(left_max, right_max, cross_max)

# Пример работы
n = 5
arr = [-10, 45, 3, 4, -9]
result = max_subarray_sum(arr, 0, n - 1)
print(result)
