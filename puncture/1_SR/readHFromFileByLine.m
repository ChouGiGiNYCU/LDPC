function H_matrix = readHFromFileByLine(file_name)
    fileID = fopen(file_name, 'r');
    if fileID == -1
        error('無法打開文件');
    end
    % Read the first two numbers that specify the dimensions
    line = fgetl(fileID);
    H_size = sscanf(line, '%d'); % 解析該行的數字，轉換為數字陣列
    cols = H_size(1);
    rows= H_size(2);
    disp(H_size.')
    line = fgetl(fileID); % 讀取最大權重
    max_weights = sscanf(line, '%d');
    disp(max_weights.')
    
    % Read the next part which gives row weights
    line = fgetl(fileID);
    cols_weights = sscanf(line, '%d');
    disp(cols_weights.');

    line = fgetl(fileID);
    rows_weights = sscanf(line, '%d');
    disp(rows_weights.')
    
    % Initialize the H matrix with zeros
    H_matrix = zeros(rows, cols);
    
    % Read the non-zero values and their positions
    for col = 1:cols
        line = fgetl(fileID);
        non_zero_positions = sscanf(line, '%d');
     
        
        % Fill the H_matrix
        for row=non_zero_positions.'
            if row~=0
                H_matrix(row, col) = 1; % Set the appropriate positions to 1
            end
        end
    end
    
    % Close the file
    fclose(fileID);
    
    % Display the H matrix
    disp(H_matrix);
end
