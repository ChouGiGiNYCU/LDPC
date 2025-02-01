function position = oneSR_method(H,puncture_bits_num)
    [rows,cols] = size(H);
    puncture = [];
    non_using_vn = [];
    idx=0;
    while idx<puncture_bits_num
        remain_vns = cols - size(non_using_vn,1);
        if remain_vns < puncture_bits_num - idx
            disp("Can't find enough puncture node \n");
            return;
        end
        VN = randi(cols); % produce 1:cols random integer
        if ismember(VN, non_using_vn) && ismember(VN,puncture)
            continue;
        end
        CNs = find(H(:,VN) == 1);
        for cn=CNs
            other_VN = find(H(cn,:)==1);
            % other_vn 裡面沒有已經被punture過後的bits 和 scns 所連接過的vn
            if isempty(intersect(other_VN , puncture)) && isempty(intersect(other_VN , non_using_vn))
                can_puncture = true;
            else
                can_puncture = false;
                break;
            end
        end
        if can_puncture
            puncture = [puncture,VN];
            idx = idx + 1;
            for cn=CNs
                other_VN = find(H(cn,:)==1);
                non_using_vn = union(non_using_vn, other_VN);
            end
            non_using_vn = union(non_using_vn,VN);
        end
    end
    position = puncture;
    position = sort(position);
end
