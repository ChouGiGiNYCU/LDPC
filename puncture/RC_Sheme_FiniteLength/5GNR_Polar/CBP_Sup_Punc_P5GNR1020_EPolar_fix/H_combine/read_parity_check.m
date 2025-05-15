function H = read_parity_check(filename)
    fid = fopen(filename, 'r');
    if fid == -1
        error('Cannot open file: %s', filename);
    end

    % 讀入基本參數
    H.n = fscanf(fid, '%d', 1); % variable nodes
    H.m = fscanf(fid, '%d', 1); % check nodes
    H.max_col_degree = fscanf(fid, '%d', 1);
    H.max_row_degree = fscanf(fid, '%d', 1);

    % 每個 VN 的 degree（即有幾個 CN 相連）
    H.max_col_arr = fscanf(fid, '%d', H.n)';

    % 每個 CN 的 degree（即有幾個 VN 相連）
    H.max_row_arr = fscanf(fid, '%d', H.m)';

    % CN_2_VN_pos: 每個 variable node 對應的 check nodes（原C++是 CN_2_VN_pos[i][j]）
    H.CN_2_VN_pos = cell(H.n, 1);
    for i = 1:H.n
        temp = fscanf(fid, '%d', H.max_col_degree)';
        temp = temp(temp > 0) - 1; % 轉成 0-based 且把 0 去掉
        H.CN_2_VN_pos{i} = temp;
    end

    % VN_2_CN_pos: 每個 check node 對應的 variable nodes
    H.VN_2_CN_pos = cell(H.m, 1);
    for i = 1:H.m
        temp = fscanf(fid, '%d', H.max_row_degree)';
        temp = temp(temp > 0) - 1; % 轉成 0-based 且把 0 去掉
        H.VN_2_CN_pos{i} = temp;
    end
    
    fclose(fid);

    H_matrix = zeros(H.m, H.n);
    for col = 1:H.n
        non_zero_positions = H.CN_2_VN_pos{col};
        % Fill the H_matrix
        for row=non_zero_positions.'
            row = row + 1;
            if row~=0
                H_matrix(row, col) = 1; % Set the appropriate positions to 1
            end
        end
    end
    H.mat = H_matrix;
end