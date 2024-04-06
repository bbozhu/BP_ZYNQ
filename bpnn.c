#include <stdio.h>  // 引入标准输入输出库
#include <stdlib.h> // 引入标准库，用于随机数等功能
#include <math.h>   // 引入数学库，主要用于exp()函数
#include <time.h>   // 引入时间库，用于初始化随机数种子

#define BUFFER_SIZE 1024 // 假设一行数据不会超过1024个字符

// 定义神经网络的结构和学习参数
#define INPUT_NODES 7           // 输入层节点数
#define HIDDEN_NODES 2          // 隐藏层节点数
#define OUTPUT_NODES 1          // 输出层节点数
#define LEARNING_RATE 0.1       // 学习率
#define EPOCHS 500            // 训练的总迭代次数
#define TRAINING_SAMPLES 64     // 训练样本数
#define VALIDATION_SAMPLES 10   // 验证样本数

void print_hidden_weights();
void print_output_weights();
void forward_propagation();
void backward_propagation();


// 网络各层节点的值
double input_layer[INPUT_NODES];        // 输入层节点
double hidden_layer[HIDDEN_NODES];      // 隐藏层节点
double output_layer[OUTPUT_NODES];      // 输出层节点

// 各层的偏置值
double hidden_layer_bias[HIDDEN_NODES]; // 隐藏层偏置
double output_layer_bias[OUTPUT_NODES]; // 输出层偏置

// 权重矩阵
double hidden_weights[INPUT_NODES][HIDDEN_NODES]; // 输入层到隐藏层的权重
double output_weights[HIDDEN_NODES][OUTPUT_NODES]; // 隐藏层到输出层的权重

// 误差和梯度
double delta_output[OUTPUT_NODES];      // 输出层的delta值
double delta_hidden[HIDDEN_NODES];      // 隐藏层的delta值

// Sigmoid 激活函数
double sigmoid(double x) {
    return 256 / (1 + exp(3-6*x/256)); // 返回Sigmoid函数的计算结果
}

// // Sigmoid函数的导数
// double sigmoid_derivative(double x) {
//     return x * (1 - x); // 返回Sigmoid导数的计算结果
// }

