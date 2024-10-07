#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cmath>

float pointerX = 0.0f;
float pointerY = 0.0f;
const float boundary = 1.0f; // Define the boundaries for wrapping
const float speed = 0.01f;   // Forward speed
float angle = 0.0f;          // Angle in radians

// シリアルポートを初期化
int initSerial(const char* portName) {
    int serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPort == -1) {
        std::cerr << "Failed to open serial port" << std::endl;
        return -1;
    }
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Error getting tty attributes" << std::endl;
        return -1;
    }
    cfsetospeed(&tty, B115200); // シリアル通信速度を115200bpsに設定
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;     // パリティなし
    tty.c_cflag &= ~CSTOPB;     // ストップビット 1
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         // データビット 8
    tty.c_cflag &= ~CRTSCTS;    // ハードウェアフロー制御なし
    tty.c_cflag |= CREAD | CLOCAL; // 有効な読み込み

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ISIG; // Raw入力モード

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // ソフトウェアフロー制御なし
    tty.c_iflag &= ~(INLCR | ICRNL); // 行末処理なし

    tty.c_oflag &= ~OPOST; // Raw出力モード

    tcsetattr(serialPort, TCSANOW, &tty); // 設定を即時反映

    return serialPort;
}

// シリアルデータを読み取り、pointerX, pointerYの値を更新
void updatePointerPositionFromSerial(int serialPort) {
    char buffer[32];
    int bytesRead = read(serialPort, &buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // 文字列終端
        float newX, newY;
        if (sscanf(buffer, "%f %f", &newX, &newY) == 2) {
            pointerX = newX;
            pointerY = newY;
        }
    }

    // Wrap around the screen
    if (pointerX > boundary) pointerX = -boundary;
    if (pointerX < -boundary) pointerX = boundary;
    if (pointerY > boundary) pointerY = -boundary;
    if (pointerY < -boundary) pointerY = boundary;
}

// 矢印の位置を更新する
void updateArrowPosition() {
    // 前進
    pointerX += speed * cos(angle);
    pointerY += speed * sin(angle);

    // Wrap around the screen
    if (pointerX > boundary) pointerX = -boundary;
    if (pointerX < -boundary) pointerX = boundary;
    if (pointerY > boundary) pointerY = -boundary;
    if (pointerY < -boundary) pointerY = boundary;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 矢印を描画
    glBegin(GL_TRIANGLES);
    glVertex2f(pointerX, pointerY + 0.05f); // Top
    glVertex2f(pointerX - 0.025f, pointerY - 0.05f); // Bottom Left
    glVertex2f(pointerX + 0.025f, pointerY - 0.05f); // Bottom Right
    glEnd();
}

int main() {
    // シリアルポートを初期化
    int serialPort = initSerial("/dev/tty.M5StickCPlus2");
    if (serialPort == -1) {
        return -1;
    }

    // GLFWの初期化
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // ウィンドウの作成
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "OpenGL Arrow Pointer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLEWの初期化
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // OpenGLの設定
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glViewport(0, 0, 2000, 2000);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D表示のための投影行列

    // メインループ
    while (!glfwWindowShouldClose(window)) {
        updatePointerPositionFromSerial(serialPort); // シリアル通信で座標を更新
        updateArrowPosition(); // 矢印を前進させる
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 後始末
    close(serialPort);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
