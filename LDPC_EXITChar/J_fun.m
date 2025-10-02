function y = J_fun(sigma)
% I = J(sigma) （ten Brink 類型擬合，足夠畫 EXIT）
    y = zeros(size(sigma));
    s = sigma;

    idx1 = (s > 0) & (s <= 1.6363);
    idx2 = (s > 1.6363);

    if any(idx1)
        a1 = 1.09542; b1 = 0.214217; c1 = 2.33737;
        y(idx1) = 1 - exp(-a1*s(idx1).^2 - b1*s(idx1) - c1);
    end
    if any(idx2)
        % 高互資訊區域採簡潔穩定擬合
        y(idx2) = 1 - exp(-0.5 * s(idx2).^2);
    end
    y = min(max(y,0),1);
end
