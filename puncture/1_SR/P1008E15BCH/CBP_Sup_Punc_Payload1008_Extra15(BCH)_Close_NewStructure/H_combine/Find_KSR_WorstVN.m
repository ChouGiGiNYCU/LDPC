function position=Find_KSR_WorstVN(H,node_num)
    % punc_pos_bits = oneSR_method(H,H2_c);
    [Hc,Hr] = size(H);
    
    punc_pos_bits = randperm(Hc,1);
    
    i=1;
    while true 
        init_vn = randperm(Hc,1); % 一開始選擇一個vn node開始
        nei_cn = transpose(find(H(:,init_vn)==1)); % vn 所連接到的cn node
        used_cn = nei_cn; 
        i = 1;
        while i<length(nei_cn)
            % let previous puncture vn node without survive check node
            cn = nei_cn(i);
            vn = find(H(cn,:)==1); 
            if all(ismember(vn,punc_pos_bits)) % 代表這個check node 底下的 vn 都被 puncture 了 (dead check node)
                i = i + 1;
                continue;
            end
            while true
                choose_idx = randperm(length(vn), 1); % 生成不重複的隨機索引
                choose_vn = vn(choose_idx);
                if isempty(intersect(choose_vn , punc_pos_bits))
                    punc_pos_bits = [punc_pos_bits,choose_vn];
                    connet_cn = transpose(find(H(:,choose_vn)==1));
                    unused_cn = setdiff(connet_cn,used_cn);
                    used_cn = union(used_cn,unused_cn);
                    nei_cn = [nei_cn,unused_cn];
                    break;
                end
                
            end
            i = i + 1;
            if length(punc_pos_bits)==node_num
                break;
            end
        end
        if length(punc_pos_bits)==node_num
            break;
        end
    end
    position = punc_pos_bits;
end