#include "sculptor.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// Função auxiliar para desenhar uma árvore conífera (pinheiro)
void drawPineTree(Sculptor &s, int posX, int posY, int posZ, int height) {
    // Tronco marrom
    s.setColor(0.40f, 0.22f, 0.12f, 1.0f);
    int trunkHeight = std::max(3, height / 3);
    s.putBox(posX - 1, posX + 1, posY, posY + trunkHeight, posZ - 1, posZ + 1);

    // Folhagem em camadas cônicas/piramidais verdes
    int folStartY = posY + trunkHeight - 1;
    int layers = 3;
    for (int l = 0; l < layers; l++) {
        int layerBaseY = folStartY + l * 3;
        int layerRadius = (layers - l) + 2;
        float greenShade = 0.28f + l * 0.05f; // Variação de tom
        s.setColor(0.08f, greenShade, 0.14f, 1.0f);

        for (int dy = 0; dy < 4; dy++) {
            int r = layerRadius - dy;
            if (r < 0) r = 0;
            s.putEllipsoid(posX, layerBaseY + dy, posZ, r, 1, r);
        }
    }
}

// Função auxiliar para desenhar uma rocha no terreno
void drawRock(Sculptor &s, int posX, int posY, int posZ, int rx, int ry, int rz) {
    s.setColor(0.45f, 0.45f, 0.48f, 1.0f);
    s.putEllipsoid(posX, posY, posZ, rx, ry, rz);
    // Realce de sombra/textura
    s.setColor(0.38f, 0.38f, 0.40f, 1.0f);
    s.putEllipsoid(posX + 1, posY - 1, posZ, rx - 1, ry, rz - 1);
}

/**
 * @brief Desenha um lobo uivando em direção à lua.
 * Utiliza modelagem geométrica analítica e transformação para coordenadas locais,
 * orientando o corpo, o pescoço e o focinho aberto diretamente em direção ao vetor da lua.
 */
