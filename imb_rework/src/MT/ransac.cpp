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
