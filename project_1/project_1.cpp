#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <numeric>
#include <thread>

using namespace std;
using namespace std::chrono;

struct QrInfo
{
    int size;
    vector<int> lb;
    vector<int> cb;
    vector<int> lt;
    vector<int> ct;
    vector<int> qb;
    vector<int> db;
};

vector<vector<vector<int>>> solutions;
vector<vector<int>> matrix;
vector<int> numBlacksLine;
vector<int> numBlacksColumn;

vector<int> lineTransitions; 
vector<int> columnTransitions;

vector<int> quadrants = {0, 0, 0, 0};

vector<int> diagonals = {0, 0};
int countVerifications = 0;


void printVector(vector<int> v) {
    for (auto ele: v) {
        cout << ele << " ";
    }
    cout << "\n";
}

void printMatrix(vector<vector<int>> m) {
    for (auto line: m) {
        for (auto ele: line) {
            cout << ele << " ";
        }
        cout << "\n";
    }
}

void printMatrixFormat(vector<vector<int>> m) {
    string output = "+";
    for (int i = 0; i < m.size(); i++) {
        output += "-";
    }
    output += "+\n";

    for (auto line: m){
        output += "|";
        for (auto ele: line){
            if (ele == 1){
                output += "#";
            } else {
                output += " ";
            }
        }
        output += "|\n";
    }

    output += "+";
    for (int i = 0; i < m.size(); i++) {
        output += "-";
    }
    output += "+";

    cout << output << endl;
}

void printSolutions() {
    cout << "#############################################################\n";
    for (auto sol: solutions) {
        cout << "\n";
        printMatrixFormat(sol);
    }
    cout << "Solutions: " << solutions.size() << "\n";
}

void createMatrices(int size) {
    matrix.resize(size, vector<int>(size, 0));

    lineTransitions.resize(size, 0);
    columnTransitions.resize(size, 0);

    numBlacksLine.resize(size, 0);
    numBlacksColumn.resize(size, 0);
}


void clearVariables() {
    matrix.clear();
    solutions.clear();
    lineTransitions.clear();
    columnTransitions.clear();
    numBlacksLine.clear();
    numBlacksColumn.clear();
    quadrants = {0, 0, 0, 0};
    diagonals = {0, 0};
}


bool checkQuadrant(int quadrantId, QrInfo qr){
    if (quadrants[quadrantId] == qr.qb[quadrantId]) {
        return true;
    }
    return false;
}

int countDiagonal(bool diag = true) {
    int count = 0;
    for (int i = 0; i < matrix.size(); i++) {
        if (diag && matrix[i][i] == 1) {
            count++;
        } else if (!diag && matrix[i][matrix.size() - i - 1] == 1) {
            count++;
        }
    }
    return count;
}

bool checkDiagonals(QrInfo qr){
    if (equal(diagonals.begin(), diagonals.end(), qr.db.begin())){
        return true;
    }
    return false;
}



void updateTransitionsMatrix(int x, int y, QrInfo qr, int value, bool add = true) {
    if (y > 0 && matrix[x][y-1] != value) {
        if (add) {
            lineTransitions[x]++;
        } else {
            lineTransitions[x]--;
        }
    }
    if (x > 0 && matrix[x-1][y] != value) {
        if (add) {
            columnTransitions[y]++;
        } else {
            columnTransitions[y]--;
        }
    }
    if (value) {
        if (x == y) {
            if (add) {
                diagonals[0]++;
            } else {
                diagonals[0]--;
            }
        } else if (qr.size - x - 1 == y) {
            if (add) {
                diagonals[1]++;
            } else {
                diagonals[1]--;
            }
        }
    }
}

void updateQuadrantsMatrix(int x, int y, QrInfo qr, int value, bool add = true) {
    if (value == 0) {
        return;
    }

    // 1º quadrant
    if (x < qr.size/2 && y >= qr.size/2) {
        if (add) {
            quadrants[0]++;
        } else {
            quadrants[0]--;
        }
    // 2º quadrant
    } else if (x < qr.size/2 && y < qr.size/2) {
        if (add) {
            quadrants[1]++;
        } else {
            quadrants[1]--;
        }
    // 3º quadrant
    } else if (x >= qr.size/2 && y < qr.size/2) {
        if (add) {
            quadrants[2]++;
        } else {
            quadrants[2]--;
        }
    // 4º quadrant
    } else if (x >= qr.size/2 && y >= qr.size/2) {
        if (add) {
            quadrants[3]++;
        } else {
            quadrants[3]--;
        }
    }
}


