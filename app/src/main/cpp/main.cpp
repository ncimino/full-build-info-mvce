/*
 * A simple hello world application. Uses a library to tokenize and join a string and prints the result.
 */
#include <iostream>

#include "string_utils.h"
#include "message.h"

int main() {
    std::string token = get_message();
    std::cout << "Token: '" << split_and_join(token) << "'" << std::endl;
    return 0;
}
