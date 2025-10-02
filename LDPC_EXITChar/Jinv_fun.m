function s = Jinv_fun(I)
% sigma = J^{-1}(I)（與上面 J_fun 相容的簡潔擬合）
    I = min(max(I,0),1);
    s = zeros(size(I));

    idx0 = (I > 0) & (I < 0.3646);
    idx1 = (I >= 0.3646) & (I < 1);

    if any(idx0)
        a = 1.09542; b = 0.214217; c = 2.33737;
        s(idx0) = ( -b + sqrt(b^2 - 4*a*(c + log(1 - I(idx0)))) ) ./ (2*a);
        s(idx0) = max(s(idx0), 0);
    end
    if any(idx1)
        s(idx1) = sqrt( -2*log( max(1 - I(idx1), eps) ) );
    end
    s(I==0) = 0;
    s(I==1) = 1e6;
end
