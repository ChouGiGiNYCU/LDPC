function H_matrix = readHFromFileByLine(filename)
    fileID = fopen(filename, 'r');
    if fileID == -1
        error('無法打開文件');
    end
    % Read the first two numbers that specify the dimensions
    H_size = fscanf(fileID, '%d', 2);
    cols = H_size(1);
    rows= H_size(2);
    disp(H_size.')
    max_weights  = fscanf(fileID, '%d', 2);
    disp(max_weights.')
    % disp(max_cols_weight)
    % Read the next part which gives row weights
    cols_weights = fscanf(fileID, '%d', cols);
    rows_weights = fscanf(fileID, '%d', rows);
    disp(rows_weights.')
    disp(cols_weights.')
    % Initialize the H matrix with zeros
    H_matrix = zeros(rows, cols);
    
    % Read the non-zero values and their positions
    for col = 1:cols
        % Read the number of non-zero elements in the current row
        non_zero_elements = cols_weights(col);
        
        % Read the column positions of the non-zero elements in the current row
        rows_positions = fscanf(fileID, '%d', non_zero_elements);
        
        % Fill the H_matrix
        for j = 1:non_zero_elements
            H_matrix(rows_positions(j), col) = 1; % Set the appropriate positions to 1
        end
    end
    
    % Close the file
    fclose(fileID);
    
    % Display the H matrix
    disp(H_matrix);
end
