% read marks file
Y = csvread('marks.csv');
R = Y ~= 0;

my_ratings = Y(:,1);

%  Normalize Ratings
[Ynorm, Ymean] = normalizeRatings(Y, R);

%  Useful Values
num_users = size(Y, 2);
num_marks = size(Y, 1);
num_features = textread("num_features.txt", "%d", "endofline", "\n");
num_iterations = textread("num_iterations.txt", "%d", "endofline", "\n");

fprintf("start with %d users %d marks learn %d features\n", num_users, num_marks, num_features)

% Set Initial Parameters (Theta, X)
X = randn(num_marks, num_features);
Theta = randn(num_users, num_features);

initial_parameters = [X(:); Theta(:)];

% Set options for fmincg
% lambda = 8 120 iterations
options = optimset('GradObj', 'on', 'MaxIter', num_iterations);

% Set Regularization
% lambdas = [0.1 0.3 0.4 0.45 0.5 0.55 0.6 0.7 1.0 2.0 5];
lambdas = textread("lambdas.txt", "%f", "endofline", "\n")';
lambda = 4;
total_cost=10000;
theta = [];

fprintf("start learning: num_features %d\n", num_features);

for index=1:size(lambdas,2)
    lambda = lambdas(index);
    fprintf("cur_lambda:%f\n", lambda);
    [theta_loc fX i] = fmincg (@(t)(cofiCostFunc(t, Ynorm, R, num_users, num_marks, ...
                            num_features, lambda)), ...
            initial_parameters, options);

    if (total_cost > fX(end))
            theta = theta_loc;
            total_cost = fX(end);
            fprintf("min_cost:%f\n", total_cost);
    end
    fprintf("fin_lambda\n");
end

% Unfold the returned theta back into U and W
X = reshape(theta(1:num_marks*num_features), num_marks, num_features);
Theta = reshape(theta(num_marks*num_features+1:end), ...
                num_users, num_features);

fprintf('Recommender system learning completed.\n');

%% ================== Part 8: Recommendation for you ====================
%  After training the model, you can now make recommendations by computing
%  the predictions matrix.
%

p = X * Theta';
my_predictions = p(:,1) + Ymean;

fb = fopen('books.csv', 'r');
books = textscan(fb, '%d %s', 'delimiter', ',');

[r, ix] = sort(my_predictions, 'descend');
fprintf('\nMy top recommendations:\n');

rfid=fopen('recommendations.csv','wt');

for i=1:size(my_predictions)
    j = ix(i);
        if (my_predictions(j) < 10)
                break;
        end
        fprintf("indexes %d Predicting rating %.1f book: %s\n", j, my_predictions(j), books{2}(j){1,1});
        fprintf(rfid, "https://fantlab.ru/%s\n", books{2}(j){1,1});
end

for i=1:size(my_predictions,1)
        if (my_ratings(i) ~= 0)
                fprintf("book %s actual/predict %d/%d\n", books{2}(i){1,1}, my_ratings(i), my_predictions(i));
        end
end