void updateInfoMatrix(int x, int y, QrInfo qr, int value, bool add = true) {
    updateTransitionsMatrix(x, y, qr, value, add);
    updateQuadrantsMatrix(x, y, qr, value, add);

    // update numBlacksMatrix
    if (add && value == 1) {
        numBlacksLine[x] += 1;
        numBlacksColumn[y] += 1;
    } else if (!add && value == 1) {
        numBlacksLine[x] -= 1;
        numBlacksColumn[y] -= 1;
    }
}

bool checkTransitions(int x, int y, QrInfo qr, bool final = false) {
    // check if there are enough transitions

    if(! final && (lineTransitions[x] > qr.lt[x] || columnTransitions[x] > qr.ct[y])) {
        return false;
    } else if (final && (lineTransitions[x] != qr.lt[x] || columnTransitions[x] != qr.ct[y])) {
        return false;
    }
    return true;
}

// 0: white
// 1: black
vector<int> getOptions(int x, int y, QrInfo qr){
    bool add0 = true;
    bool add1 = true;

    int s1 = qr.size/2;
    int s2 = qr.size - s1;
    // check if quadrant is all 0 or 1
    if (x < qr.size/2 && y >= qr.size/2) {
        if (qr.qb[0] == 0) {
            add1 = false;
            //return {0};
        } else if (qr.qb[0] == s1 * s2) {
            add0 = false;
            //return {1};
        }
    } else if (x < qr.size/2 && y < qr.size/2) {
        if (qr.qb[1] == 0) {
            add1 = false;
            //return {0};
        } else if (qr.qb[1] == s1 * s1) {
            add0 = false;
            //return {1};
        }
    } else if (x >= qr.size/2 && y < qr.size/2) {
        if (qr.qb[2] == 0) {
            add1 = false;
            //return {0};
        } else if (qr.qb[2] == s1 * s2) {
            add0 = false;
            //return {1};
        }
    } else if (x >= qr.size/2 && y >= qr.size/2) {
        if (qr.qb[3] == 0) {
            add1 = false;
            //return {0};
        } else if (qr.qb[3] == s2 * s2) {
            add0 = false;
            //return {1};
        }
    }
    
    // check if line or collumn is all 0 or 1
    if (qr.lb[x] == 0 || qr.cb[y] == 0) {
        add1 = false;
        //return {0};
    }
    if (qr.lb[x] == qr.size || qr.cb[y] == qr.size) {
        add0 = false;
        //return {1};
    }

    // check if line/column is alternated
    if (y > 0 && qr.lt[x] == qr.size - 1) {
        if(matrix[x][y-1]) {
            add1 = false;
        } else {
            add0 = false;
        }
        //return {!matrix[x][y-1]};
    }
     if (x > 0 && qr.ct[y] == qr.size - 1) {
        if(matrix[x-1][y]) {
            add1 = false;
        } else {
            add0 = false;
        }
        //return {!matrix[x-1][y]};
    }

    // check if can add black
    if (numBlacksLine[x] >= qr.lb[x] || numBlacksColumn[y] >= qr.cb[y]){
        add1 = false;
    }


    // check if transitions are available
    // check same line
    if (lineTransitions[x] >= qr.lt[x]) {
        if (y > 0) {
            if (matrix[x][y-1] == 0) {
                add1 = false;
            } else if (matrix[x][y-1] == 1) {
                add0 = false;
            }
        }
    } 
    // check same column
    if (columnTransitions[y] >= qr.ct[y]) {
        if (x > 0) {
            if (matrix[x-1][y] == 0) {
                add1 = false;
            } else if (matrix[x-1][y] == 1) {
                add0 = false;
            }
        }
    }

    // check if can add black in quadrant
    // 1º quadrant
    if (x < qr.size/2 && y >= qr.size/2) {
        if (quadrants[0] >= qr.qb[0]) {
            add1 = false;
        }
    // 2º quadrant
    } else if (x < qr.size/2 && y < qr.size/2) {
        if (quadrants[1] >= qr.qb[1]) {
            add1 = false;
        }
    // 3º quadrant
    } else if (x >= qr.size/2 && y < qr.size/2) {
        if (quadrants[2] >= qr.qb[2]) {
            add1 = false;
        }
    // 4º quadrant
    } else if (x >= qr.size/2 && y >= qr.size/2) {
        if (quadrants[3] >= qr.qb[3]) {
            add1 = false;
        }
    }



    vector<int> options;
    if (add0) {
        options.push_back(0);
    }
    if (add1) {
        options.push_back(1);
    }

    return options;
}

bool verifyTransitions(QrInfo qr) {
    if (equal(qr.lt.begin(), qr.lt.end(), lineTransitions.begin()) && equal(qr.ct.begin(), qr.ct.end(), columnTransitions.begin())) {
        return true;
    }
    return false;
}

