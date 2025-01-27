function girth = findGirth(H_matrix)
    % 此函數用於計算稀疏矩陣的最小循環長度（girth）

    % 獲取矩陣尺寸
    [rows, cols] = size(H_matrix);

    % 構建鄰接圖
    graph = sparse(rows + cols, rows + cols);
    for i = 1:rows
        cols_connected = find(H_matrix(i, :) > 0);
        for j = 1:length(cols_connected)
            for k = j+1:length(cols_connected)
                graph(cols_connected(j), cols_connected(k) + rows) = 1;
                graph(cols_connected(k) + rows, cols_connected(j)) = 1;
            end
        end
    end

    % 初始化最小循環長度
    girth = Inf;

    % BFS 遍歷以找到最小循環
    for start = 1:rows + cols
        visited = -ones(rows + cols, 1); % 紀錄訪問
        visited(start) = 0; % 設置起始節點

        queue = [start];
        while ~isempty(queue)
            node = queue(1);
            queue(1) = [];

            neighbors = find(graph(node, :));
            for neighbor = neighbors
                if visited(neighbor) == -1
                    visited(neighbor) = visited(node) + 1;
                    queue = [queue, neighbor];
                elseif visited(neighbor) >= visited(node)
                    % 找到一個循環
                    girth = min(girth, visited(node) + visited(neighbor) + 1);
                end
            end
        end
    end

    % 如果沒有找到循環，設置為無窮大
    if girth == Inf
        girth = -1; % 表示沒有循環
    end
end
