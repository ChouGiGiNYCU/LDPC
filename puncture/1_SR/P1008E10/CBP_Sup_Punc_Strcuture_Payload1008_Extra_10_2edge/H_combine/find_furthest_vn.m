function furthest_vns = find_furthest_vn(H, start_vn)
    % 此function vn idx start 為 0
    visited = false(1, H.n);
    visited(start_vn + 1) = true; % MATLAB 是 1-based，所以 +1

    q = {[start_vn]};
    furthest_vns = [start_vn];

    while ~isempty(q)
        curr_vn_set = q{1}; q(1) = [];
        last_node = curr_vn_set;
        next_vn_set = [];

        for i = 1:length(curr_vn_set)
            curr_vn = curr_vn_set(i);
            CNs = H.CN_2_VN_pos{curr_vn + 1}; % 取得此 VN 連接的 CN

            for cn = CNs
                neighbors = H.VN_2_CN_pos{cn + 1}; % 取得此 CN 連接的其他 VN
                for neighbor = neighbors
                    if ~visited(neighbor + 1)
                        next_vn_set(end+1) = neighbor; %#ok<AGROW>
                        visited(neighbor + 1) = true;
                    end
                end
            end
        end

        if ~isempty(next_vn_set)
            q{end+1} = unique(next_vn_set);
            furthest_vns = unique(next_vn_set); % 最後一層為最遠
        end
    end
end
