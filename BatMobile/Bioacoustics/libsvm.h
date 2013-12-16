/*
 * Copyright (c) 2012, Chris Scott, C.D.Scott@leeds.ac.uk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSVM_H
#define LIBSVM_H

#include "svm.h"
#include <string>
#include <vector>
#include <deque>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <string>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
class LibSVM {

public:
    LibSVM();
    ~LibSVM();
    bool loadModel(std::string path);
    std::pair<int, double> predict(std::deque<double>& features);

private:

    svm_parameter param;// set by parse_command_line
    svm_problem prob;// set by read_problem
    svm_model *model;
    svm_node *x_space;
    int cross_validation;
    int nr_fold;

};

inline LibSVM::LibSVM() {
    //
}

inline LibSVM::~LibSVM() {
    //
}

inline bool LibSVM::loadModel(std::string path) {
    if(path.empty()) return false;
    model = svm_load_model(path.c_str());

    return (!model) ? false : true;
}

//predict class from features (scale features first!)
inline std::pair<int, double> LibSVM::predict(std::deque<double>& features) {
    struct svm_node *x;
    x = (struct svm_node *) malloc((1+features.size())*sizeof(struct svm_node));
    int index = 0;
    for(unsigned int i=0; i<features.size(); ++i) {
        x[index].index = i+1;//feature index
        x[index++].value = features[i];//feature value
    }
    x[index].index = -1;//required by libSVM

    std::pair<int, double> p(0,0);
    if(param.probability) {
        //double *prob_estimates=Malloc(double,svm_get_nr_class(model));
        std::vector<double> prob_estimates(svm_get_nr_class(model));
        p.first = svm_predict_probability(model,x,&prob_estimates[0]);//winning class label
        p.second = prob_estimates[(p.first-1)];//winning class probability
        for(unsigned int i=0; i<prob_estimates.size(); ++i) {
            p.second = std::max(p.second, prob_estimates[i]);
        }
    }
    else p.first = svm_predict(model, x);

    free(x);
    return p;
}

#endif // LIBSVM_H
