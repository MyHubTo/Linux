#include <stdio.h>

int main() {
    char arr[] = "abcdef";
    for (int i = 0; i <= sizeof(arr); i++) { // 注意这里使用 <= 包含 '\0'
        if (arr[i] == '\0') {
            printf("\\0"); // 以 '\0' 形式输出
        } 
    }
    return 0;
}