// 假定CSV文件中每一行的数据由逗号分隔
void load_weights_and_biases(const char *weights_hid_path, const char *biases_hid_path, 
                             const char *weights_out_path, const char *biases_out_path) {
    FILE *file;
    int i, j;
    char buffer[BUFFER_SIZE]; // 用于读取行的缓冲区
    
    // 加载隐藏层权重
    file = fopen(weights_hid_path, "r");
    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        perror("Error opening file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < INPUT_NODES; i++) {
        for (j = 0; j < HIDDEN_NODES; j++) {
            if (fscanf(file, "%lf,", &hidden_weights[i][j]) != 1) {
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
    for (i = 0; i < HIDDEN_NODES; i++) {
        if (fscanf(file, "%lf,", &hidden_layer_bias[i]) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);

    // 加载输出层权重
    file = fopen(weights_out_path, "r");
    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        perror("Error opening file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < HIDDEN_NODES; i++) {
        for (j = 0; j < OUTPUT_NODES; j++) {
            if (fscanf(file, "%lf,", &output_weights[i][j]) != 1) {
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
    for (i = 0; i < OUTPUT_NODES; i++) {
        if (fscanf(file, "%lf,", &output_layer_bias[i]) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}

void load_input_samples(const char *input_path, double training_inputs[TRAINING_SAMPLES][INPUT_NODES]) {
    FILE *file = fopen(input_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TRAINING_SAMPLES; i++) {
        for (int j = 0; j < INPUT_NODES; j++) {
            if (fscanf(file, "%lf,", &training_inputs[i][j]) != 1) {
                perror("Error reading file");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);
}

void load_output_samples(const char *output_path, double training_outputs[TRAINING_SAMPLES]) {
    FILE *file = fopen(output_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TRAINING_SAMPLES; i++) {
        if (fscanf(file, "%lf,", &training_outputs[i]) != 1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }    
    }
    fclose(file);
}

// // 前向传播算法（包含偏置）
// void forward_propagation(double input[]) {
//     // 计算隐藏层的值，包括偏置
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         hidden_layer[i] = hidden_layer_bias[i]; // 初始化为偏置值
//         for (int j = 0; j < INPUT_NODES; j++) {
//             hidden_layer[i] += input[j] * hidden_weights[j][i];
//         }
//         hidden_layer[i] = sigmoid(hidden_layer[i]); // 激活函数
//     }

//     // 计算输出层的值，包括偏置
//     for (int i = 0; i < OUTPUT_NODES; i++) {
//         output_layer[i] = output_layer_bias[i]; // 初始化为偏置值
//         for (int j = 0; j < HIDDEN_NODES; j++) {
//             output_layer[i] += hidden_layer[j] * output_weights[j][i];
//         }
//         output_layer[i] = sigmoid(output_layer[i]); // 激活函数
//     }
// }

// // 反向传播算法（包含偏置更新）
// void backward_propagation(double input[], double target) {
//     // 计算输出层的delta值
//     for (int i = 0; i < OUTPUT_NODES; i++) {
//         delta_output[i] = (output_layer[i] - target) * sigmoid_derivative(output_layer[i]);
//     }

//     // 更新输出层权重和偏置
//     for (int i = 0; i < OUTPUT_NODES; i++) {
//         output_layer_bias[i] -= LEARNING_RATE * delta_output[i];
//         for (int j = 0; j < HIDDEN_NODES; j++) {
//             output_weights[j][i] -= LEARNING_RATE * delta_output[i] * hidden_layer[j];
//         }
//     }

//     // 计算隐藏层的delta值
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         delta_hidden[i] = 0;
//         for (int j = 0; j < OUTPUT_NODES; j++) {
//             delta_hidden[i] += delta_output[j] * output_weights[i][j];
//         }
//         delta_hidden[i] *= sigmoid_derivative(hidden_layer[i]);
//     }

//     // 更新输入层到隐藏层的权重和隐藏层的偏置
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         hidden_layer_bias[i] -= LEARNING_RATE * delta_hidden[i];
//         for (int j = 0; j < INPUT_NODES; j++) {
//             hidden_weights[j][i] -= LEARNING_RATE * delta_hidden[i] * input[j];
//         }
//     }
// }

// 前向传播算法
void forward_propagation(double input[]) {
    // 计算隐藏层的值
    for (int i = 0; i < HIDDEN_NODES; i++) {
        hidden_layer[i] = 0; // 初始化隐藏层节点值为0
        for (int j = 0; j < INPUT_NODES; j++) {
            // 累加输入层到隐藏层的加权和
            hidden_layer[i] += input[j] * hidden_weights[j][i];
        }
        // 加上偏置，并通过激活函数
        hidden_layer[i] = (hidden_layer[i] /256)+hidden_layer_bias[i];
        // hidden_layer[i] = hidden_layer[i] /256 ;
        hidden_layer[i] = sigmoid(hidden_layer[i]);
    }

    // 计算输出层的值
    for (int i = 0; i < OUTPUT_NODES; i++) {
        output_layer[i] = 0; // 初始化输出层节点值为0
        for (int j = 0; j < HIDDEN_NODES; j++) {
            // 累加隐藏层到输出层的加权和
            output_layer[i] += hidden_layer[j] * output_weights[j][i];
        }
        // 加上偏置，并通过激活函数
        output_layer[i] = (output_layer[i] /256) + output_layer_bias[i];
        // output_layer[i] = sigmoid(output_layer[i]);
    }
}

// // 反向传播算法
// void backward_propagation(double input[], double target) {
//     // 计算输出层的delta值
//     for (int i = 0; i < OUTPUT_NODES; i++) {
//         // 使用目标值和输出值计算梯度
//         delta_output[i] = (output_layer[i] - target) * sigmoid_derivative(output_layer[i]);
//     }

//     // 计算隐藏层的delta值
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         delta_hidden[i] = 0; // 初始化delta值为0
//         for (int j = 0; j < OUTPUT_NODES; j++) {
//             // 累加输出层delta值和权重的乘积
//             delta_hidden[i] += delta_output[j] * output_weights[i][j];
//         }
//         // 计算最终的隐藏层delta值
//         delta_hidden[i] *= sigmoid_derivative(hidden_layer[i]);
//     }

//     // 更新隐藏层到输出层的权重
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         for (int j = 0; j < OUTPUT_NODES; j++) {
//             output_weights[i][j] -= LEARNING_RATE * delta_output[j] * hidden_layer[i];
//         }
//     }

//     // 更新输出层的偏置
//     for (int i = 0; i < OUTPUT_NODES; i++) {
//         output_layer_bias[i] -= LEARNING_RATE * delta_output[i];
//     }

//     // 更新输入层到隐藏层的权重
//     for (int i = 0; i < INPUT_NODES; i++) {
//         for (int j = 0; j < HIDDEN_NODES; j++) {
//             hidden_weights[i][j] -= LEARNING_RATE * delta_hidden[j] * input[i];
//         }
//     }

//     // 更新隐藏层的偏置
//     for (int i = 0; i < HIDDEN_NODES; i++) {
//         hidden_layer_bias[i] -= LEARNING_RATE * delta_hidden[i];
//     }
// }


// validate_network函数修改为返回平均误差、最大误差和均方误差
void validate_network(double training_inputs[TRAINING_SAMPLES][INPUT_NODES], double training_outputs[TRAINING_SAMPLES], double *average_error, double *max_error, double *mse) {
    srand(time(NULL)); // 初始化随机数种子
    double sum_error = 0;
    *max_error = 0;
    *mse = 0;

    for (int i = 0; i < VALIDATION_SAMPLES; i++) {
        int index = rand() % TRAINING_SAMPLES;

        forward_propagation(training_inputs[index]);
        double error = training_outputs[index] - output_layer[0];

        sum_error += fabs(error);
        *mse += error * error;
        if (fabs(error) > *max_error) {
            *max_error = fabs(error);
        }
    }

    *average_error = sum_error / VALIDATION_SAMPLES;
    *mse /= VALIDATION_SAMPLES;
}

// 修改train_network以在每次迭代后打印验证指标
void train_network(double training_inputs[TRAINING_SAMPLES][INPUT_NODES], double training_outputs[TRAINING_SAMPLES]) {
    double average_error, max_error, mse;
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int i = 0; i < TRAINING_SAMPLES; i++) {
            forward_propagation(training_inputs[i]);
            // backward_propagation(training_inputs[i], training_outputs[i]);
        }
        
        // 打印隐藏层的权重
        print_hidden_weights();
        // 打印输出层权重
        print_output_weights();
        // 每训练一次后进行验证
        validate_network(training_inputs, training_outputs, &average_error, &max_error, &mse);
        // 打印验证结果
        printf("Epoch %d: Average Error: %f, Max Error: %f, MSE: %f\n", epoch, average_error, max_error, mse);
    }
}

// 打印隐藏层权重
void print_hidden_weights() {
    printf("Hidden Layer Weights:\n");
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            // 打印每个权重
            printf("Weight[%d][%d]: %f\n", i, j, hidden_weights[i][j]);
        }
    }
}

// 打印输出层权重的函数
void print_output_weights() {
    printf("Output Layer Weights:\n");
    for (int i = 0; i < HIDDEN_NODES; i++) {
        for (int j = 0; j < OUTPUT_NODES; j++) {
            // 打印每个权重
            printf("Weight[%d][%d]: %f\n", i, j, output_weights[i][j]);
        }
    }
}

// 主函数
int main() {
    double training_inputs[TRAINING_SAMPLES][INPUT_NODES];
    double training_outputs[TRAINING_SAMPLES];
    // 假设这些文件路径是正确的，且CSV文件位于程序的运行目录中
    const char *input_path = "X.csv"; // 假设输出层偏置的文件名是b_out.csv
    const char *output_path = "labels.csv"; // 假设输出层偏置的文件名是b_out.csv
    const char *weights_hid_path = "w_hid.csv";
    const char *biases_hid_path = "w_hid.csv"; // 假设隐藏层偏置的文件名是b_hid.csv
    const char *weights_out_path = "w_out.csv";
    const char *biases_out_path = "w_out.csv"; // 假设输出层偏置的文件名是b_out.csv

    load_input_samples(input_path, training_inputs);
    load_output_samples(output_path, training_outputs);
    // 调用函数来加载权重和偏置
    load_weights_and_biases(weights_hid_path, biases_hid_path, weights_out_path, biases_out_path);

    // 训练网络
    train_network(training_inputs, training_outputs);


    return 0;
}
