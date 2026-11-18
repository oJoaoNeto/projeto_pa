#include "sculptor.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

Sculptor::Sculptor(int _nx, int _ny, int _nz) {
    nx = _nx;
    ny = _ny;
    nz = _nz;
    r = g = b = a = 1.0f;

    // Alocação dinâmica da matriz 3D
    v = new Voxel**[nx];
    for (int i = 0; i < nx; i++) {
        v[i] = new Voxel*[ny];
        for (int j = 0; j < ny; j++) {
            v[i][j] = new Voxel[nz];
            for (int k = 0; k < nz; k++) {
                v[i][j][k].isOn = false;
                v[i][j][k].r = 0.0f;
                v[i][j][k].g = 0.0f;
                v[i][j][k].b = 0.0f;
                v[i][j][k].a = 0.0f;
            }
        }
    }
}

Sculptor::~Sculptor() {
    if (v != nullptr) {
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                delete[] v[i][j];
            }
            delete[] v[i];
        }
        delete[] v;
        v = nullptr;
    }
}

void Sculptor::setColor(float _r, float _g, float _b, float _alpha) {
    r = std::clamp(_r, 0.0f, 1.0f);
    g = std::clamp(_g, 0.0f, 1.0f);
    b = std::clamp(_b, 0.0f, 1.0f);
    a = std::clamp(_alpha, 0.0f, 1.0f);
}

void Sculptor::putVoxel(int x, int y, int z) {
    if (x >= 0 && x < nx && y >= 0 && y < ny && z >= 0 && z < nz) {
        v[x][y][z].isOn = true;
        v[x][y][z].r = r;
        v[x][y][z].g = g;
        v[x][y][z].b = b;
        v[x][y][z].a = a;
    }
}

void Sculptor::cutVoxel(int x, int y, int z) {
    if (x >= 0 && x < nx && y >= 0 && y < ny && z >= 0 && z < nz) {
        v[x][y][z].isOn = false;
    }
}

void Sculptor::putBox(int x0, int x1, int y0, int y1, int z0, int z1) {
    int start_x = std::max(0, std::min(x0, x1));
    int end_x   = std::min(nx - 1, std::max(x0, x1));
    int start_y = std::max(0, std::min(y0, y1));
    int end_y   = std::min(ny - 1, std::max(y0, y1));
    int start_z = std::max(0, std::min(z0, z1));
    int end_z   = std::min(nz - 1, std::max(z0, z1));

    for (int i = start_x; i <= end_x; i++) {
        for (int j = start_y; j <= end_y; j++) {
            for (int k = start_z; k <= end_z; k++) {
                v[i][j][k].isOn = true;
                v[i][j][k].r = r;
                v[i][j][k].g = g;
                v[i][j][k].b = b;
                v[i][j][k].a = a;
            }
        }
    }
}

void Sculptor::cutBox(int x0, int x1, int y0, int y1, int z0, int z1) {
    int start_x = std::max(0, std::min(x0, x1));
    int end_x   = std::min(nx - 1, std::max(x0, x1));
    int start_y = std::max(0, std::min(y0, y1));
    int end_y   = std::min(ny - 1, std::max(y0, y1));
    int start_z = std::max(0, std::min(z0, z1));
    int end_z   = std::min(nz - 1, std::max(z0, z1));

    for (int i = start_x; i <= end_x; i++) {
        for (int j = start_y; j <= end_y; j++) {
            for (int k = start_z; k <= end_z; k++) {
                v[i][j][k].isOn = false;
            }
        }
    }
}

