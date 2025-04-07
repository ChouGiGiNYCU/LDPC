function selected = random_unique_pick(array, used_set)
    while true
        idx = randi(length(array));
        candidate = array(idx);
        if ~ismember(candidate, used_set)
            selected = candidate;
            return;
        end
    end
end