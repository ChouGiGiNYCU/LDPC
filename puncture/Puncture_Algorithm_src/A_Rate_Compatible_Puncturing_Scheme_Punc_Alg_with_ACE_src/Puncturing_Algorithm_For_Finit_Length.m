function position = Puncturing_Algorithm_For_Finit_Length(H,puncture_bits_num,Already_Punc_VNs)
    %% init 
    Punc_VNs = Already_Punc_VNs;
    [rows,cols] = size(H);
    CN2VN_pos = {}; %record each row that have 1's col position
    for r=1:rows
        col_vns = find(H(r,:)==1);
        CN2VN_pos{end+1} = col_vns; 
    end
    VN2CN_pos = {}; %record each col that have 1's row position
    for c=1:cols
        row_vns = transpose(find(H(:,c)==1));
        VN2CN_pos{end+1} = row_vns ; 
    end
    %% pre-process ACE score
    cycle_min = 8;
    cycle_max = 8;
    ACE_map = zeros(cols,2); % VN : cycle_flag , score 
    for cycle_length=cylce_min:2:cycle_max
        for VN=1:cols
            [ACE_score,cycle_flag] = ACE(H,VN2CN_pos,CN2VN_pos,cycle_length,VN);
            ACE_map(VN,1) = cycle_flag;
            ACE_map(VN,2) = ACE_score;
        end
    end
    
    while size(Punc_VNs,2)~=puncture_bits_num
        disp(length(Punc_VNs));
        %% step2 - Count F(c) 、 U(c)
        % F(c) : 每個cn旁，有多少的 punc vn
        % U(c) : recovery tree 中 min unpunc vn 的數量
        F_CN = zeros(1,rows);
        for CN=1:rows
            F_CN(CN) = size(intersect(CN2VN_pos{CN},Punc_VNs),2);
        end
        
        U_CN = min_recovery_tree(H,CN2VN_pos,VN2CN_pos,Punc_VNs,rows);
        
        %% step3 找出 Candidate Check Node
        % ψ(psi) 是 check node set 中 連接 punc_vn 最少的
        % Candidate CheckNode 是 ψ 中 recovery tree中的 min unpunc_vn 的check node
        % set
        psi = find(F_CN==(min(F_CN)));
        min_recovery_tree_CNnum = U_CN(psi(1));
        Candidate_Check = [];
        for CN=psi
            if min_recovery_tree_CNnum==U_CN(CN)
                Candidate_Check(end+1) = CN;
            elseif  U_CN(CN) < min_recovery_tree_CNnum
                Candidate_Check = [CN];
                min_recovery_tree_CNnum = U_CN(CN); 
            elseif  U_CN(CN) == min_recovery_tree_CNnum
                Candidate_Check(end+1) = CN;
            end
        end
        %% step-4 find Candidate_VariableNode
        % Candidate_VariableNode 未被 punc 過的 variable node
        Candidate_Var = [];
        for CN=Candidate_Check
            Nei_CN_VNs = CN2VN_pos{CN};
            Candidate_Var = [Candidate_Var,Nei_CN_VNs];
        end
        Candidate_Var = setdiff(Candidate_Var,Punc_VNs); % remove punc_vn
        %% step-5
        % H(v) vn board 有多少個 punc_vn 數量
        H_V = [];
        min_total_punc_vns_num = 0;
        for VN=Candidate_Var
            Nei_CNs = VN2CN_pos{VN};
            total_punc_vns_num = 0;
            for CN=Nei_CNs
               Nei_CN_VNs = setdiff(CN2VN_pos{CN},VN); 
               punc_vns   = intersect(Nei_CN_VNs,Punc_VNs);
               total_punc_vns_num  = total_punc_vns_num  + size(punc_vns,2);
            end
            % 保留total_punc_vns_num最少的
            if isempty(H_V)
                min_total_punc_vns_num = total_punc_vns_num;
                H_V = [VN];
            elseif min_total_punc_vns_num==total_punc_vns_num
                H_V(end+1) = VN;
            elseif min_total_punc_vns_num > total_punc_vns_num
                min_total_punc_vns_num = total_punc_vns_num;
                H_V = [VN];
            end
        end
        %% step-6 在H(V)中選出 degree較低的 variable node
        Low_Degree_VNs = [];
        min_degree =  size(VN2CN_pos{H_V(1)},2);
        for VN=H_V
            degree = size(VN2CN_pos{VN},2);
            if min_degree==degree
               Low_Degree_VNs(end+1) = VN;
            elseif min_degree > degree 
                Low_Degree_VNs = [VN];
                min_degree = degree ;
            end
        end
        %% step-7 把 K(v) 小的 vn node 保留
        % K(v) 是把vn所連接的cn去看recovery tree 中的 unpunc_vn 數量
        % O is 最小的 K(v)集合
        O = [];
        min_total_unpunc_vns = 0;
        for VN = Low_Degree_VNs
            Nei_CNs = VN2CN_pos{VN};
            total_unpunc_vns = 0;
            for CN=Nei_CNs
                total_unpunc_vns = total_unpunc_vns + U_CN(CN);
            end
            if isempty(O)
                min_total_unpunc_vns = total_unpunc_vns;
                O = [VN];
            elseif min_total_unpunc_vns > total_unpunc_vns
                min_total_unpunc_vns = total_unpunc_vns;
                O = [VN];
            elseif min_total_unpunc_vns == total_unpunc_vns
                O(end+1) = VN;
            end
        end
        %%  step-8 count low approximate cycle extrinsic message degree(ACE)
        % but ACE use cycle length 電腦跑不出來...
        wedge = [];
        for VN=O
            if ACE_map(VN,1)==0 % not find cycle
                wedge = [wedge,VN];
            elseif ACE_map(wedge(1),2) > ACE_map(VN,2)
                wedge = [VN];
            elseif ACE_map(wedge(1),2) == ACE_map(VN,2)
                wedge = [wedge,VN];
            end
        
        end
        index = randi(length(wedge));  % Generate a random index between 1 and the length of the array
        choose_VN = wedge(index);  % Select the element at the random index
        Punc_VNs(end+1) = choose_VN; 
        
        % while cycle_length < lmax && find_flag==false
        % 
        %     min_ACE_score = 1000000000;
        %     for VN=O
        %         ACE_score = ACE(H,VN2CN_pos,CN2VN_pos,cycle_length,VN);
        %         if min_ACE_score > ACE_score
        %             min_ACE_score = ACE_score;
        %             wedge = VN;
        %         elseif min_ACE_score==ACE_score
        %             % fprintf("Coming in\n");
        %             wedge = [wedge , VN];
        %         end
        %         % fprintf('VN : %d - ACE_score : %d \n',VN,ACE_score);
        %     end
        %     % fprintf('wedge: ');
        %     % fprintf('%d ', wedge);
        %     % fprintf('\n');
        % 
        %     if numel(wedge)==1
        %         choose_VN = wedge;
        %         find_flag = true;
        %         break;
        %     elseif cycle_length~=lmax
        %         O = wedge;
        %         wedge = [];
        %         cycle_length = cycle_length + 2;
        %     end
        %     % fprintf('[cycle = %d] wedge size = %d\n', cycle_length, numel(wedge));
        % 
        % end
        
        % if  ~find_flag && numel(wedge)>=1
        %     choose_VN = wedge(randi(numel(wedge)));
        %     % sentence = sprintf("wedge size : 0");
        %     % disp(sentence);
        % elseif ~find_flag && numel(wedge)==0
        %     choose_VN = O(randi(numel(Nei_VNs)));
        % end

        % index = randi(length(O));  % Generate a random index between 1 and the length of the array
        % choose_puncvn = O(index);  % Select the element at the random index
        
    end
    %%
    position = Punc_VNs;
    
end