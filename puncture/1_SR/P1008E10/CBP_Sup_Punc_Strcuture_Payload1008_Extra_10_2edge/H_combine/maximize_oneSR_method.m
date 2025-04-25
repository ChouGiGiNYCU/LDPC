function position = maximize_oneSR_method(H,puncture_bits_num)
    position = [];
    H = H.mat;
    [rows,cols] = size(H);
    Remain_c = [1:rows]; % CNs - 連接的vn都沒被puncture過
    Fixed = []; % unpunctured vn node
    Index = []; % punctured vn node
    All_sc = []; % the set of all survived cn(SCN)
    Candidate = [1:cols]; % VNs 可以被punctured的vn set 
    while ~isempty(Candidate)
        idx = randi(length(Candidate)); % produce 1:size(Candidate) random integer
        VN = Candidate(idx);
        Nei_VN_SCN = transpose(find(H(:,VN)==1)); % puncture vn node 的 scn 
        Index = union(Index,VN); % punctured node union
        All_sc = union(All_sc,Nei_VN_SCN);
        Remain_c = setdiff(Remain_c, All_sc);

        for SCN=Nei_VN_SCN
            Nei_SCN_VNs = find(H(SCN,:)==1);
            Candidate = setdiff(Candidate,Nei_SCN_VNs);
        end
    end
    
    % puncture bits 已滿足的話提早跳出來
    if length(Index)>=puncture_bits_num
        sentence = sprintf("Step1 - Straight forward pick 1-SR node is enough(Total 1-SR node : %d) !!",length(Index));
        disp(sentence);
        selected_indices = randperm(length(Index), puncture_bits_num); % 產生 k 個隨機索引
        position = sort(Index(selected_indices));
        return;
    end
    
    % sentence= sprintf('Remain_c : %s \n',num2str(Remain_c));
    % disp(sentence);
    
    while ~isempty(Remain_c)
        % 把沒有連接到 puncture vn 的 cn ， 對應連接的 vn 再給考慮進來一次
        Candidate = [];
        for cn=Remain_c
            Nei_cn_vn = find(H(cn,:)==1);
            Candidate = union(Candidate,Nei_cn_vn);
        end
        % Fixed 表示不能動的 vn node(動的話，就會有puncture node失去最後一個SCN)
        Candidate = setdiff(Candidate,Fixed);

        % 選擇的 VN 所連接的SCN要越少越好 (影響越小)
        choose_vn = Candidate(1);
        Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
        d_SCN_num = length(intersect(Nei_choosen_vn_CNs,All_sc)); % vn 所連接的 SCN 數量
        for VN=Candidate
            Nei_VN_CNs =  transpose(find(H(:,VN)==1));
            Nei_SCN_num = length(intersect(Nei_VN_CNs,All_sc)); % 找出 SCN 數量
            if d_SCN_num > Nei_SCN_num
                choose_vn = VN;
                d_SCN_num = Nei_SCN_num;
                Nei_choosen_vn_CNs = Nei_VN_CNs;
            end
        end
        % 找出 choose_vn 中SCN是否只剩下一個(因為現在選的 choose_vn 所連接的 CN 一部分是已經是其他puncture
        % VN 的 SCN)，避免 choose_vn 變成 2-SR，必須保留一個 SCN 下來
        remain_SCN = setdiff(Nei_choosen_vn_CNs,All_sc);

        Index = union(Index,choose_vn);
        All_sc = union(All_sc,Nei_choosen_vn_CNs);

        if length(remain_SCN)==1
            CN = remain_SCN(1);
            Nei_CN_VNs = find(H(CN,:)==1);
            Remain_c = setdiff(Remain_c,All_sc);
            Fixed = union(Fixed,Nei_CN_VNs);
        end

        if length(Index)>puncture_bits_num
            disp("Step2 - Choose Remain_c(without punctured node) Connecting vn node !!");
            position = Index;
        end
    end
    % 最後一部 是把punctured vn 的SCN多的慢慢刪減，找到而外的1-SR node
    % 未完待續
end