void drawWolf(Sculptor &s,
              float posX, float posY, float posZ,
              float moonX, float moonY, float moonZ,
              float scale = 1.0f,
              int furVariant = 0) {

    // Vetor de direção em direção à lua no plano horizontal
    float dx = moonX - posX;
    float dz = moonZ - posZ;
    float distXZ = std::sqrt(dx * dx + dz * dz);
    if (distXZ < 0.001f) distXZ = 0.001f;

    // Ângulo horizontal (yaw) em direção à lua
    float cosT = dx / distXZ;
    float sinT = dz / distXZ;

    // Variações de cor da pelagem conforme variante (cinza lobo, lobo escuro, lobo ártico)
    float furR = 0.45f, furG = 0.45f, furB = 0.48f;
    float darkR = 0.26f, darkG = 0.26f, darkB = 0.28f;
    float paleR = 0.72f, paleG = 0.72f, paleB = 0.75f;

    if (furVariant == 1) { // Lobo alfa / mais escuro
        furR = 0.38f; furG = 0.38f; furB = 0.40f;
        darkR = 0.18f; darkG = 0.18f; darkB = 0.20f;
        paleR = 0.65f; paleG = 0.65f; paleB = 0.68f;
    } else if (furVariant == 2) { // Lobo cinza prateado
        furR = 0.52f; furG = 0.52f; furB = 0.55f;
        darkR = 0.32f; darkG = 0.32f; darkB = 0.35f;
        paleR = 0.80f; paleG = 0.80f; paleB = 0.82f;
    }

    // Caixa delimitadora do lobo no espaço global
    int boxR = static_cast<int>(14.0f * scale) + 2;
    int minX = std::max(0, static_cast<int>(posX - boxR));
    int maxX = std::min(s.getNx() - 1, static_cast<int>(posX + boxR));
    int minY = std::max(0, static_cast<int>(posY));
    int maxY = std::min(s.getNy() - 1, static_cast<int>(posY + 20.0f * scale));
    int minZ = std::max(0, static_cast<int>(posZ - boxR));
    int maxZ = std::min(s.getNz() - 1, static_cast<int>(posZ + boxR));

    // Varredura por voxels com teste analítico das partes do lobo
    for (int ix = minX; ix <= maxX; ix++) {
        for (int iy = minY; iy <= maxY; iy++) {
            for (int iz = minZ; iz <= maxZ; iz++) {
                // Vetor relativo ao centro da base do lobo
                float rx = ix - posX;
                float ry = iy - posY;
                float rz = iz - posZ;

                // Coordenadas locais normalizadas pela escala:
                // u: frente (em direção à lua)
                // v: lateral (direita/esquerda)
                // w: altura (vertical)
                float u = (rx * cosT + rz * sinT) / scale;
                float v = (-rx * sinT + rz * cosT) / scale;
                float w = ry / scale;

                bool isVoxelSet = false;
                float vr = furR, vg = furG, vb = furB;

                // 1. PATAS E PERNAS (w de 0 a 5.5)
                if (w >= 0.0f && w <= 5.5f) {
                    // Pata dianteira esquerda: u ~ +3.2, v ~ +1.6
                    float dFL = (u - 3.2f) * (u - 3.2f) + (v - 1.6f) * (v - 1.6f);
                    // Pata dianteira direita: u ~ +3.2, v ~ -1.6
                    float dFR = (u - 3.2f) * (u - 3.2f) + (v + 1.6f) * (v + 1.6f);
                    // Pata traseira esquerda: u ~ -3.5, v ~ +1.6
                    float dBL = (u + 3.5f) * (u + 3.5f) + (v - 1.6f) * (v - 1.6f);
                    // Pata traseira direita: u ~ -3.5, v ~ -1.6
                    float dBR = (u + 3.5f) * (u + 3.5f) + (v + 1.6f) * (v + 1.6f);

                    float legRadius = 0.95f;
                    // Coxas traseiras ligeiramente mais grossas na parte superior
                    if (w > 3.0f && (dBL <= 2.2f || dBR <= 2.2f)) {
                        isVoxelSet = true;
                        vr = furR; vg = furG; vb = furB;
                    } else if (dFL <= legRadius || dFR <= legRadius || dBL <= legRadius || dBR <= legRadius) {
                        isVoxelSet = true;
                        if (w <= 1.0f) { // Patas na base
                            vr = paleR; vg = paleG; vb = paleB;
                        } else {
                            vr = furR; vg = furG; vb = furB;
                        }
                    }
                }

                // 2. CORPO / TRONCO (elipsoide principal e peito)
                if (!isVoxelSet && w >= 4.0f && w <= 9.0f) {
                    float cu = u / 4.4f;
                    float cv = v / 2.1f;
                    float cw = (w - 6.6f) / 2.3f;
                    if (cu * cu + cv * cv + cw * cw <= 1.0f) {
                        isVoxelSet = true;
                        if (w > 7.6f) { // Dorso escuro
                            vr = darkR; vg = darkG; vb = darkB;
                        } else if (w < 5.8f || u > 1.5f) { // Peitoral e ventre claros
                            vr = paleR; vg = paleG; vb = paleB;
                        } else {
                            vr = furR; vg = furG; vb = furB;
                        }
                    }
                }

                // 3. CAUDA (estendida para trás e inclinada para baixo)
                if (!isVoxelSet && u <= -3.6f && u >= -8.5f && w >= 2.5f && w <= 7.2f) {
                    // Eixo da cauda: liga (-3.8, 6.6) até (-8.0, 3.2)
                    float tailT = (-3.8f - u) / 4.2f; // de 0 a 1
                    if (tailT >= 0.0f && tailT <= 1.0f) {
                        float expectedW = 6.6f - tailT * 3.4f;
                        float dTail = (v * v) + (w - expectedW) * (w - expectedW);
                        if (dTail <= 1.3f) {
                            isVoxelSet = true;
                            if (tailT > 0.7f) { // Ponta da cauda escura
                                vr = darkR; vg = darkG; vb = darkB;
                            } else {
                                vr = furR; vg = furG; vb = furB;
                            }
                        }
                    }
                }

                // 4. PESCOÇO UIVANDO (erguido para frente e para cima em direção à lua)
                if (!isVoxelSet && u >= 1.8f && u <= 5.8f && w >= 6.8f && w <= 12.0f) {
                    // Eixo do pescoço: de (2.4, 7.0) a (5.2, 11.5)
                    float neckT = (w - 7.0f) / 4.5f;
                    if (neckT >= 0.0f && neckT <= 1.0f) {
                        float expectedU = 2.4f + neckT * 2.8f;
                        float dNeck = (u - expectedU) * (u - expectedU) + (v * v);
                        if (dNeck <= 2.2f - neckT * 0.4f) {
                            isVoxelSet = true;
                            if (v > -0.8f && v < 0.8f && u > expectedU) { // Garganta exposta clara
                                vr = paleR; vg = paleG; vb = paleB;
                            } else {
                                vr = furR; vg = furG; vb = furB;
                            }
                        }
                    }
                }

                // 5. CABEÇA E ORELHAS
                if (!isVoxelSet && u >= 4.2f && u <= 7.6f && w >= 11.0f && w <= 15.5f) {
                    // Crânio central em (5.8, 12.2)
                    float dHead = ((u - 5.8f) * (u - 5.8f)) / 1.9f +
                                  (v * v) / 1.7f +
                                  ((w - 12.2f) * (w - 12.2f)) / 1.8f;

                    if (dHead <= 1.0f) {
                        isVoxelSet = true;
                        // Olhos dourados nas laterais
                        if (std::abs(v) >= 1.1f && std::abs(v) <= 1.5f && u >= 6.0f && w >= 12.4f && w <= 13.0f) {
                            vr = 0.95f; vg = 0.75f; vb = 0.15f; // Olho âmbar
                        } else {
                            vr = furR; vg = furG; vb = furB;
                        }
                    }

                    // Orelhas pontudas erguidas para trás/topo (u ~ 4.8, v = +-1.4, w de 13.2 a 15.5)
                    if (!isVoxelSet && w >= 13.0f && w <= 15.3f && u >= 4.4f && u <= 5.6f) {
                        float dEarL = (u - 5.0f) * (u - 5.0f) + (v - 1.4f) * (v - 1.4f);
                        float dEarR = (u - 5.0f) * (u - 5.0f) + (v + 1.4f) * (v + 1.4f);
                        float earR = 0.75f - (w - 13.0f) * 0.25f;
                        if (earR > 0.0f && (dEarL <= earR || dEarR <= earR)) {
                            isVoxelSet = true;
                            vr = darkR; vg = darkG; vb = darkB;
                        }
                    }
                }

                // 6. FOCINHO E BOCA ABERTA UIVANDO (apontado para o céu em direção à lua!)
                // Eixo do focinho: estende-se de (6.0, 12.6) para (9.5, 16.2) em ângulo de ~50°
                if (!isVoxelSet && u >= 5.8f && u <= 10.0f && w >= 12.2f && w <= 17.0f) {
                    float snoutT = (u - 5.8f) / 3.6f; // de 0 (base) a 1 (ponta)
                    if (snoutT >= 0.0f && snoutT <= 1.0f) {
                        float expectedW = 12.6f + snoutT * 3.5f; // Rampa ascendente do uivo
                        float dw = w - expectedW;
                        float dSnout = (v * v) + (dw * dw);

                        float snoutRadius = 1.15f - snoutT * 0.35f;

                        if (dSnout <= snoutRadius * snoutRadius) {
                            // Boca aberta uivando: corte na parte inferior do focinho
                            if (snoutT > 0.25f && snoutT < 0.85f && dw < -0.2f && dw > -0.9f) {
                                // Interior da boca aberta (vermelho escuro / cavidade do uivo)
                                isVoxelSet = true;
                                vr = 0.32f; vg = 0.08f; vb = 0.08f;
                            } else {
                                isVoxelSet = true;
                                if (snoutT >= 0.85f) { // Trufa/focinho preto na ponta
                                    vr = 0.10f; vg = 0.10f; vb = 0.10f;
                                } else if (dw < 0.0f) { // Mandíbula inferior
                                    vr = paleR; vg = paleG; vb = paleB;
                                } else {
                                    vr = furR; vg = furG; vb = furB;
                                }
                            }
                        }
                    }
                }

                if (isVoxelSet) {
                    s.setColor(vr, vg, vb, 1.0f);
                    s.putVoxel(ix, iy, iz);
                }
            }
        }
    }
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "  GERADOR DE CENARIO 3D (.OFF): LOBOS UIVANDO PARA A LUA\n";
    std::cout << "  Disciplina: Programacao Avancada (DCA/UFRN)\n";
    std::cout << "========================================================\n";

    // Dimensões do espaço volumétrico
    const int NX = 120; // Largura (Eixo X)
    const int NY = 75;  // Altura (Eixo Y)
    const int NZ = 90;  // Profundidade (Eixo Z)

    std::cout << "Inicializando matriz Sculptor 3D [" << NX << " x " << NY << " x " << NZ << "]...\n";
    Sculptor cenario(NX, NY, NZ);

    // =========================================================================
    // 1. CAMPO VERDE (CHÃO E RELEVO NATURAL)
    // =========================================================================
    std::cout << "1/5 Modelando o campo verde e relevo natural...\n";

    // Camada de terra/base subterrânea
    cenario.setColor(0.35f, 0.22f, 0.12f, 1.0f);
    cenario.putBox(0, NX - 1, 0, 1, 0, NZ - 1);

    // Camada de grama principal em todo o vale
    cenario.setColor(0.20f, 0.68f, 0.22f, 1.0f);
    cenario.putBox(0, NX - 1, 2, 4, 0, NZ - 1);

    // Ondulações de grama suave no vale (colinas verdes baixas)
    cenario.setColor(0.24f, 0.74f, 0.25f, 1.0f);
    cenario.putEllipsoid(70, 4, 35, 25, 2, 20);
    cenario.putEllipsoid(85, 4, 65, 20, 2, 18);
    cenario.putEllipsoid(45, 4, 70, 18, 2, 15);

    // Variações de tom de verde no campo (musgo e grama densa)
    cenario.setColor(0.16f, 0.58f, 0.18f, 1.0f);
    cenario.putEllipsoid(55, 5, 25, 14, 1, 12);
    cenario.putEllipsoid(65, 5, 50, 16, 1, 14);

    // =========================================================================
    // 2. MONTANHA NA ESQUERDA SUPERIOR (COM PICO ROCHOSO E PEDESTAL)
    // =========================================================================
    std::cout << "2/5 Esculpindo a grande montanha na esquerda superior...\n";

    // Base montanhosa maciça na região esquerda (X baixo, Z moderado a baixo)
    // Camada 1: Encosta rochosa inferior
    cenario.setColor(0.40f, 0.40f, 0.42f, 1.0f);
    cenario.putEllipsoid(18, 10, 26, 22, 12, 24);
    cenario.putEllipsoid(10, 8, 40, 15, 10, 18);

    // Camada 2: Corpo médio da montanha (basalto e granito escuro)
    cenario.setColor(0.34f, 0.35f, 0.38f, 1.0f);
    cenario.putEllipsoid(20, 22, 26, 18, 12, 20);
    cenario.putEllipsoid(12, 20, 32, 14, 10, 15);

    // Camada 3: Elevação superior da montanha
    cenario.setColor(0.44f, 0.45f, 0.48f, 1.0f);
    cenario.putEllipsoid(22, 34, 26, 13, 10, 14);
    cenario.putEllipsoid(18, 38, 24, 10, 8, 11);

    // Camada 4: Cume e crista rochosa com toques de neve/gelo no topo
    cenario.setColor(0.86f, 0.90f, 0.94f, 1.0f); // Neve na crista alta
    cenario.putEllipsoid(18, 45, 22, 8, 4, 8);
    cenario.putEllipsoid(22, 44, 25, 7, 3, 7);

    // Pedestal do pico (Platô onde o Lobo Alfa ficará soberano)
    // Localizado em X ~ 27, Y ~ 45..46, Z ~ 27
    cenario.setColor(0.48f, 0.49f, 0.52f, 1.0f);
    cenario.putBox(22, 31, 44, 46, 22, 31);
    cenario.setColor(0.52f, 0.53f, 0.56f, 1.0f);
    cenario.putBox(24, 29, 46, 47, 24, 29); // Borda do precipício

    // =========================================================================
    // 3. A LUA (ESFERA BRILHANTE NO CÉU SUPERIOR DIREITO)
    // =========================================================================
    std::cout << "3/5 Criando a lua no ceu...\n";
    const float MOON_X = 96.0f;
    const float MOON_Y = 58.0f;
    const float MOON_Z = 70.0f;
    const int MOON_RADIUS = 7;

    // Disco lunar / esfera prateada brilhante
    cenario.setColor(0.98f, 0.98f, 0.88f, 1.0f);
    cenario.putSphere(static_cast<int>(MOON_X),
                      static_cast<int>(MOON_Y),
                      static_cast<int>(MOON_Z),
                      MOON_RADIUS);

    // Detalhes das crateras lunares (mares lunares / manchas cinzas suaves)
    cenario.setColor(0.84f, 0.84f, 0.78f, 1.0f);
    cenario.putSphere(static_cast<int>(MOON_X - 2), static_cast<int>(MOON_Y + 1), static_cast<int>(MOON_Z - 3), 2);
    cenario.putSphere(static_cast<int>(MOON_X + 1), static_cast<int>(MOON_Y - 2), static_cast<int>(MOON_Z - 2), 2);
    cenario.putSphere(static_cast<int>(MOON_X - 1), static_cast<int>(MOON_Y - 1), static_cast<int>(MOON_Z - 4), 1);

    // =========================================================================
    // 4. ELEMENTOS DE AMBIENTAÇÃO (ÁRVORES E ROCHAS NO VALE)
    // =========================================================================
    std::cout << "4/5 Adicionando pinheiros e pedras silvestres...\n";

    // Pinheiros ao fundo e nas bordas do campo
    drawPineTree(cenario, 12, 5, 75, 14);
    drawPineTree(cenario, 22, 5, 80, 16);
    drawPineTree(cenario, 34, 5, 78, 13);
    drawPineTree(cenario, 88, 5, 20, 12);
    drawPineTree(cenario, 102, 5, 30, 15);
    drawPineTree(cenario, 105, 5, 45, 14);

    // Rochas naturais espalhadas pelo campo verde
    drawRock(cenario, 75, 6, 25, 4, 2, 3);
    drawRock(cenario, 50, 5, 45, 3, 2, 2);
    drawRock(cenario, 85, 5, 50, 4, 2, 3);

    // =========================================================================
    // 5. OS LOBOS (TODOS UIVANDO EM DIREÇÃO À LUA!)
    // =========================================================================
    std::cout << "5/5 Posicionando a alcateia de lobos uivando para a lua...\n";

    // LOBO 1: O Lobo Alfa no Topo da Montanha (Esquerda Superior)
    // Posicionado triunfante no topo do precipício rochoso
    std::cout << "  - Lobo Alfa no pico da montanha (topo esquerdo)...\n";
    drawWolf(cenario,
             26.5f, 47.0f, 26.5f,     // Posição no topo da montanha
             MOON_X, MOON_Y, MOON_Z,  // Alvo: a Lua
             1.18f,                   // Escala de Lobo Alfa imponente
             1);                      // Pelagem alfa escura

    // LOBO 2: Lobo no Campo Verde (Centro-Esquerda)
    std::cout << "  - Lobo 2 no campo verde...\n";
    drawWolf(cenario,
             52.0f, 5.0f, 32.0f,
             MOON_X, MOON_Y, MOON_Z,
             1.0f,
             0);

    // LOBO 3: Lobo sobre a Rocha Elevada no Campo
    std::cout << "  - Lobo 3 uivando sobre a colina rochosa...\n";
    drawWolf(cenario,
             75.0f, 8.0f, 25.0f,
             MOON_X, MOON_Y, MOON_Z,
             0.95f,
             2);

    // LOBO 4: Lobo próximo à borda do bosque de coníferas
    std::cout << "  - Lobo 4 proximo a clareira dos pinheiros...\n";
    drawWolf(cenario,
             42.0f, 5.0f, 60.0f,
             MOON_X, MOON_Y, MOON_Z,
             1.0f,
             0);

    // LOBO 5: Lobo no centro do vale verde
    std::cout << "  - Lobo 5 no centro da planicie...\n";
    drawWolf(cenario,
             65.0f, 5.0f, 52.0f,
             MOON_X, MOON_Y, MOON_Z,
             1.05f,
             1);

    // =========================================================================
    // GRAVAÇÃO DO ARQUIVO .OFF
    // =========================================================================
    std::cout << "\nExportando arquivo .off...\n";

    // 1. Arquivo otimizado (ideal para carregamento instantâneo em sites como 3dviewer.net)
    const char* arquivoOtimizado = "cenario_lobos.off";
    cenario.writeOFF(arquivoOtimizado, true);

    // 2. Arquivo padrão clássico (sem remoção de faces internas, caso desejado)
    const char* arquivoPadrao = "cenario_lobos_completo.off";
    cenario.writeOFF(arquivoPadrao, false);

    std::cout << "\nProcesso concluido com sucesso!\n";
    std::cout << "-> Arquivo principal: \"" << arquivoOtimizado << "\" (recomendado para visualizadores web)\n";
    std::cout << "-> Arquivo alternativo: \"" << arquivoPadrao << "\" (formato bruto clássico da disciplina)\n";

    return 0;
}
