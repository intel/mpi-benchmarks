/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2016-2017 Intel Corporation.                                *
 * All rights reserved.                                                      *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

#include <iostream>
#include <vector>
#include <set>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

//const int amount = 200;
//const float base_number = 420.0;

template <typename T>
struct Model {
    int size;
    T N;
    void init(const vector<T> &data) {
        T sum = 0;
        for (size_t i = 0; i < data.size(); i++) {
            sum += data[i];
        }
        sum /= data.size();
        N = sum;
    }
    int fit(const vector<T> &data, T threshold) {
        int nfit = 0;
        for (size_t i = 0; i < data.size(); i++) {
            T distance = N - data[i];
            if (distance < 0) 
                distance = -distance;
            if (distance < threshold)
                nfit++;    
        }
        return nfit;
    }
    void print() {
        cout << N << endl;
    }
    Model(int asize) : size(asize) {}
    Model(const Model &other) { size = other.size; N = other.N; }
    Model &operator=(const Model &other) { this->size = other.size; this->N = other.N; return *this; }
};

/*
template <typename T> T get_rand(T base) { return 0; }

template <> float get_rand<float>(float base) { return (float)(rand() % (int)(base * 1000.0f)) / 1000.0f;  }

template <typename T>
void generate(vector<T> &data, T base_number, int n) {
    T n1 = 30;
    T n2 = 65;
    int nrand = 4;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    for (int i = 0; i < n; i++) {
        if ((i%(nrand/2)) == 0 && i && (rand()%2)) {
            data.push_back(base_number + (get_rand(n2) - n2/(T)6));
        } else {
            data.push_back(base_number + (get_rand(n1) - n1/(T)2));
        }
    }
}
*/

template <typename T>
void select(const vector<T> &data, vector<T> &inliers, int n) {
    set<size_t> samples;
    for (int i = 0; i < n; i++) {
        int sample = rand()%data.size();
        if (samples.count(sample) == 0) {
            samples.insert(sample);
            inliers.push_back(data[sample]);
        } else {
            i--;
        }
    }
}

template<typename T, typename M>
M ransac(const vector<T> &data, T threshold, float percent, int &niter) 
{
    int model_size = data.size() / 20;
    if (model_size < 20) model_size = 20;
    if (model_size > 1000) model_size = 1000;
    M BestModel(model_size);
    int amount = data.size();
    int best_fit = 0;
    niter = 0;
    while (true) {
        vector<T> inliers;
        select(data, inliers, BestModel.size);
        M Attempt(model_size);
        Attempt.init(inliers);
        int nfit = Attempt.fit(data, threshold);
        best_fit = max(best_fit, nfit);
        if (best_fit == nfit)
            BestModel = Attempt;
        if (best_fit > (int)(amount * percent))
            break;
        if (++niter % 20 == 0) {
            if (niter % 100 == 0) {
                float remove = (float)(percent * 0.002);
                percent -= remove;   
                cout << niter << " percent: " << percent << endl;
            }
            if (niter % 100 != 0) {
                T add = (T)(threshold * 0.1);
                threshold += add;   
                cout << niter << " threshold: " << threshold << endl;
            }
        }
        if (niter > 1000) {
            cout << "ransac: failed" << endl;
            M Simple(0);
            Simple.init(data);
            return Simple;
        }
    }
    cout << "ransac: niter=" << niter << " fit=" << best_fit << endl;
    return BestModel;
}

/*
int main()
{
    vector<float> data;
    generate<float>(data, base_number, amount);

//    for (int i = 0; i < data.size(); i++)
//        cout << ">> " << data[i] << endl;

    {
        Model<float, 20> Simple;
        Simple.init(data);
        Simple.print();
    }

    {
        float threshold = (float)(base_number * 0.03);
        Model<float, 10> Best = ransac<float, Model<float, 10> >(data, threshold, 0.99);
        Best.print();
    }
    return 0;
}    
*/
