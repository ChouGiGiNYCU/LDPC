function matrix = readMatrixFromFileByLine(filename)
    % 開啟文件進行讀取 ('r' 表示讀模式)
    fileID = fopen(filename, 'r');

    % 確保文件成功開啟
    if fileID == -1
        error('無法開啟文件: %s', filename);
    end

    % 初始化一個空矩陣
    matrix = [];

    % 逐行讀取文件內容
    lineNumber = 0;
    while ~feof(fileID)
        line = fgetl(fileID); % 讀取文件中的一行
        if ischar(line)
            % 將該行的字符轉換為數字向量
            lineData = sscanf(line, '%d');
            % 將該行數據添加到矩陣中
            matrix = [matrix; lineData'];
            lineNumber = lineNumber + 1;
        end
    end

    % 關閉文件
    fclose(fileID);

    disp('文件中的矩陣已讀取成功:');
    disp(matrix);
end


