#ifndef SCULPTOR_H
#define SCULPTOR_H

/**
 * @brief Estrutura Voxel representando um elemento volumétrico tridimensional.
 * Armazena as componentes de cor (r, g, b), opacidade (a) e se está ativo (isOn).
 */
struct Voxel {
    float r;    ///< Canal vermelho (0.0 a 1.0)
    float g;    ///< Canal verde (0.0 a 1.0)
    float b;    ///< Canal azul (0.0 a 1.0)
    float a;    ///< Canal alfa / transparência (0.0 a 1.0)
    bool isOn;  ///< Estado do voxel: true para ativo (visível), false para desativado
};

/**
 * @brief Classe Sculptor para modelagem digital 3D em matriz de voxels.
 * Permite desenhar e esculpir primitivas geométricas e exportar a cena para formato .off.
 */
class Sculptor {
protected:
    Voxel ***v;         ///< Matriz tridimensional dinâmica de voxels [nx][ny][nz]
    int nx;             ///< Dimensão no eixo X (largura)
    int ny;             ///< Dimensão no eixo Y (altura)
    int nz;             ///< Dimensão no eixo Z (profundidade)
    float r, g, b, a;   ///< Cor e transparência de desenho atuais

public:
    /**
     * @brief Construtor da classe Sculptor.
     * @param _nx Dimensão máxima em X
     * @param _ny Dimensão máxima em Y
     * @param _nz Dimensão máxima em Z
     */
    Sculptor(int _nx, int _ny, int _nz);

    /**
     * @brief Destrutor da classe Sculptor. Libera a memória alocada dinamicamente.
     */
    ~Sculptor();

    /**
     * @brief Define a cor de desenho para as próximas operações.
     * @param r Componente vermelha [0.0, 1.0]
     * @param g Componente verde [0.0, 1.0]
     * @param b Componente azul [0.0, 1.0]
     * @param alpha Opacidade [0.0, 1.0]
     */
    void setColor(float r, float g, float b, float alpha);

    /**
     * @brief Ativa o voxel na coordenada (x, y, z) com a cor atual.
     */
    void putVoxel(int x, int y, int z);

    /**
     * @brief Desativa o voxel na coordenada (x, y, z).
     */
    void cutVoxel(int x, int y, int z);

    /**
     * @brief Cria um paralelepípedo sólido de voxels ativos entre os intervalos especificados.
     */
    void putBox(int x0, int x1, int y0, int y1, int z0, int z1);

    /**
     * @brief Remove voxels em um paralelepípedo entre os intervalos especificados.
     */
    void cutBox(int x0, int x1, int y0, int y1, int z0, int z1);

    /**
     * @brief Cria uma esfera sólida centrada em (xcenter, ycenter, zcenter).
     */
    void putSphere(int xcenter, int ycenter, int zcenter, int radius);

    /**
     * @brief Remove voxels pertencentes a uma esfera.
     */
    void cutSphere(int xcenter, int ycenter, int zcenter, int radius);

    /**
     * @brief Cria um elipsoide sólido centrado em (xcenter, ycenter, zcenter) com raios rx, ry, rz.
     */
    void putEllipsoid(int xcenter, int ycenter, int zcenter, int rx, int ry, int rz);

    /**
     * @brief Remove voxels pertencentes a um elipsoide.
     */
    void cutEllipsoid(int xcenter, int ycenter, int zcenter, int rx, int ry, int rz);

    /**
     * @brief Grava a escultura em um arquivo no formato .off (Object File Format).
     * @param filename Nome do arquivo de saída (ex: "cenario.off")
     * @param optimize Se true, exporta apenas as faces expostas (reduz tamanho em até 90%
     *                 e melhora a velocidade e compatibilidade com visualizadores web).
     *                 Se false, exporta todos os voxels ativos (6 faces por voxel).
     */
    void writeOFF(const char* filename, bool optimize = true);

    // Getters para limites
    int getNx() const { return nx; }
    int getNy() const { return ny; }
    int getNz() const { return nz; }
};

#endif // SCULPTOR_H
