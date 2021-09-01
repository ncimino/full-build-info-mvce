#define UTILITIES_MODULE_EXPORT
#include "string_utils.h"

#include "linked_list.h"

static bool is_token_valid(const std::string & token) {
    return !token.empty();
}

static void add_if_valid(const std::string & token, linked_list * list) {
    if (is_token_valid(token)) {
        list->add(token);
    }
}

std::string join(const linked_list & source) {
    std::string result;
    for (int i = 0; i < source.size(); ++i) {
        if (!result.empty()) {
            result.append(" ");
        }
        result.append(source.get(i));
#ifdef NDEBUG
        result.append("DEBUG");
#endif
    }

    return result;
}

linked_list split(const std::string & source) {
    std::string::size_type last_find = 0;
    std::string::size_type current_find = 0;
    linked_list result;

    while (std::string::npos != (current_find = source.find(" ", last_find))) {
        std::string token = source.substr(last_find);
        if (std::string::npos != current_find) {
            token = token.substr(0, current_find - last_find);
        }

        add_if_valid(token, &result);
        last_find = current_find + 1;
    }
    
    std::string token = source.substr(last_find);
    add_if_valid(token, &result);
    
    return result;
}

UTILITIES_API std::string split_and_join(const std::string & source) {
    linked_list list = split(source);
    std::string result = join(list);
    return result;
}
