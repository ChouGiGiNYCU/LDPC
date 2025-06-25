function U_CN = min_recovery_tree(H,CN2VN_pos,VN2CN_pos,Punc_VNs,CNs)
    % 輸入要找的CN，以CN當作Tree的root，找出 unpunc_vn 在這棵tree中
    % 反過來想，先對每一個punc_vn 找出可以最少被還原的boader vns數量
    % 概念用bottom-up method 方法去找
    U_CN = zeros(1,CNs);
    min_NeiVNs_to_PuncVNs_num = zeros(1,size(H,2));
    copy_Punc_VNs = Punc_VNs;
    Already_recover_PuncVNs = [];
    % 先處理可以 1-SR 還原的 Punc_VN 
    for punc_vn=copy_Punc_VNs
        min_NeiVNs = 1e9;
        for Nei_CN=VN2CN_pos{punc_vn}
            Nei_CN_VNs = setdiff(CN2VN_pos{Nei_CN},punc_vn);
            if ~any(ismember(Punc_VNs,Nei_CN_VNs))
                Nei_CN_VNs_num = size(Nei_CN_VNs,2);
                min_NeiVNs = min(min_NeiVNs,Nei_CN_VNs_num);
            end
        end
        if min_NeiVNs~=1e9
            min_NeiVNs_to_PuncVNs_num(punc_vn) = min_NeiVNs;
            Already_recover_PuncVNs(end+1) = punc_vn; 
        end
    end
    copy_Punc_VNs = setdiff(copy_Punc_VNs,Already_recover_PuncVNs);
    % 處理需要 2-SR 或 更多SR
    while ~isempty(copy_Punc_VNs)
        for Punc_VN=copy_Punc_VNs
            min_NeiVNs = 1e9;
            for Nei_CN=VN2CN_pos{punc_vn}
                % 此 Nei_CN 所連接的unpunc vn數量 + 如果有punc，punc_vn 所連接的到最小的 unpunc_vn數量
                Can_Recovery_flag = true;
                Nei_CN_CanRecovery_PuncVN_num = 0; 
                Nei_CN_VNs = setdiff(CN2VN_pos{Nei_CN},Punc_VN);
                for Nei_CN_VN=Nei_CN_VNs 
                    
                    if any(ismember(Punc_VN,Nei_CN_VN)) % 此Nei_CN_VN 為 Punc_VN
                        if min_NeiVNs_to_PuncVNs_num(Nei_CN_VN)~=0 % 此Nei_CN_VN 已經找到最小的recovery tree了 
                            Nei_CN_CanRecovery_PuncVN_num = Nei_CN_CanRecovery_PuncVN_num + min_NeiVNs_to_PuncVNs_num(Nei_CN_VN);
                        else
                            Can_Recovery_flag = false;
                            break;
                        end
                    else % 代表此 Nei_CN_VN 沒有被 Punc
                        Nei_CN_CanRecovery_PuncVN_num = Nei_CN_CanRecovery_PuncVN_num + 1;
                    end

                end % Nei_CN_VN
                if Can_Recovery_flag==true
                    min_NeiVNs = min(min_NeiVNs,Nei_CN_CanRecovery_PuncVN_num);
                end
            end % Nei_CN
            if min_NeiVNs~=1e9 % 代表找到這個Punc_VN對應的最小 recovery tree了
                min_NeiVNs_to_PuncVNs_num(Punc_VN) = min_NeiVNs;
                Already_recover_PuncVNs(end+1) = Punc_VN; 
            end
        end % Punc_VN
        copy_Punc_VNs = setdiff(copy_Punc_VNs,Already_recover_PuncVNs);
    end
    

    % 開始對 CN 找出最小 recovert tree
    for CN=1:CNs
        Nei_CN_VNs = CN2VN_pos{CN};
        unpunc_VNs_num = 0;
        for VN=Nei_CN_VNs
            if min_NeiVNs_to_PuncVNs_num(VN)==0
                unpunc_VNs_num = unpunc_VNs_num + 1; % unpunc vn
            else
                unpunc_VNs_num = unpunc_VNs_num + min_NeiVNs_to_PuncVNs_num(VN); % punc_vn
            end
            
        end
        U_CN(CN) = unpunc_VNs_num ;
    end
end

