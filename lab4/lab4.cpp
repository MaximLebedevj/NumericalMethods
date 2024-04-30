#include <cmath>
#include <fstream>

double phi(double x, double y) {
    return -x + 4 * pow(y, 2); 
}

int main() {
    std::ofstream fout("lab4_points.txt");
    int a, b, M, N;
    a = b = 1;
    M = N = 100;
    double eps = 1e-4;

    double hx = static_cast<double>(a) / N;
    double hy = static_cast<double>(b) / M;
    
    double *xi = new double[N + 1];
    for (int i = 0; i < N + 1; i++) {
        xi[i] = hx * i;
        fout << xi[i] << " ";
    }
    fout << "\n";

    double *yi = new double[M + 1];
    for (int i = 0; i < M + 1; i++) {
        yi[i] = hy * i;
        fout << yi[i] << " ";
    }
    fout << "\n";

    double **u = new double*[N + 1];
    for (int i = 0; i < N + 1; ++i) {
        u[i] = new double[M + 1];
    }

    // Инициализируем u[][] нулями
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            u[i][j] = 0;
        }
    }

    // Граничное условие u(x, 0)
    for (int i = 0; i < N + 1; i++) {
        u[i][0] = phi(xi[i], 0);
    }

    // Граничное условие u(x, M)
    for (int i = 0; i < N + 1; i++) {
        u[i][M] = phi(xi[i], yi[M]);
    }

    // Граничное условие u(0, y)
    for (int j = 0; j < M + 1; j++) {
        u[0][j] = phi(0, yi[j]);
    }

    // Граничное условие u(N, y)
    for (int j = 0; j < M + 1; j++) {
        u[N][j] = phi(xi[N], yi[j]);
    }

    double **u0 = new double*[N + 1];
    for (int i = 0; i < N + 1; ++i) {
        u0[i] = new double[M + 1];
    }

    double **u1 = new double*[N + 1];
    for (int i = 0; i < N + 1; ++i) {
        u1[i] = new double[M + 1];
    }

    // Копируем u[][] в u0[][] и u1[][]
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            u0[i][j] = u[i][j];
            u1[i][j] = u[i][j];
        }
    }

    double max;
    double temp;
    int iter = 0;
    while (true) {
        max = -1.0;
        iter++;
        for (int i = 1; i < N; i++) {
            for (int j = 1; j < M; j++) {
                u1[i][j] = 1.0 / 4.0 * (u1[i - 1][j] + u0[i + 1][j] + u0[i][j + 1] + u1[i][j - 1]);
            }
        }
        for (int i = 0; i < N + 1; i++) {
            for (int j = 0; j < M + 1; j++) {
                temp = fabs(u1[i][j] - u0[i][j]);
                max = (temp > max) ? temp : max; 
            }
        }
        if (max <= eps) {
            break;
        }
        for (int i = 0; i < N + 1; i++) {
            for (int j = 0; j < M + 1; j++) {
                u0[i][j] = u1[i][j];
            }
        }
    }

    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            fout << u1[i][j] << " ";
        }
        fout << "\n";
    }

    delete[] xi;
    delete[] yi;

    for (int i = 0; i < N + 1; ++i) {
        delete [] u[i];
        delete [] u0[i];
        delete [] u1[i];
    }

    delete[] u;
    delete[] u0;
    delete[] u1;

    fout.close();

    return 0;
}