bool checkBlacksLC(QrInfo qr) {
    if (!equal(numBlacksLine.begin() + qr.size/2, numBlacksLine.end(), qr.lb.begin() + qr.size/2) || !equal(numBlacksColumn.begin() + qr.size/2, numBlacksColumn.end(), qr.cb.begin() + qr.size/2)) {
        return false;
    }
    return true;
}


bool findSolutions(int x, int y, QrInfo qr){
    //cout << "x: " << x << " y: " << y << endl; 
    //printVector(quadrants);
    //printVector(qr.qb);
    //printMatrix(matrix);
    //cout << "----------------" << endl;
    if (x == 0 && y == qr.size/2){
        if (!checkQuadrant(1, qr)){
            return false;
        }
    } else if (y == 0 && x == qr.size/2){ 
        if (!checkQuadrant(0, qr)){
            return false;
        }
    } else if (x == qr.size/2 && y == qr.size/2){
        if (!checkQuadrant(2, qr)){
            return false;
        }
        // check secondary diagonal
        if (diagonals[1] != qr.db[1]) {
            return false;
        }
    }    

    vector<int> opt = getOptions(x, y, qr); 

    for (auto sel: opt){  
        matrix[x][y] = sel;
        
        updateInfoMatrix(x, y, qr, sel, true);


        if (x == qr.size - 1 && y == qr.size - 1) {
            countVerifications++;           

            // check quadrants and diagonals 
            if (checkQuadrant(3, qr) && verifyTransitions(qr) && checkDiagonals(qr)) {
                // save solution
                solutions.push_back(matrix);
            }

        // 2º quadrant 
        } else if (x < qr.size/2 && y < qr.size/2) {
            // change to next quadrant (jump to 1º)
            if (x == qr.size/2 - 1 && y == qr.size/2 - 1){
                findSolutions(0, qr.size/2, qr);
            // change line
            } else if (y == qr.size/2 - 1){
                findSolutions(x + 1, 0, qr);
            } else {
                findSolutions(x, y + 1, qr);
            }
        // 1º quadrant 
        } else if (x < qr.size/2 && y >= qr.size/2) {
            // change to next quadrant (jump to 3º)
            if (x == qr.size/2 - 1 && y == qr.size - 1){
                findSolutions(qr.size/2, 0, qr);
            // change line
            } else if (y == qr.size - 1){
                findSolutions(x + 1, qr.size/2, qr);
            } else {
                findSolutions(x, y + 1, qr);
            }
        // 3º quadrant
        } else if (x >= qr.size/2 && y < qr.size/2) {
            // change to next quadrant (jump to 4º)
            if (x == qr.size - 1 && y == qr.size/2 - 1){
                findSolutions(qr.size/2, qr.size/2, qr);
            // change line
            } else if (y == qr.size/2 - 1){
                findSolutions(x + 1, 0, qr);
            } else {
                findSolutions(x, y + 1, qr);
            }
        // 4º quadrant
        } else if (x >= qr.size/2 && y >= qr.size/2) {
            // change line
            if (y == qr.size - 1){
                findSolutions(x + 1, qr.size/2, qr);
            } else {
                findSolutions(x, y + 1, qr);
            }
        }

        // undo changes
        updateInfoMatrix(x, y, qr, sel, false);
        matrix[x][y] = 0;
        
    }
    return false;
}

void printQrInfo(QrInfo qr)
{
    cout << "N: " << qr.size << "\n";

    cout << "LB: ";
    for (auto i: qr.lb){
        cout << i << " ";
    }
    cout << "\n";

    cout << "CB: ";
    for (auto i: qr.cb){
        cout << i << " ";
    }
    cout << "\n";

    cout << "LT: ";
    for (auto i: qr.lt){
        cout << i << " ";
    }
    cout << "\n";

    cout << "CT: ";
    for (auto i: qr.ct){
        cout << i << " ";
    }
    cout << "\n";

    cout << "QB: ";
    for (auto i: qr.qb){
        cout << i << " ";
    }
    cout << "\n";

    cout << "DB: ";
    for (auto i: qr.db){
        cout << i << " ";
    }
    cout << "\n";
}


int sumVector(vector<int> v){
    int sum = 0;
    for (auto i: v){
        sum += i;
    }
    return sum;
}

