function compareTwoFiles(file1, file2)
    % 開啟兩個文件
    fid1 = fopen(file1, 'r');
    fid2 = fopen(file2, 'r');

    % 檢查是否成功打開文件
    if fid1 == -1
        error('無法打開文件: %s', file1);
    end
    if fid2 == -1
        error('無法打開文件: %s', file2);
    end

    line_num = 0; % 記錄行數
    is_identical = true; % 記錄兩個文件是否完全相同

    % 逐行讀取並比較
    while ~feof(fid1) || ~feof(fid2)
        % 讀取一行
        line1 = fgetl(fid1);
        line2 = fgetl(fid2);
        line_num = line_num + 1;

        % 如果其中一個文件已到結尾但另一個還沒結束
        if feof(fid1) ~= feof(fid2)
            fprintf('❌ 文件長度不同！\n');
            is_identical = false;
            break;
        end

        % 比較內容
        if ~strcmp(line1, line2)
            fprintf('❌ 第 %d 行不匹配：\n', line_num);
            fprintf('  %s: %s\n', file1, line1);
            fprintf('  %s: %s\n', file2, line2);
            is_identical = false;
        end
    end

    % 關閉文件
    fclose(fid1);
    fclose(fid2);

    % 結果輸出
    if is_identical
        fprintf('✅ 兩個文件內容完全相同！\n');
    else
        fprintf('❌ 兩個文件內容不相同。\n');
    end
end