void Sculptor::putSphere(int xcenter, int ycenter, int zcenter, int radius) {
    int start_x = std::max(0, xcenter - radius);
    int end_x   = std::min(nx - 1, xcenter + radius);
    int start_y = std::max(0, ycenter - radius);
    int end_y   = std::min(ny - 1, ycenter + radius);
    int start_z = std::max(0, zcenter - radius);
    int end_z   = std::min(nz - 1, zcenter + radius);

    double r2 = static_cast<double>(radius) * radius;

    for (int i = start_x; i <= end_x; i++) {
        double dx = i - xcenter;
        for (int j = start_y; j <= end_y; j++) {
            double dy = j - ycenter;
            for (int k = start_z; k <= end_z; k++) {
                double dz = k - zcenter;
                if (dx * dx + dy * dy + dz * dz <= r2) {
                    v[i][j][k].isOn = true;
                    v[i][j][k].r = r;
                    v[i][j][k].g = g;
                    v[i][j][k].b = b;
                    v[i][j][k].a = a;
                }
            }
        }
    }
}

void Sculptor::cutSphere(int xcenter, int ycenter, int zcenter, int radius) {
    int start_x = std::max(0, xcenter - radius);
    int end_x   = std::min(nx - 1, xcenter + radius);
    int start_y = std::max(0, ycenter - radius);
    int end_y   = std::min(ny - 1, ycenter + radius);
    int start_z = std::max(0, zcenter - radius);
    int end_z   = std::min(nz - 1, zcenter + radius);

    double r2 = static_cast<double>(radius) * radius;

    for (int i = start_x; i <= end_x; i++) {
        double dx = i - xcenter;
        for (int j = start_y; j <= end_y; j++) {
            double dy = j - ycenter;
            for (int k = start_z; k <= end_z; k++) {
                double dz = k - zcenter;
                if (dx * dx + dy * dy + dz * dz <= r2) {
                    v[i][j][k].isOn = false;
                }
            }
        }
    }
}

void Sculptor::putEllipsoid(int xcenter, int ycenter, int zcenter, int rx, int ry, int rz) {
    if (rx <= 0 || ry <= 0 || rz <= 0) return;

    int start_x = std::max(0, xcenter - rx);
    int end_x   = std::min(nx - 1, xcenter + rx);
    int start_y = std::max(0, ycenter - ry);
    int end_y   = std::min(ny - 1, ycenter + ry);
    int start_z = std::max(0, zcenter - rz);
    int end_z   = std::min(nz - 1, zcenter + rz);

    double rx2 = static_cast<double>(rx) * rx;
    double ry2 = static_cast<double>(ry) * ry;
    double rz2 = static_cast<double>(rz) * rz;

    for (int i = start_x; i <= end_x; i++) {
        double dx = i - xcenter;
        for (int j = start_y; j <= end_y; j++) {
            double dy = j - ycenter;
            for (int k = start_z; k <= end_z; k++) {
                double dz = k - zcenter;
                if ((dx * dx) / rx2 + (dy * dy) / ry2 + (dz * dz) / rz2 <= 1.0) {
                    v[i][j][k].isOn = true;
                    v[i][j][k].r = r;
                    v[i][j][k].g = g;
                    v[i][j][k].b = b;
                    v[i][j][k].a = a;
                }
            }
        }
    }
}

void Sculptor::cutEllipsoid(int xcenter, int ycenter, int zcenter, int rx, int ry, int rz) {
    if (rx <= 0 || ry <= 0 || rz <= 0) return;

    int start_x = std::max(0, xcenter - rx);
    int end_x   = std::min(nx - 1, xcenter + rx);
    int start_y = std::max(0, ycenter - ry);
    int end_y   = std::min(ny - 1, ycenter + ry);
    int start_z = std::max(0, zcenter - rz);
    int end_z   = std::min(nz - 1, zcenter + rz);

    double rx2 = static_cast<double>(rx) * rx;
    double ry2 = static_cast<double>(ry) * ry;
    double rz2 = static_cast<double>(rz) * rz;

    for (int i = start_x; i <= end_x; i++) {
        double dx = i - xcenter;
        for (int j = start_y; j <= end_y; j++) {
            double dy = j - ycenter;
            for (int k = start_z; k <= end_z; k++) {
                double dz = k - zcenter;
                if ((dx * dx) / rx2 + (dy * dy) / ry2 + (dz * dz) / rz2 <= 1.0) {
                    v[i][j][k].isOn = false;
                }
            }
        }
    }
}

