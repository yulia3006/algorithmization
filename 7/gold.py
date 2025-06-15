def max_gold(n, M, weights):
    #создаем таблицу для хранения максимальных весов
    dp = [0] * (M + 1)

    for i in range(n):
        # обходим возможные веса в обратном порядке
        for j in range(M, weights[i] - 1, -1):
            dp[j] = max(dp[j], dp[j - weights[i]] + weights[i])

    return dp[M]

# Пример работы
n1, M1 = 5, 15
weights1 = [3, 8, 1, 2, 5]
print(max_gold(n1, M1, weights1))

n2, M2 = 5, 19
weights2 = [10, 10, 7, 7, 4]
print(max_gold(n2, M2, weights2))
