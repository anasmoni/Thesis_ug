# different train and test input files



# FEA - Feature extraction algorithm

# DDC - decission dependent correlation

# Qy(Xi, Xj) = ( I(Y,Xi) + I(Y,Xj) - I(Y; Xi,Xj) ) / H(Y)

# I(Y; Xi,Xj) = I(Y;Xj) + I(Y; Xi|Xj)

# so , Qy(Xi, Xj) = ( I(Y,Xi) - I(Y; Xi|Xj) ) / H(Y)


import math
from collections import Counter
from random import seed
from random import randrange
from random import random
from csv import reader
from math import exp
import numpy as np
from pyitlib import discrete_random_variable as drv
from sklearn.metrics import mutual_info_score
from collections import defaultdict


# Find the min and max values for each column
def dataset_minmax(dataset):
    minmax = list()
    stats = [[min(column), max(column)] for column in zip(*dataset)]
    return stats


# Rescale dataset columns to the range 0-1
def normalize_dataset(dataset, minmax):
    for row in dataset:
        for i in range(len(row) - 1):
            row[i] = (row[i] - minmax[i][0]) / max(1, minmax[i][1] - minmax[i][0])


# Split a dataset into k folds
def cross_validation_split(dataset, n_folds):
    dataset_split = list()
    dataset_copy = list(dataset)
    fold_size = int(len(dataset) / n_folds)
    for i in range(n_folds):
        fold = list()
        while len(fold) < fold_size:
            index = randrange(len(dataset_copy))
            fold.append(dataset_copy.pop(index))
        dataset_split.append(fold)
    return dataset_split


# Calculate accuracy percentage
def accuracy_metric(actual, predicted):
    correct = 0
    for i in range(len(actual)):
        if actual[i] == predicted[i]:
            correct += 1
    return correct / float(len(actual)) * 100.0


# Evaluate an algorithm using a cross validation split
def evaluate_algorithm(train_set, test_set, algorithm, n_folds, *args):
    predicted = algorithm(train_set, test_set, *args)
    actual = [row[-1] for row in test_set]
    accuracy = accuracy_metric(actual, predicted)
    return accuracy


# Calculate neuron activation for an input
def activate(weights, inputs):
    activation = weights[-1]
    for i in range(len(weights) - 1):
        activation += weights[i] * inputs[i]
    return activation


# Transfer neuron activation
def transfer(activation):
    return 1.0 / (1.0 + exp(-activation))


# Forward propagate input to a network output
def forward_propagate(network, row):
    inputs = row
    for layer in network:
        new_inputs = []
        for neuron in layer:
            activation = activate(neuron['weights'], inputs)
            neuron['output'] = transfer(activation)
            new_inputs.append(neuron['output'])
        inputs = new_inputs
    return inputs


# Calculate the derivative of an neuron output
def transfer_derivative(output):
    return output * (1.0 - output)


# Backpropagate error and store in neurons
def backward_propagate_error(network, expected):
    for i in reversed(range(len(network))):
        layer = network[i]
        errors = list()
        if i != len(network) - 1:
            for j in range(len(layer)):
                error = 0.0
                for neuron in network[i + 1]:
                    error += (neuron['weights'][j] * neuron['delta'])
                errors.append(error)
        else:
            for j in range(len(layer)):
                neuron = layer[j]
                errors.append(expected[j] - neuron['output'])
        for j in range(len(layer)):
            neuron = layer[j]
            neuron['delta'] = errors[j] * transfer_derivative(neuron['output'])


# Update network weights with error
def update_weights(network, row, l_rate):
    for i in range(len(network)):
        inputs = row[:-1]
        if i != 0:
            inputs = [neuron['output'] for neuron in network[i - 1]]
        for neuron in network[i]:
            for j in range(len(inputs)):
                neuron['weights'][j] += l_rate * neuron['delta'] * inputs[j]
            neuron['weights'][-1] += l_rate * neuron['delta']


# Train a network for a fixed number of epochs
def train_network(network, train, l_rate, n_epoch, n_outputs):
    for epoch in range(n_epoch):
        for row in train:
            outputs = forward_propagate(network, row)
            expected = [0 for i in range(n_outputs + 45)]

            expected[int(row[-1])] = 1
            backward_propagate_error(network, expected)
            update_weights(network, row, l_rate)


# Initialize a network
def initialize_network(n_inputs, n_hidden, n_outputs):
    network = list()
    hidden_layer = [{'weights': [random() for i in range(n_inputs + 1)]} for i in range(n_hidden)]
    network.append(hidden_layer)
    output_layer = [{'weights': [random() for i in range(n_hidden + 1)]} for i in range(n_outputs)]
    network.append(output_layer)
    return network


# Make a prediction with a network
def predict(network, row):
    outputs = forward_propagate(network, row)
    return outputs.index(max(outputs))


# Backpropagation Algorithm With Stochastic Gradient Descent
def back_propagation(train, test, l_rate, n_epoch, n_hidden):
    n_inputs = len(train[0]) - 1
    n_outputs = len(set([row[-1] for row in train]))
    network = initialize_network(n_inputs, n_hidden, n_outputs)
    train_network(network, train, l_rate, n_epoch, n_outputs)
    predictions = list()
    for row in test:
        prediction = predict(network, row)
        predictions.append(prediction)
    return (predictions)


# classification algorithm
def CA(train_set,test_set):
    # evaluate algorithm
    seed(1)
    n_folds = 5
    l_rate = 0.3
    n_epoch = 500
    n_hidden = 3
    Accuracy = evaluate_algorithm(train_set,test_set, back_propagation, n_folds, l_rate, n_epoch, n_hidden)
    # print('Scores: %s' % scores)
    return Accuracy


# mutual information to find I(Y;X) of FEA
def calc_MI(x, y, bins):
    c_xy = np.histogram2d(x, y, bins)[0]
    mi = mutual_info_score(None, None, contingency=c_xy)
    return mi / math.log(2)


def get_subset(m, R):
    idx = 0
    S = []
    while (m > 0):
        if (m % 2):
            S.append(R[idx])

        m = m >> 1
        idx = idx + 1
    return S


def get_masks(m):
    temp = []
    for i in range(1, m):
        temp.append((bin(i).count('1'), i))
    temp.sort()
    temp.reverse()
    ret = []
    for i in temp:
        ret.append(i[1])
    return ret


train_data = np.loadtxt("SPECT_Train.txt", delimiter=',')
test_data = np.loadtxt("SPECT_Test.txt", delimiter=',')


F = len(train_data[0]) - 1

Y = train_data[:, F]  # decision variable

R = []
MI = []

for i in range(F):
    Xi = train_data[:, i]
    mi = calc_MI(Y, Xi, 100)
    MI.append((mi, i))

MI.sort()
MI.reverse()

for i in range(min(10, len(MI))):
    R.append(MI[i][1])

mx = 0
G = []
M = 1 << len(R)
print('R ', len(R))

masks = get_masks(M)
S = []  # subsets

for m in masks:
    S.append(get_subset(m, R))

minmax = dataset_minmax(train_data)
normalize_dataset(train_data, minmax)
minmax = dataset_minmax(test_data)
normalize_dataset(test_data, minmax)

print('Accuracy with all features', CA(train_data.tolist(), test_data.tolist()))

for s in S:

    s.append(F)
    train = train_data[:, s]
    test = test_data[:,s]
    acc_now = CA(train.tolist(), test.tolist())

    if (acc_now >= mx):
        G = s
        print(acc_now)
        print(G)
        mx = acc_now


G.remove(F)

print('G ', len(G))

print('Accuracy ', mx)