void Sculptor::writeOFF(const char* filename, bool optimize) {
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo " << filename << " para escrita.\n";
        return;
    }

    // Configuração de precisão numérica
    fout << std::fixed << std::setprecision(2);

    if (!optimize) {
        // Modo Padrão: exporta todos os voxels ativos (6 faces por voxel)
        int n_voxels = 0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        n_voxels++;
                    }
                }
            }
        }

        int total_vertices = 8 * n_voxels;
        int total_faces = 6 * n_voxels;

        fout << "OFF\n";
        fout << total_vertices << " " << total_faces << " 0\n";

        // Vértices
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        float x = static_cast<float>(i);
                        float y = static_cast<float>(j);
                        float z = static_cast<float>(k);

                        fout << x - 0.5f << " " << y + 0.5f << " " << z - 0.5f << "\n";
                        fout << x - 0.5f << " " << y - 0.5f << " " << z - 0.5f << "\n";
                        fout << x + 0.5f << " " << y - 0.5f << " " << z - 0.5f << "\n";
                        fout << x + 0.5f << " " << y + 0.5f << " " << z - 0.5f << "\n";
                        fout << x - 0.5f << " " << y + 0.5f << " " << z + 0.5f << "\n";
                        fout << x - 0.5f << " " << y - 0.5f << " " << z + 0.5f << "\n";
                        fout << x + 0.5f << " " << y - 0.5f << " " << z + 0.5f << "\n";
                        fout << x + 0.5f << " " << y + 0.5f << " " << z + 0.5f << "\n";
                    }
                }
            }
        }

        // Faces com cores normalizadas
        int base = 0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        float cr = v[i][j][k].r;
                        float cg = v[i][j][k].g;
                        float cb = v[i][j][k].b;
                        float ca = v[i][j][k].a;

                        fout << "4 " << base + 0 << " " << base + 3 << " " << base + 2 << " " << base + 1 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";
                        fout << "4 " << base + 4 << " " << base + 5 << " " << base + 6 << " " << base + 7 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";
                        fout << "4 " << base + 0 << " " << base + 1 << " " << base + 5 << " " << base + 4 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";
                        fout << "4 " << base + 1 << " " << base + 2 << " " << base + 6 << " " << base + 5 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";
                        fout << "4 " << base + 2 << " " << base + 3 << " " << base + 7 << " " << base + 6 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";
                        fout << "4 " << base + 3 << " " << base + 0 << " " << base + 4 << " " << base + 7 << " "
                             << cr << " " << cg << " " << cb << " " << ca << "\n";

                        base += 8;
                    }
                }
            }
        }
    } else {
        // Modo Otimizado para Visualizadores Web:
        // Apenas faces adjacentes a voxels vazios (faces expostas) são exportadas.
        // Isso elimina faces internas ocultas, reduzindo o arquivo drasticamente
        // sem nenhuma alteração na aparência visual externa.

        int visible_voxels = 0;
        int total_visible_faces = 0;

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        bool faceZNeg = (k == 0)      || !v[i][j][k - 1].isOn;
                        bool faceZPos = (k == nz - 1) || !v[i][j][k + 1].isOn;
                        bool faceXNeg = (i == 0)      || !v[i - 1][j][k].isOn;
                        bool faceYNeg = (j == 0)      || !v[i][j - 1][k].isOn;
                        bool faceXPos = (i == nx - 1) || !v[i + 1][j][k].isOn;
                        bool faceYPos = (j == ny - 1) || !v[i][j + 1][k].isOn;

                        int facesThisVoxel = (faceZNeg ? 1 : 0) + (faceZPos ? 1 : 0) +
                                             (faceXNeg ? 1 : 0) + (faceYNeg ? 1 : 0) +
                                             (faceXPos ? 1 : 0) + (faceYPos ? 1 : 0);

                        if (facesThisVoxel > 0) {
                            visible_voxels++;
                            total_visible_faces += facesThisVoxel;
                        }
                    }
                }
            }
        }

        fout << "OFF\n";
        fout << (8 * visible_voxels) << " " << total_visible_faces << " 0\n";

        // Vértices dos voxels que têm faces visíveis
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        bool faceZNeg = (k == 0)      || !v[i][j][k - 1].isOn;
                        bool faceZPos = (k == nz - 1) || !v[i][j][k + 1].isOn;
                        bool faceXNeg = (i == 0)      || !v[i - 1][j][k].isOn;
                        bool faceYNeg = (j == 0)      || !v[i][j - 1][k].isOn;
                        bool faceXPos = (i == nx - 1) || !v[i + 1][j][k].isOn;
                        bool faceYPos = (j == ny - 1) || !v[i][j + 1][k].isOn;

                        if (faceZNeg || faceZPos || faceXNeg || faceYNeg || faceXPos || faceYPos) {
                            float x = static_cast<float>(i);
                            float y = static_cast<float>(j);
                            float z = static_cast<float>(k);

                            fout << x - 0.5f << " " << y + 0.5f << " " << z - 0.5f << "\n";
                            fout << x - 0.5f << " " << y - 0.5f << " " << z - 0.5f << "\n";
                            fout << x + 0.5f << " " << y - 0.5f << " " << z - 0.5f << "\n";
                            fout << x + 0.5f << " " << y + 0.5f << " " << z - 0.5f << "\n";
                            fout << x - 0.5f << " " << y + 0.5f << " " << z + 0.5f << "\n";
                            fout << x - 0.5f << " " << y - 0.5f << " " << z + 0.5f << "\n";
                            fout << x + 0.5f << " " << y - 0.5f << " " << z + 0.5f << "\n";
                            fout << x + 0.5f << " " << y + 0.5f << " " << z + 0.5f << "\n";
                        }
                    }
                }
            }
        }

        // Faces visíveis com normais orientadas para fora
        int base = 0;
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    if (v[i][j][k].isOn) {
                        bool faceZNeg = (k == 0)      || !v[i][j][k - 1].isOn;
                        bool faceZPos = (k == nz - 1) || !v[i][j][k + 1].isOn;
                        bool faceXNeg = (i == 0)      || !v[i - 1][j][k].isOn;
                        bool faceYNeg = (j == 0)      || !v[i][j - 1][k].isOn;
                        bool faceXPos = (i == nx - 1) || !v[i + 1][j][k].isOn;
                        bool faceYPos = (j == ny - 1) || !v[i][j + 1][k].isOn;

                        if (faceZNeg || faceZPos || faceXNeg || faceYNeg || faceXPos || faceYPos) {
                            float cr = v[i][j][k].r;
                            float cg = v[i][j][k].g;
                            float cb = v[i][j][k].b;
                            float ca = v[i][j][k].a;

                            // Face Z-
                            if (faceZNeg) {
                                fout << "4 " << base + 0 << " " << base + 3 << " " << base + 2 << " " << base + 1 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }
                            // Face Z+
                            if (faceZPos) {
                                fout << "4 " << base + 4 << " " << base + 5 << " " << base + 6 << " " << base + 7 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }
                            // Face X-
                            if (faceXNeg) {
                                fout << "4 " << base + 0 << " " << base + 1 << " " << base + 5 << " " << base + 4 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }
                            // Face Y-
                            if (faceYNeg) {
                                fout << "4 " << base + 1 << " " << base + 2 << " " << base + 6 << " " << base + 5 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }
                            // Face X+
                            if (faceXPos) {
                                fout << "4 " << base + 2 << " " << base + 3 << " " << base + 7 << " " << base + 6 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }
                            // Face Y+
                            if (faceYPos) {
                                fout << "4 " << base + 3 << " " << base + 0 << " " << base + 4 << " " << base + 7 << " "
                                     << cr << " " << cg << " " << cb << " " << ca << "\n";
                            }

                            base += 8;
                        }
                    }
                }
            }
        }
    }

    fout.close();
    std::cout << "Arquivo \"" << filename << "\" gravado com sucesso!\n";
}
