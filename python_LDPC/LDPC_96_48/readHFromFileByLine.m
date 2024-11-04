function H_matrix = readHFromFileByLine(filename)
    fileID = fopen(filename, 'r');

    % Read the first two numbers that specify the dimensions
    H_size = fscanf(fileID, '%d %d', 2);
    rows = H_size(1);
    cols = H_size(2);
    
    % Read the next part which gives row weights
    weights = fscanf(fileID, '%d', rows);
    disp(weights)
    % Initialize the H matrix with zeros
    H_matrix = zeros(rows, cols);
    
    % Read the non-zero values and their positions
    for i = 1:rows
        % Read the number of non-zero elements in the current row
        non_zero_elements = weights(i);
        
        % Read the column positions of the non-zero elements in the current row
        col_positions = fscanf(fileID, '%d', non_zero_elements);
        
        % Fill the H_matrix
        for j = 1:non_zero_elements
            H_matrix(i, col_positions(j)) = 1; % Set the appropriate positions to 1
        end
    end
    
    % Close the file
    fclose(fileID);
    
    % Display the H matrix
    % disp(H_matrix);
end
