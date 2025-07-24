function position = Puncturing_Algorithm_For_NonGreedy_Method(H,puncture_bits_num)
    %% init 
    Prior_Candidate_Vns = [];
    [rows,cols] = size(H);
    cn_deg = zeros(1,rows); % cn degree
    CN2VN_pos = {}; %record each row that have 1's col position
    for r=1:rows
        col_vns = find(H(r,:)==1);
        CN2VN_pos{end+1} = col_vns;
        cn_deg(r) = size(col_vns,2);
    end

    VN2CN_pos = {}; %record each col that have 1's row position
    vn_deg = zeros(1,cols); % vn degree
    for c=1:cols
        row_cns = transpose(find(H(:,c)==1));
        VN2CN_pos{end+1} = row_cns; 
        vn_deg(c) = size(row_cns,2);
    end
        
    V_u = 1:cols;
    C_u = 1:rows;
    S_v2c = zeros(cols,rows);
    for VN=V_u
        for Nei_CN=VN2CN_pos{VN}
            S_v2c(VN,Nei_CN) = 1;
        end
    end

    Cr = []; % reserved check node
    Vp_2_Cr_map = zeros(1,cols);
    %% Algorithm 1 - Find prior Candidate VNs for puncturing
    while length(Prior_Candidate_Vns)~=puncture_bits_num 
        %% step1 - Count W(c->v) 、 W(c)
        % W(c->v) : unpunc vn 的數量 for c in C(v) from v
        % W(c)    : unpunc vn 的數量 在 recovery tree (root 為 c)
        W_c2v = zeros(rows,cols);
        W_c   = zeros(1,rows);
        for CN=C_u
            Nei_VNs = CN2VN_pos{CN};
            for VN=Nei_VNs
                for Non_VNs_VN=setdiff(Nei_VNs,VN)
                    W_c2v(CN,VN) = W_c2v(CN,VN) + S_v2c(Non_VNs_VN,CN); 
                end
            end
            
            for VN=Nei_VNs
                W_c(CN) = W_c(CN) + S_v2c(VN,CN);
            end
        end
        %% step2 -  找出 check node candidate
        % Ψ1 - cn with 最小的W(cn) , \forall c in C_u
        % Ψ2 - cn with 最小的d(cn) , \forall c in Ψ1
        % Ψ3 - cn with 最小的d_p(cn) , \forall c in Ψ2
        phi_1 = [];
        min_Wc = 1e9;
        for CN=C_u
            if min_Wc > W_c(CN)
                min_Wc = W_c(CN);
                phi_1 = [CN];
            elseif min_Wc == W_c(CN)
                phi_1(end+1) = CN;
            end
        end
        
        each_cn_deg = cn_deg(phi_1);
        min_each_cn_deg = min(each_cn_deg);
        idx = find(each_cn_deg==min_each_cn_deg);
        phi_2 = phi_1(idx);

        phi_3 = [];
        % find d_p(c) = 0 , cn 連接的 punc_vn 數量為零
        for CN=phi_2
            if  length(intersect(CN2VN_pos{CN},Prior_Candidate_Vns))==0
                phi_3(end+1) = CN;
            end
        end

        %% step-3 find Candidate VNs
        % Vu(cn) unpunc_vn with V(cn)
        % Ω0 - {vn , \forall vn in Vu(cn) , \forall cn in Ψ3}
        % dr(vn) - number of reserved check node in C(vn)
        % Ω1 - {vn with min dr(vn), \forall vn in Ω0}
        % Ω2 - {vn with min d(vn), \forall vn in Ω2}
        
        omega_0 = [];
        for CN=phi_3
            Nei_unpunc_VNs = setdiff(CN2VN_pos{CN},Prior_Candidate_Vns);
            if isempty(omega_0)
                omega_0 = [omega_0,Nei_unpunc_VNs];
            end
            omega_0 = union(omega_0,Nei_unpunc_VNs);
        end

        omega_1 = [];
        min_reverved_CN_num = 1e9;
        for VN=omega_0
            Nei_CN = VN2CN_pos{VN};
            reserved_CN_num = length(intersect(Cr,Nei_CN)); % dr
            if reserved_CN_num < min_reverved_CN_num
                min_reverved_CN_num =  reserved_CN_num;
                omega_1 = [VN];
            elseif reserved_CN_num == min_reverved_CN_num
                omega_1(end+1)=VN;
            end
        end

        each_vn_deg = vn_deg(omega_1);
        min_vn_deg_num = min(each_vn_deg);
        idx = find(each_vn_deg==min_vn_deg_num);
        omega_2 = omega_1(idx);
        
        %% step4 - VN當作 root 計算recovery tree 中的 unpunc_vn
        %S_E(vn) - number of unpunctured vn in expand recovery tree
        % Ω3 - {vn with min S_E(vn), \forall vn in Ω2}
        S_E = zeros(1,cols);
        for VN=omega_2
            Nei_CNs = VN2CN_pos{VN};
            for CN=Nei_CNs
                S_E(VN) = S_E(VN) + W_c2v(CN,VN);  
            end
        end

        each_S_E_vn = S_E(omega_2);
        min_each_S_E_vn_num = min(each_S_E_vn);
        idx = find(each_S_E_vn==min_each_S_E_vn_num);
        omega_3 = omega_2(idx);
        %% step5 determine prior candidate vn
        if ~isempty(omega_3)
            % select punc vn
            
            for VN=omega_3
                Nei_CNs = VN2CN_pos{VN};
                reserved_CN = intersect(Nei_CNs,phi_3);
                if numel(reserved_CN) >= 1
                    % 隨機選一個位置
                    idx = randi(numel(reserved_CN));
                    % 只保留該隨機選到的那個檢查節點
                    reserved_CN = reserved_CN(idx);
                    Prior_VN = VN;
                    Prior_Candidate_Vns = union(Prior_Candidate_Vns,Prior_VN);
                    V_u = setdiff(V_u,CN2VN_pos{reserved_CN});
                    Cr = union(Cr,reserved_CN);
                    C_u = setdiff(C_u,reserved_CN);
                    Vp_2_Cr_map(Prior_VN) = reserved_CN;
                    for CN=VN2CN_pos{Prior_VN}
                        S_v2c(Prior_VN,CN) = 0;
                        for Nei_CNs_WithoutCN=setdiff(VN2CN_pos{Prior_VN},CN)
                            S_v2c(Prior_VN,CN) = S_v2c(Prior_VN,CN) + W_c2v(Nei_CNs_WithoutCN,Prior_VN);
                        end
                    end
                    break;
                else 
                    continue;
                end
            end
        else
            break;
        end
        
    end   
    %%
    P_v = Prior_Candidate_Vns;
    P_c = Cr;
    %% Algorithm2 - Find puncturing VNs
    V_p = []; % puncture vn node
    V_u = 1:cols;
    V_p = [];
    C_u = 1:rows;
    C_r = [];
    S_v2c = zeros(cols,rows);
    for VN=V_u
        for CN=VN2CN_pos{VN}
            S_v2c(VN,CN) = 1; 
        end
    end

    while  length(V_p)~= puncture_bits_num
        %% step1 - Count W(c->v) 、 W(c)
        % W(c->v) : unpunc vn 的數量 for c in C(v) from v
        % W(c)    : unpunc vn 的數量 在 recovery tree (root 為 c)
        W_c2v = zeros(rows,cols);
        W_c   = zeros(1,rows);
        for CN=C_u
            Nei_VNs = CN2VN_pos{CN};
            for VN=Nei_VNs
                for Non_VNs_VN=setdiff(Nei_VNs,VN)
                    W_c2v(CN,VN) = W_c2v(CN,VN) + S_v2c(Non_VNs_VN,CN); 
                end
            end
            
            for VN=Nei_VNs
                W_c(CN) = W_c(CN) + S_v2c(VN,CN);
            end
        end
        %% step2 - find the set of eligible candidates
        % Ψ1 - cn with 最小的W(cn) , \forall c in C_u
        % Ψ2 - cn with 最小的d(cn) , \forall c in Ψ1
        % Ψ3 - cn with 最小的d_p(cn) , \forall c in Ψ2
        
        phi_1 = [];
        min_Wc = 1e9;
        for CN=C_u
            if min_Wc > W_c(CN)
                min_Wc = W_c(CN);
                phi_1 = [CN];
            elseif min_Wc == W_c(CN)
                phi_1(end+1) = CN;
            end
        end
        
        each_cn_deg = cn_deg(phi_1);
        min_each_cn_deg = min(each_cn_deg);
        idx = find(each_cn_deg==min_each_cn_deg);
        phi_2 = phi_1(idx);

        phi_3 = [];
        min_punc_vn_num = 1e9;
        % find min d_p(c) , cn 連接的 punc_vn 數量為零
        for CN=phi_2
            d_p = length(intersect(CN2VN_pos{CN},V_p));
            if  d_p == min_punc_vn_num
                phi_3(end+1) = CN;
            elseif d_p < min_punc_vn_num
                min_punc_vn_num = d_p;
                phi_3 = [CN];
            end
        end
        %% step-3 find Candidate VNs
        % Vu(cn) unpunc_vn with V(cn)
        % Ω0 - {vn , \forall vn in Vu(cn) , \forall cn in Ψ3}
        % dr(vn) - number of reserved check node in C(vn)
        % Ω1 - {vn with min dr(vn), \forall vn in Ω0}
        % Ω2 - {vn with min d(vn), \forall vn in Ω2}
        
        omega_0 = [];
        for CN=phi_3
            Nei_unpunc_VNs = setdiff(CN2VN_pos{CN},V_p);
            omega_0 = union(Nei_unpunc_VNs,Nei_unpunc_VNs);
        end

        omega_1 = [];
        min_reverved_CN_num = 1e9;
        for VN=omega_0
            Nei_CN = VN2CN_pos{VN};
            reserved_CN_num = length(intersect(Cr,Nei_CN)); % dr
            if reserved_CN_num < min_reverved_CN_num
                min_reverved_CN_num =  reserved_CN_num;
                omega_1 = [VN];
            elseif reserved_CN_num == min_reverved_CN_num
                omega_1(end+1)=VN;
            end
        end

        each_vn_deg = vn_deg(omega_1);
        min_vn_deg_num = min(each_vn_deg);
        idx = find(each_vn_deg==min_vn_deg_num);
        omega_2 = omega_1(idx);
        %% step4 - VN當作 root 計算recovery tree 中的 unpunc_vn
        %S_E(vn) - number of unpunctured vn in expand recovery tree
        % Ω3 - {vn with min S_E(vn), \forall vn in Ω2}
        S_E = zeros(1,cols);
        for VN=omega_2
            Nei_CNs = VN2CN_pos{VN};
            for CN=Nei_CNs
                S_E(VN) = S_E(VN) + W_c2v(CN,VN);  
            end
        end

        each_S_E_vn = S_E(omega_2);
        min_each_S_E_vn_num = min(each_S_E_vn);
        idx = find(each_S_E_vn==min_each_S_E_vn_num);
        omega_3 = omega_2(idx);
        %% step5 - Finde the intersection with prior candidates
        % P_1 - 跟 candidate 的 交集
        % du(vn) - vn 所連接到 unreserved check node的數量
        % P_2 - 找出max du(vn) \forall vn in P_1
        % P_3 - 找出degree低的 ,\forall vn in P_2
        P_1 = intersect(omega_3,P_v);
        P_2 = [];
        min_du = 1e9;
        for VN=P_1 
            Nei_CNs = VN2CN_pos{VN};
            Nei_CNs_without_reserved_node_num = length(setdiff(Nei_CNs,C_r));
            if Nei_CNs_without_reserved_node_num < min_du   
                min_du = Nei_CNs_without_reserved_node_num;
                P_2 = [VN];
            elseif Nei_CNs_without_reserved_node_num == min_du   
                P_2(end+1) = VN;
            end
        end

        each_vn_deg = vn_deg(P_2);
        min_vn_deg_num = min(each_vn_deg);
        idx = find(each_vn_deg==min_vn_deg_num);
        P_3 = P_2(idx);
        %% find punc vn
        if ~isempty(P_3)
            idx = randi(numel(P_3)); % 隨機選一個位置
            choose_VN = P_3(idx);
            choose_CN = Vp_2_Cr_map(choose_VN);
            reserved_CN = Vp_2_Cr_map(choose_VN);
            if choose_CN~=0 && any(ismember(phi_3,choose_CN))
                reserved_CN = choose_CN;
            else
                Nei_CNs = VN2CN_pos{choose_VN};
                reserved_CN  = intersect(phi_3,Nei_CNs);
                reserved_CN = reserved_CN(1); % random choose 
            end
            P_v = setdiff(P_v,choose_VN);
        else
            while true
                idx = randi(numel(omega_3)); % 隨機選一個位置
                choose_VN = omega_3(idx);
                Nei_CNs = VN2CN_pos{choose_VN};
                reserved_CN  = intersect(phi_3,Nei_CNs);
                if ~isempty(reserved_CN)
                    reserved_CN = reserved_CN(1); % random choose 
                    break;
                end
            end
        end
        V_u = setdiff(V_u,choose_VN);
        
        %% Verify if dead check node are formed
        % du(v) - number of unreserved check node
        % du(c) - number of unpunctured variable node
        flag = true;
        du_v = length(setdiff(VN2CN_pos{choose_VN},C_r));
        dv   = vn_deg(choose_VN);
        reserved_CN
        du_c = length(setdiff(CN2VN_pos{reserved_CN},V_p));
        dc   = cn_deg(reserved_CN);
        if du_v==dv || du_c==dc
            flag = true;
        else
            I_max = 5; % 可以看成是 在弟幾次iteration 可以被還原回來
            I = 0;
            V_puls_p = union(V_p,choose_VN);
            while I<I_max &&  ~isempty(V_puls_p)
                for VN=V_puls_p
                    Nei_CNs = VN2CN_pos{VN};
                    for CN=Nei_CNs
                        if length(intersect(V_puls_p,setdiff(CN2VN_pos{CN},VN)))==0
                            V_puls_p = setdiff(V_puls_p,VN);
                            break;
                        end
                    end

                end
                I = I + 1;
            end
            if length(V_puls_p)==0
                flag = true;
            else
                flag = false;
            end
        end
        %%
        if flag==true
            V_p = union(V_p,choose_VN);
            C_r = union(C_r,reserved_CN);
            C_u = setdiff(C_u,reserved_CN);
            for CN=VN2CN_pos{choose_VN}
                S_v2c(choose_VN,CN) = 0;
                for Nei_CNs_WithoutCN=setdiff(VN2CN_pos{choose_VN},CN)
                    S_v2c(choose_VN,CN) = S_v2c(choose_VN,CN) + W_c2v(Nei_CNs_WithoutCN,choose_VN);
                end
            end
        end
        %% 
        if length(V_p)==puncture_bits_num
            position = V_p;
            break;
        end
    end
    position = V_p;
end
    