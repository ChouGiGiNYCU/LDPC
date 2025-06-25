function [ACE_score,Have_cycle_flag]= ACE(H,VN2CN_pos,CN2VN_pos,cycle_length,VN)
    [rows,cols] = size(H);
    Nei_CNs = VN2CN_pos{VN};
    % 初始化 queue 與結果
    paths = {};
    q = {}; 
    head = 1;
    tail = 1;
    q{tail} = VN;
    

    % BFS
    while head <= tail
        q_top = q{head};
        head = head + 1;
        L = numel(q_top);
        if L > cycle_length
            continue;
        end
        % 第一層 (L==1): VN -> CN
        if L == 1
            curr_vn = q_top(end);
            for Nei_CN = VN2CN_pos{VN}
                tail = tail + 1;
                q{tail} = [q_top, Nei_CN];
            end
        % 奇數層 (VN->CN->...->VN): extend到新的 CN
        elseif mod(L,2) == 1
            Curr_VN = q_top(end);
            Prev_CN = q_top(end-1);
            for Nei_CN = setdiff(VN2CN_pos{Curr_VN},Prev_CN)
                tail = tail + 1;
                q{tail} = [q_top,Nei_CN];
            end
        % 偶數層 (VN->CN->...->CN): extend到新的 VN
        else
            Curr_CN = q_top(end);
            Prev_VN = q_top(end-1);
            for Nei_VN = setdiff(CN2VN_pos{Curr_CN},Prev_VN)
                % 檢查是否回到起點形成 cycle
                if Nei_VN == q_top(1) && L == cycle_length
                    paths{end+1} = q_top;
                    continue;
                end
                tail = tail + 1;
                q{tail} = [q_top, Nei_VN];
            end
        end
    end
    path_size = numel(paths);
    ACE_score = 0;
    if path_size==0
        ACE_score = 1e9;
        Have_cycle_flag = 0;
        return;
    end
    Have_cycle_flag = 1;
    for i=1:path_size
        Path = paths{i};     % 取出第 i 條路徑
        VN_idx = 1;
        Path_size = length(Path);
        while VN_idx<Path_size
            Curr_VN = Path(VN_idx);
            Curr_VN_deg = length(VN2CN_pos{Curr_VN});
            ACE_score = ACE_score + Curr_VN_deg - 2; 
            VN_idx = VN_idx + 2;
        end
    end
    
end