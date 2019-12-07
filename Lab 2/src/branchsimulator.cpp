#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>

using namespace std;

int read_file(int argc, char *argv[], int &M, int &K, vector<pair<uint32_t, bool>> &trace)
{
    if (argc != 3)
    {
        cout << "Please input valid file names" << endl;
        cout << "E.g. ./branchsimulator.out config.txt trace.txt" << endl;
        return -1;
    }
    fstream configFile;
    configFile.open(argv[1]);
    if (configFile.is_open())
    {
        configFile >> M;
        configFile >> K;
    }
    else
    {
        cout << "Error reading file "
             << argv[1] << endl;
        return -2;
    }
    configFile.close();

    uint32_t address;
    bool ifBranch;
    string line;
    fstream traceFile;
    traceFile.open(argv[2]);
    if (traceFile.is_open())
    {
        while (getline(traceFile, line))
        {
            stringstream ss;
            ss << line;
            ss >> hex >> address >> ifBranch;
            trace.push_back({address, ifBranch});
        }
    }
    else
    {
        cout << "Error reading file "
             << argv[2] << endl;
        return -3;
    }
    traceFile.close();
    return 0;
}

/// 2 bit finite state machine
class SC
{
public:
    SC() : _state(3){};

    bool ChangeState(bool s)
    {
        _state += s ? 1 : -1;
        _state = (s + _state == 1) ? 3 * s : _state;
        _state = _state > 3 ? 3 : _state < 0 ? 0 : _state;
        return _state > 1;
    }

    bool GetState() const
    {
        return _state > 1;
    }

private:
    char _state;
};

class PHT
{
public:
    PHT(int m, int k) : m(m), k(k)
    {
        for (auto i = 0; i < (1 << k); i++)
            _PHT.push_back(vector<SC>(1 << m));
    }

    vector<SC> &operator[](int x)
    {
        return _PHT[x];
    }

private:
    int m, k;
    vector<vector<SC>> _PHT;
};

int main(int argc, char *argv[])
{
    int M, K;
    vector<pair<uint32_t, bool>> trace;
    int r;
    if ((r = read_file(argc, argv, M, K, trace)) != 0)
        return r;
    if (!(0 < M <= 20 && 0 < K <= 8))
    {
        cout << "Please input valid M, K (0<M<21,0<K<9)" << endl;
        return 1;
    }

    cout << "M: " << M << ", K: " << K << endl;

    fstream output("trace.out", fstream::out);
    char BHR = 0;
    const int MASK = (1 << M) - 1;
    PHT Pht(M, K);

    for (auto t : trace)
    {
        output << Pht[BHR][t.first & MASK].GetState() << endl;
        Pht[BHR][t.first & MASK].ChangeState(t.second);
        BHR = (BHR >> 1) + t.second * (1 << (K - 1));
    }
    output.close();
    return 0;
}