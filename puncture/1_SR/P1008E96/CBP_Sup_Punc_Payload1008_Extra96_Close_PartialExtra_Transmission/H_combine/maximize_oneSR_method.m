function position = maximize_oneSR_method(H,puncture_bits_num)
    position = [];
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
        
        same_VNs = setdiff(1:cols,VN);
        for SCN=Nei_VN_SCN
            Nei_SCN_VNs = find(H(SCN,:)==1);
            Candidate = setdiff(Candidate,Nei_SCN_VNs);
            same_VNs = intersect(same_VNs,Nei_SCN_VNs);
        end
        Fixed = union(Fixed,same_VNs); % 如果此 punc VN 的Nei_SCN_VNs 都有共用一個 vn的話，此 vn 不能被 punc
    end
    clear Candidate;

    % puncture bits 已滿足的話提早跳出來
    if length(Index)>=puncture_bits_num
        sentence = sprintf("Step1 - Straight forward pick 1-SR node is enough(Total 1-SR node : %d) !!",length(Index));
        disp(sentence);
        selected_indices = randperm(length(Index), puncture_bits_num); % 產生 k 個隨機索引
        position = sort(Index(selected_indices));
        return;
    end
    
    sentence= sprintf('Remain_c : %s \n',num2str(Remain_c));
    disp(sentence);
    
    % Remain_c 所連接到的 vn 都是 unpuncture 
    while ~isempty(Remain_c)
        % 把沒有連接到 puncture vn 的 cn ， 對應連接的 vn 再給考慮進來一次
        Candidate = [];
        
        for cn=Remain_c
            Nei_cn_vn = find(H(cn,:)==1);
            Candidate = union(Candidate,Nei_cn_vn);
            Candidate = reshape(Candidate, 1, []);  % → [1 2]
        end
        % Fixed 表示不能動的 vn node(動的話，就會有puncture node失去最後一個SCN)
        Candidate = setdiff(Candidate,Fixed);
        Candidate = setdiff(Candidate,Index);
        if isempty(Candidate) 
            break;
        end
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
        % 找出 choose_vn 中SCN是否只剩下一個，必須保留一個 SCN 下來
        SCNs = 0;
        Nei_CN_VNs = [];
        for cn=Nei_choosen_vn_CNs
            Nei_VNs = setdiff(find(H(cn,:)==1),choose_vn);
            if ~any(ismember(Index,Nei_VNs)) % Nei_VNs都沒有再punc裡面，代表有一個SCNs
               SCNs = SCNs + 1;
               Nei_CN_VNs = Nei_VNs;
            end
        end

        if SCNs==0 % 沒有SCN ， 這個vn不能被punc
            Fixed = union(Fixed,choose_vn);
            continue;    
        end
        
        Index = union(Index,choose_vn);
        All_sc = union(All_sc,Nei_choosen_vn_CNs);
        
        if SCNs==1 % VN只剩下一個SCN
            Fixed = union(Fixed,Nei_CN_VNs);
            Remain_c = setdiff(Remain_c,All_sc);
        end
        
        if length(Index)==puncture_bits_num
            sentence = sprintf("Step2 - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
            disp(sentence);
            position = sort(Index);
            return;
        end
        
    end
    
    % 最後一部 是把punctured vn 的SCN多的慢慢刪減，找到而外的1-SR node
    while length(Index)~=puncture_bits_num
        % 初始化
        VN = Index(1);
        Nei_VN_SCNs = transpose(find(H(:,VN)==1));
        d_SCN_num = 0; % punc vn 所連接 1-SR CN 數量
        Candidate_c = [];
        for SCN=Nei_VN_SCNs
            Nei_SCN_VNs = setdiff(find(H(SCN,:)==1),VN);
            if ~any(ismember(Index,Nei_SCN_VNs))
                d_SCN_num = d_SCN_num + 1;
                Candidate_c = [Candidate_c,SCN];
            end
        end
        % 每個 punc vn都去找對應多少 SCN(找出最多的SCN)
        for vn=Index
            Nei_SCNs = transpose(find(H(:,vn)==1));
            SCNs_num = 0;
            SCNs_set = [];
            for SCN=Nei_SCNs
                Nei_VNs = setdiff(find(H(SCN,:)==1),vn);
                if ~any(ismember(Index,Nei_VNs))
                    SCNs_num = SCNs_num + 1;
                    SCNs_set = [SCNs_set,SCN];
                end
            end
            if SCNs_num > d_SCN_num
                d_SCN_num =  SCNs_num;
                Candidate_c = [];
                Candidate_c = SCNs_set;
            elseif SCNs_num==d_SCN_num
                Candidate_c = union(Candidate_c,SCNs_set);
            end
        end
        % 如果目前最多的SCN是1的話跳出來
        if d_SCN_num==1
            break;
        end
        Candidate = [];
        for cn=Candidate_c
            VNs = find(H(cn,:)==1);
            Candidate = union(Candidate,VNs);
            Candidate = reshape(Candidate, 1, []);  % → [1 2]
        end
        Candidate = setdiff(Candidate,Fixed);
        Candidate = setdiff(Candidate,Index);

        % 選擇的 VN 所連接的SCN要越少越好 (影響越小)
        choose_vn = Candidate(1);
        VNs_set =[choose_vn];
        Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
        d_SCN_num = length(intersect(Nei_choosen_vn_CNs,All_sc)); % vn 所連接的 SCN 數量
        for VN=Candidate
            Nei_VN_CNs =  transpose(find(H(:,VN)==1));
            Nei_SCN_num = length(intersect(Nei_VN_CNs,All_sc)); % 找出 SCN 數量
            if d_SCN_num > Nei_SCN_num
                choose_vn = VN;
                VNs_set = [choose_vn];
                d_SCN_num = Nei_SCN_num;
                Nei_choosen_vn_CNs = Nei_VN_CNs;
            elseif d_SCN_num == Nei_SCN_num
                VNs_set = union(VNs_set,VN);
            end
        end

        for choose_vn=VNs_set
            % 找出 choose_vn 中SCN是否只剩下一個，必須保留一個 SCN 下來
            SCNs = 0;
            Nei_CN_VNs = [];
            Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
            for cn=Nei_choosen_vn_CNs
                Nei_VNs = setdiff(find(H(cn,:)==1),choose_vn);
                if ~any(ismember(Index,Nei_VNs)) % Nei_VNs都沒有再punc裡面，代表有一個SCNs
                   SCNs = SCNs + 1;
                   Nei_CN_VNs = Nei_VNs;
                end
            end
    
            if SCNs==0 % 沒有SCN ， 這個vn不能被punc
                Fixed = union(Fixed,choose_vn);
                continue;    
            end

            % check 如果這個vn punc 鄰近的 punc vn 是否會變成 2-SR
            flag = false;
            for cn=transpose(find(H(:,choose_vn)==1))
                Punc_VNs = intersect(find(H(cn,:)==1),Index);
                for VN=Punc_VNs
                    SCNs_set = transpose(find(H(:,VN)==1))
                    SCN_num = 0;
                    for SCN=SCNs_set
                           Nei_SCN_VNs = setdiff(find(H(SCN,:)==1),VN);
                           if ~any(ismember(Index,Nei_SCN_VNs))
                               SCN_num = SCN_num + 1; 
                           end
                    end
                    if SCN_num==1
                        flag = true;
                    end
                end
                if flag==true
                    break;
                end
            end
            if flag==false
                
                Index = union(Index,choose_vn);
                All_sc = union(All_sc,Nei_choosen_vn_CNs);
                if SCNs==1 % VN只剩下一個SCN
                    Fixed = union(Fixed,Nei_CN_VNs);
                end
                break;
            end
        end
        
        if length(Index)==puncture_bits_num
            sentence = sprintf("Step3 - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
            disp(sentence);
            position = sort(Index);
            return;
        end

    end
    position = sort(Index);
    return;
end