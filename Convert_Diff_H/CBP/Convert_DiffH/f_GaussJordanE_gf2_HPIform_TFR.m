function [N, K, M_ori, M_sys, sysG, sysH, G, PCM] = f_GaussJordanE_gf2_HPIform_TFR(PCM)
    sysH=PCM;
    N=size(PCM, 2);
    M_ori=size(PCM, 1);

    swaps=zeros(M_ori,2);
    swaps_count=1;
    j=1;
    index=1;
    while index<=M_ori && j<=N
        i=index;
        while (sysH(i,j)==0)&&(i<M_ori)
            i=i+1;
        end
        if sysH(i,j)==1
            temp=sysH(i,:);
            sysH(i,:)=sysH(index,:);
            sysH(index,:)=temp;
            for idx=1:M_ori
                if (index~=idx)&&(sysH(idx,j)==1)
                    sysH(idx,:)=mod(sysH(idx,:)+sysH(index,:),2);
                end
            end
            swaps(swaps_count,:)=[index j];
            swaps_count=swaps_count+1;
            index=index+1;
            j=index;
        else
            j=j+1;
        end
    end
    
    for i=1:swaps_count-1
        temp=sysH(:,swaps(i,1));
        sysH(:,swaps(i,1))=sysH(:,swaps(i,2));
        sysH(:,swaps(i,2))=temp;
    end
    sysH_row_weight = sum(sysH,2);
    PCM_rank = sum(sysH_row_weight~=0);
    
    if PCM_rank==M_ori
        M_sys = M_ori;
        K = N-M_sys;
        
        G = [(sysH(:,M_sys+1:N))', eye(K)];
        sysH = [sysH(:,M_sys+1:N), eye(N-K)];
        
        for i=swaps_count-1:-1:1
            temp=G(:,swaps(i,1));
            G(:,swaps(i,1))=G(:,swaps(i,2));
            G(:,swaps(i,2))=temp;
        end
        sysG = [eye(K), (sysH(:,M_sys+1:N))'];
    else
        sysH = sysH(1:PCM_rank,:);
        M_sys = PCM_rank;
        K = N-M_sys;
        G = [(sysH(:,M_sys+1:N))', eye(K)];
        sysH = [sysH(:,M_sys+1:N), eye(N-K)];
        
        for i=swaps_count-1:-1:1
            temp=G(:,swaps(i,1));
            G(:,swaps(i,1))=G(:,swaps(i,2));
            G(:,swaps(i,2))=temp;
        end
        sysG = [eye(K), (sysH(:,M_sys+1:N))'];
    end
    
    
    %disp(sum(sum((mod(PCM*G',2)))));
% H = PCM;
% rows = size(H, 1);
% cols = size(H, 2);
% 
% r = 1;
% for c = cols - rows + 1:cols
%     if H(r,c) == 0
%         % Swap needed
%         for r2 = r + 1:rows
%             if H(r2,c) ~= 0
%                 tmp = H(r, :);
%                 H(r, :) = H(r2, :);
%                 H(r2, :) = tmp;
%             end
%         end
% 
%         % Ups...
%         if H(r,c) == 0
%             error('H is singular');
%         end
%     end
% 
%     % Forward substitute
%     for r2 = r + 1:rows
%         if H(r2, c) == 1
%             H(r2, :) = xor(H(r2, :), H(r, :));
%         end
%     end
% 
%     % Back Substitution
%     for r2 = 1:r - 1
%         if H(r2, c) == 1
%             H(r2, :) = xor(H(r2, :), H(r, :));
%         end
%     end
% 
%     % Next row
%     r = r + 1;
% end