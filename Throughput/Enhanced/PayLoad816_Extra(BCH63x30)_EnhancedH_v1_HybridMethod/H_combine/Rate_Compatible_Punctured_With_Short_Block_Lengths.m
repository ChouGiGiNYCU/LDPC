function Punc_VNs = Rate_Compatible_Punctured_With_Short_Block_Lengths(H,punc_bits_num,Already_Punc_VNs)
    %% init
    Punc_VNs = [];
    [rows,cols] = size(H);
    VN2CN_pos = {};
    for VN=1:cols
        Nei_CNs = transpose(find(H(:,VN)==1)); 
        VN2CN_pos{VN} = Nei_CNs;
    end
    CN2VN_pos = {};
    for CN=1:rows
        Nei_VNs = find(H(CN,:)==1); 
        CN2VN_pos{CN} = Nei_VNs;
    end

    %% Step 0.0 
    % Cns set
    R = {};
    R{1} = []; %R0
    R{2} = []; %R1
    R_inf = 1:rows;
    % VNs set
    G = {};
    G{1} = []; %G0
    G{2} = []; %G1
    G_inf = 1:cols;
    W = zeros(1,rows);
    S = zeros(1,cols);
    k = 1;
    %% Pre - Process  Already Punc VNs 假設都是 1-SR
    % 從VN 裡面選擇 所連接的 CN degree 低的
    G_inf = setdiff(G_inf,Already_Punc_VNs);
    varrho = {};
    for VN=Already_Punc_VNs
        Nei_CNs = intersect(VN2CN_pos{VN},R_inf);
        C{VN}   = intersect(VN2CN_pos{VN},R_inf);
        min_row_degree = 1e9;
        Candidate_SCNs = [];
        for SCN=Nei_CNs
            if length(intersect(setdiff(CN2VN_pos{SCN},VN),Already_Punc_VNs))~=0
                % fprintf("%d have one SCN connect other Already Punc_VN!!\n",VN);
                continue;
            end
            varrho{CN} = intersect(CN2VN_pos{SCN},G_inf);
            row_degree = length(varrho{CN});
            if min_row_degree > row_degree 
                min_row_degree = row_degree;
                Candidate_SCNs = SCN;
            elseif min_row_degree == row_degree
                Candidate_SCNs = [Candidate_SCNs,SCN]; 
            end
        end
        
        rand_idx = randperm(length(Candidate_SCNs),1);
        choose_SCN = Candidate_SCNs(rand_idx);
        
        G{2} = union(G{2},VN); % 1-SR
    
        Nei_VNs = setdiff(varrho{choose_SCN},VN);
        G{1} = union(G{1},Nei_VNs); % G0
        
        Nei_VNs = varrho{choose_SCN};
        G_inf = setdiff(G_inf,Nei_VNs);
    
        R{2} = [R{2},choose_SCN];
        
        Nei_CNs = setdiff(C{VN},choose_SCN);
        R{1} = [R{1},Nei_CNs];
    
        Nei_CNs = C{VN};
        R_inf = setdiff(R_inf,Nei_CNs);
        % -----------------------------------
        for Nei_VN=setdiff(varrho{choose_SCN},VN)
            S(Nei_VN) = 1;
        end
        for Nei_VN = setdiff(CN2VN_pos{choose_SCN},VN)
            S(VN) = S(Nei_VN) + S(VN);
        end
        
    end
    
    %%
    while true
        %% Step 1.0 Group Column Indices
        % varrho - 每個 row 中的 vns
        varrho = {};
        % fprintf("R_inf : ");
        % fprintf("%d ",R_inf );
        % fprintf("\n");
        for CN=R_inf
            varrho{CN} = intersect(CN2VN_pos{CN},G_inf);
        end
        
        %% Step 2.0 Find Candidate Rows(根據degree低的)
        % 找出 R_inf 裡面的 CN degree 低的
        % omega is Candidate rows set (CNs set)
        omega = [];
        min_CN_degree = 1e9;
        for CN=R_inf
            CN_degree = length(varrho{CN});
            % if CN_degree==0
            %     continue;
            % end
            if min_CN_degree > CN_degree  
                min_CN_degree = CN_degree;
                omega = CN;
            elseif min_CN_degree == CN_degree  
                omega = union(omega,CN);
            end
        end
        % fprintf("omega :  ");
        % fprintf("%d ",omega);
        % fprintf("\n");
        %% Step 3.0 Group Row Indices(根據Cns set 去看 VNs )
        % C is CNs set 根據
        C = {};
        for CN=omega
            for VN=varrho{CN}
                % record each VN connect SCNs 
                C{VN} = intersect(VN2CN_pos{VN},R_inf);
            end
        end
        %% Step 3.1 Find Best Rows
        % Find subset omega
        min_SCN_degree = 1e9;
        subset_omega = [];
        for CN=omega
            for VN=varrho{CN}
                % find low SCN degree 
                SCN_degree = length(C{VN});
                % fprintf("SCN_degree : %d\n",SCN_degree);
                if min_SCN_degree>SCN_degree 
                    min_SCN_degree = SCN_degree;
                    subset_omega = [CN];
                elseif min_SCN_degree==SCN_degree 
                    subset_omega = union(subset_omega,CN);
                end
            end
        end
        % fprintf("subset omega :  ");
        % fprintf("%d ",subset_omega);
        % fprintf("\n");
        %% Step 3.2 Make a Set of Ordered Pairs
        % O is pairs with {(CN1*,VN1*),(CN2*,VN2*),(CN3*,VN3*)}
        O = {};
        idx = 1;
        for CN = subset_omega
            % 找到最小 SCN degree 的 VNs
            SCN_degrees = cellfun(@(v) length(v), C(varrho{CN}));
            min_deg = min(SCN_degrees);
            min_VNs = varrho{CN}(SCN_degrees == min_deg);
        
            for VN = min_VNs
                O{idx} = [CN, VN];
                idx = idx + 1;
            end
        end

        %% Step 3.3 Find the Best Pair
        choose_vn = [];
        choose_cn = [];
        min_tree_vn_degree = 1e9;
        
        for idx=1:length(O)
            set = O{idx};
            CN  = set(1);
            VN  = set(2);
            for Nei_VN=CN2VN_pos{CN}
                W(CN) = W(CN) + S(Nei_VN);
            end
            if min_tree_vn_degree > W(CN)
                min_tree_vn_degree  = W(CN);
                choose_cn = CN;
                choose_vn = VN;
            elseif min_tree_vn_degree == W(CN)
                choose_cn = [choose_cn,CN];
                choose_vn = [choose_vn,VN];
            end
        end
        % if more than 1 candidate -> random pick
        if length(choose_cn)==0
            fprintf("No CN can choose in %d-SR\n",k);
        else
            random_idx = randperm(length(choose_cn),1);
            choose_cn =  choose_cn(random_idx);
            choose_vn=  choose_vn(random_idx);
        end
        % fprintf("choose_cn :  ");
        % fprintf("%d ",choose_cn);
        % fprintf("\n");
        % fprintf("choose_vn :  ");
        % fprintf("%d ",choose_vn);
        % fprintf("\n");
        %% Step 4.0
        if length(choose_cn)~=0
        
            G{k+1} = union(G{k+1},choose_vn);
        
            Nei_VNs = setdiff(varrho{choose_cn},choose_vn);
            G{1} = union(G{1},Nei_VNs); % G0
            
            Nei_VNs = varrho{choose_cn};
            G_inf = setdiff(G_inf,Nei_VNs);
        
            R{k+1} = [R{k+1},choose_cn];
            
            Nei_CNs = setdiff(C{choose_vn},choose_cn);
            R{1} = [R{1},Nei_CNs];
        
            Nei_CNs = C{choose_vn};
            R_inf = setdiff(R_inf,Nei_CNs);
        
            for VN=setdiff(varrho{choose_cn},choose_vn)
                S(VN) = 1;
            end
            for VN = setdiff(CN2VN_pos{choose_cn},choose_vn)
                S(choose_vn) = S(choose_vn) + S(VN);
            end
        end
        %% Step 5.0 Check stop Condition
        if isempty(G_inf)
            break;
        end
        %% Step 5.1 Decision
        if  ~isempty(R_inf)
            continue;
        end
        %% Step 5.2 No more Undetermined Rows
        R_inf = [];
        % fprintf("R{1} : ");
        % fprintf("%d ",R{1});
        % fprintf("\n");
        
        for CN=R{1} 
            assert(isnumeric(R{1}), 'R{1} must be a numeric array')
            row_degree = length(intersect(CN2VN_pos{CN},G_inf));
            if row_degree>0
                R_inf = union(R_inf,CN);
            end
        end
        if isempty(R_inf)
            break
        end
        % fprintf("R_inf : ");
        % fprintf("%d ",R_inf);
        % fprintf("\n");
        %% Step 6.0
        k = k + 1;
        G{k+1} = [];
        R{k+1} = [];
    end
    
    fprintf("Find most %d-SR node\n",k);
    for i=2:k+1
        fprintf("%d-SR node number : %d\n",i-1,length(G{i}));
    end
    %% find punc node
    for i=2:k
        diff = punc_bits_num - length(Punc_VNs);
        VNs = G{i};
        Vns_num = length(VNs);
        if Vns_num < diff
            Punc_VNs = union(Punc_VNs,VNs);
        else
            while length(Punc_VNs)<punc_bits_num
                % init 
                R = 1:rows;
                % find low SCN degree
                min_SCN_degree = 1e9;
                Candidate_VNs = [];
                for VN=G{i}
                    SCN_degree = length(intersect(VN2CN_pos{VN},R));
                    if min_SCN_degree > SCN_degree 
                        min_SCN_degree = SCN_degree;
                        Candidate_VNs = VN;
                    elseif min_SCN_degree  == SCN_degree 
                        Candidate_VNs = union(Candidate_VNs,VN);
                    end
                end
                % check each vn degree
                Candidate_VNs_2 = [];
                min_col_degree = 1e9;
                for VN=Candidate_VNs
                    col_degree = length(VN2CN_pos{VN});
                    if min_col_degree > col_degree
                        min_col_degree = col_degree;
                        Candidate_VNs_2 = VN;
                    elseif min_col_degree == col_degree
                        Candidate_VNs_2 = union(Candidate_VNs_2,VN);
                    end
                end
                
                % random choose
                VN = Candidate_VNs_2(randperm(length(Candidate_VNs_2),1));
                Punc_VNs = union(Punc_VNs,VN);
                Punc_VNs = reshape(Punc_VNs, 1, []);  % 強制 row vector
                G{i} = setdiff(G{i},VN);
                R = setdiff(R,VN2CN_pos{VN});
            end
        end
    end

end



