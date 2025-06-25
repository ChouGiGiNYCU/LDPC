function position = Efficient_Puncturing_Scheme_for_Irregular_LDPC(H,puncture_bits_num,Already_Punc_VNs,lmax)
    %% init 
    Punc_VNs = Already_Punc_VNs;
    O = [];
    [rows,cols] = size(H);
    C = 1:rows;
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
    
    %% Pre-Process
   

    %%
    while numel(Punc_VNs)~=puncture_bits_num
        %% step 2.1.0
        % O is check node set for Punc Vn
        % Candidate_C  - CN with lowdegree PuncVns for c , \forall c in C\O
        C = setdiff(C,O);
        min_Punc_VNs_deg = 1e9;
        Candidate_C = [];
        for CN=C
            Punc_VNs_deg = length(intersect(CN2VN_pos{CN},Punc_VNs)); % Non Punc Vns degree
            if min_Punc_VNs_deg > Punc_VNs_deg
                Candidate_C        = CN;
            elseif min_Punc_VNs_deg == Punc_VNs_deg
                Candidate_C(end+1) = CN;
            end
        end

        % random choose cn
        choose_CN = Candidate_C(randi(length(Candidate_C)));
        O = union(O,choose_CN);
        %% step 2.1.1
        Nei_VNs = setdiff(CN2VN_pos{choose_CN},Punc_VNs);
        if length(Nei_VNs)==length(CN2VN_pos{choose_CN})
            % step 2.1.2
            min_vn_deg = 1e9;
            Candidate_V = [];
            for VN=Nei_VNs  % choose low degree VN
                VN_deg = length(VN2CN_pos{VN});
                if min_vn_deg > VN_deg
                    min_vn_deg = VN_deg;
                    Candidate_V = [VN];
                elseif min_vn_deg == VN_deg
                    Candidate_V(end+1) = VN;
                end
            end
           % if mutiple Candidate VN -> random choose
           choose_VN = Candidate_V(randi(length(Candidate_V)));
        else
            % ACE
            cycle_length = 8;
            wedge = [];
            find_flag = false;
            while cycle_length < lmax && find_flag==false

                min_ACE_score = 1e9;
                for VN=Nei_VNs
                    ACE_score = ACE(H,VN2CN_pos,CN2VN_pos,cycle_length,VN);
                    % sentence = sprintf("ACE_score : %d",ACE_score);
                    % disp(sentence);
                    if min_ACE_score > ACE_score
                        min_ACE_score = ACE_score;
                        wedge = VN;
                    elseif min_ACE_score==ACE_score
                        wedge(end+1) = VN;
                    end
                end

                if numel(wedge)==1
                    choose_VN = wedge;
                    find_flag = true;
                    break;
                else
                    wedge = [];
                    cycle_length = cycle_length + 2;
                end

                
            end
            if  ~find_flag && numel(wedge)>=1
                choose_VN = wedge(randi(numel(wedge)));
                % sentence = sprintf("wedge size : 0");
                % disp(sentence);
            elseif numel(wedge)==0
                % sentence = sprintf("wedge size : 0");
                % disp(sentence);
            end
            
        end

        %% Step 2.1.4
        Punc_VNs = [Punc_VNs,choose_VN];
       
    end
    %%
    position = Punc_VNs;
    
end