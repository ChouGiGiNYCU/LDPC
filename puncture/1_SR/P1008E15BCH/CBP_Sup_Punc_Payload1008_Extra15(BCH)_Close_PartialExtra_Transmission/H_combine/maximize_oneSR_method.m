function position = maximize_oneSR_method(H,puncture_bits_num)
    position = [];
    [rows,cols] = size(H);
    Remain_c = [1:rows]; % CNs - 連接的vn都沒被puncture過
    Fixed = []; % unpunctured vn node
    Index = []; % punctured vn node
    All_sc = [-1]; % the set of all survived cn(SCN)
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
    sentence = sprintf("Step1 Finish - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
    disp(sentence);
    %% step2
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
            % return ;
        end
        % 此 choose_vn 必定有一個 SCN 了
        % 選擇的 VN 所連接的SCN要越少越好 (影響越小) , init
        choose_vn = Candidate(1);
        VNs_set =  [choose_vn];
        Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
        d_SCN_num = length(intersect(Nei_choosen_vn_CNs,All_sc)); % vn 所連接的 SCN 數量
        for VN=Candidate
            Nei_VN_CNs =  transpose(find(H(:,VN)==1));
            Nei_SCN_num = length(intersect(Nei_VN_CNs,All_sc)); % 找出 SCN 數量
            if d_SCN_num > Nei_SCN_num
                VNs_set = [VN];
                d_SCN_num = Nei_SCN_num;
                Nei_choosen_vn_CNs = Nei_VN_CNs;
            elseif  d_SCN_num==Nei_SCN_num
                VNs_set = [VNs_set,VN];
            end
        end

        for choose_vn=VNs_set
            % check 如果這個choose_vn punc後 鄰近的 punc_vn 是否會變成 2-SR
            flag = false;
            for cn=transpose(find(H(:,choose_vn)==1))
                Punc_VNs = intersect(find(H(cn,:)==1),Index);
                for Punc_VN=Punc_VNs
                    SCNs_set = transpose(find(H(:,Punc_VN)==1)); % Punc_vn 所連接到cn
                    SCN_num = 0;
                    SCN_VNs = [];

                    for SCN=SCNs_set
                           Nei_SCN_VNs = setdiff(find(H(SCN,:)==1),Punc_VN);
                           if any(ismember(Nei_SCN_VNs,choose_vn))
                                PuncVN_SCN_chooseVN = Nei_SCN_VNs;
                           elseif ~any(ismember(Index,Nei_SCN_VNs)) && ~any(ismember(Nei_SCN_VNs,choose_vn)) % 假設此choose_vn不在這個SCN連接下，因為這個choose_vn要被punc
                               SCN_num = SCN_num + 1;
                               SCN_VNs = Nei_SCN_VNs;
                           end
                    end
                    % 如果這個Punc_VN只有一個 SCN ，那唯一SCN的 不能被幹掉
                    if SCN_num==0
                        Fixed = union(Fixed,PuncVN_SCN_chooseVN);
                        flag = true; % 此 choose_VN punc過後不會有其他vn變成2-SR     
                    end
                end
                
            end
            if flag==false % 代表此 choose_vn 可以被 punc 掉
                Index = union(Index,choose_vn);
                Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
                All_sc = union(All_sc,Nei_choosen_vn_CNs);
                Remain_c = setdiff(Remain_c,All_sc);
                break;
            end
        end
        
        if length(Index)==puncture_bits_num
            sentence = sprintf("Step2 - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
            disp(sentence);
            position = sort(Index);
            return;
        end
        
    end
    sentence = sprintf("Step2 Finish - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
    disp(sentence);
    %% step3
    % 最後一部 是把punctured vn 的SCN多的慢慢刪減，找到而外的1-SR node
    while length(Index)~=puncture_bits_num
        Candidate_c = [];
        % 初始化
        VN = Index(1);
        Nei_VN_SCNs = transpose(find(H(:,VN)==1));
        d_SCN_num = 0; % punc vn 所連接 1-SR CN 數量
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
                Candidate_c = SCNs_set;
            elseif SCNs_num==d_SCN_num
                Candidate_c = union(Candidate_c,SCNs_set);
            end
        end
        % 如果目前最多的SCN是1的話跳出來
        if d_SCN_num==1
            disp("Can't find anymore 1-SR SCN, since SCN for each VN is 1 \n");
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
        if isempty(Candidate)
            disp("Can't find anymore 1-SR SCN, since No Candidate_VN\n");
            break;
        end
        
        d_SCN_num = -1;
        % 選擇的 VN 所連接的SCN要越少越好 (影響越小)
        % 這些 Candidate 必須有兩個SCN(含)才行
        VNs_set = [];
        for VN=Candidate
            Nei_VN_CNs =  transpose(find(H(:,VN)==1));
            Nei_SCN_num = 0; % 找出 SCN 數量 
            for SCN=Nei_VN_CNs
                Nei_SCN_VNs = setdiff(find(H(SCN,:)==1),VN);
                if ~any(ismember(Index,Nei_SCN_VNs))
                    Nei_SCN_num = Nei_SCN_num + 1;
                end
            end
            if d_SCN_num==-1 && Nei_SCN_num>1
                d_SCN_num = Nei_SCN_num;
                VNs_set = [VN];
            elseif d_SCN_num > Nei_SCN_num && Nei_SCN_num>1 
                d_SCN_num = Nei_SCN_num;
                VNs_set = [VN];
                % Nei_choosen_vn_CNs = Nei_VN_CNs;
            elseif d_SCN_num == Nei_SCN_num
                VNs_set = union(VNs_set,VN);
            end
        end
        if isempty(VNs_set)
            disp("No VNs_set have more 2SCN !!\n");
            break;
        end
        
        for choose_vn=VNs_set
            % check 如果這個vn punc 鄰近的 punc vn 是否會變成 2-SR
            flag = false;
            for cn=transpose(find(H(:,choose_vn)==1))
                Punc_VNs = intersect(find(H(cn,:)==1),Index);
                for Punc_VN=Punc_VNs
                    SCNs_set = transpose(find(H(:,Punc_VN)==1));
                    SCN_num = 0;
                    SCN_VNs = [];

                    for SCN=SCNs_set
                           Nei_SCN_VNs = setdiff(find(H(SCN,:)==1),Punc_VN);
                           if ~any(ismember(Nei_SCN_VNs,choose_vn))
                                PuncVN_SCN_chooseVN = Nei_SCN_VNs;
                           end
                           if ~any(ismember(Index,Nei_SCN_VNs)) && ~any(ismember(Nei_SCN_VNs,choose_vn)) % 假設此choose_vn不在這個SCN連接下
                               SCN_num = SCN_num + 1;
                               SCN_VNs = Nei_SCN_VNs;
                           end
                    end
                    % 如果這個Punc_VN只有一個 SCN ，那唯一SCN的 不能被幹掉
                    if SCN_num==0
                        Fixed = union(Fixed,PuncVN_SCN_chooseVN);
                        flag = true; % 此 choose_VN punc過後不會有其他vn變成2-SR     
                    end
                end
                
            end
            if flag==false % 代表此 choose_vn 可以被 punc 掉
                Index = union(Index,choose_vn);
                Nei_choosen_vn_CNs = transpose(find(H(:,choose_vn)==1));
                All_sc = union(All_sc,Nei_choosen_vn_CNs);
            end
        end
        
        if length(Index)==puncture_bits_num
            sentence = sprintf("Step3 - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
            disp(sentence);
            position = sort(Index);
            return;
        end
        sentence = sprintf("Step3 - Find nonSCN connect vn to 1-SR !! - length : %d) !!",length(Index));
        disp(sentence);
    end
    position = sort(Index);
    return;
end