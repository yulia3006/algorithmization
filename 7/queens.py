def solve_n_queens(n):
    def is_safe(board, row, col):
        #проверяем безопасно ли разместить ферзя на позиции
        for i in range(row):
            if board[i] == col or \
               board[i] - i == col - row or \
               board[i] + i == col + row:
                return False
        return True
    def backtrack(row, board, result):
        if row == n:
            #если все ферзи размещены добавляем решение
            result.append(board.copy())
            return
        for col in range(n):
            if is_safe(board, row, col):
                board[row] = col
                backtrack(row + 1, board, result)
                board[row] = -1
    result = []
    board = [-1] * n
    backtrack(0, board, result)
    return result
def print_solutions(solutions, n):
    for sol in solutions:
        board = [['.'] * n for _ in range(n)]
        for row, col in enumerate(sol):
            board[row][col] = 'Q'
        for row in board:
            print(' '.join(row))
        print()
# пример работы
n = 6
solutions = solve_n_queens(n)
print_solutions(solutions, n)