bool tryFindDefect(QrInfo qr) {

    if (!(sumVector(qr.lb) == sumVector(qr.cb) && sumVector(qr.cb) == sumVector(qr.qb))) {
        return true;
    }

    int s1 = qr.size/2;

    // check lines with quadrants
    // 1 and 2º quadrants with half of the lines
    if (accumulate(qr.lb.begin(), qr.lb.begin() + s1, 0) != (qr.qb[0] + qr.qb[1])) {
        return true;
    }
    // 3 and 4º quadrants with half of the lines
    if (accumulate(qr.lb.begin() +  s1, qr.lb.end(), 0) != (qr.qb[2] + qr.qb[3])) {
        return true;
    }
    // 2 and 3º quadrants with half of the columns
    if (accumulate(qr.cb.begin(), qr.cb.begin() + s1, 0) != (qr.qb[1] + qr.qb[2])) {
        return true;
    }
    // 1 and 4º quadrants with half of the columns
    if (accumulate(qr.cb.begin() + s1, qr.cb.end(), 0) != (qr.qb[0] + qr.qb[3])) {
        return true;
    }

    // check diagonals with max number of blacks
    if ((qr.qb[1] + qr.qb[3] < qr.db[0]) || (qr.qb[0] + qr.qb[2] < qr.db[1])) {
        return true;
    }
    // check if vars are larger than matrix
    for (int i = 0; i < qr.size; i++) {
        if (qr.lb[i] > qr.size || qr.cb[i] > qr.size) {
            return true;
        }
    }

    // quadrants
    s1 = qr.size/2;
    int s2 = qr.size - s1;
    if (qr.qb[0] > s1*s2 || qr.qb[1] > s1*s1 || qr.qb[2] > s2*s1 || qr.qb[3] > s2*s2) {
        return true;
    }


    // check for defects in transitions
    for (int i = 0; i < qr.size; i++) {
        int maxTransLine;
        int maxTransCol;
        // check lines
        if (qr.lb[i] < qr.size/2) {
            maxTransLine = qr.lb[i]  * 2;
        } else {
            maxTransLine = (qr.size - qr.lb[i]) * 2;
        }
        // check columns
        if (qr.cb[i] < qr.size/2) {
            maxTransCol = qr.cb[i] * 2;
        } else {
            maxTransCol = (qr.size - qr.cb[i]) * 2;
        }

        if (qr.lt[i] > maxTransLine || qr.ct[i] > maxTransCol) {
            return true;
        }

        // compare numBlacks with numTransitions
        // if there are 0 transitions, n_blaks needs to be 0 or qr.size
        if ((qr.lt[i] == 0 && !(qr.lb[i] == 0 || qr.lb[i] == qr.size))) {
            return true;
        }
        if ((qr.ct[i] == 0 && !(qr.cb[i] == 0 || qr.cb[i] == qr.size))) {
            return true;
        }

        if ((qr.lt[i] > 0 && qr.lb[i] == 0) || (qr.ct[i] > 0 && qr.cb[i] == 0)) {
            return true;
        }

    }
    return false;
}

void writeToVector(int size, vector<int>* vect) {
    int n;
    for (int i = 0; i < size; i++) {
        cin >> n;
        vect->push_back(n);
    }
}


int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    // Read input from stdin 

    int numOfQr = 0;
    cin >> numOfQr;

    vector<QrInfo> qrCodes(numOfQr);

    for (int i = 0; i < numOfQr; i++){

        cin >> qrCodes[i].size;

        writeToVector(qrCodes[i].size, &qrCodes[i].lb);
        writeToVector(qrCodes[i].size, &qrCodes[i].cb);
        writeToVector(qrCodes[i].size, &qrCodes[i].lt);
        writeToVector(qrCodes[i].size, &qrCodes[i].ct);
        writeToVector(4, &qrCodes[i].qb);
        writeToVector(2, &qrCodes[i].db);
    
    }


    for (auto qr: qrCodes){

        //auto start = high_resolution_clock::now();
        if (!tryFindDefect(qr)) {     // 
            createMatrices(qr.size);
            findSolutions(0, 0, qr);
            //printSolutions();

            //cout << "Verifications: " << countVerifications << endl;


            if (solutions.size() == 0){
                cout << "DEFECT: No QR Code generated!" << endl;
            } else if (solutions.size() == 1){
                cout << "VALID: 1 QR Code generated!" << endl;
                printMatrixFormat(solutions[0]);
            } else {
                cout << "INVALID: " << solutions.size() << " QR Codes generated!" << endl;
            }

            clearVariables();
        } else {
            cout << "DEFECT: No QR Code generated!" << endl;
        }
        //auto end = high_resolution_clock::now();
        //auto duration = duration_cast<milliseconds>(end - start);


        //cout << "Duration: " << duration.count() << " ms" << endl;
        

        solutions.clear();
    }


    return 0;
}