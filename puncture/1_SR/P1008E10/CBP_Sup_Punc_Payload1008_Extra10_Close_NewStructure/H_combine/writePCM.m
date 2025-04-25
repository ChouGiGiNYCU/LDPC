function writePCM(matrix, filename)
    % 開啟文件進行寫入 ('w' 表示寫模式)
    fileID = fopen(filename, 'w');
    
    % 確保文件成功開啟
    if fileID == -1
        error('無法開啟文件: %s', filename);
    end

    % 獲取矩陣的大小
    [rows, cols] = size(matrix);
    % write n,k
    
    fprintf(fileID, '%d ', cols);
    fprintf(fileID, '%d ', rows);
    fprintf(fileID, '\n'); % 換行
    each_col_ones = [];
    each_row_ones = [];
    for i = 1:cols
        ones_sum = sum(matrix(:,i));
        each_col_ones = [each_col_ones ,ones_sum];
    end
    for i = 1:rows
        ones_sum = sum(matrix(i,:));
        each_row_ones = [each_row_ones ,ones_sum];
    end
    fprintf(fileID, '%d ', max(each_col_ones));
    fprintf(fileID, '%d ', max(each_row_ones));
    fprintf(fileID, '\n'); % 換行
    for i = 1:cols
        fprintf(fileID, '%d ', each_col_ones(i));
    end
    fprintf(fileID, '\n'); % 換行
    for i = 1:rows
        fprintf(fileID, '%d ', each_row_ones(i));
    end
    fprintf(fileID, '\n'); % 換行

    for c=1:cols
        num = 0;
        for r=1:rows
            if matrix(r,c)==1
                fprintf(fileID, '%d ', r);
                num = num + 1;
            end
        end
        if num~=max(each_col_ones)
            for i=1:max(each_col_ones)-num
                fprintf(fileID, '%d ', 0);
            end
        end
        fprintf(fileID, '\n'); % 換行
    end
    
    for r=1:rows
        num = 0;
        for c=1:cols
            if matrix(r,c)==1
                fprintf(fileID, '%d ', c);
                num = num + 1;
            end
        end
        if num~=max(each_row_ones)
            for i=1:max(each_row_ones)-num
                fprintf(fileID, '%d ', 0);
            end
        end
        fprintf(fileID, '\n'); % 換行
    end
    % 關閉文件
    fclose(fileID);

    disp(['矩陣已寫入到文件: ', filename]);
end