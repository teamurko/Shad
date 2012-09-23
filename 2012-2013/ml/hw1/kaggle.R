library('e1071')   # package for svm
library('pROC')    # package for auc calculation
library('RWeka')
library('rpart')

# training bagging composition with base svm classifier
bagging.learn = function(train.set,
                         class.name = "Label", 
                         iterations = 10,      
                         subset_ratio = 0.3)
{
  form = as.formula(paste(class.name, '~.'))
  bagging.model = list(2);
  bagging.model[[1]] = list(length = iterations);
  bagging.model[[2]] = levels(train.set$Label);
  subset_size = floor(subset_ratio * nrow(train.set));
  for(i in 1:iterations) {
      train.subset = train.set[sample(1:nrow(train.set),
                                      subset_size,
                                      replace = TRUE),];
      weights = c(0.31625835189309581, 0.68374164810690419)
      names(weights) = c('0', '1')
      base_classifier = svm(form, train.subset, kernel='linear');
      bagging.model[[1]][[i]] = base_classifier;
    
  }
  return (bagging.model)
}

# composition voting
bagging.voting = function(base_classifiers.votes)
{
  labels = unique(base_classifiers.votes)
  leader = -1;
  max_count = -1;
  for(i in 1:length(labels)) {
    label = labels[i];
    count = length(base_classifiers.votes[base_classifiers.votes == label]);
    if(count > max_count) {
      leader = label
      max_count = count
    }
  }
  return(leader)
}

# composition prediction
bagging.predict = function(bagging.model,
                           test.set)
{
  labels = bagging.model[[2]];
  base_classifiers = bagging.model[[1]]
  predictions = matrix(nrow = nrow(test.set), ncol = length(base_classifiers))
  for(i in 1:length(bagging.model[[1]])) {
    predictions[,i] = as.numeric(predict(base_classifiers[[i]], test.set))
  }
  bagging.predictions = apply(predictions, 1, bagging.voting)
  bagging.predictions = as.factor(labels[bagging.predictions]);
  return (bagging.predictions)
}

predict_bagging = function(train.data, test_data)
{
  model = bagging.learn(train_data, "Label", iterations=10, subset_ratio=0.9);
  Label = bagging.predict(model, test_data);
  cbind(test_data, Label)
}

save_to = function(predicted.data, filename)
{
  write.table(predicted.data, file=filename, sep=',', row.names=FALSE, quote=FALSE);
}

do_cross_validation = function(train_data, algorithm, num_groups=10)
{
  size = length(train_data$Label);
  group_size = (size + num_groups - 1) %/% num_groups;
  groups = split(1:size, ceiling(1:size / group_size));
  error = 0.0;
  for (indices_group in groups) {
    train_subset = train_data[setdiff(1:size, indices_group),]
    check_subset = train_data[indices_group,]
    test_data = subset(check_subset, select=F1:F24)
    predicted_data = algorithm(train_subset, test_data)
    diff = as.numeric(check_subset$Label) - as.numeric(predicted_data$Label)
    error = error + sum(sapply(diff, abs)) / length(diff)
  }
  error / length(groups)
}
  
train_data = read.csv('train.csv')
train_data$Label = factor(train_data$Label)
print(do_cross_validation(train_data, predict_bagging, num_groups=5))
#test_data = read.csv('test.csv')
#save_to(predict_bagging(train_data, test_data), "/Users/pakso/Fun/Shad/2012-2013/ml/hw1/ans.txt")
