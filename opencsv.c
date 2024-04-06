#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    float input[64][7]; // 假设有64个数据点，每个点有7个特征
    int i, j;

    // 打开文件
    file = fopen("X.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // 读取数据
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 7; j++) {
            // 这里假设每个特征值都是浮点数
            if (fscanf(file, "%f,", &input[i][j]) != 1) {
                perror("Error reading file");
                fclose(file);
                return -1;
            }
        }
    }

    // 关闭文件
    fclose(file);

    // 打印数据用于验证
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 7; j++) {
            printf("%f ", input[i][j]);
        }
        printf("\n");
    }

    return 0;
}


#include <stdio.h>
#include <stdlib.h>

// 假定CSV文件中每一行的数据由逗号分隔
void load_weights_and_biases(const char *weights_hid_path, const char *biases_hid_path, 
                             const char *weights_out_path, const char *biases_out_path) {
    FILE *file;
    int i, j;

    // 加载隐藏层权重
    file = fopen(weights_hid_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < INPUT_NEURONS; i++) {
        for (j = 0; j < HIDDEN_NEURONS; j++) {
            if (fscanf(file, "%lf,", &w_input_hidden[i][j]) != 1) {
                perror("Error reading file");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);

    // 加载隐藏层偏置
    file = fopen(biases_hid_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < HIDDEN_NEURONS; i++) {
        if (fscanf(file, "%lf,", &b_hidden[i]) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);

    // 加载输出层权重
    file = fopen(weights_out_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < HIDDEN_NEURONS; i++) {
        for (j = 0; j < OUTPUT_NEURONS; j++) {
            if (fscanf(file, "%lf,", &w_hidden_output[i][j]) != 1) {
                perror("Error reading file");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);

    // 加载输出层偏置
    file = fopen(biases_out_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < OUTPUT_NEURONS; i++) {
        if (fscanf(file, "%lf,", &b_output[i]) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

void load_input_samples(const char *input_path, double inputs[SAMPLES][INPUT_NEURONS]) {
    FILE *file = fopen(input_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SAMPLES; i++) {
        for (int j = 0; j < INPUT_NEURONS; j++) {
            if (fscanf(file, "%lf,", &inputs[i][j]) != 1) {
                perror("Error reading file");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);
}
